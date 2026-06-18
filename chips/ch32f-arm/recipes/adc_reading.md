# Recipe: ADC Reading

## Overview

Configure ADC for analog-to-digital conversion on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void ADC_DeInit(ADC_TypeDef* ADCx);
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);

// Calibration
void ADC_ResetCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx);
void ADC_StartCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx);

// Channel configuration
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);

// Conversion
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);

// Interrupt and DMA
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);

// Status
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT);
```

## ADC Init Structure

```c
typedef struct {
    uint32_t ADC_Mode;                      // ADC_Mode_Independent (most common)
    FunctionalState ADC_ScanConvMode;       // ENABLE for multi-channel, DISABLE for single
    FunctionalState ADC_ContinuousConvMode; // ENABLE for continuous, DISABLE for single
    uint32_t ADC_ExternalTrigConv;          // ADC_ExternalTrigConv_None for software trigger
    uint32_t ADC_DataAlign;                 // ADC_DataAlign_Right or ADC_DataAlign_Left
    uint8_t ADC_NbrOfChannel;               // Number of channels (1-16)
} ADC_InitTypeDef;
```

## ADC Channels

| Channel | Pin | Description |
|---------|-----|-------------|
| ADC_Channel_0 | PA0 | Analog input 0 |
| ADC_Channel_1 | PA1 | Analog input 1 |
| ADC_Channel_2 | PA2 | Analog input 2 |
| ADC_Channel_3 | PA3 | Analog input 3 |
| ADC_Channel_4 | PA4 | Analog input 4 |
| ADC_Channel_5 | PA5 | Analog input 5 |
| ADC_Channel_6 | PA6 | Analog input 6 |
| ADC_Channel_7 | PA7 | Analog input 7 |
| ADC_Channel_8 | PB0 | Analog input 8 |
| ADC_Channel_9 | PB1 | Analog input 9 |
| ADC_Channel_10 | PC0 | Analog input 10 |
| ADC_Channel_11 | PC1 | Analog input 11 |
| ADC_Channel_12 | PC2 | Analog input 12 |
| ADC_Channel_13 | PC3 | Analog input 13 |
| ADC_Channel_14 | PC4 | Analog input 14 |
| ADC_Channel_15 | PC5 | Analog input 15 |
| ADC_Channel_16 | - | Internal temperature sensor |
| ADC_Channel_17 | - | Internal voltage reference (Vrefint) |

## ADC Sample Time

| Constant | Cycles | Description |
|----------|--------|-------------|
| ADC_SampleTime_1Cycles5 | 1.5 | Fastest, least accurate |
| ADC_SampleTime_7Cycles5 | 7.5 | Fast |
| ADC_SampleTime_13Cycles5 | 13.5 | Medium |
| ADC_SampleTime_28Cycles5 | 28.5 | Medium |
| ADC_SampleTime_41Cycles5 | 41.5 | Slow |
| ADC_SampleTime_55Cycles5 | 55.5 | Slow |
| ADC_SampleTime_71Cycles5 | 71.5 | Slower |
| ADC_SampleTime_239Cycles5 | 239.5 | Slowest, most accurate |

## Example: Single Channel Read

```c
#include "debug.h"

void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 = ADC1_IN0 (analog input)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Enable ADC1
    ADC_Cmd(ADC1, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_ReadChannel(uint8_t channel)
{
    // Configure channel
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);

    // Start conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // Wait for conversion complete
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    // Read result
    return ADC_GetConversionValue(ADC1);
}

float ADC1_ReadVoltage(uint8_t channel)
{
    uint16_t adc_value = ADC1_ReadChannel(channel);
    return (float)adc_value * 3.3f / 4096.0f;  // 12-bit ADC, 3.3V reference
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    ADC1_Init();
    printf("ADC Reading TEST\r\n");

    while(1)
    {
        uint16_t raw = ADC1_ReadChannel(ADC_Channel_0);
        float voltage = ADC1_ReadVoltage(ADC_Channel_0);
        printf("ADC0: %d (%.3fV)\r\n", raw, voltage);
        Delay_Ms(500);
    }
}
```

## Example: Temperature Sensor

```c
float Read_Temperature(void)
{
    // Enable temperature sensor
    ADC_TempSensorVrefintCmd(ENABLE);

    // Read channel 16 (temperature sensor)
    uint16_t raw = ADC1_ReadChannel(ADC_Channel_TempSensor);

    // Convert to temperature (approximate)
    // V25 = 1.43V, Avg_Slope = 4.3 mV/C
    float voltage = (float)raw * 3.3f / 4096.0f;
    float temperature = (1.43f - voltage) / 0.0043f + 25.0f;

    return temperature;
}
```

## Multi-Channel Scanning

```c
void ADC1_MultiChannel_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    // ... GPIO init for multiple pins ...

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 3;  // 3 channels
    ADC_Init(ADC1, &ADC_InitStructure);

    // Configure channels with ranks
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);

    ADC_Cmd(ADC1, ENABLE);
    // ... calibration ...
}
```
