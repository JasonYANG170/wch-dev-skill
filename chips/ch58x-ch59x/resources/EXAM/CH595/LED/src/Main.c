/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2024/11/20
* Description        : LED例子
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

__attribute__((__aligned__(4))) uint32_t tx_MainData[8] = {0x01020304,0x10203040,0x03,0x04,0x05,0x06,0x07,0x08};
__attribute__((__aligned__(4))) uint32_t tx_AuxData[8] = {0xA5A55A5A,0xA5A55A5A,0x03,0x04,0x05,0x06,0x07,0x08};

#define  LSB_HSB         1           // LED串行数据位序, 1:高位在前;  0:低位在前
#define  POLAR           0           // LED数据输出极性, 0:直通，数据0输出0，数据1输出1; 1为反相

/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   调试初始化
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main()
{
	HSECFG_Capacitance(HSECap_20p);
    SetSysClock(SYSCLK_FREQ);
    /* 配置串口调试 */
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );

    // 使用LED时需关闭调试模式
    sys_safe_access_enable();
    R16_SLP_WAKE_CFG &= ~RB_DEBUG_EN;
    sys_safe_access_disable();

    //led clk
    GPIOA_ModeCfg( GPIO_Pin_15, GPIO_ModeOut_PP_5mA );

    //led data
    //LED 0
    GPIOA_ModeCfg( GPIO_Pin_14, GPIO_ModeOut_PP_5mA );
    //LED 1
    GPIOA_ModeCfg( GPIO_Pin_13, GPIO_ModeOut_PP_5mA );
    //LED 2
    GPIOA_ModeCfg( GPIO_Pin_12, GPIO_ModeOut_PP_5mA );
    //LED 3
    GPIOA_ModeCfg( GPIO_Pin_11, GPIO_ModeOut_PP_5mA );

    //配置分频和模式选择
    ch59x_led_controller_init(CH59X_LED_OUT_MODE_SINGLE, 128);

    //开始发送,后面再发送就在中断里面发送了
    LED_DMACfg(ENABLE, (uint32_t)&tx_MainData, (uint32_t)&tx_AuxData, 4, Mode_Els);

#if LSB_HSB   //LSB HSB
    R8_LED_CTRL_MOD ^= RB_LED_BIT_ORDER;
#endif

#if POLAR     //极性
    R8_LED_CTRL_MOD ^= RB_LED_OUT_POLAR;
#endif

    LED_ENABLE();
    PFIC_EnableIRQ(LED_IRQn);

    while(1);
}

/*********************************************************************
 * @fn      LED_IRQHandler
 *
 * @brief   LED中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void LED_IRQHandler(void)
{
    //清空中断标志
    if(LED_GetITFlag(RB_LED_IF_DMA_END))  // 获取中断标志
    {
       LED_ClearITFlag(RB_LED_IF_DMA_END); // 清除中断标志
       ch59x_led_controller_send((uint32_t)&tx_MainData, (uint32_t)&tx_AuxData, 4, Mode_Els);
    }
}
