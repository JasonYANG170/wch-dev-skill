# Recipe: UART Communication

## Scenario
Initialize USART for serial communication -- transmit, receive, interrupt-driven, and DMA modes.

## API Quick Reference

### USART Init Structure
```c
typedef struct
{
    uint32_t USART_BaudRate;              // e.g., 115200
    uint16_t USART_WordLength;            // USART_WordLength_8b, USART_WordLength_9b
    uint16_t USART_StopBits;              // USART_StopBits_1, USART_StopBits_2
    uint16_t USART_Parity;                // USART_Parity_No, USART_Parity_Even, USART_Parity_Odd
    uint16_t USART_Mode;                  // USART_Mode_Tx, USART_Mode_Rx, (both: USART_Mode_Tx|USART_Mode_Rx)
    uint16_t USART_HardwareFlowControl;   // USART_HardwareFlowControl_None
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

### Common Flags
```c
USART_FLAG_TXE   // Transmit data register empty
USART_FLAG_TC    // Transmission complete
USART_FLAG_RXNE  // Read data register not empty
USART_FLAG_ORE   // Overrun error
USART_FLAG_FE    // Framing error
USART_FLAG_PE    // Parity error
```

### Common Interrupts
```c
USART_IT_TXE     // TX empty interrupt
USART_IT_TC      // TX complete interrupt
USART_IT_RXNE    // RX not empty interrupt
USART_IT_IDLE    // IDLE line detected
USART_IT_ERR     // Error interrupt
```

### Default Pins (CH32V003)
| Signal | Pin |
|--------|-----|
| USART1_TX | PD5 |
| USART1_RX | PD6 |

## Complete Call Chains

### Polling Transmit
```c
void USART1_Init_Polling(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    // TX pin - AF push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // RX pin - floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // USART config
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

// Send a byte
void USART1_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// Send string
void USART1_SendString(char *str)
{
    while(*str)
    {
        USART1_SendByte(*str++);
    }
}
```

### Interrupt-Driven Receive
```c
volatile uint8_t rx_buffer[64];
volatile uint8_t rx_count = 0;

void USART1_Init_Interrupt(void)
{
    // ... same GPIO and USART init as polling ...

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        rx_buffer[rx_count++] = USART_ReceiveData(USART1);
        if(rx_count >= sizeof(rx_buffer)) rx_count = 0;
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

    if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
}
```

### DMA Transmit
```c
void USART1_DMA_Init(void)
{
    // ... GPIO and USART init ...

    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);  // USART1_TX = DMA1 Channel 4
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)tx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = tx_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel4, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}
```

## Debug Print (printf redirection)

The `debug.h` header provides `USART_Printf_Init()` which redirects `printf()` to USART1:

```c
#include "debug.h"

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("Hello CH32V003!\r\n");
    while(1);
}
```

## Common Errors

1. **TX pin not configured as AF_PP** -- Output won't work with `GPIO_Mode_Out_PP`
2. **RX pin not configured as IN_FLOATING** -- Won't receive data
3. **Baud rate mismatch** -- Both ends must use same baud rate
4. **Forgetting to clear overrun flag** -- `USART_IT_ORE` can block further reception
