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
*External lines trigger ADC conversion routine:
*ADC1 channel 2 (PA4) ADC2 channel 2 (PC8) ADC3 channel 2 (PD8)- Ing group channel, TIM2-CC2 triggered,
*In this mode, an ADC conversion is triggered by an event TIM2-CC2 , and ADC1 convent then next  triggered ADC3 convent  then next  triggered ADC2 convent
*
*/


#include "debug.h"



void ADC1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure={0};
	GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC|RCC_HB2Periph_GPIOD, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC2, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC3, ENABLE );
	RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div4,RCC_PPRE2_Div2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);
	ADC_DeInit(ADC3);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.INJENUM = ADC_INJE_NUM3;
	
	ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedChannelCascadeCmd(ADC1,ENABLE);  
    ADC_CascadeContsFrontConfig(ADC1,ADC_CONTS_Front_ADC2); 
	ADC_INJESEQConfig(ADC1,ADC_INJE_SEQ2);
	ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_None); 
    ADC_InjectedChannelCascadeDLYConfig(ADC1, 0x00);

    NVIC_EnableIRQ(ADC1_IRQn);
	ADC_ITConfig( ADC1, ADC_IT_JEOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	

    ADC_Init(ADC2, &ADC_InitStructure);

	ADC_InjectedSequencerLengthConfig(ADC2, 1);
	ADC_SampleTimeModeConfig(ADC2, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC2, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
	ADC_ExternalTrigInjectedConvConfig(ADC2,ADC_ExternalTrigInjecConv_T2_CC1 );
	ADC_ExternalTrigInjectedConvCmd(ADC2, ENABLE);

	ADC_INJESEQConfig(ADC2,ADC_INJE_SEQ1);
    NVIC_EnableIRQ(ADC2_IRQn);
    ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));


	ADC_Init(ADC3, &ADC_InitStructure);

    ADC_InjectedChannelCascadeCmd(ADC3,ENABLE);  
    ADC_CascadeContsFrontConfig(ADC3,ADC_CONTS_Front_ADC1); 
	ADC_INJESEQConfig(ADC3,ADC_INJE_SEQ3);
	ADC_InjectedSequencerLengthConfig(ADC3, 1);
	ADC_SampleTimeModeConfig(ADC3, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC3, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC3, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_None); 	
	ADC_ExternalTrigInjectedConvCmd(ADC3, ENABLE);
    NVIC_EnableIRQ(ADC3_IRQn);
    ADC_ITConfig(ADC3, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC3, ENABLE);

    ADC_BufferCmd(ADC3, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC3);
    while(ADC_GetResetCalibrationStatus(ADC3));
    ADC_StartCalibration(ADC3);
    while(ADC_GetCalibrationStatus(ADC3));
}
/*********************************************************************
 * @fn      TIM2_PWM_In
 *
 * @brief   TIM2,PWM input mode init,Only CH1 and CH2 channels can be used.
 *
 * @param   arr - Auto reload values
 *          psc - Prescaler value
 *          ccp - Preload value
 *
 * @return  none
 */
void TIM2_PWM_In( u16 arr, u16 psc, u16 ccp )
{
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_HB1PeriphClockCmd( RCC_HB1Periph_TIM2, ENABLE);


    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM2, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Disable );

    TIM_ARRPreloadConfig( TIM2, ENABLE );

    TIM_SelectOutputTrigger( TIM2, TIM_TRGOSource_Update );

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
	USART_Printf_Init(115200);
	SystemCoreClockUpdate();
	printf("SystemClk:%d\r\n", SystemCoreClock);
	printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
	ADC_Function_Init();
	TIM2_PWM_In( 6000, 48000-1, 3000 );

	while(1);
}

void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   ADC1 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_IRQHandler()
{
	u16 ADC_val1;

	if(ADC_GetITStatus( ADC1, ADC_IT_JEOC))
	{
		ADC_val1 =  ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

#if 1

		printf( "ADC1 %04d \r\n", ADC_val1);
#endif
	}

	ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
	
}

void ADC2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   ADC1 Interrupt Service Function.
 *
 * @return  none
 */
void ADC2_IRQHandler()
{
	u16 ADC_val2;

	if(ADC_GetITStatus( ADC2, ADC_IT_JEOC))
	{
		ADC_val2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
#if 1

		printf( "ADC2 %04d\r\n", ADC_val2);
#endif
	}

	ADC_ClearITPendingBit( ADC2, ADC_IT_JEOC);
	
}

void ADC3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   ADC1 Interrupt Service Function.
 *
 * @return  none
 */
void ADC3_IRQHandler()
{
	u16 ADC_val3;

	if(ADC_GetITStatus( ADC3, ADC_IT_JEOC))
	{
		ADC_val3 = ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_1);

#if 1

		printf( "ADC3 %04d \r\n", ADC_val3);
#endif
	}

	ADC_ClearITPendingBit( ADC3, ADC_IT_JEOC);
	
}
