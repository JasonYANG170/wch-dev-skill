# ADC Reading

## Overview

CH32H417 has 2 standard ADCs (ADC1, ADC2) with 12-bit resolution, plus a HSADC (High-Speed ADC) for faster sampling. The standard ADCs support dual-mode operation, injection channels, and analog watchdog.

## Standard ADC Key Functions

```c
void ADC_DeInit(ADC_TypeDef* ADCx);
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
void ADC_TempSensorVrefintCmd(FunctionalState NewState);
void ADC_BufferCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_LowPowerModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
```

## HSADC Key Functions

```c
void HSADC_DeInit(void);
void HSADC_Init(HSADC_InitTypeDef *HSADC_InitStruct);
void HSADC_Cmd(FunctionalState NewState);
void HSADC_DMACmd(FunctionalState NewState);
void HSADC_BurstModeCmd(FunctionalState NewState);
void HSADC_ChannelConfig(uint8_t HSADC_Channel);
uint16_t HSADC_GetConversionValue(void);
```

## Basic ADC Single-Channel Example

```c
#include "ch32h417.h"

void ADC1_Single_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_InitTypeDef ADC_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 as analog input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC configuration
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_InitStruct.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStruct.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_Cmd(ADC1, ENABLE);

    // Calibrate
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_ReadChannel(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_CyclesMode5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

// Read temperature sensor
float Read_Temperature(void)
{
    ADC_TempSensorVrefintCmd(ENABLE);
    uint16_t adc_val = ADC1_ReadChannel(ADC_Channel_TempSensor);
    // Convert to temperature using calibration data
    return TempSensor_Volt_To_Temper(adc_val);
}
```

## HSADC Example (High-Speed)

```c
void HSADC_Init_Example(void)
{
    HSADC_InitTypeDef HSADC_InitStruct = {0};

    HSADC_InitStruct.HSADC_FirstConversionCycle = HSADC_First_Conversion_Cycle_8;
    HSADC_InitStruct.HSADC_DataSize = HSADC_DataSize_16b;
    HSADC_InitStruct.HSADC_ClockDivision = 0;
    HSADC_InitStruct.HSADC_DMA = ENABLE;
    HSADC_InitStruct.HSADC_DualBuffer = DISABLE;
    HSADC_InitStruct.HSADC_BurstMode = ENABLE;
    HSADC_InitStruct.HSADC_BurstMode_TransferLen = 1024;
    HSADC_InitStruct.HSADC_DMA_TransferLen = 1024;
    HSADC_InitStruct.HSADC_RxAddress0 = (uint32_t)adc_buffer;

    HSADC_Init(&HSADC_InitStruct);

    // Configure channel
    HSADC_ChannelConfig(HSADC_Channel_0);

    HSADC_Cmd(ENABLE);
    HSADC_DMACmd(ENABLE);
    HSADC_BurstModeCmd(ENABLE);
}
```

## ADC Channels

| Channel | Description |
|---------|-------------|
| `ADC_Channel_0` - `ADC_Channel_15` | External analog inputs |
| `ADC_Channel_16` | Temperature sensor |
| `ADC_Channel_17` | Internal voltage reference (Vrefint) |

## PGA Gain Options

| Macro | Gain |
|-------|------|
| `ADC_Pga_1` | 1x (no gain) |
| `ADC_Pga_4` | 4x |
| `ADC_Pga_16` | 16x |
| `ADC_Pga_64` | 64x |

## Sampling Time Modes

| Mode | Description |
|------|-------------|
| `ADC_SampleTime_CyclesMode0` | Shortest sampling time |
| `ADC_SampleTime_CyclesMode7` | Longest sampling time |

## Dual ADC Mode

```c
// Configure dual ADC mode for simultaneous conversion
ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
// ADC1 and ADC2 convert simultaneously
uint32_t dual_value = ADC_GetDualModeConversionValue();
// upper 16 bits = ADC2, lower 16 bits = ADC1
```
