# Recipe: DMA Transfer

## When to Use
User wants to implement DMA transfers for memory-to-memory copy, peripheral-to-memory, or circular buffer modes.

## API Reference (from ch32v20x_dma.h)

### Init Structure
```c
typedef struct {
    uint32_t DMA_PeripheralBaseAddr;  // Peripheral data register address
    uint32_t DMA_MemoryBaseAddr;      // Memory buffer address
    uint32_t DMA_DIR;                 // DMA_DIR_PeripheralSRC or DMA_DIR_PeripheralDST
    uint32_t DMA_BufferSize;          // Number of data items
    uint32_t DMA_PeripheralInc;       // DMA_PeripheralInc_Enable/Disable
    uint32_t DMA_MemoryInc;           // DMA_MemoryInc_Enable/Disable
    uint32_t DMA_PeripheralDataSize;  // DMA_PeripheralDataSize_Byte/_HalfWord/_Word
    uint32_t DMA_MemoryDataSize;      // DMA_MemoryDataSize_Byte/_HalfWord/_Word
    uint32_t DMA_Mode;                // DMA_Mode_Normal or DMA_Mode_Circular
    uint32_t DMA_Priority;            // DMA_Priority_Low/_Medium/_High/_VeryHigh
    uint32_t DMA_M2M;                 // DMA_M2M_Enable/Disable (memory-to-memory)
} DMA_InitTypeDef;
```

### Key Functions
```c
void       DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, DMA_InitTypeDef *DMA_InitStruct);
void       DMA_Cmd(DMA_Channel_TypeDef *DMAy_Channelx, FunctionalState NewState);
void       DMA_SetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx, uint16_t DataNumber);
uint16_t   DMA_GetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx);
void       DMA_ITConfig(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void       DMA_ClearFlag(uint32_t DMAy_FLAG);
```

### DMA Channel Mapping (CH32V20x)

| Channel | Peripheral Request |
|---------|-------------------|
| DMA1_Channel1 | ADC1 |
| DMA1_Channel2 | USART3_TX |
| DMA1_Channel3 | SPI1_TX / USART1_TX (remap) |
| DMA1_Channel4 | SPI1_RX / USART1_RX (remap) |
| DMA1_Channel5 | USART2_TX / I2C1_TX |
| DMA1_Channel6 | USART2_RX / I2C1_RX / TIM3_CH1/TRIG |
| DMA1_Channel7 | TIM2_CH2/CH4 / TIM4_CH1 |
| DMA1_Channel8 | TIM5_CH4 / TIM5_TRIG |

## Example: Memory-to-Memory DMA Transfer

```c
#include "ch32v20x.h"

void DMA_MemCopy(uint32_t *dst, uint32_t *src, uint16_t count)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)src;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dst;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;  // M2M: DST is "memory"
    DMA_InitStruct.DMA_BufferSize = count;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel1, ENABLE);

    // Wait for transfer complete
    while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
    DMA_ClearFlag(DMA1_FLAG_TC1);
    DMA_Cmd(DMA1_Channel1, DISABLE);
}
```

## Example: USART1 TX with DMA

```c
uint8_t tx_buf[] = "Hello DMA!\r\n";

void USART1_DMA_TX_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA1 Channel 3 = USART1_TX (or Channel 4 with remap)
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)tx_buf;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = sizeof(tx_buf) - 1;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);

    // Enable DMA on USART1 TX
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

void USART1_DMA_Send(uint16_t len)
{
    DMA_SetCurrDataCounter(DMA1_Channel3, len);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET);
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, DISABLE);
}
```

## Example: ADC1 Continuous Scan with DMA

```c
#define ADC_CH_COUNT  3
uint16_t adc_dma_buf[ADC_CH_COUNT];

void ADC1_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // DMA1 Channel 1 = ADC1
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->RDATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_dma_buf;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = ADC_CH_COUNT;
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
    ADC_InitStruct.ADC_NbrOfChannel = ADC_CH_COUNT;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // adc_dma_buf[] is continuously updated by DMA
}
```

## Pitfalls
- **Channel assignment is fixed** -- check datasheet for your chip's DMA channel mapping
- **Circular mode for continuous transfers** -- use `DMA_Mode_Circular` for ADC/audio
- **Normal mode for one-shot** -- use `DMA_Mode_Normal` for memory copy or single transfers
- **Buffer alignment** -- ensure buffer addresses match data size (word-aligned for 32-bit)
- **Enable DMA on peripheral side too** -- e.g., `ADC_DMACmd()`, `USART_DMACmd()`, `SPI_I2S_DMACmd()`
