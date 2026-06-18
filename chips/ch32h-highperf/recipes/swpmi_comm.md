# SWPMI Single Wire Protocol

## Overview

CH32H417 has an SWPMI (Single Wire Protocol Master Interface) for communication with SWP-compliant smart cards (UICC/SIM). SWPMI uses a single bidirectional data line with Manchester encoding at up to 2 Mbps. Supports ACT (activation), U-Frame, and I-Frame communication.

## Key API Functions

```c
void SWPMI_DeInit(void);
void SWPMI_Init(SWPMI_InitTypeDef* SWPMI_InitStruct);
void SWPMI_Cmd(FunctionalState NewState);
void SWPMI_ActivateCmd(FunctionalState NewState);
void SWPMI_TransmitData32(uint32_t Data);
uint32_t SWPMI_ReceiveData32(void);
FlagStatus SWPMI_GetFlagStatus(uint32_t SWPMI_FLAG);
void SWPMI_ClearFlag(uint32_t SWPMI_FLAG);
void SWPMI_ITConfig(uint32_t SWPMI_IT, FunctionalState NewState);
```

## SWPMI Initialization

```c
#include "ch32h417.h"

#define SWPMI_BITRATE  1000000  // 1 Mbps

void SWPMI_Function_Init(void)
{
    SWPMI_InitTypeDef SWPMI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SWPMI, ENABLE);

    SWPMI_InitStructure.BitRate = SWPMI_BITRATE;
    SWPMI_InitStructure.RxBufferingMode = DISABLE;
    SWPMI_InitStructure.TxBufferingMode = DISABLE;
    SWPMI_InitStructure.LoopBackMode = DISABLE;
    SWPMI_Init(&SWPMI_InitStructure);

    // Enable transceiver
    SWPMI_Cmd(ENABLE);

    // Wait for ready flag before activating bus
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_RDY) == RESET);

    // Activate SWPMI
    SWPMI_ActivateCmd(ENABLE);
}
```

## GPIO Configuration (PC6)

```c
void SWPMI_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOC, ENABLE);

    // First pull low to reset I/O state
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    Delay_Ms(500);

    // Then configure as alternate function
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
```

## Smart Card Communication Example

```c
void SWPMI_SmartCard_Comm(void)
{
    uint32_t RxBuffer[8] = {0};
    uint8_t cnt = 0;

    SWPMI_GPIO_Init();
    SWPMI_Function_Init();

    // Wait for ACT_SYNC_ID from card
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    // Send ACT_POWER_MODE (Low power)
    uint32_t TxBuffer_ACT = 0x00006202;
    SWPMI_TransmitData32(TxBuffer_ACT);
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);

    // Wait for card response
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    // Send U-Frame RESET
    uint32_t TxBuffer_RESET = 0x0004F903;
    SWPMI_TransmitData32(TxBuffer_RESET);
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    // Send I-Frame OPEN_PIPE
    uint32_t TxBuffer_OPEN = 0x03818003;
    SWPMI_TransmitData32(TxBuffer_OPEN);
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_TXE) == RESET);
    while (SWPMI_GetFlagStatus(SWPMI_FLAG_RXNE) == RESET);
    RxBuffer[cnt++] = SWPMI_ReceiveData32();

    printf("Response[0]: %08x\r\n", RxBuffer[0]);
    printf("Response[1]: %08x\r\n", RxBuffer[1]);
    printf("Response[2]: %08x\r\n", RxBuffer[2]);
    printf("Response[3]: %08x\r\n", RxBuffer[3]);
}
```

## SWPMI Frame Types

| Frame | Description |
|-------|-------------|
| ACT-Frame | Activation frame (power mode) |
| U-Frame | Unnumbered frame (reset, etc.) |
| I-Frame | Information frame (data transfer) |

## SWPMI Flags

| Flag | Description |
|------|-------------|
| `SWPMI_FLAG_RXNE` | Receive buffer not empty |
| `SWPMI_FLAG_TXE` | Transmit buffer empty |
| `SWPMI_FLAG_RDY` | Transceiver ready |
| `SWPMI_FLAG_TC` | Transfer complete |

## Important Notes

- The SWPMI data line is on PC6 with AF11
- Must pull PC6 low first to reset I/O state before activating
- Wait for `SWPMI_FLAG_RDY` before calling `SWPMI_ActivateCmd()`
- Bit rate is configurable up to 2 Mbps
