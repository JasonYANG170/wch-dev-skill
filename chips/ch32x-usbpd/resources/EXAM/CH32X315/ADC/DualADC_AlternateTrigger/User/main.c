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
  Dual ADC alternate trigger sampling routine:
 ADC1 channel 1 (PA3), ADC2 channel 3 (PA5), the injection group triggers the conversion through timer 2.
*/

#include "debug.h"

/* Global define */

/* Global Variable */
u16 ADC_val[2];
/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void  ADC_Function_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure={0};
    GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC2, ENABLE );
    RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);

    ADC_InitStructure.ADC_Mode = ADC_Mode_AlterTrig;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;

    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_SampleTimeModeConfig(ADC1, ADC_Channel_1, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_1,ADC_SampleTime_CyclesMode7);


    ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_T2_CC1 );
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	
	
    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_InjectedSequencerLengthConfig(ADC2, 1);
    ADC_SampleTimeModeConfig(ADC2, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_3,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigInjectedConvConfig(ADC2,ADC_ExternalTrigInjecConv_None );

    ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
	
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
	Delay_Init();		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n",DBGMCU_GetCHIPID() );
	ADC_Function_Init();
	TIM2_PWM_In( 600, 48000-1, 300 );
	while(1)
	{
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC )){/*printf("wait\r\n");*/};
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
        ADC_val[0] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        printf("JADC1_ch2=%d\r\n",ADC_val[0]);
        Delay_Ms(100);

        while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_JEOC )){/*printf("wait\r\n");*/};
        ADC_ClearFlag(ADC2, ADC_FLAG_JEOC);
        ADC_val[1] = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
        printf("JADC2_ch3=%d\r\n",ADC_val[1]);
        Delay_Ms(100);
	}
}
