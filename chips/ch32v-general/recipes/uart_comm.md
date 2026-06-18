# Recipe: USART Communication

## When to Use
User wants to implement UART/USART communication (serial TX/RX, printf debug, interrupt-driven).

## API Reference (from ch32v20x_usart.h)

### Init Structure
```c
typedef struct {
    uint32_t USART_BaudRate;              // Baud rate (e.g., 115200)
    uint16_t USART_WordLength;            // USART_WordLength_8b, USART_WordLength_9b
    uint16_t USART_StopBits;              // USART_StopBits_1, USART_StopBits_2
    uint16_t USART_Parity;                // USART_Parity_No, USART_Parity_Even, USART_Parity_Odd
    uint16_t USART_Mode;                  // USART_Mode_Tx | USART_Mode_Rx
    uint16_t USART_HardwareFlowControl;   // USART_HardwareFlowControl_None, _RTS, _CTS, _RTS_CTS
} USART_InitTypeDef;
```

### Key Functions
```c
void       USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct);
void       USART_Cmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_SendData(USART_TypeDef *USARTx, uint16_t Data);
uint16_t   USART_ReceiveData(USART_TypeDef *USARTx);
void       USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);
ITStatus   USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT);
void       USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT);
void       USART_DMACmd(USART_TypeDef *USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
```

### USART Interrupt Flags
| Flag | Description |
|------|-------------|
| `USART_IT_RXNE` | Receive data register not empty |
| `USART_IT_TXE` | Transmit data register empty |
| `USART_IT_TC` | Transmission complete |
| `USART_IT_IDLE` | Idle line detected |
| `USART_IT_ORE` | Overrun error |

### USART Status Flags
| Flag | Description |
|------|-------------|
| `USART_FLAG_TXE` | TX register empty |
| `USART_FLAG_TC` | Transmission complete |
| `USART_FLAG_RXNE` | RX register not empty |
| `USART_FLAG_ORE` | Overrun error |
| `USART_FLAG_FE` | Framing error |
| `USART_FLAG_PE` | Parity error |

## Example: USART1 Polling

```c
#include "ch32v20x.h"

void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // TX - PA9 (AF push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RX - PA10 (floating input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART1_SendString(const char *str)
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

## Example: USART1 Interrupt-Driven RX

```c
#include "ch32v20x.h"
#include <string.h>

#define RX_BUF_SIZE 256
static uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = (uint8_t)USART_ReceiveData(USART1);
        uint16_t next = (rx_head + 1) % RX_BUF_SIZE;
        if(next != rx_tail)
        {
            rx_buf[rx_head] = data;
            rx_head = next;
        }
    }
}

void USART1_RX_Interrupt_Init(uint32_t baudrate)
{
    NVIC_InitTypeDef NVIC_InitStruct;

    // ... GPIO and USART init as above ...

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

int USART1_Available(void)
{
    return (rx_head - rx_tail + RX_BUF_SIZE) % RX_BUF_SIZE;
}

uint8_t USART1_Read(void)
{
    while(rx_head == rx_tail);
    uint8_t data = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return data;
}
```

## Example: USART1 TX with DMA

```c
void USART1_DMA_TX_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA1 Channel 4 = USART1 TX
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)tx_buffer;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = buffer_size;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);

    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

void USART1_DMA_Send(uint16_t length)
{
    DMA_SetCurrDataCounter(DMA1_Channel4, length);
    DMA_Cmd(DMA1_Channel4, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
}
```

## USART Instance Pins

| USART | TX Pin | RX Pin | APB |
|-------|--------|--------|-----|
| USART1 | PA9 | PA10 | APB2 |
| USART2 | PA2 | PA3 | APB1 |
| USART3 | PB10 | PB11 | APB1 |
| UART4 | PC10 | PC11 | APB1 |
| UART5 | PC12 | PD2 | APB1 |

## Pitfalls
- **USART1 is on APB2**, USART2-5 are on APB1 -- different RCC enable functions
- **TX pin must be AF_PP**, RX pin must be IN_FLOATING
- **Baud rate calculation** depends on APB clock -- verify `SystemCoreClock` and APB prescaler
