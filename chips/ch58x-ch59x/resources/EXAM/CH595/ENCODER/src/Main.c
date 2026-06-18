/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2025/01/04
 * Description        : 编码器示例
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

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
    uint16_t i;
    uint8_t  s;

    // KeyScan运行在低功耗模式（睡眠唤醒）时需要调节uldo，默认值+3档。
    KeyScanPowAdj();
    HSECFG_Capacitance(HSECap_20p);
    SetSysClock(SYSCLK_FREQ);

    /* 配置串口调试 */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

    /* ENC检测高电平信号，配置IO为下拉输入 */
#if 1
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PD);
    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PD);
    R16_PIN_ANALOG_IE |= RB_PIN_ENC;
#else
    GPIOB_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PD);
    GPIOB_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PD);
#endif

    /* ENC模式配置，设定计数终值为最大值，在T1,T2边沿计数模式 */
    ENC_Config(ENABLE, 0xFFFF, Mode_T1T2);

    /* 演示编码器中断 */
    ENC_ITCfg(ENABLE, RB_IE_DIR_INC);       // 使能前进中断
    ENC_ITCfg(ENABLE, RB_IE_DIR_DEC);       // 使能后退中断
    PFIC_EnableIRQ(ENCODER_IRQn);

    /* 演示编码器唤醒睡眠 */
#if 1
    DelayMs(1000);
    ENC_Wake(ENABLE);
    PRINT("sleep mode sleep \n");
    DelayMs(2);
    LowPower_Sleep( RB_PWR_RAM24K | RB_PWR_RAM8K);
    PRINT("wake.. \n");
#endif

    while(1)
    {
        DelayMs(300);
        PRINT("%d\n", ENC_GetCurrentDir);       // 获取编码器当前方向
        PRINT("cnt:%d\n",ENC_GetCurrentCount);  // 获取编码器当前计数值
    }

    while(1);
}

/*********************************************************************
 * @fn      ENCODER_IRQHandler
 *
 * @brief   Encoder中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void ENCODER_IRQHandler( void )
{
    if(ENC_GetITFlag(RB_IF_DIR_INC))
    {
        PRINT("INC\n");
        ENC_ClearITFlag(RB_IF_DIR_INC);
    }
    if(ENC_GetITFlag(RB_IF_DIR_DEC))
    {
        PRINT("DEC\n");
        ENC_ClearITFlag(RB_IF_DIR_DEC);
    }
}


