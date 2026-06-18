/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *low power, standby mode routine:
 *WKUP(PA0)
 *This routine demonstrates that WFI enters the standby mode, the rising edge of
 *the WKUP (PA0) pin exits the standby mode,Program reset after wake-up.
 *For the small package model of the chip, there are some pins that have not been 
 *led out compared to the largest package, or some pins that have been packaged but 
 *not used. These pins need to be set as anolog input to reduce current consumption. 
 */

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      All_GPIO_AIN_Init
 *
 * @brief   Before entering standby mode, all I/O pins not used for wake-up functions 
 *          shall be configured to analog input mode.
 * @return  none
 */
void All_GPIO_AIN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC |
                          RCC_PB2Periph_GPIOD | RCC_PB2Periph_GPIOE | RCC_PB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
    GPIO_Init( GPIOE, &GPIO_InitStructure );
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    Delay_Ms(500);	
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_PWR, ENABLE);

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        printf("\r\nStandby wake up reset\r\n");
    }
    else
    {
        printf("\r\nStandby....\r\n");
        /* Before entering standby mode, all I/O pins not used for wake-up functions 
         shall be configured to analog input mode. */
        All_GPIO_AIN_Init();
        PWR_WakeUpPinCmd(ENABLE);
        Delay_Ms(1000);
        PWR_EnterSTANDBYMode();
    }

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}
