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
 *Bothedge capture routines:
 *Routine enables capture level indication function(CHxCVR 16_bit).
 *TIM1_CH1(PC3)
 *TIM1_CH2(PC4)
 */

#include "debug.h"

/* Global Variable */
#define  MODE0  0
#define  MODE1  1

#define MODE  MODE0
//#define MODE  MODE1

void TIM1_TRG_COM_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles TIM1_TRG_COM.
 *
 * @return  none
 */
void TIM1_TRG_COM_IRQHandler(void)
{
    if( TIM_GetITStatus( TIM1, TIM_IT_Trigger ) != RESET )
    {
        printf( "CH1_Val:%08X\r\n", TIM1->CH1CVR );
    }
    TIM_ClearITPendingBit( TIM1, TIM_IT_Trigger);
}

/*********************************************************************
 * @fn      TIM1_CC_IRQHandler
 *
 * @brief   This function handles TIM1_CC.
 *
 * @return  none
 */
void TIM1_CC_IRQHandler(void)
{
    if( TIM_GetITStatus( TIM1, TIM_IT_CC2 ) != RESET )
    {
        printf("CH2CVR:%08X\r\n",TIM1->CH2CVR);
    }
    TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
}

/*********************************************************************
 * @fn      CH1_Capture_Bothedge_Init
 *
 * @brief   Initializes CH1 Capture Bothedge.
 *
 * @param   arr - the period value.
 *                 psc - the prescaler value.
 *
 * @return  none
 */
void CH1_Capture_Bothedge_Init( u16 arr, u16 psc )
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_ICInitTypeDef TIM_ICInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOC|RCC_PB2Periph_TIM1|RCC_PB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig(GPIO_PartialRemap3_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;

    TIM_ICInit(TIM1, &TIM_ICInitStructure );

    NVIC_SetPriority(TIM1_TRG_COM_IRQn, 0);
    NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);
    TIM_ITConfig( TIM1, TIM_IT_Trigger, ENABLE );

    TIM_IndicateCaptureLevelCmd(TIM1, ENABLE );//ENABLE CAPLVL

    TIM_SelectInputTrigger( TIM1, TIM_TS_TI1F_ED );
    TIM_SelectSlaveMode( TIM1, TIM_SlaveMode_Reset );
    TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable );
    TIM_Cmd( TIM1, ENABLE );
}

/*********************************************************************
 * @fn      CH2_Capture_Bothedge_Init
 *
 * @brief   Initializes CH2 Capture Bothedge.
 *
 * @param   arr - the period value.
 *                 psc - the prescaler value.
 *
 * @return  none
 */
void CH2_Capture_Bothedge_Init( u16 arr, u16 psc )
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_ICInitTypeDef TIM_ICInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOC|RCC_PB2Periph_TIM1|RCC_PB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig(GPIO_PartialRemap3_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;
    TIM_ICInit(TIM1, &TIM_ICInitStructure );

    TIM_IndicateCaptureLevelCmd(TIM1, ENABLE);//ENABLE CAPLVL

    TIM_ITConfig( TIM1, TIM_IT_CC2, ENABLE );
    NVIC_SetPriority(TIM1_CC_IRQn, 0);
    NVIC_EnableIRQ(TIM1_CC_IRQn);

    TIM_SelectInputTrigger( TIM1, TIM_TS_TI1F_ED );
    TIM_IC2BothEdge_Cmd(TIM1, ENABLE);

    TIM_Cmd( TIM1, ENABLE );
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
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

#if (MODE == MODE0)
    CH1_Capture_Bothedge_Init( 0xFFFF, 48000-1 );
#else
    CH2_Capture_Bothedge_Init( 0xFFFF, 48000-1 );
#endif

    while(1);
}
