/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/10
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *Delay Instruction Routine:
 *Use .
 *
 */

#include "debug.h"

/* Global define */

/* Global Variable */
uint32_t LineAccur;

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
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("Accuracy delay test\r\n");

    /* Delay instruction overtime test */
    NVIC_EnableIRQ(Software_IRQn);
    __set_DLY(0, 0, ENABLE, ENABLE, 0);
    // Delay instruction, if LOAD instruction dosen't match in 1 tick, soft interrupt will be generated
    LineAccur = __LINE__ + 1;
    DelayIntrisic(DLY_SRC_HCLK, DLY_Match_Load, 0, 1);
    __NOP();
    asm volatile("lw t0, 0(x0)":::"t0");

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Infinite loop, delay instruction accuracy test, the PA1 will be toggled at 100 ticks accuracily*/
    while (1)
    {
        /* Perform intrinsic delay operation */
        DelayIntrisic(DLY_SRC_HCLK, DLY_Match_Delay, 0, 100);
        /* Toggle GPIOA pin 1 output state */
        GPIOA->OUTDR ^= GPIO_Pin_1;
    }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void SW_Handler()
{
    printf("Soft interrupt -- Source line %d\r\n", LineAccur);
    __clear_DLY_OV();
}
