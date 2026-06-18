# Touch Key Sensing

## Overview

CH32H417 has a Touch Key (TKey) peripheral for capacitive touch sensing. It uses the ADC to measure the charge/discharge time of a touch pad, which changes when a finger is nearby. Supports multiple touch channels and works with the standard ADC infrastructure.

## Key API Functions

```c
// TouchKey uses direct register access with ADC
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t Channel, uint8_t Rank, uint8_t SampleTime);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
void ADC_LowPowerModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
```

## TouchKey Initialization

```c
#include "ch32h417.h"

void TouchKey_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Enable ADC and GPIO clocks
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ADC1 | RCC_HB2Periph_GPIOA, ENABLE);
    RCC_ADCCLKConfig(RCC_ADCCLKSource_HCLK);
    RCC_ADCHCLKCLKAsSourceConfig(RCC_PPRE2_DIV2, RCC_HCLK_ADCPRE_DIV8);

    // Configure touch pin as analog input (PA1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC configuration
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Enable low power mode for touch sensing
    ADC_LowPowerModeCmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // Enable TouchKey and Buffer via register
    TKey1->CTLR1 |= (1 << 26) | (1 << 24);
}
```

## Touch Key Reading

```c
uint16_t TouchKey_Read(uint8_t channel)
{
    // Configure ADC channel
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_CyclesMode7);

    // Set charging and discharging time
    TKey1->IDATAR1 = 0x9F;   // Charging time
    TKey1->RDATAR = 0xFF;    // Discharging time

    // Wait for conversion
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    return (uint16_t)TKey1->RDATAR;
}
```

## Complete TouchKey Application

```c
void Hardware(void)
{
    uint16_t touch_value;
    uint16_t baseline = 0;
    uint16_t threshold = 100;  // Adjust based on your pad design

    TouchKey_Init();

    // Calibrate baseline (no touch)
    baseline = TouchKey_Read(ADC_Channel_1);
    printf("Baseline: %d\r\n", baseline);

    while (1)
    {
        touch_value = TouchKey_Read(ADC_Channel_1);

        // Detect touch: value decreases when finger is near
        if ((baseline - touch_value) > threshold)
        {
            printf("Touch detected! Value: %d\r\n", touch_value);
        }

        Delay_Ms(100);
    }
}
```

## Available TouchKey Channels

The TouchKey channels map to ADC channels. The available channels depend on the chip package and pin assignment. Common touch-capable pins:

| Channel | Pin | Description |
|---------|-----|-------------|
| ADC_Channel_0 | PA0 | Touch key 0 |
| ADC_Channel_1 | PA1 | Touch key 1 |
| ADC_Channel_2 | PA2 | Touch key 2 |
| ADC_Channel_3 | PA3 | Touch key 3 |

## TKey Register Access

```c
// Enable TouchKey and Buffer
TKey1->CTLR1 |= (1 << 26) | (1 << 24);

// Set charging time (IDATAR1)
TKey1->IDATAR1 = 0x9F;

// Set discharging time (RDATAR)
TKey1->RDATAR = 0xFF;

// Read result from RDATAR
uint16_t value = TKey1->RDATAR;
```

## Important Notes

- TouchKey uses ADC1 with low-power mode enabled
- Charging/discharging times (IDATAR1, RDATAR) affect sensitivity
- Higher charging time = more sensitivity but slower response
- The touch pad should be connected directly to the ADC pin (no series resistor)
- Baseline calibration is recommended after power-up
- Touch detection: finger proximity decreases the RDATAR value
