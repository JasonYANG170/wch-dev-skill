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
*Dual ADC regular simultaneous sampling routine:
*ADC1 channel 3 (PA5), ADC2 channel 3 (PC9)), ADC3 channel 3 (PD9), ADC4 channel 3 (PC1)) and the rule group channel obtains dual ADC
*conversion data through DMA interrupt.
*/

#include "debug.h"

/* Global define */

/* Global Variable */
u16 ADC_val1[2];
u16 ADC_val2[2];
u32 TxBuf1[1];
u32 TxBuf2[1];
volatile u8 DMA1_IT_Flag,DMA2_IT_Flag;
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel9_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
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

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC|RCC_HB2Periph_GPIOD, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1|RCC_HB2Periph_ADC2, ENABLE );
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC3|RCC_HB2Periph_ADC4, ENABLE );
    RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    ADC_DeInit(ADC3);
    ADC_DeInit(ADC4);
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;

    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_SampleTimeModeConfig(ADC1, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_3,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_RegularChannelCascadeDLYConfig(ADC1, 0x00);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	
	
    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_SampleTimeModeConfig(ADC2, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_3,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));


    ADC_Init(ADC3, &ADC_InitStructure);
    ADC_SampleTimeModeConfig(ADC3, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC3, ADC_Channel_3,ADC_SampleTime_CyclesMode7);

    ADC_RegularChannelCascadeCmd(ADC3,ENABLE);
    ADC_CascadeContsFrontConfig(ADC3,ADC_CONTS_Front_ADC1);

    ADC_ExternalTrigConvCmd(ADC3, ENABLE);
    ADC_DMACmd(ADC3, ENABLE);
    ADC_Cmd(ADC3, ENABLE);

    ADC_BufferCmd(ADC3, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC3);
    while(ADC_GetResetCalibrationStatus(ADC3));
    ADC_StartCalibration(ADC3);
    while(ADC_GetCalibrationStatus(ADC3));

    ADC_Init(ADC4, &ADC_InitStructure);
    ADC_SampleTimeModeConfig(ADC4, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC4, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC4, ADC_Channel_3,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigConvCmd(ADC4, ENABLE);
    ADC_Cmd(ADC4, ENABLE);

    ADC_BufferCmd(ADC4, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC4);
    while(ADC_GetResetCalibrationStatus(ADC4));
    ADC_StartCalibration(ADC4);
    while(ADC_GetCalibrationStatus(ADC4));
	
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
    TIM_OC2Init( TIM2, &TIM_OCInitStructure );
    TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Disable );

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
    DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf1, 1 );
	DMA_Cmd( DMA1_Channel1, ENABLE );
    DMA_Tx_Init( DMA1_Channel9, (u32)&ADC3->RDATAR, (u32)TxBuf2, 1 );
	DMA_Cmd( DMA1_Channel9, ENABLE );
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC, ENABLE );
    NVIC_EnableIRQ(DMA1_Channel9_IRQn);
    NVIC_SetPriority(DMA1_Channel9_IRQn,0x70);
    DMA_ITConfig( DMA1_Channel9, DMA_IT_TC, ENABLE );
	TIM2_PWM_In( 6000, 48000-1, 3000 );
	while(1)
    {
        if(DMA2_IT_Flag==1)
        {
            DMA2_IT_Flag=0;
            printf("ADC3 ch3=%d\r\n",ADC_val2[0]);
            printf("ADC4 ch3=%d\r\n",ADC_val2[1]);
        }

        if(DMA1_IT_Flag==1)
        {
            DMA1_IT_Flag=0;
            printf("ADC1 ch3=%d\r\n",ADC_val1[0]);
            printf("ADC2 ch3=%d\r\n",ADC_val1[1]);
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

/*********************************************************************
 * @fn      DMA1_Channel9_IRQHandler
 *
 * @brief   This function handles DMA1 Channel9 exception.
 *
 * @return  none
 */
void DMA1_Channel9_IRQHandler()
{
    if(DMA_GetITStatus(DMA1_IT_TC9)==SET ){
        DMA2_IT_Flag=1;
        DMA_ClearITPendingBit(DMA1_IT_GL9);

        ADC_val2[0]=TxBuf2[0]&0xffff;
        ADC_val2[1]=(TxBuf2[0]>>16)&0xffff;
    }
}
