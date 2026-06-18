/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2025/01/04
 * Description        : 低功耗按键检测例程
 *                      注意:按键扫描例程默认使用了PA8，因此RXD不进行配置
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"
#include "CH59x_keyscan.h"

uint32_t KeyValue_CurCnt, KeyValue_AfterCnt = 0;

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

    HSECFG_Capacitance(HSECap_20p);
    SetSysClock(SYSCLK_FREQ);
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);

    /* 配置串口调试 */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

    /* 按键扫描配置 */
    KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);

    /* 按键按下中断演示 */
    KeyScan_ITCfg(ENABLE, RB_KEY_PRESSED_IE | RB_KEY_RELEASED_IE);
    PFIC_EnableIRQ(KEYSCAN_IRQn);

    /* 按键按下唤醒睡眠演示 */
#if 1
    DelayMs(1000);
    KeyPress_Wake(ENABLE);
    PRINT("sleep mode sleep \n");
    DelayMs(2);
    LowPower_Sleep( RB_PWR_RAM24K | RB_PWR_RAM8K | RB_XT_PRE_EN );
    PRINT("wake.. \n");
#endif

    while(1);
}

/*********************************************************************
 * @fn      KEYSCAN_IRQHandler
 *
 * @brief   KEYSCAN_IRQHandler
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void KEYSCAN_IRQHandler( void )
{
    if(KeyScan_GetITFlag(RB_KEY_PRESSED_IF))  // 获取中断标志
    {
        KeyValue_CurCnt = KeyValue;
        if(KeyValue_CurCnt != KeyValue_AfterCnt)
        {
            PRINT("KeyPressed\n");
            PRINT("KEY:%x\n",KeyValue_CurCnt);
            KeyValue_AfterCnt = KeyValue_CurCnt;
        }
        KeyScan_ClearITFlag(RB_KEY_PRESSED_IF); // 清除中断标志
    }
    if( KeyScan_GetITFlag(RB_KEY_RELEASED_IF) )
    {
        PRINT("KeyReleased\n");
        PRINT("KEY:%x\n",KeyValue_CurCnt);
        KeyValue_CurCnt = KeyValue;
        KeyValue_AfterCnt = KeyValue_CurCnt;
        KeyScan_ClearITFlag(RB_KEY_RELEASED_IF);
    }
}


