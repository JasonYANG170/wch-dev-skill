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
*ADC1 channel 2 (PA4) ADC2 channel 2 (PC8) - Reg group channel, external trigger pin (PA12) high level triggers EXTI line 12event,
*In this mode, an ADC conversion is triggered by an event on EXTI line 12, and ADC2-ADC3-ADC4-ADC1 after 1/4 (Sample+Conv),this routine
*can enhance sample 4 times only chage cascade delay time to 1/4 (Sample+Conv).
*
*/


#include "debug.h"

/* Global define */


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

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC|RCC_HB2Periph_GPIOB|RCC_HB2Periph_GPIOD, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_ADC2, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC3|RCC_HB2Periph_ADC4, ENABLE );
	RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);
	ADC_DeInit(ADC3);
	ADC_DeInit(ADC4);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_Ext_IT12;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelCascadeCmd(ADC1,ENABLE);
    ADC_CascadeContsFrontConfig(ADC1,ADC_CONTS_Front_ADC4); 
	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode0);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);


    NVIC_EnableIRQ(ADC1_IRQn);
	ADC_ITConfig( ADC1, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	
    ADC_Init(ADC2, &ADC_InitStructure);
	ADC_RegularChannelCascadeDLYConfig(ADC2, 0x04);

	ADC_SampleTimeModeConfig(ADC2, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_2,ADC_SampleTime_CyclesMode0);
	ADC_ExternalTrigConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));

	ADC_Init(ADC3, &ADC_InitStructure);

    ADC_RegularChannelCascadeCmd(ADC3,ENABLE);
    ADC_CascadeContsFrontConfig(ADC3,ADC_CONTS_Front_ADC2); 
	ADC_RegularChannelCascadeDLYConfig(ADC3, 0x04);
	ADC_SampleTimeModeConfig(ADC3, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC3, ADC_Channel_2,ADC_SampleTime_CyclesMode0);
	ADC_ExternalTrigConvCmd(ADC3, ENABLE);

	ADC_Cmd(ADC3, ENABLE);

    ADC_BufferCmd(ADC3, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC3);
    while(ADC_GetResetCalibrationStatus(ADC3));
    ADC_StartCalibration(ADC3);
    while(ADC_GetCalibrationStatus(ADC3));

	ADC_Init(ADC4, &ADC_InitStructure);

    ADC_RegularChannelCascadeCmd(ADC4,ENABLE);
    ADC_CascadeContsFrontConfig(ADC4,ADC_CONTS_Front_ADC3); 
	ADC_RegularChannelCascadeDLYConfig(ADC4, 0x04);
	ADC_SampleTimeModeConfig(ADC4, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC4, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC4, ADC_Channel_2,ADC_SampleTime_CyclesMode0);
	ADC_ExternalTrigConvCmd(ADC4, ENABLE);

	ADC_Cmd(ADC4, ENABLE);

    ADC_BufferCmd(ADC4, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC4);
    while(ADC_GetResetCalibrationStatus(ADC4));
    ADC_StartCalibration(ADC4);
    while(ADC_GetCalibrationStatus(ADC4));
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

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE );
	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO, ENABLE );

	GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource12 );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
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
	printf("SystemClk:%d\r\n", SystemCoreClock);
	printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
	ADC_Function_Init();
	EXTI_Event_Init();

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
	u16 ADC_val1,ADC_val2,ADC_val3,ADC_val4;

	if(ADC_GetITStatus( ADC1, ADC_IT_EOC))
	{
		ADC_val1 = ADC_GetConversionValue(ADC1);
		ADC_val2 = ADC_GetConversionValue(ADC2);
		ADC_val3 = ADC_GetConversionValue(ADC3);
		ADC_val4 = ADC_GetConversionValue(ADC4);
#if 1

		printf( "ADC1 %04d ADC2 %04d   ADC3 %04d   ADC4 %04d\r\n", ADC_val1,ADC_val2,ADC_val3,ADC_val4);
#endif
	}

	ADC_ClearITPendingBit( ADC1, ADC_IT_EOC);
	
}
