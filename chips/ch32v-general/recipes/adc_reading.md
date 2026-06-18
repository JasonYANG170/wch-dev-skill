# Recipe: ADC Reading

## When to Use
User wants to read analog voltages, use DMA for multi-channel ADC, injected channels, analog watchdog, or internal temperature sensor.

## API Reference (from ch32v20x_adc.h)

### Init Structure
```c
typedef struct {
    uint32_t ADC_Mode;                // ADC_Mode_Independent, dual modes
    FunctionalState ADC_ScanConvMode; // ENABLE/DISABLE for multi-channel
    FunctionalState ADC_ContinuousConvMode; // ENABLE for continuous, DISABLE for single
    uint32_t ADC_ExternalTrigConv;    // Trigger source (ADC_ExternalTrigConv_None for software)
    uint32_t ADC_DataAlign;           // ADC_DataAlign_Right, ADC_DataAlign_Left
    uint8_t ADC_NbrOfChannel;         // Number of channels (1-16)
    uint32_t ADC_OutputBuffer;        // ADC_OutputBuffer_Enable/Disable
    uint32_t ADC_Pga;                 // ADC_Pga_1, _4, _16, _64
} ADC_InitTypeDef;
```

### Key Functions
```c
void       ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct);
void       ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ResetCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_StartCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
uint16_t   ADC_GetConversionValue(ADC_TypeDef *ADCx);
void       ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
uint16_t   ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel);
void       ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void       ADC_TempSensorVrefintCmd(FunctionalState NewState);
```

### ADC Channels
| Channel | Description |
|---------|-------------|
| `ADC_Channel_0` .. `ADC_Channel_15` | External analog inputs |
| `ADC_Channel_16` / `ADC_Channel_TempSensor` | Internal temperature sensor |
| `ADC_Channel_17` / `ADC_Channel_Vrefint` | Internal voltage reference |

### ADC Sampling Time
| Constant | Cycles |
|----------|--------|
| `ADC_SampleTime_1Cycles5` | 1.5 |
| `ADC_SampleTime_7Cycles5` | 7.5 |
| `ADC_SampleTime_13Cycles5` | 13.5 |
| `ADC_SampleTime_28Cycles5` | 28.5 |
| `ADC_SampleTime_41Cycles5` | 41.5 |
| `ADC_SampleTime_55Cycles5` | 55.5 |
| `ADC_SampleTime_71Cycles5` | 71.5 |
| `ADC_SampleTime_239Cycles5` | 239.5 |

## Example: Single Channel ADC Read

```c
#include "ch32v20x.h"

void ADC1_Init_Single(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // ADC clock = PCLK2/6

    // PA0 as analog input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_Cmd(ADC1, ENABLE);

    // Calibration (REQUIRED before first conversion)
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_ReadChannel(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

// Convert to voltage (assuming 3.3V reference)
float ADC_ToVoltage(uint16_t adc_value)
{
    return (float)adc_value * 3.3f / 4096.0f;
}
```

## Example: Multi-Channel ADC with DMA

```c
#define ADC_CHANNELS 3
uint16_t adc_buffer[ADC_CHANNELS];

void ADC1_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // PA0, PA1, PA2 as analog inputs
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // DMA1 Channel 1 = ADC1
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->RDATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = ADC_CHANNELS;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // ADC config
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = ADC_CHANNELS;
    ADC_Init(ADC1, &ADC_InitStruct);

    // Channel sequence
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
```

## Example: Internal Temperature Sensor

```c
float Read_Temperature(void)
{
    uint16_t adc_val;
    float voltage, temperature;

    ADC_TempSensorVrefintCmd(ENABLE);
    adc_val = ADC1_ReadChannel(ADC_Channel_TempSensor);
    ADC_TempSensorVrefintCmd(DISABLE);

    voltage = (float)adc_val * 3.3f / 4096.0f;
    // Typical formula: T = (V - 1.43) / 0.0043 + 25
    temperature = (voltage - 1.43f) / 0.0043f + 25.0f;

    return temperature;
}
```

## Pitfalls
- **Calibration is REQUIRED** -- always call `ADC_ResetCalibration` + `ADC_StartCalibration` before first conversion
- **ADC clock divider** -- ADC clock must be <= 14MHz; use `RCC_ADCCLKConfig(RCC_PCLK2_Div6)`
- **Temperature sensor needs enable** -- call `ADC_TempSensorVrefintCmd(ENABLE)` before reading channel 16
- **DMA mode must be circular** for continuous multi-channel scanning
