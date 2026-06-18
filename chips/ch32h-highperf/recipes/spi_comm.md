# SPI Communication

## Overview

CH32H417 has 3 SPI interfaces (SPI1-3) with I2S support. SPI1 supports high-speed mode for faster clock rates.

## Key API Functions

```c
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_HighSpeedMode_Config(SPI_TypeDef *SPIx, uint8_t SPI_SPEED_MODE, FunctionalState NewState);
```

## SPI Master Example

```c
#include "ch32h417.h"

void SPI1_Master_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    SPI_InitTypeDef SPI_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA5 = SPI1_SCK, PA7 = SPI1_MOSI (AF5)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF5);

    // PA6 = SPI1_MISO (AF5)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF5);

    // PA4 = NSS (software controlled, AF5)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // SPI configuration
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode2;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_TransferByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}
```

## High-Speed SPI Mode

CH32H417 SPI1 supports high-speed mode for faster clock rates:

```c
// Enable high-speed mode 1
SPI_HighSpeedMode_Config(SPI1, SPI_HIGH_SPEED_MODE1, ENABLE);

// Enable high-speed mode 2
SPI_HighSpeedMode_Config(SPI1, SPI_HIGH_SPEED_MODE2, ENABLE);
```

## Baud Rate Prescaler Options

| Prescaler | Description |
|-----------|-------------|
| `SPI_BaudRatePrescaler_Mode0` | fPCLK/2 |
| `SPI_BaudRatePrescaler_Mode1` | fPCLK/4 |
| `SPI_BaudRatePrescaler_Mode2` | fPCLK/8 |
| `SPI_BaudRatePrescaler_Mode3` | fPCLK/16 |
| `SPI_BaudRatePrescaler_Mode4` | fPCLK/32 |
| `SPI_BaudRatePrescaler_Mode5` | fPCLK/64 |
| `SPI_BaudRatePrescaler_Mode6` | fPCLK/128 |
| `SPI_BaudRatePrescaler_Mode7` | fPCLK/256 |

## SPI Flags

| Flag | Description |
|------|-------------|
| `SPI_I2S_FLAG_TXE` | Transmit buffer empty |
| `SPI_I2S_FLAG_RXNE` | Receive buffer not empty |
| `SPI_I2S_FLAG_BSY` | Busy flag |
| `SPI_FLAG_MODF` | Mode fault |
| `SPI_FLAG_CRCERR` | CRC error |
| `SPI_I2S_FLAG_OVR` | Overrun |

## DMA Transfer Example

```c
void SPI1_DMA_Init(void)
{
    SPI1_Master_Init();

    // Enable DMA for TX and RX
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);

    // Configure DMA for SPI1_TX (DMA1_Channel3)
    // Configure DMA for SPI1_RX (DMA1_Channel2)
    // ... (similar to USART DMA pattern)
}
```
