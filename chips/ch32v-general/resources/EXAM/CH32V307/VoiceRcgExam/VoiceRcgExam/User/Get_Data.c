/********************************** (C) COPYRIGHT *******************************
* File Name          : Get_Data.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2026/03/24
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "Get_Data.h"
#include "VoiceRcg.h"


extern volatile uint8_t g_data_ready;

extern __attribute__((aligned(4))) uint16_t V_Data[SampleDataLen];
void voice_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    ADC_InitTypeDef ADC_InitStructure={0};
    DMA_InitTypeDef DMA_InitStructure={0};

    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure={0};
    TIM_OCInitTypeDef         TIM_OCInitStructure={0};

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 90-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 200-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    /* TIM1 channel1 configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x10;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(ADC1->RDATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)V_Data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = SampleDataLen;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC1 regular channel1 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_41Cycles5);
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    /* ADC1  */
    ADC_ExternalTrigConvCmd(ADC1,ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);


    NVIC_SetPriority(DMA1_Channel1_IRQn,0xE0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/*********************************************************************
 * @fn      DMA1_Channel1_IRQHandler
 *
 * @brief   This function DMA1 Channel1 exception.
 *
 * @return  none
 */
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC1))
   {
       TIM_Cmd(TIM1, DISABLE);
//       printf("tc1\r\n");
       g_data_ready=1;
       DMA_ClearITPendingBit(DMA1_IT_TC1|DMA1_IT_GL1|DMA1_IT_HT1);
   }
}




