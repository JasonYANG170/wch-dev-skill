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
/**
 * @note
 * The code initializes the power voltage detector (PVD) and enters low-power stop mode, waking up when
 * the PVD interrupt is triggered.
 */

#include "debug.h"

/* Global define */

/* Global Variable */

void PVD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PVD_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        SystemInit();
        USART_Printf_Init(115200);
        printf("Run at EXTI\r\n");
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}

/*********************************************************************
 * @fn       PVD_Init
 *
 * @brief    The function initializes the power voltage detector (PVD) by 
 *           configuring the necessary registers and enabling the PVD interrupt.
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
    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_Init(&EXIT_InitStructure);

    NVIC_EnableIRQ(PVD_IRQn);
    NVIC_SetPriority(PVD_IRQn, 2);

    PWR_PVDLevelConfig(PWR_PVDLevel_MODE2);

    PWR_PVDCmd(ENABLE);
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
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);

    printf("\r\nstop mode...\r\n");
    Delay_Ms(100);
    All_GPIO_IPD_Init();
    PVD_Init();
    
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    printf("Wake up!!\r\n");

    while (1)
    {
        Delay_Ms(1000);
        printf("run in main\r\n");
    }
}
