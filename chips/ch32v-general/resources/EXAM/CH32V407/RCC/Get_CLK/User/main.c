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
  *Get system-HCLK-AHB1-AHB2 clock routine:
  *MCO(PA8)
 *   This example demonstrates MCO(PA8) pin output system clock and get clock;
 *   -RCC_GetClocksFreq() function to get systemclk-HCLK-AHB1-AHB2 clock
 *   -SystemCoreClockUpdate() function to get HCLK clock
 *
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
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    RCC_ClocksTypeDef RCC_ClocksStatus={0};

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    printf("SYSCLK_Frequency-%d\r\n", RCC_ClocksStatus.SYSCLK_Frequency);
    printf("HCLK_Frequency-%d\r\n", RCC_ClocksStatus.HCLK_Frequency);
    printf("PCLK1_Frequency-%d\r\n", RCC_ClocksStatus.PCLK1_Frequency);
    printf("PCLK2_Frequency-%d\r\n", RCC_ClocksStatus.PCLK2_Frequency);

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* MCO Output GPIOA 8 */
    RCC_MCOConfig( RCC_MCO_SYSCLK );

    while(1)
    {
    }
}
