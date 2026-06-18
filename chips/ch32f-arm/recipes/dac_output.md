# Recipe: DAC Output

## Overview

Configure DAC for analog voltage output on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void DAC_DeInit(void);
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct);
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);

// Data output
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);

// Wave generation
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState);

// Software trigger
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);

// DMA
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);
```

## DAC Init Structure

```c
typedef struct {
    uint32_t DAC_Trigger;                      // Trigger source
    uint32_t DAC_WaveGeneration;               // Wave generation mode
    uint32_t DAC_LFSRUnmask_TriangleAmplitude; // LFSR mask or triangle amplitude
    uint32_t DAC_OutputBuffer;                 // Output buffer enable
} DAC_InitTypeDef;
```

## DAC Channels

| Channel | Pin | Description |
|---------|-----|-------------|
| DAC_Channel_1 | PA4 | DAC output 1 |
| DAC_Channel_2 | PA5 | DAC output 2 |

## DAC Trigger Sources

| Constant | Description |
|----------|-------------|
| DAC_Trigger_None | Software trigger only |
| DAC_Trigger_T6_TRGO | TIM6 trigger |
| DAC_Trigger_T7_TRGO | TIM7 trigger |
| DAC_Trigger_T2_TRGO | TIM2 trigger |
| DAC_Trigger_T3_TRGO | TIM3 trigger |
| DAC_Trigger_T4_TRGO | TIM4 trigger |
| DAC_Trigger_T5_TRGO | TIM5 trigger |
| DAC_Trigger_Ext_IT9 | External interrupt 9 |
| DAC_Trigger_Software | Software trigger |

## DAC Data Alignment

| Constant | Description |
|----------|-------------|
| DAC_Align_12b_R | 12-bit right-aligned |
| DAC_Align_12b_L | 12-bit left-aligned |
| DAC_Align_8b_R | 8-bit right-aligned |

## DAC Wave Generation

| Constant | Description |
|----------|-------------|
| DAC_WaveGeneration_None | No wave generation |
| DAC_WaveGeneration_Noise | Noise wave generation |
| DAC_WaveGeneration_Triangle | Triangle wave generation |

## Example: DAC Simple Output

```c
#include "debug.h"

void DAC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef DAC_InitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA4 = DAC1_OUT (analog input mode)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure DAC Channel 1
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // Enable DAC Channel 1
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

void DAC1_SetVoltage(float voltage)
{
    // Convert voltage to 12-bit value (0-4095)
    // Output voltage = VREF * data / 4096
    uint16_t value = (uint16_t)(voltage * 4095.0f / 3.3f);
    if(value > 4095) value = 4095;
    DAC_SetChannel1Data(DAC_Align_12b_R, value);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    DAC1_Init();
    printf("DAC Output TEST\r\n");

    // Set DAC to 1.65V (half of 3.3V)
    DAC1_SetVoltage(1.65f);
    printf("DAC set to 1.65V\r\n");

    while(1)
    {
        // Generate triangle wave
        for(uint16_t i = 0; i < 4096; i += 16)
        {
            DAC_SetChannel1Data(DAC_Align_12b_R, i);
            Delay_Ms(1);
        }
        for(uint16_t i = 4095; i > 0; i -= 16)
        {
            DAC_SetChannel1Data(DAC_Align_12b_R, i);
            Delay_Ms(1);
        }
    }
}
```

## Example: DAC with DMA Triangle Wave

```c
#include "debug.h"

#define DAC_BUFFER_SIZE 256
uint16_t dac_buffer[DAC_BUFFER_SIZE];

void DAC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    DAC_InitTypeDef DAC_InitStructure = {0};

    // Enable clocks
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // Generate triangle wave data
    for(uint16_t i = 0; i < DAC_BUFFER_SIZE; i++)
    {
        dac_buffer[i] = (i < DAC_BUFFER_SIZE/2) ? (i * 4095 / (DAC_BUFFER_SIZE/2)) :
                        (4095 - (i - DAC_BUFFER_SIZE/2) * 4095 / (DAC_BUFFER_SIZE/2));
    }

    // Configure DMA for DAC Channel 1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dac_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DAC_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel3, ENABLE);

    // Configure DAC with TIM6 trigger
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    DAC_DMACmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // Configure TIM6 for periodic trigger
    // ... TIM6 initialization for desired frequency ...
}
```
