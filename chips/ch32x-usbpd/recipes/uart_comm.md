# USART Communication

> **Summary**: Initialize and use USART for serial communication on CH32X/CH6xx chips.

## Trigger Intent

- "UART communication"
- "Serial printf"
- "USART send receive"
- "Debug serial output"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | APB2 for USART1, APB1 for USART2/3/4 |
| Reference | `CH32X035EVT/EVT/EXAM/USART/` |

## Step-by-Step

### USART1 Polling Transmit

```c
#include "ch32x035.h"
#include "debug.h"

void USART1_Init(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    USART_InitTypeDef USART_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = USART1 TX (alternate function push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA8 = USART1 RX (floating input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // USART configuration
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendByte(uint8_t data) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}

void USART1_SendString(const char *str) {
    while(*str) {
        USART1_SendByte(*str++);
    }
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();

    USART1_Init(115200);
    USART1_SendString("USART1 polling example\r\n");

    while(1) {
        USART1_SendString("Hello\r\n");
        Delay_Ms(1000);
    }
}
```

### USART1 Interrupt-Driven Receive

```c
volatile uint8_t rx_buffer[64];
volatile uint8_t rx_index = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t data = (uint8_t)USART_ReceiveData(USART1);
        if(rx_index < sizeof(rx_buffer) - 1) {
            rx_buffer[rx_index++] = data;
            rx_buffer[rx_index] = 0;  // Null terminate
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// After USART1_Init():
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

NVIC_InitTypeDef NVIC_InitStruct = {0};
NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStruct);
```

### Using debug.h for printf

```c
#include "ch32x035.h"
#include "debug.h"

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);  // Init USART1 for printf

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("Value:%d\r\n", 42);

    while(1);
}
```

## USART Flag Reference

| Flag | Constant | Description |
|------|----------|-------------|
| TXE | `USART_FLAG_TXE` | Transmit data register empty |
| TC | `USART_FLAG_TC` | Transmission complete |
| RXNE | `USART_FLAG_RXNE` | Receive data register not empty |
| IDLE | `USART_FLAG_IDLE` | Idle line detected |
| ORE | `USART_FLAG_ORE` | Overrun error |

## USART Instance Availability

| USART | TX Pin | RX Pin | Clock Bus |
|-------|--------|--------|-----------|
| USART1 | PA9 | PA8 | APB2 |
| USART2 | PA2 | PA3 | APB1 |
| USART3 | PB10 | PB11 | APB1 |
| USART4 | PB0 | PB1 | APB1 |

All USARTs support pin remap. Check `GPIO_PinRemapConfig()` for alternate pin assignments.

## Common Errors

- Using `GPIO_Mode_Out_PP` for TX pin -- must use `GPIO_Mode_AF_PP`
- Not checking `USART_FLAG_TXE` before `USART_SendData()` -- data overrun
- Wrong clock bus (APB2 vs APB1) for `RCC_APBxPeriphClockCmd()`
