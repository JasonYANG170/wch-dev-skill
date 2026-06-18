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
* *@Note 
*  Dual ADC combined regular + injection + simultaneous sampling routine:
*  Rule group ADC1 channel 2 (PA4), ADC2 channel 4 (PA6), injection group ADC1 channel 3 (PC9) ADC2 channel 5 (PC11))
* The rule group injection groups are all triggered by software, and the dual ADC rule group data is obtained through
*  the DMA interrupt, and the dual ADC injection group data is obtained through the ADC interrupt.
*/


#include "debug.h"
/* Global Variable */
u32 TxBuf[1];
u16 Adc_Val[2];
u16 ADC_Val1,ADC_Val2;
volatile u8 Injected_IT_Flag,DMA_IT_Flag;

void ADC1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));

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
   

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOC, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1, ENABLE );
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC2, ENABLE );
    RCC_ADCCLKSourceConfig(RCC_ADCCLKSource_HCLK);
	RCC_ADCCLKConfig(RCC_ADCPRE_Div2,RCC_PPRE2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 |GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_Init(ADC1, &ADC_InitStructure);

	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_2, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_2,ADC_SampleTime_CyclesMode7);


    ADC_InjectedSequencerLengthConfig(ADC1, 1);

	ADC_SampleTimeModeConfig(ADC1, ADC_Channel_3, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1);
	ADC_SetMode0SampleTimeConfig(ADC1, ADC_Channel_3,ADC_SampleTime_CyclesMode7);
    
    ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_None );

    NVIC_EnableIRQ(ADC1_IRQn);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_Init(ADC2, &ADC_InitStructure);

    ADC_SampleTimeModeConfig(ADC2, ADC_Channel_4, ADC_Sample_Time_Mode0);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_4,ADC_SampleTime_CyclesMode7);


    ADC_InjectedSequencerLengthConfig(ADC2, 1);
	ADC_SampleTimeModeConfig(ADC2, ADC_Channel_5, ADC_Sample_Time_Mode0);
	ADC_InjectedChannelConfig(ADC2, ADC_Channel_5, 1);
	ADC_SetMode0SampleTimeConfig(ADC2, ADC_Channel_5,ADC_SampleTime_CyclesMode7);

    ADC_ExternalTrigInjectedConvConfig(ADC2,ADC_ExternalTrigInjecConv_None );

    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
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
    USART_Printf_Init(115200);
	SystemCoreClockUpdate();
	Delay_Init();	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n",DBGMCU_GetCHIPID() );
	ADC_Function_Init();
	
	DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)TxBuf, 1 );
	DMA_Cmd( DMA1_Channel1, ENABLE );

	while(1)
	{
	    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        ADC_SoftwareStartConvCmd(ADC2, ENABLE);
        ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
        ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);

        if(Injected_IT_Flag==1)
        {
            Injected_IT_Flag=0;
            printf( "JADC1 ch3=%04d\r\n", ADC_Val1);
            printf( "JADC2 ch5=%04d\r\n", ADC_Val2);
        }

        if(DMA_IT_Flag==1)
        {
            DMA_IT_Flag=0;
            printf("ADC1 ch2=%d\r\n",Adc_Val[0]);
            printf("ADC2 ch4=%d\r\n",Adc_Val[1]);
        }
        Delay_Ms(1000);
	}
}
/*********************************************************************
 * @fn      ADC1_IRQHandler
 *
 * @brief   This function handles ADC1  exception.
 *
 * @return  none
 */
void ADC1_IRQHandler()
{
    if(ADC_GetITStatus( ADC1, ADC_IT_JEOC)){
        Injected_IT_Flag=1;

        ADC_Val1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_Val2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
    }
    ADC_ClearITPendingBit( ADC1, ADC_IT_JEOC);
    ADC_ClearITPendingBit( ADC2, ADC_IT_JEOC);
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
        DMA_IT_Flag=1;
        DMA_ClearITPendingBit(DMA1_IT_GL1);

        Adc_Val[0]=TxBuf[0]&0xffff;
        Adc_Val[1]=(TxBuf[0]>>16)&0xffff;
    }
}
