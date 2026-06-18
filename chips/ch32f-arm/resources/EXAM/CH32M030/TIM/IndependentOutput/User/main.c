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
 *TIM1 CH1N(CH6),CH2N(CH7),CH3N(CH8) independent Output routine
 *TIM1_CH6----PB8
 *TIM1_CH7----PB10
 *TIM1_CH8----PB12
 */

#include "debug.h"

/*********************************************************************
 * @fn      TIM1_IndependentOutput_Init
 *
 * @brief   Initializes TIM1 Independent Output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_IndependentOutput_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOB | RCC_PB2Periph_TIM1, ENABLE );

    /* TIM1_CH1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH8 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /*only support TIM_CounterMode_Up when in independent mode*/
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;//CH1\CH2\CH3 duty cycle
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure );
    TIM_OC2Init( TIM1, &TIM_OCInitStructure );
    TIM_OC3Init( TIM1, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_OC2PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_ARRPreloadConfig( TIM1, ENABLE );

    TIM_SetCompare6(TIM1, 10);//CH6/CH1N duty cycle
    TIM_SetCompare7(TIM1, 20);//CH7/CH2N duty cycle
    TIM_SetCompare8(TIM1, 30);//CH8/CH3N duty cycle

    TIM_IndependentOutput_ChannelxN_Cmd(TIM1, TIM_Channel_1N, ENABLE);
    TIM_IndependentOutput_ChannelxN_Cmd(TIM1, TIM_Channel_2N, ENABLE);
    TIM_IndependentOutput_ChannelxN_Cmd(TIM1, TIM_Channel_3N, ENABLE);

    TIM_ChannelxPWMModeConfig(TIM1, TIM_Channel_1N, TIM_OCMode_PWM1);
    TIM_ChannelxPWMModeConfig(TIM1, TIM_Channel_2N, TIM_OCMode_PWM2);
    TIM_ChannelxPWMModeConfig(TIM1, TIM_Channel_3N, TIM_OCMode_PWM1);

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
    TIM1_IndependentOutput_Init(100-1, 7200-1, 50);

    while(1);
}
