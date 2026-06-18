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
 *Low power, standby mode routine:
 *This routine demonstrates executing WFI\WFE to enter standby mode, inputting a low level on
 *the PA7 pin to trigger the external interrupt EXTI_Line7,or exiting standby mode through
 *automatic wake-up, Program execution continues after wake-up.
 *
 */

#include "debug.h"

/* Global define */

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

#define  Enter_MODE  Enter_WFI
//#define  Enter_MODE  Enter_WFE

#define WAKEUP_MODE_NO_AWU      0
#define WAKEUP_MODE_AWU         1

//#define STANDBY_WAKEUP_MODE     WAKEUP_MODE_NO_AWU
#define STANDBY_WAKEUP_MODE     WAKEUP_MODE_AWU

/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief   Initializes EXTI collection.
 *
 * @return  none
 */
void EXTI_INT_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

#if (STANDBY_WAKEUP_MODE == WAKEUP_MODE_NO_AWU)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO |RCC_PB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;

#else
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
#endif
#if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


#if(STANDBY_WAKEUP_MODE == WAKEUP_MODE_NO_AWU)
    #if(Enter_MODE == Enter_WFI)
    NVIC_SetPriority(EXTI7_0_IRQn, 1<<6);
    NVIC_EnableIRQ(EXTI7_0_IRQn);
    #endif
#else
    #if(Enter_MODE == Enter_WFI)
    NVIC_SetPriority(AWU_IRQn, 1<<6);
    NVIC_EnableIRQ(AWU_IRQn);
    #endif
#endif
}

/*********************************************************************
 * @fn      GPIO_CFG
 *
 * @brief   To reduce power consumption, it is recommended to configure unused GPIOs that support
 *          pull-up input as pull-up input mode and other GPIOs as analog input mode.
 *
 * @return  none
 */
void GPIO_CFG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SDI_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Enable PB0 and PB1 pull-up resistors */
    EXTEN->EXTEN_KEYR = EXTEN_KEY1;
    EXTEN->EXTEN_KEYR = EXTEN_KEY2;
    EXTEN->EXTEN_CTLR1 |= ( EXTEN_UDP_PUE | EXTEN_UDM_PUE );
    EXTEN->EXTEN_CTLR1 &= ~( EXTEN_UDP_PDE | EXTEN_UDP_PDE );
    EXTEN->EXTEN_CTLR1 &= ~( EXTEN_UDP_AE | EXTEN_UDM_AE | EXTEN_UDM_BUFOE | EXTEN_UDP_BUFOE );
    EXTEN->EXTEN_CTLR1 |= ( EXTEN_UDP_DAC_0 | EXTEN_UDM_DAC_0 );

    /* Configure PB2-PB6 as pull-up input */
    GPIO_InitStructure.GPIO_Pin = 0x7C;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure PB7-PB15 as analog input */
    GPIO_InitStructure.GPIO_Pin = 0xFF80;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure PC0-PC4 and PC6~PC15 as pull-up input */
    GPIO_InitStructure.GPIO_Pin = 0xFFDF;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Configure PC5 as analog input */
    GPIO_InitStructure.GPIO_Pin = 0x20;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
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
    GPIO_CFG();
    EXTI_INT_INIT();

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

#if(STANDBY_WAKEUP_MODE == WAKEUP_MODE_NO_AWU)
    printf("Standby Mode NO AWU Test\r\n");
#else
    printf("Standby Mode AWU Test\r\n");
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_PWR, ENABLE);
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    PWR_AWU_SetWindowValue(0xF);
    PWR_AutoWakeUpCmd(ENABLE);
#endif
#if(Enter_MODE == Enter_WFI)
    printf("Start Enter_WFI\r\n");
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);
#else
    printf("Start Enter_WFE\r\n");
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFE);
    SystemInit();
#endif

    printf("\r\nstandby wake up!!! \r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}

#if(Enter_MODE == Enter_WFI)

#if(STANDBY_WAKEUP_MODE == WAKEUP_MODE_AWU)
void AWU_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      AWU_IRQHandler
 *
 * @brief   This function handles AWU Handler.
 *
 * @return  none
 */
void AWU_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line17)!=RESET)
    {
        SystemInit();
        printf("\r\nAWU Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line17);     /* Clear Flag */
        NVIC_DisableIRQ(AWU_IRQn);
    }
}

#else
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      EXTI7_0_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
    {
        SystemInit();
        printf("EXTI Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line7);     /* Clear Flag */
    }
}
#endif

#endif
