/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/08
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 * low power, stop mode routine:
 * EXTI_Line0(PB0)
 * This routine demonstrates WFI\WFE enters stop mode, PB0 pin input low level triggers 
 * EXTI_Line0 to exit stop mode,Program execution continues after wake-up.
 * For the small package model of the chip, there are some pins that have not been 
 * led out compared to the largest package, or some pins that have been packaged but 
 * not used. These pins need to be set as pull-down inputs to reduce current consumption. 
 */

#include "debug.h"

/* Global define */

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

/* STOP Mode Definition */
#define  STOP_ON   0
#define  STOP_LP   1

// #define  Enter_MODE  Enter_WFI
#define  Enter_MODE  Enter_WFE

// #define  STOP_MODE  STOP_ON
#define  STOP_MODE  STOP_LP

/* Global Variable */

/*********************************************************************
 * @fn      EXTI0_INT_INIT
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

    /* GPIOB.0 ----> EXTI_Line0 */
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
    uint32_t tmp = 0;
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(1000);
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("\r\nstop mode...\r\n");
    All_GPIO_IPD_Init();
    EXTI0_INT_INIT();
    Delay_Ms(500);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    /* Enable the power supply regulation bit preset function, 
    and then adjust the LDO_VDD12A/LDO_VDDK configuration values 
    to keep the LDO in a low-voltage state during the stop mode, 
    thereby reducing the power consumption current of the chip. 
        Note: If the preset function is turned off after waking up, 
    it is necessary to ensure that LDO_VDD12A/LDO_VDDK have been 
    switched to the reset value before this.*/
    PWR_PLANCmd(ENABLE);
    tmp = PWR->CTLR;
    tmp &= ~(0x3f << 8);
    tmp |= (0x1b << 8);
    PWR->CTLR = tmp;

#if(Enter_MODE == Enter_WFI)
    #if(STOP_MODE == STOP_ON)
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    #elif(STOP_MODE == STOP_LP)
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    #endif
#else
    #if(STOP_MODE == STOP_ON)
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
    #elif(STOP_MODE == STOP_LP)
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
    #endif
    SystemInit();
    USART_Printf_Init(115200);
#endif
    printf("\r\n##########\r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("run in main... \r\n");
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
    SystemInit();
    USART_Printf_Init(115200);	
    printf("EXTI0 Wake up\r\n");
    EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
  }
}
#endif
