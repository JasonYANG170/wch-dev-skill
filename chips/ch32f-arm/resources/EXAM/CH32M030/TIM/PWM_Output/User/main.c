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
 *PWM output routine:
 *TIM1_CH1(PB9)
 *This example demonstrates that the TIM_CH1(PB9) pin outputs PWM in Normal frequency Mode and
 *High speed frequency (144M).
 *
 */

#include "debug.h"

/* High Speed Mode Definition */
#define MODE1   1 //Normal speed frequency ,support frequency division.
#define MODE2   2 //High speed frequency (144M),Do not support frequency division.

/* Mode  Selection */
//#define MODE    MODE1
#define MODE    MODE2

/*********************************************************************
 * @fn      TIM1_PWMOut_Init
 *
 * @brief   Initializes TIM1 PWM output .
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOB|RCC_PB2Periph_TIM1, ENABLE );

#if(MODE==MODE2)
    TIM_CLKSourceConfig(TIM1, TIM_CLKSource_PLL);//HSCK_MODE
#endif

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;

	TIM_OC1Init( TIM1, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Enable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );
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
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

#if (MODE == MODE1)
	TIM1_PWMOut_Init(100-1,7200-1,50);
#else
    TIM1_PWMOut_Init(14400-1,1-1,7200);//do not support frequency division
#endif

	while(1);
}
