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
 * low power, sleep mode routine:
 * EXTI_Line0(PB0)
 * This routine demonstrates WFI\WFE enters sleep mode, PB0 pin input low level triggers 
 * EXTI_Line0 to exit sleep mode,Program execution continues after wake-up.
 * For the small package model of the chip, there are some pins that have not been 
 * led out compared to the largest package, or some pins that have been packaged but 
 * not used. These pins need to be set as pull-down inputs to reduce current consumption. 
 */

#include "debug.h"

/* Global define */

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

#define  Enter_MODE  Enter_WFE
// #define  Enter_MODE  Enter_WFI

/* Global Variable */

/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief   Initializes EXTI0 collection.
 *
 * @return  none
 */
void EXTI0_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* GPIOA.0 ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    #if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    #else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    #endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

#if(Enter_MODE == Enter_WFI)
    NVIC_EnableIRQ(EXTI7_0_IRQn);
    NVIC_SetPriority(EXTI7_0_IRQn, 2);
#endif
}

/*********************************************************************
 * @fn      All_GPIO_IPD_Init
 *
 * @brief   To reduce power consumption, unused I/O is configured in pull-down input mode.
 *
 * @return  none
 */
void All_GPIO_IPD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |
                          RCC_HB2Periph_GPIOD | RCC_HB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
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
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("\r\nsleep mode...\r\n");
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    All_GPIO_IPD_Init();
    EXTI0_INT_INIT();
#if(Enter_MODE == Enter_WFI)
    __WFI();
#else
    __WFE();
#endif
    USART_Printf_Init(115200);
    printf("\r\n########## \r\n");

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}

#if(Enter_MODE == Enter_WFI)
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
  {
    USART_Printf_Init(115200);
    printf("EXTI0 Wake up\r\n");
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
  }
}
#endif