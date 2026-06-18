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
 *MCO pin clock output routine:
 *MCO(PA15)
 *This example demonstrates that the MCO(PA15) pin outputs the system clock.
 *
*/

#include "debug.h"

/* Global define */

/* Global Variable */

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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("RCC MCO\r\n");
	GPIO_MCOInit();
	RCC_MCOConfig(RCC_MCO_HSE);

	while (1)
		;
}
