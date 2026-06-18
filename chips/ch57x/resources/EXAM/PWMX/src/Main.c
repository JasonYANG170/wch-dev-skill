/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description 		   : PWM1-5功能演示
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

#define  PWM8     1
#define  PWM16    0

uint16_t nextPWMCyc = 50000-1;

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main()
{
    // 关闭两线调试
    R16_PIN_ALTERNATE &= ~RB_PIN_DEBUG_EN;
    HSECFG_Capacitance(HSECap_18p);
    SetSysClock(CLK_SOURCE_HSE_PLL_100MHz);

#if PWM8

    /* 配置GPIO */
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);  // PA7 - PWM1
    GPIOA_ModeCfg(GPIO_Pin_2, GPIO_ModeOut_PP_5mA);  // PA2 - PWM2
    GPIOA_ModeCfg(GPIO_Pin_3, GPIO_ModeOut_PP_5mA);  // PA3 - PWM3
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // PA4 - PWM4
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeOut_PP_5mA);  // PA8 - PWM5

    PWMX_CLKCfg(4);                                  // cycle = 4/Fsys
    PWMX_CycleCfg(PWMX_Cycle_64);                    // 周期 = 64*cycle

    PWMX_ACTOUT(CH_PWM1, 64 / 4, Low_Level, ENABLE);  // 25% 占空比
    PWMX_ACTOUT(CH_PWM2, 64 / 4, Low_Level, ENABLE);  // 25% 占空比
    PWMX_ACTOUT(CH_PWM3, 64 / 4, Low_Level, ENABLE);  // 25% 占空比
    PWMX_ACTOUT(CH_PWM4, 64 / 4, Low_Level, ENABLE);  // 25% 占空比
    PWMX_ACTOUT(CH_PWM5, 64 / 4, Low_Level, ENABLE);  // 25% 占空比

#endif

#if PWM16

    /* 配置GPIO */
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);  // PA7 - PWM1
    GPIOA_ModeCfg(GPIO_Pin_2, GPIO_ModeOut_PP_5mA);  // PA2 - PWM2
    GPIOA_ModeCfg(GPIO_Pin_3, GPIO_ModeOut_PP_5mA);  // PA3 - PWM3
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // PA4 - PWM4
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeOut_PP_5mA);  // PA8 - PWM5

    PWMX_CLKCfg(4);                                   // cycle = 4/Fsys
    PWM_16bit_CycleEnable();                          // 16位宽使能
    PWMX_16bit_CycleCfg(CH_PWM_ALL, 60000-1);           // 16 数据宽度 Ncyc=RB_PWM_CYC_VALUE+1
    PWMX_16bit_ACTOUT(CH_PWM1, 30000, Low_Level, ENABLE);  // 50%占空比
    PWMX_16bit_ACTOUT(CH_PWM2, 15000, Low_Level, ENABLE);  // 25%占空比
    PWMX_16bit_ACTOUT(CH_PWM3, 45000, Low_Level, ENABLE);  // 75%占空比
    PWMX_16bit_ACTOUT(CH_PWM4, 30000, High_Level, ENABLE); // 50%占空比
    PWMX_16bit_ACTOUT(CH_PWM5, 15000, High_Level, ENABLE); // 25%占空比


#if 1
    // 切换周期演示，在进行周期切换时需要等到上一个周期完成后再切，以PWM1为例
    nextPWMCyc = 50000/2-1;
    PWM_INTCfg(ENABLE, RB_PWM_IE_CYC);
    PFIC_EnableIRQ(PWMX_IRQn);

#endif

#endif

    while(1);
}


/*********************************************************************
 * @fn      PWMX_IRQHandler
 *
 * @brief   PWMX中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void PWMX_IRQHandler(void)
{
    if(R8_PWM_INT_FLAG & RB_PWM_IF_CYC)
    {
        R8_PWM_INT_FLAG = RB_PWM_IF_CYC;
        PWMX_16bit_ACTOUT(CH_PWM1|CH_PWM2|CH_PWM3, nextPWMCyc/2, Low_Level, ENABLE);  // 切换周期时需要先改变占空比
        PWMX_16bit_CycleCfg(CH_PWM1|CH_PWM2|CH_PWM3, nextPWMCyc);   //注意PWM1/2/3的16位周期必须同时改，PWM4/5的16位周期必须同时改
    }
    PFIC_DisableIRQ(PWMX_IRQn);
}


