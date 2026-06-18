# Recipe: DMA Transfer

## Scenario
Configure DMA for memory-to-memory copy, memory-to-peripheral, and peripheral-to-memory transfers.

## DMA Channels

### CH32V003 / CH32V006
| Channel | Peripheral Requests |
|---------|-------------------|
| DMA1_Channel1 | ADC1 |
| DMA1_Channel2 | (reserved) |
| DMA1_Channel3 | M2M / (reserved) |
| DMA1_Channel4 | SPI1_TX / USART1_TX |
| DMA1_Channel5 | SPI1_RX / USART1_RX |
| DMA1_Channel6 | I2C1_TX / TIM1_UP |
| DMA1_Channel7 | I2C1_RX / TIM1_CH1/CH2 |

### CH32L103
Same as CH32V003 plus additional channels for USB and other peripherals.

## API Quick Reference

### DMA Init Structure
```c
typedef struct
{
    uint32_t DMA_PeripheralBaseAddr;  // Peripheral data register address
    uint32_t DMA_MemoryBaseAddr;      // Memory buffer address
    uint32_t DMA_DIR;                 // DMA_DIR_PeripheralSRC or DMA_DIR_PeripheralDST
    uint32_t DMA_BufferSize;          // Number of data items to transfer
    uint32_t DMA_PeripheralInc;       // DMA_PeripheralInc_Enable / Disable
    uint32_t DMA_MemoryInc;           // DMA_MemoryInc_Enable / Disable
    uint32_t DMA_PeripheralDataSize;  // DMA_PeripheralDataSize_Byte / HalfWord / Word
    uint32_t DMA_MemoryDataSize;      // DMA_MemoryDataSize_Byte / HalfWord / Word
    uint32_t DMA_Mode;                // DMA_Mode_Normal or DMA_Mode_Circular
    uint32_t DMA_Priority;            // DMA_Priority_VeryHigh / High / Medium / Low
    uint32_t DMA_M2M;                 // DMA_M2M_Enable (mem-to-mem) or DMA_M2M_Disable
} DMA_InitTypeDef;
```

### Key Functions
```c
void       DMA_DeInit(DMA_Channel_TypeDef *DMAy_Channelx);
void       DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, DMA_InitTypeDef *DMA_InitStruct);
void       DMA_StructInit(DMA_InitTypeDef *DMA_InitStruct);
void       DMA_Cmd(DMA_Channel_TypeDef *DMAy_Channelx, FunctionalState NewState);
void       DMA_ITConfig(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
FlagStatus DMA_GetFlagStatus(uint32_t DMA_FLAG);
void       DMA_ClearFlag(uint32_t DMA_FLAG);
ITStatus   DMA_GetITStatus(uint32_t DMA_IT);
void       DMA_ClearITPendingBit(uint32_t DMA_IT);
```

### Direction Constants
```c
DMA_DIR_PeripheralSRC  // Peripheral -> Memory (receive)
DMA_DIR_PeripheralDST  // Memory -> Peripheral (transmit)
```

### Flag Format
```c
// DMA_FLAG_TCx = Transfer Complete, DMA_FLAG_HTx = Half Transfer
// DMA_FLAG_TExx = Transfer Error
// x = channel number (1-7)
DMA1_FLAG_TC1, DMA1_FLAG_TC2, ..., DMA1_FLAG_TC7
DMA1_FLAG_HT1, DMA1_FLAG_HT2, ..., DMA1_FLAG_HT7
DMA1_IT_TC1, DMA1_IT_TC2, ..., DMA1_IT_TC7
```

## Complete Call Chains

### Memory-to-Memory Transfer
```c
#define Buf_Size  32
u32 SRC_BUF[Buf_Size] = {0x01020304, 0x05060708 /* ... */};
u32 DST_BUF[Buf_Size] = {0};

void DMA_Mem2Mem_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SRC_BUF;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DST_BUF;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = Buf_Size * 4;  // bytes
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;  // Memory-to-memory mode
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

// Wait for completion
while(DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET) {}
printf("DMA Transfer Complete\r\n");
```

### ADC Continuous DMA (Peripheral-to-Memory, Circular)
```c
u16 adc_buf[3];  // Buffer for 3 ADC channels

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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  // PC4 = CH0
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // PD3 = CH2
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // PD2 = CH3
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // DMA Channel 1 -- ADC1
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)adc_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // Continuous
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

// adc_buf[0], adc_buf[1], adc_buf[2] updated automatically by DMA
```

### UART DMA Transmit (Memory-to-Peripheral)
```c
void USART_DMA_Tx_Init(u8 *buf, u16 len)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Channel 4 = USART1_TX
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
}

// Enable USART DMA request, then start DMA
USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
DMA_Cmd(DMA1_Channel4, ENABLE);

// Wait for transfer complete
while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET) {}
```

### I2C DMA Transmit
```c
void I2C_DMA_Tx_Init(u8 *buf, u16 len)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Channel 6 = I2C1_TX
    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2C1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

// Enable I2C DMA, then start after address phase
I2C_DMACmd(I2C1, ENABLE);
DMA_Cmd(DMA1_Channel6, ENABLE);
```

### DMA with Interrupt
```c
void DMA_Interrupt_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
}

void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC6) != RESET)
    {
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC6);
    }
}
```

## DMA Channel Assignment Summary

| Channel | TX (Memory->Peripheral) | RX (Peripheral->Memory) |
|---------|------------------------|------------------------|
| 1 | -- | ADC1 |
| 4 | USART1_TX / SPI1_TX | -- |
| 5 | -- | USART1_RX / SPI1_RX |
| 6 | I2C1_TX / TIM1_UP | -- |
| 7 | -- | I2C1_RX / TIM1_CH1/CH2 |
| 3 | M2M (any) | M2M (any) |

## Common Errors

1. **DMA clock not enabled** -- Always call `RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE)` first
2. **Buffer size in bytes, not elements** -- For `u32` array of 32 elements, use `Buf_Size * 4`
3. **Peripheral address wrong** -- Use the data register (e.g., `&ADC1->RDATAR`, `&I2C1->DATAR`)
4. **Circular vs Normal** -- Use `DMA_Mode_Circular` for continuous ADC; `DMA_Mode_Normal` for one-shot
5. **M2M flag for memory-to-memory** -- Must set `DMA_M2M_Enable` for mem-to-mem; `Disable` for peripheral transfers
6. **Data size mismatch** -- ADC uses HalfWord (16-bit); UART/I2C use Byte (8-bit)
