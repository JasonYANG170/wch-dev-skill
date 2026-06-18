# Recipe: SPI Communication

## Overview

Configure SPI for master/slave communication on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

// Data transfer
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);

// Interrupt and DMA
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);

// Status
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
```

## SPI Init Structure

```c
typedef struct {
    uint16_t SPI_Direction;           // SPI_Direction_2Lines_FullDuplex, _2Lines_RxOnly, _1Line_Rx, _1Line_Tx
    uint16_t SPI_Mode;                // SPI_Mode_Master or SPI_Mode_Slave
    uint16_t SPI_DataSize;            // SPI_DataSize_8b or SPI_DataSize_16b
    uint16_t SPI_CPOL;                // SPI_CPOL_Low or SPI_CPOL_High
    uint16_t SPI_CPHA;                // SPI_CPHA_1Edge or SPI_CPHA_2Edge
    uint16_t SPI_NSS;                 // SPI_NSS_Soft or SPI_NSS_Hard
    uint16_t SPI_BaudRatePrescaler;   // SPI_BaudRatePrescaler_2 to _256
    uint16_t SPI_FirstBit;            // SPI_FirstBit_MSB or SPI_FirstBit_LSB
    uint16_t SPI_CRCPolynomial;       // CRC polynomial (if CRC enabled)
} SPI_InitTypeDef;
```

## SPI Mode Summary

| CPOL | CPHA | Mode | Description |
|------|------|------|-------------|
| 0 | 0 | Mode 0 | Clock idle low, sample on rising edge |
| 0 | 1 | Mode 1 | Clock idle low, sample on falling edge |
| 1 | 0 | Mode 2 | Clock idle high, sample on falling edge |
| 1 | 1 | Mode 3 | Clock idle high, sample on rising edge |

## Example: SPI1 Master

```c
#include "debug.h"

void SPI1_Master_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // PA5 = SPI1_SCK (AF push-pull)
    // PA6 = SPI1_MISO (floating input)
    // PA7 = SPI1_MOSI (AF push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA4 = NSS (software controlled, push-pull output)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); // Deselect slave

    // Configure SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_TransferByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

// Usage: Read SPI device register
uint8_t SPI_ReadRegister(uint8_t reg)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);  // Select slave
    SPI1_TransferByte(reg | 0x80);       // Send register address with read bit
    uint8_t value = SPI1_TransferByte(0xFF); // Read data
    GPIO_SetBits(GPIOA, GPIO_Pin_4);    // Deselect slave
    return value;
}
```

## Available SPI Instances

| Instance | Bus | SCK | MISO | MOSI |
|----------|-----|-----|------|------|
| SPI1 | APB2 | PA5 | PA6 | PA7 |
| SPI2 | APB1 | PB13 | PB14 | PB15 |
| SPI3 | APB1 | PB3 | PB4 | PB5 |
