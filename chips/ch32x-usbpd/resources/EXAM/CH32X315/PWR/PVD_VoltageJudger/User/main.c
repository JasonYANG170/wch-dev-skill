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
 * The code initializes the power voltage detector (PVD) and prints the voltage level.
 */
#include "debug.h"

/* Global define */

/* Global Variable */
volatile uint8_t Voltage_Flag = 0;

void PVD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PVD_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        Voltage_Flag = !PWR_GetFlagStatus(PWR_FLAG_PVDO);
        printf("Run at EXTI\r\n");
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}

/*********************************************************************
 * @fn       PVD_Init
 *
 * @brief    The function initializes the power voltage detector (PVD) by configuring the necessary registers and
 *          enabling the PVD interrupt.
 *
 * @return   none
 */

void PVD_Init(void)
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);

    EXTI_InitTypeDef EXIT_InitStructure = {0};
    EXIT_InitStructure.EXTI_Line = EXTI_Line16;
    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXIT_InitStructure);

    NVIC_EnableIRQ(PVD_IRQn);
    NVIC_SetPriority(PVD_IRQn, 2);

    PWR_PVDLevelConfig(PWR_PVDLevel_MODE2);
    PWR_PVDCmd(ENABLE);
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
    Delay_Ms(100);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    printf("Voltage judger TEST\r\n");

    PVD_Init();
    Voltage_Flag = !PWR_GetFlagStatus(PWR_FLAG_PVDO);

    while (1)
    {
        Delay_Ms(250);
        Voltage_Flag ? printf("**Voltage is higher than rising edge threshold\r\n") : printf("##Voltage is lower than falling edge threshold\r\n");
    }
}
