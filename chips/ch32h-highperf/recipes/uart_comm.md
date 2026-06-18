# USART/UART Communication

## Overview

CH32H417 has up to 8 USART/UART interfaces (USART1-3, UART4-8). USART1-3 support synchronous mode, hardware flow control (RTS/CTS), LIN, SmartCard, and IrDA. UART4-8 are asynchronous only.

## Key API Functions

```c
void USART_DeInit(USART_TypeDef* USARTx);
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
void USART_StructInit(USART_InitTypeDef* USART_InitStruct);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
```

## Basic Polling Example

```c
#include "ch32h417.h"

void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    USART_InitTypeDef USART_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = USART1_TX (AF7)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF7);

    // PA10 = USART1_RX (AF7)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF7);

    // USART configuration
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendByte(uint8_t data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}

uint8_t USART1_RecvByte(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART1);
}

void USART1_SendString(const char *str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}
```

## Interrupt-Driven Example

```c
volatile uint8_t rx_buffer[256];
volatile uint16_t rx_head = 0, rx_tail = 0;

void USART1_IT_Init(uint32_t baudrate)
{
    USART1_Init(baudrate);  // Basic init first

    // Enable RXNE interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Configure NVIC
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        rx_buffer[rx_head++] = (uint8_t)USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

    if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
}
```

## DMA-Driven Example

```c
#define RX_DMA_BUF_SIZE 256
uint8_t rx_dma_buf[RX_DMA_BUF_SIZE];

void USART1_DMA_Init(uint32_t baudrate)
{
    USART1_Init(baudrate);

    // Enable DMA
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    // Configure DMA1 Channel5 for USART1_RX
    DMA_InitTypeDef DMA_InitStruct = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)rx_dma_buf;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = RX_DMA_BUF_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);
    DMA_Cmd(DMA1_Channel5, ENABLE);
}
```

## Word Length Options

| Macro | Description |
|-------|-------------|
| `USART_WordLength_5b` | 5 data bits |
| `USART_WordLength_6b` | 6 data bits |
| `USART_WordLength_7b` | 7 data bits |
| `USART_WordLength_8b` | 8 data bits (default) |
| `USART_WordLength_9b` | 9 data bits |

## Status Flags

| Flag | Description |
|------|-------------|
| `USART_FLAG_TXE` | Transmit data register empty |
| `USART_FLAG_TC` | Transmission complete |
| `USART_FLAG_RXNE` | Receive data register not empty |
| `USART_FLAG_IDLE` | Idle line detected |
| `USART_FLAG_ORE` | Overrun error |
| `USART_FLAG_FE` | Framing error |
| `USART_FLAG_PE` | Parity error |
| `USART_FLAG_CTS` | CTS flag |
