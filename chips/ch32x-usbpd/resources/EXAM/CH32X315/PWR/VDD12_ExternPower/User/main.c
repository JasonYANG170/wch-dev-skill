/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/01/12
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 * This example demonstrates the method to reduce the internal VDD12 voltage when 
 * the chip's VDD12 power supply is externally provided.
 */
 
#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint32_t tmp = 0;
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    Delay_Ms(500);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    /*  The current in the VDD12 power domain of the chip is relatively high. 
	 If the VDD12 power supply is externally provided and the internal LDO is turned off, 
	 the following function can be executed to reduce the chip's power consumption and 
	 alleviate overheating. Note that this operation should only be performed when the 
	 external 1.2V power supply has stabilized.*/
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    tmp = PWR->CTLR;
    tmp &= ~(0x3f << 8);
    tmp |= (0x24 << 8);
    PWR->CTLR = tmp;

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}

