# DMA Transfer

## Overview

CH32H417 has DMA1 and DMA2, each with 8 channels. A DMAMUX (DMA Multiplexer) allows flexible mapping of peripheral requests to DMA channels.

## Key API Functions

```c
void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx);
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber);
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(DMA_TypeDef* DMAx, uint32_t DMAy_FLAG);
void DMA_ClearFlag(DMA_TypeDef* DMAx, uint32_t DMAy_FLAG);
ITStatus DMA_GetITStatus(DMA_TypeDef* DMAx, uint32_t DMAy_IT);
void DMA_ClearITPendingBit(DMA_TypeDef* DMAx, uint32_t DMAy_IT);
void DMA_MuxChannelConfig(uint8_t DMA_MuxChannelx, uint32_t DMA_Requestx);
```

## DMA Init Structure

```c
typedef struct {
    uint32_t DMA_PeripheralBaseAddr;    // Peripheral base address
    uint32_t DMA_Memory0BaseAddr;       // Memory base address
    uint32_t DMA_DIR;                   // Transfer direction
    uint32_t DMA_BufferSize;            // Number of data items
    uint32_t DMA_PeripheralInc;         // Peripheral address increment
    uint32_t DMA_MemoryInc;             // Memory address increment
    uint32_t DMA_PeripheralDataSize;    // Peripheral data width
    uint32_t DMA_MemoryDataSize;        // Memory data width
    uint32_t DMA_Mode;                  // Normal or Circular
    uint32_t DMA_Priority;              // Channel priority
    uint32_t DMA_M2M;                   // Memory-to-memory mode
    uint32_t DMA_BufferMode;            // Single or Double buffer
    uint32_t DMA_Memory1BaseAddr;       // Second memory address (double buffer)
    uint32_t DMA_DoubleBuffer_StartMemory; // Starting buffer
} DMA_InitTypeDef;
```

## Memory-to-Peripheral Example (USART TX)

```c
#include "ch32h417.h"

uint8_t tx_data[] = "Hello DMA!\r\n";

void USART1_DMA_TX_Init(void)
{
    // Enable clocks
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Map DMA channel for USART1_TX via DMAMUX
    DMA_MuxChannelConfig(DMA_MuxChannel4, DMA_Request_USART1_TX);

    DMA_InitTypeDef DMA_InitStruct = {0};
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)tx_data;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = sizeof(tx_data) - 1;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);

    // Enable USART DMA TX
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}
```

## Peripheral-to-Memory Example (ADC)

```c
#define ADC_DMA_SIZE 100
uint16_t adc_dma_buf[ADC_DMA_SIZE];

void ADC1_DMA_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Map DMA channel for ADC1
    DMA_MuxChannelConfig(DMA_MuxChannel1, DMA_Request_ADC1);

    DMA_InitTypeDef DMA_InitStruct = {0};
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->RDATAR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)adc_dma_buf;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = ADC_DMA_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    ADC_DMACmd(ADC1, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}
```

## Memory-to-Memory Example

```c
void MemCopy_DMA(uint32_t *src, uint32_t *dst, uint32_t len)
{
    DMA_InitTypeDef DMA_InitStruct = {0};

    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)src;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)dst;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = len;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel1, ENABLE);

    // Wait for completion
    while(DMA_GetCurrDataCounter(DMA1_Channel1) > 0);
}
```

## DMA Interrupt

```c
void DMA1_Channel1_IT_Init(void)
{
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);  // Transfer Complete

    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1, DMA1_IT_TC1))
    {
        // Transfer complete
        DMA_ClearITPendingBit(DMA1, DMA1_IT_TC1);
    }
}
```

## DMA Data Size Options

| Macro | Size |
|-------|------|
| `DMA_PeripheralDataSize_Byte` | 8-bit |
| `DMA_PeripheralDataSize_HalfWord` | 16-bit |
| `DMA_PeripheralDataSize_Word` | 32-bit |

## DMA Priority

| Macro | Priority |
|-------|----------|
| `DMA_Priority_Low` | Lowest |
| `DMA_Priority_Medium` | Medium |
| `DMA_Priority_High` | High |
| `DMA_Priority_VeryHigh` | Highest |

## Double Buffer Mode

```c
DMA_InitStruct.DMA_BufferMode = DMA_DoubleBufferMode;
DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)buffer0;
DMA_InitStruct.DMA_Memory1BaseAddr = (uint32_t)buffer1;
DMA_InitStruct.DMA_DoubleBuffer_StartMemory = DMA_DoubleBufferMode_Memory_0;
```
