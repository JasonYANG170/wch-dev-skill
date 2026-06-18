# DAC Output

## Overview

CH32H417 has a 12-bit DAC with 2 channels (DAC_Channel_1 on PA4, DAC_Channel_2 on PA5). Supports software trigger, timer trigger, noise wave generation, triangle wave generation, and DMA transfers.

## Key API Functions

```c
void DAC_DeInit(void);
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
uint16_t DAC_GetOutputValue(uint32_t DAC_Channel);
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave);
void DAC_SetChannel1LFSRUnmask(uint32_t DAC_LFSRUnmask);
void DAC_SetChannel1TriangleAmplitude(uint32_t DAC_TriangleAmplitude);
```

## Basic DAC Output Example

```c
#include "ch32h417.h"

void DAC_Normal_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef DAC_InitType = {0};

    // Enable clocks
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_DAC, ENABLE);

    // PA4 as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC configuration
    DAC_InitType.DAC_Trigger = DAC_Trigger_None;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

void DAC_Output_Value(uint16_t value)
{
    DAC_SetChannel1Data(DAC_Align_12b_R, value);
}
```

## DAC with Timer Trigger and DMA

```c
// Timer trigger for periodic DAC output (e.g., waveform generation)
void TIM8_DAC_Trigger_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM8, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);

    // TIM8 TRGO triggers DAC
    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
    TIM_Cmd(TIM8, ENABLE);
}

void DAC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    uint16_t dac_buffer[8] = {0x123, 0x234, 0x345, 0x456, 0x567, 0x678, 0x789, 0x89a};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA2, ENABLE);

    // DAC1 uses DMA2 Channel 3, DAC2 uses DMA2 Channel 4
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->R12BDHR1);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dac_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);

    // Configure DAC for timer trigger with DMA
    DAC_InitTypeDef DAC_InitType = {0};
    DAC_InitType.DAC_Trigger = DAC_Trigger_T8_TRGO;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);

    // Map DMA channel via DMAMUX
    DMA_MuxChannelConfig(DMA_MuxChannel11, 0x67);
}
```

## Noise Wave Generation

```c
void DAC_Noise_Init(void)
{
    DAC_InitTypeDef DAC_InitType = {0};

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_SetChannel1Data(DAC_Align_12b_R, 0x7FF); // Initial value

    // Trigger noise generation
    DAC->SWTR |= 0x01;  // Software trigger, auto-resets by hardware
}
```

## DAC Alignment Options

| Macro | Description |
|-------|-------------|
| `DAC_Align_12b_R` | 12-bit right-aligned |
| `DAC_Align_12b_L` | 12-bit left-aligned |
| `DAC_Align_8b_R` | 8-bit right-aligned |

## DMA Channel Mapping

| DAC Channel | DMA Channel |
|-------------|-------------|
| DAC_Channel_1 | DMA2_Channel3 |
| DAC_Channel_2 | DMA2_Channel4 |
