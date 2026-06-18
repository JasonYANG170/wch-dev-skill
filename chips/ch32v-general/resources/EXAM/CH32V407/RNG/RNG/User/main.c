/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *@Note
 * Random number generator routine:
 *Generate a 32-bit random number and print it through the serial port (PA9).
 *
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
    u32 random = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    RCC_HBPeriphClockCmd(RCC_HBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    while(1)
    {
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET)
        {
        }
        random=RNG_GetRandomNumber();
        printf("random:0x%08x\r\n",random);\
        Delay_Ms(500);
    }

}
