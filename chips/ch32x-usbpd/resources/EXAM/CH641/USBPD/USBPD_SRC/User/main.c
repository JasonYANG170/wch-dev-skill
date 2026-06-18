/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/03
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *
 * PD SRC Sample code
 *
 * This sample code may have compatibility issues and is for learning purposes only.
 *
 * Be sure to close the pull-down resistors on both CC wires  when using this Sample code!
 *
 * Select either CC2 or CC3 to modify the configuration in PD_Process.h
 *
 * If CC2 is selected, make sure the hardware circuit PB1 pin is not attached to 5.5K pull-down resistance
 *
 * The inability to control the VBUS voltage on the board may lead to some compatibility problems,
 * mainly manifested in the inability of some devices to complete the PD communication process.
 *
 */

#include "debug.h"
#include "PD_Process.h"

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
volatile UINT8  Tim_Ms_Cnt = 0x00;

/*********************************************************************
 * @fn      TIM1_Init
 *
 * @brief   Initialize TIM1
 *
 * @return  none
 */
void TIM1_Init( u16 arr, u16 psc )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
    NVIC_InitTypeDef NVIC_InitStructure={0};
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE );
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig( TIM1, TIM_IT_Update, ENABLE );
    TIM_Cmd( TIM1, ENABLE );
}


/*********************************************************************
 * @fn      EXTI_INIT
 *
 * @brief   Initializes EXTI2 collection.
 *
 * @return  none
 */
void EXTI_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    /* GPIOB.0 ----> EXTI_Line0 */
       GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
       EXTI_InitStructure.EXTI_Line = EXTI_Line0;
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
       EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;
       EXTI_Init(&EXTI_InitStructure);
#if(CC_2==CC2)
       /* GPIOB.1 ----> EXTI_Line1 */
             GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
             EXTI_InitStructure.EXTI_Line = EXTI_Line1;
             EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
             EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
             EXTI_InitStructure.EXTI_LineCmd = ENABLE;
             EXTI_Init(&EXTI_InitStructure);
#elif(CC_2==CC3)
    /* GPIOB.9 ----> EXTI_Line9 */
         GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
         EXTI_InitStructure.EXTI_Line = EXTI_Line9;
         EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
         EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
         EXTI_InitStructure.EXTI_LineCmd = ENABLE;
         EXTI_Init(&EXTI_InitStructure);
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig(GPIO_Remap_SDI_Disable, ENABLE);
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "PD SCR TEST\r\n" );
    PD_Init( );
    EXTI_INIT();
    TIM1_Init( 999, 48-1);
    while(1)
    {
        /* Get the calculated timing interval value */
        TIM_ITConfig( TIM1, TIM_IT_Update , DISABLE );
        Tmr_Ms_Dlt = Tim_Ms_Cnt - Tmr_Ms_Cnt_Last;
        Tmr_Ms_Cnt_Last = Tim_Ms_Cnt;
        TIM_ITConfig( TIM1, TIM_IT_Update , ENABLE );

        PD_Ctl.Det_Timer += Tmr_Ms_Dlt;
        if( PD_Ctl.Det_Timer > 4 )
        {
            PD_Ctl.Det_Timer = 0;
            PD_Det_Proc( );
        }
        PD_Main_Proc( );
    }
}

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 interrupt.
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void)
{
    if( TIM_GetITStatus( TIM1, TIM_IT_Update ) != RESET )
    {
        Tim_Ms_Cnt++;
        TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
    }
}


/*********************************************************************
 * @fn      EXTI7_0_IRQHandler
 *
 * @brief   This function handles EXTI0 and EXTI1 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
    {
        printf(" CC1 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
        NVIC_DisableIRQ(EXTI7_0_IRQn);
    }
#if(CC_2==CC2)
    if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
    {
        printf(" CC2 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line1);     /* Clear Flag */
        NVIC_DisableIRQ(EXTI7_0_IRQn);
    }
#elif(CC_2==CC3)
#endif
}

/*********************************************************************
 * @fn      EXTI15_8_IRQHandler
 *
 * @brief   This function handles EXTI9 Handler.
 *
 * @return  none
 */
void EXTI15_8_IRQHandler(void)
{
#if(CC_2==CC2)
#elif(CC_2==CC3)
    if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
    {
        printf(" CC3 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line9);     /* Clear Flag */
        NVIC_DisableIRQ(EXTI15_8_IRQn);
    }
#endif
}
