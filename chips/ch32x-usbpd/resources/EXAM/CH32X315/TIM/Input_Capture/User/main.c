/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
Input capture routine:
 TIM2_CH1(PA4)
 This example demonstrates the TIM_CH1(PA4) pin floating input, which detects an edge
  transition to trigger a TIM1 capture interrupt,
 The rising edge triggers the TIM_IT_CC1 interrupt, and the falling edge triggers the
  TIM_IT_CC2 interrupt.
 
*/

#include "debug.h"

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 Interrupt exception.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{

	if( TIM_GetITStatus( TIM2, TIM_IT_CC1 ) != RESET )
	{
		printf( "CH1_Val:%d\r\n", TIM_GetCapture1( TIM2 ) );
	}

	if( TIM_GetITStatus( TIM2, TIM_IT_CC2 ) != RESET )
	{
		printf( "CH2_Val:%d\r\n", TIM_GetCapture2( TIM2 ) );
	}

	TIM_ClearITPendingBit( TIM2, TIM_IT_CC1 | TIM_IT_CC2 );
}


/*********************************************************************
 * @fn      Input_Capture_Init
 *
 * @brief   Initializes TIM2 input capture.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void Input_Capture_Init( u16 arr, u16 psc )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_ICInitTypeDef TIM_ICInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOB|RCC_HB2Periph_AFIO, ENABLE );
    RCC_HB1PeriphClockCmd( RCC_HB1Periph_TIM2, ENABLE );
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

	TIM_PWMIConfig( TIM2, &TIM_ICInitStructure );

	NVIC_SetPriority(TIM2_IRQn,0);
	NVIC_EnableIRQ(TIM2_IRQn);

	TIM_ITConfig( TIM2, TIM_IT_CC1 | TIM_IT_CC2, ENABLE );

	TIM_SelectInputTrigger( TIM2, TIM_TS_TI1FP1 );
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_Reset );
	TIM_SelectMasterSlaveMode( TIM2, TIM_MasterSlaveMode_Enable );
	TIM_Cmd( TIM2, ENABLE );
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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	printf("Input_Capture\r\n");
    Input_Capture_Init( 0xFFFF, 24000-1 );
	while (1)
		;
}
