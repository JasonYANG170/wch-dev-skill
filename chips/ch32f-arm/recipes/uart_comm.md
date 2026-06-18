# Recipe: UART/USART Communication

## Overview

Configure USART for serial communication on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void USART_DeInit(USART_TypeDef* USARTx);
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
void USART_StructInit(USART_InitTypeDef* USART_InitStruct);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);

// Data transfer
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);

// Interrupt configuration
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);

// DMA
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);

// Status flags
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
```

## USART Init Structure

```c
typedef struct {
    uint32_t USART_BaudRate;            // Baud rate (e.g., 115200)
    uint16_t USART_WordLength;          // USART_WordLength_8b or USART_WordLength_9b
    uint16_t USART_StopBits;            // USART_StopBits_1, USART_StopBits_0_5, USART_StopBits_2, USART_StopBits_1_5
    uint16_t USART_Parity;              // USART_Parity_No, USART_Parity_Even, USART_Parity_Odd
    uint16_t USART_Mode;                // USART_Mode_Tx | USART_Mode_Rx
    uint16_t USART_HardwareFlowControl; // USART_HardwareFlowControl_None, _RTS, _CTS, _RTS_CTS
} USART_InitTypeDef;
```

## USART Interrupt Flags

| Flag | Description |
|------|-------------|
| USART_IT_RXNE | Receive data register not empty |
| USART_IT_TXE | Transmit data register empty |
| USART_IT_TC | Transmission complete |
| USART_IT_IDLE | Idle line detected |
| USART_IT_PE | Parity error |
| USART_IT_FE | Framing error |
| USART_IT_NE | Noise error |
| USART_IT_ORE | Overrun error |

## Example: USART1 Polling

```c
#include "debug.h"

void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = USART1_TX (AF push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10 = USART1_RX (floating input)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART1
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART1_SendString(const char* str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}

uint8_t USART1_ReceiveByte(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART1);
}
```

## Example: USART1 Interrupt Receive

```c
static volatile uint8_t rx_buffer[256];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = (uint8_t)USART_ReceiveData(USART1);
        rx_buffer[rx_head++] = data;
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

void USART1_IRQ_Init(uint32_t baudrate)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // ... GPIO and USART init as above ...

    // Enable RXNE interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Configure NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

## Available USART Instances

| Instance | Bus | TX Pin | RX Pin | Notes |
|----------|-----|--------|--------|-------|
| USART1 | APB2 | PA9 | PA10 | Can be remapped to PB6/PB7 |
| USART2 | APB1 | PA2 | PA3 | Can be remapped to PD5/PD6 |
| USART3 | APB1 | PB10 | PB11 | Can be remapped to PD8/PD9 or PC10/PC11 |

## USART Remapping

```c
// Remap USART1 to PB6/PB7
GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

// Partial remap USART3 to PC10/PC11
GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

// Full remap USART3 to PD8/PD9
GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
```
