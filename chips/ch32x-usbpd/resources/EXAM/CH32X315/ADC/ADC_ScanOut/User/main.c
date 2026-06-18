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
 *ADC_Scan Out routine:
 *ADC ADCS0 (PA10),ADC ADCS1 (PA11) ADC ADCS2 (PA12) when ADC end convent ADCSx out in this mode can combine with CH448.
 *
 */


#include "debug.h"

/* Global define */

/* Global Variable */
u16 ADC_val1[2];
volatile u8 DMA1_IT_Flag;
u32 TxBuf1[1];
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
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


	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOB, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_ADC2, ENABLE );
    RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK); 	
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);	
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ScanSelConfig(ADC1_SCAN_OUT);
    ADC_ScanModeConfig(ADC1,ADC_ScanCH_SRC_Regular);
    ADC_ScanCountOutputConfig(ADC1,ADC_ScanCntOutMode7);

	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_1, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_1,ADC_SampleTime_CyclesMode7);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_SampleTimeModeConfig(ADC2, ADC_Channel_0, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_0,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
}

/*********************************************************************
 * @fn      TIM1_PWM_In
 *
 * @brief   Initializes TIM1 PWM output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWM_In( u16 arr, u16 psc, u16 ccp )
{

	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOA | RCC_HB2Periph_TIM1, ENABLE );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Update );
	TIM_Cmd( TIM1, ENABLE );
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );

	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC, ENABLE );
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
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n",DBGMCU_GetCHIPID() );

	ADC_Function_Init();
	DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf1, 1 );
	DMA_Cmd( DMA1_Channel1, ENABLE );
	TIM1_PWM_In( 1000, 48000-1, 500 );
	while(1)
	{
        if(DMA1_IT_Flag==1)
        {
            DMA1_IT_Flag=0;
            printf("ADC1 ch1=%d\r\n",ADC_val1[0]);
            printf("ADC2 ch0=%d\r\n",ADC_val1[1]);
        }
        Delay_Ms(1);
	}

}

/*********************************************************************
 * @fn      DMA1_Channel1_IRQHandler
 *
 * @brief   This function handles DMA1 Channel1 exception.
 *
 * @return  none
 */
void DMA1_Channel1_IRQHandler()
{
   if(DMA_GetITStatus(DMA1_IT_TC1)==SET ){
        DMA1_IT_Flag=1;
        DMA_ClearITPendingBit(DMA1_IT_GL1);
        ADC_val1[0]=TxBuf1[0]&0xffff;
        ADC_val1[1]=(TxBuf1[0]>>16)&0xffff;
    }
}


