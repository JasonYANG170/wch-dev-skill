/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 Get system-HCLK-Core-ADCCLK clock routine:
 MCO(PA15)
    This example demonstrates MCO(PA15) pin output system clock and get clock;
    -RCC_GetClocksFreq() function to get systemclk-HCLK-AHB1-AHB2 clock
    -SystemCoreClockUpdate() function to get HCLK clock
*/

#include "debug.h"

/*********************************************************************
 * @fn      GPIO_MCOInit
 *
 * @brief   Initializes RCC_MCO
 *
 * @return  none
 */
void GPIO_MCOInit()
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_AFIO, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF0);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_MCOConfig(RCC_MCO_HSI);
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
	RCC_ClocksTypeDef RCC_ClocksStatus={0};
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    RCC_GetClocksFreq(&RCC_ClocksStatus);
	printf("SYSCLK_Frequency-%d\r\n", RCC_ClocksStatus.SYSCLK_Frequency);
    printf("HCLK_Frequency-%d\r\n", RCC_ClocksStatus.HCLK_Frequency);
    printf("Core_Frequency-%d\r\n", RCC_ClocksStatus.Core_Frequency);
	printf("ADCCLK_Frequency-%d\r\n", RCC_ClocksStatus.ADCCLK_Frequency);
    GPIO_MCOInit();

	while (1)
		;
}
