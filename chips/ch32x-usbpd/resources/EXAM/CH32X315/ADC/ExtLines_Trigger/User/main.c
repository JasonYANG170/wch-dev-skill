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
*ADC channel 2 (PA4) - injection group channel, external trigger pin (PA15) high level triggers EXTI line 15 event,
*In this mode, an ADC conversion is triggered by an event on EXTI line 15, and a JEOC interrupt is generated after
*the conversion is completed.
*
*/


#include "debug.h"

/* Global define */

/* Global Variable */
u16 Adc_Val[2];
u32 temp;

void ADC1_2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));

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

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div8,RCC_PPRE2_Div1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_InjectedSequencerLengthConfig(ADC1, 1);

	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode7);
	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_Ext_IT15 );
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);



    NVIC_EnableIRQ(ADC1_IRQn);
	ADC_ITConfig( ADC1, ADC_IT_JEOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	
}
/*********************************************************************
 * @fn      EXTI_Event_Init
 *
 * @brief   Initializes EXTI.
 *
 * @return  none
 */
void EXTI_Event_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure={0};
	GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA, ENABLE );
	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO, ENABLE );

	GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource15 );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
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
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n",DBGMCU_GetCHIPID() );
	ADC_Function_Init();
	EXTI_Event_Init();

	while(1);
}
void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_IRQHandler()
{
	u16 ADC_val;

	if(ADC_GetITStatus( ADC1, ADC_IT_JEOC))
	{
		ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
#if 1
		printf("ADC Extline trigger conversion...\r\n");
		printf( "JADC%04d\r\n", ADC_val);
#endif
	}

	ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
}
