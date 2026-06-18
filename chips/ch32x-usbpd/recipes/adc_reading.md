# ADC Reading

> **Summary**: Configure ADC for analog-to-digital conversion on CH32X/CH6xx chips.

## Trigger Intent

- "ADC reading"
- "Analog input"
- "ADC conversion"
- "Read voltage"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)` |
| GPIO | Pin must be configured as `GPIO_Mode_AIN` |
| Reference | `CH32X035EVT/EVT/EXAM/ADC/` |

## Step-by-Step

### Single Channel ADC Read

```c
#include "ch32x035.h"
#include "debug.h"

void ADC1_Init_Single(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_InitTypeDef ADC_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 = ADC channel 0 (analog input)
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
    ADC_Init(ADC1, &ADC_InitStruct);

    // Configure channel 0, rank 1, sample time 7 cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles);

    // Set ADC clock divider
    ADC_CLKConfig(ADC1, ADC_CLK_Div8);

    ADC_Cmd(ADC1, ENABLE);

    // Calibration (required before first conversion)
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_Read(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    ADC1_Init_Single();
    printf("ADC ready\r\n");

    while(1) {
        uint16_t adc_val = ADC1_Read();
        float voltage = (float)adc_val / 4096 * 3.3;
        printf("ADC: %d, Voltage: %.3fV\r\n", adc_val, voltage);
        Delay_Ms(500);
    }
}
```

### Multi-Channel Scan

```c
void ADC1_Init_MultiChannel(void) {
    ADC_InitTypeDef ADC_InitStruct = {0};

    // ... GPIO init for PA0, PA1, PA2 as AIN ...

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStruct);

    // Configure 3 channels with different ranks
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_7Cycles);

    // Enable DMA for scan mode
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    // ... calibration ...
}
```

### Internal Vrefint Channel

```c
// Channel 15 = Vrefint (internal reference)
ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_7Cycles);
// Typical Vrefint = 1.2V, use to calculate actual VDD
// VDD = 1.2V * 4096 / ADC_Vrefint
```

## ADC Channel Reference (CH32X035)

| Channel | Pin | Notes |
|---------|-----|-------|
| ADC_Channel_0 | PA0 | External |
| ADC_Channel_1 | PA1 | External |
| ADC_Channel_2 | PA2 | External |
| ADC_Channel_3 | PA3 | External |
| ADC_Channel_4 | PA4 | External |
| ADC_Channel_5 | PA5 | External |
| ADC_Channel_6 | PA6 | External |
| ADC_Channel_7 | PA7 | External |
| ADC_Channel_8 | PB0 | External |
| ADC_Channel_9 | PB1 | External |
| ADC_Channel_10-14 | Internal | Reserved |
| ADC_Channel_15 | Vrefint | Internal 1.2V reference |

## ADC Sample Time Reference

| Cycles | Constant |
|--------|----------|
| 4 | `ADC_SampleTime_4Cycles` |
| 5 | `ADC_SampleTime_5Cycles` |
| 6 | `ADC_SampleTime_6Cycles` |
| 7 | `ADC_SampleTime_7Cycles` |
| 8 | `ADC_SampleTime_8Cycles` |
| 9 | `ADC_SampleTime_9Cycles` |
| 10 | `ADC_SampleTime_10Cycles` |
| 11 | `ADC_SampleTime_11Cycles` |

## Common Errors

- Forgetting calibration after `ADC_Cmd(ENABLE)` -- inaccurate readings
- GPIO pin not set to `GPIO_Mode_AIN` -- ADC reads wrong value
- ADC clock divider too high -- slow conversion
- Not waiting for `ADC_FLAG_EOC` before reading -- stale data
