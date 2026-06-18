/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/22
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *MCO pin clock output routine:
 *MCO(PB7)
 *This example demonstrates the MCO(PB7) pin output clock
 *
 */

#include "debug.h"

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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    USART_Printf_Init(115200);
    SystemCoreClockUpdate();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* MCO Output GPIOC 4 */
    RCC_MCOConfig( RCC_MCO_SYSCLK );
//    RCC_MCOConfig( RCC_MCO_HSI );
//    RCC_MCOConfig( RCC_MCO_PLLCLK );

    while(1);
}
