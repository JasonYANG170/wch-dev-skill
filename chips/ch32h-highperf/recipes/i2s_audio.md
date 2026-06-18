# I2S Audio Interface

## Overview

CH32H417 has I2S (Inter-IC Sound) capability on SPI2 and SPI3 peripherals. Supports Master/Slave TX/RX modes, Phillips standard, 16/24/32-bit data, and various audio frequencies. I2S uses DMA for efficient audio data transfer.

## Key API Functions

```c
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
```

## I2S Master TX Example (SPI2)

```c
#include "ch32h417.h"

void I2S2_MasterTx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};

    // Enable clocks
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI2, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE);

    // WS PB12(AF5) - Word Select
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CK PB13(AF5) - Clock
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD PC1(AF5) - Serial Data
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // I2S configuration
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);

    // Enable DMA request
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    I2S_Cmd(SPI2, ENABLE);
}
```

## I2S Slave RX Example (SPI3)

```c
void I2S3_SlaveRx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI3, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

    // WS PA15(AF6), CK PA14(AF1), SD PA13(AF1) - all as floating input
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF6);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI3, &I2S_InitStructure);

    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);
    I2S_Cmd(SPI3, ENABLE);
}
```

## I2S DMA Transfer

```c
uint16_t I2S2_Tx[10] = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555,
                         0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA};
uint16_t I2S3_Rx[10];

void I2S_DMA_Transfer(void)
{
    // TX DMA on DMA1 Channel 5
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1 | RCC_HBPeriph_DMA2, ENABLE);

    // TX DMA
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2S2_Tx;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 10;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // RX DMA on DMA2 Channel 1
    DMA_DeInit(DMA2_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)I2S3_Rx;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_Init(DMA2_Channel1, &DMA_InitStructure);

    // Configure DMA MUX for I2S
    DMA_MuxChannelConfig(DMA_MuxChannel5, 65);   // SPI2_TX
    DMA_MuxChannelConfig(DMA_MuxChannel9, 68);   // SPI3_RX

    DMA_Cmd(DMA2_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    // Wait for completion
    while (!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC5));
    while (!DMA_GetFlagStatus(DMA2, DMA2_FLAG_TC1));
}
```

## I2S Operating Modes

| Macro | Description |
|-------|-------------|
| `I2S_Mode_SlaveTx` | Slave transmit |
| `I2S_Mode_SlaveRx` | Slave receive |
| `I2S_Mode_MasterTx` | Master transmit |
| `I2S_Mode_MasterRx` | Master receive |

## Audio Standards

| Macro | Description |
|-------|-------------|
| `I2S_Standard_Phillips` | Phillips I2S standard |
| `I2S_Standard_MSB` | MSB-aligned standard |
| `I2S_Standard_LSB` | LSB-aligned standard |
| `I2S_Standard_PCMShort` | PCM short frame |

## Audio Frequencies

| Macro | Frequency |
|-------|-----------|
| `I2S_AudioFreq_192k` | 192 kHz |
| `I2S_AudioFreq_96k` | 96 kHz |
| `I2S_AudioFreq_48k` | 48 kHz |
| `I2S_AudioFreq_44k` | 44.1 kHz |
| `I2S_AudioFreq_32k` | 32 kHz |
| `I2S_AudioFreq_22k` | 22.05 kHz |
| `I2S_AudioFreq_16k` | 16 kHz |
| `I2S_AudioFreq_11k` | 11.025 kHz |
| `I2S_AudioFreq_8k` | 8 kHz |
