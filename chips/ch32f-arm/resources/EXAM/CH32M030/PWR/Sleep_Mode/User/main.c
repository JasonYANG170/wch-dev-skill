/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *low power, sleep mode routine:
 *EXTI_Line7(PA7)
 *This routine demonstrates that WFI\WFE enters the sleep mode, and the PA7 pin input
 *low level triggers the external interrupt EXTI_Line7 to exit the sleep mode,
 *Program execution continues after wake-up.
 *
 */

#include "debug.h"

/* Global define */

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

#define  Enter_MODE  Enter_WFI
//#define  Enter_MODE  Enter_WFE

/* Global Variable */

/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief   Initializes EXTI collection.
 *
 * @return  none
 */
void EXTI_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO | RCC_PB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOA.7 ----> EXTI_Line7 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
#if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

#if(Enter_MODE == Enter_WFI)
    NVIC_SetPriority(EXTI7_0_IRQn, 0);
    NVIC_EnableIRQ(EXTI7_0_IRQn);
#endif
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
    Delay_Ms(1000);
    Delay_Ms(1000);
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    printf("Sleep Mode Test\r\n");
    EXTI_INT_INIT();

    printf("\r\n ********** \r\n");
#if(Enter_MODE == Enter_WFI)
    __WFI();
#else
    __WFE();
#endif
    printf("\r\n ########## \r\n");

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}

#if(Enter_MODE == Enter_WFI)
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      EXTI7_0_IRQHandler
 *
 * @brief   This function handles EXTI7 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
    {
        printf("EXTI Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line7);     /* Clear Flag */
    }
}
#endif
