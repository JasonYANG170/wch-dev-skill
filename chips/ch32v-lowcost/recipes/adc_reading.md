# Recipe: ADC Reading

## Scenario
Configure ADC for single-channel, multi-channel, DMA, and analog watchdog operation.

## API Quick Reference

### ADC Init Structure
```c
typedef struct
{
    uint32_t ADC_Mode;                // ADC_Mode_Independent
    FunctionalState ADC_ScanConvMode; // ENABLE for multi-channel, DISABLE for single
    FunctionalState ADC_ContinuousConvMode; // ENABLE for continuous, DISABLE for single
    uint32_t ADC_ExternalTrigConv;    // ADC_ExternalTrigConv_None for software trigger
    uint32_t ADC_DataAlign;           // ADC_DataAlign_Right (standard)
    uint8_t ADC_NbrOfChannel;         // Number of channels to convert
} ADC_InitTypeDef;
```

### Key Functions
```c
void       ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct);
void       ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
uint16_t   ADC_GetConversionValue(ADC_TypeDef *ADCx);
void       ADC_ResetCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_StartCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_Calibration_Vol(ADC_TypeDef *ADCx, uint32_t ADC_CALVOL);
void       ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ITConfig(ADC_TypeDef *ADCx, uint16_t ADC_IT, FunctionalState NewState);
void       ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void       ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void       ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel);
```

### ADC Channels (CH32V003)
| Channel | Pin | Notes |
|---------|-----|-------|
| ADC_Channel_0 | PC4 | |
| ADC_Channel_1 | PD4 | |
| ADC_Channel_2 | PD3 | |
| ADC_Channel_3 | PD2 | |
| ADC_Channel_4 | PD1 | |
| ADC_Channel_5 | PD0 | |
| ADC_Channel_6 | PC0 | |
| ADC_Channel_7 | PC1 | |
| ADC_Channel_8 | -- | Internal Vrefint |
| ADC_Channel_9 | -- | Internal Vcalint |

### Sampling Times
```c
ADC_SampleTime_3Cycles    // Fastest, less accurate
ADC_SampleTime_9Cycles
ADC_SampleTime_15Cycles
ADC_SampleTime_30Cycles
ADC_SampleTime_43Cycles
ADC_SampleTime_57Cycles
ADC_SampleTime_73Cycles
ADC_SampleTime_241Cycles  // Slowest, most accurate
```

### Calibration
```c
ADC_CALVOL_50PERCENT   // 50% calibration voltage
ADC_CALVOL_75PERCENT   // 75% calibration voltage
```

## Complete Call Chains

### Single Channel Polling
```c
void ADC_Single_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // ADC pin as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  // PC4 = ADC_CH0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_Cmd(ADC1, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC_Read_Channel(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

// Usage:
uint16_t adc_value = ADC_Read_Channel(ADC_Channel_0);
float voltage = (float)adc_value / 4096.0f * 3.3f;
```

### Multi-Channel DMA
```c
u16 adc_buf[3];  // Buffer for 3 channels

void ADC_DMA_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // Configure ADC pins as analog
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // DMA config
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)adc_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // ADC config
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Channel order
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_241Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_241Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_241Cycles);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// adc_buf[0], adc_buf[1], adc_buf[2] are updated automatically by DMA
```

### Analog Watchdog
```c
void ADC_Watchdog_Init(void)
{
    // ... ADC init as single channel ...

    ADC_AnalogWatchdogThresholdsConfig(ADC1, 0x0B00, 0x0300);  // High: 2.3V, Low: 0.6V
    ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_0);
    ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);

    ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
    // Configure NVIC for ADC_IRQn
}
```

## Common Errors

1. **GPIO not set to AIN** -- ADC won't read if pin is not `GPIO_Mode_AIN`
2. **ADC clock too fast** -- Use `RCC_PCLK2_Div8` or higher divider
3. **Missing calibration** -- Always calibrate after `ADC_Cmd(ENABLE)`
4. **EOC flag not cleared** -- Read `ADC_GetConversionValue()` clears EOC automatically
5. **Scan mode disabled for multi-channel** -- Must enable `ADC_ScanConvMode` for >1 channel
