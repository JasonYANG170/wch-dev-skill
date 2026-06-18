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
 *low power, sleep mode routine:
 *EXTI_Line2(PA2)
 *This routine demonstrates that WFI enters the sleep mode, and the PA2 pin input
 *low level triggers the external interrupt EXTI_Line2 to exit the sleep mode,
 *Program execution continues after wake-up.
 *
 */

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      EXTI2_INT_INIT
 *
 * @brief   Initializes EXTI2 collection.
 *
 * @return  none
 */
void EXTI2_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOA.2 ----> EXTI_Line2 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(1000);
    Delay_Ms(1000);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    printf("Sleep Mode Test\r\n");
    EXTI2_INT_INIT();

    printf("\r\n ********** \r\n");
    __WFI();
    printf("\r\n ########## \r\n");

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}


void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      EXTI2_IRQHandler
 *
 * @brief   This function handles EXTI2 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
    {
        printf("EXTI2 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line2);     /* Clear Flag */
    }
}

