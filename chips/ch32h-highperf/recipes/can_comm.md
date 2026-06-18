# CAN Bus Communication

## Overview

CH32H417 has up to 3 CAN controllers (CAN1-3) supporting CAN 2.0B protocol with up to 1Mbps data rate. Each controller has 28 filters, 3 transmit mailboxes, and 2 receive FIFOs.

## Key API Functions

```c
void CAN_DeInit(CAN_TypeDef *CANx);
uint8_t CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct);
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);
void CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct);
uint8_t CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage);
uint8_t CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox);
void CAN_CancelTransmit(CAN_TypeDef *CANx, uint8_t Mailbox);
void CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage);
void CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber);
uint8_t CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber);
void CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState);
FlagStatus CAN_GetFlagStatus(CAN_TypeDef *CANx, uint32_t CAN_FLAG);
void CAN_ClearFlag(CAN_TypeDef *CANx, uint32_t CAN_FLAG);
ITStatus CAN_GetITStatus(CAN_TypeDef *CANx, uint32_t CAN_IT);
void CAN_ClearITPendingBit(CAN_TypeDef *CANx, uint32_t CAN_IT);
void CAN_SlaveStartBank(uint8_t CAN2_BankNumber, uint8_t CAN3_BankNumber);
```

## CAN Initialization Example

```c
#include "ch32h417.h"
#include "ch32h417_can.h"

void CAN1_Init_500k(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    CAN_InitTypeDef CAN_InitStruct = {0};
    CAN_FilterInitTypeDef CAN_FilterInitStruct = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // PA11 = CAN1_RX, PA12 = CAN1_TX (AF9)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF9);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF9);

    // CAN configuration: 500kbps at 120MHz APB1
    CAN_InitStruct.CAN_Prescaler = 12;     // 120MHz / 12 / (1+6+1) = 1.25MHz... adjust as needed
    CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = ENABLE;       // Auto bus-off recovery
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;      // Auto retransmission
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;
    CAN_Init(CAN1, &CAN_InitStruct);

    // Filter: accept all messages
    CAN_FilterInitStruct.CAN_FilterNumber = 0;
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStruct);
}
```

## Send CAN Message

```c
void CAN1_SendMessage(uint32_t id, uint8_t *data, uint8_t len)
{
    CanTxMsg TxMessage;
    TxMessage.StdId = id;
    TxMessage.ExtId = 0;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = len;
    memcpy(TxMessage.Data, data, len);

    uint8_t mailbox = CAN_Transmit(CAN1, &TxMessage);

    // Wait for transmission
    while(CAN_TransmitStatus(CAN1, mailbox) == CAN_TxStatus_Pending);
}
```

## Receive CAN Message

```c
void CAN1_ReceiveMessage(CanRxMsg *RxMessage)
{
    if(CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
    {
        CAN_Receive(CAN1, CAN_FIFO0, RxMessage);
        CAN_FIFORelease(CAN1, CAN_FIFO0);
    }
}
```

## CAN Interrupt Example

```c
void CAN1_IT_Init(void)
{
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);  // FIFO0 message pending

    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_Init(&NVIC_InitStruct);
}

void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

    // Process received message
    if(RxMessage.IDE == CAN_Id_Standard)
    {
        // Standard ID message
    }

    CAN_FIFORelease(CAN1, CAN_FIFO0);
}
```

## Multi-CAN Filter Configuration

```c
// Filter for specific ID (e.g., 0x123)
CAN_FilterInitStruct.CAN_FilterNumber = 0;
CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
CAN_FilterInitStruct.CAN_FilterIdHigh = (0x123 << 5);  // Standard ID in upper bits
CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
CAN_FilterInitStruct.CAN_FilterMaskIdHigh = (0x7FF << 5);  // Exact match
CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
CAN_FilterInit(&CAN_FilterInitStruct);

// Split filters between CAN2 and CAN3
CAN_SlaveStartBank(14, 21);  // CAN2 starts at bank 14, CAN3 at bank 21
```

## CAN Operating Modes

| Mode | Description |
|------|-------------|
| `CAN_Mode_Normal` | Normal operation |
| `CAN_Mode_LoopBack` | Loopback mode (TX internally connected to RX) |
| `CAN_Mode_Silent` | Silent mode (monitor only) |
| `CAN_Mode_Silent_LoopBack` | Combined loopback + silent |

## CAN Bit Timing

For 500kbps at 120MHz APB1 clock:
```
Prescaler = 12
BS1 = 6tq, BS2 = 5tq, SJW = 1tq
Bit time = (1 + 6 + 5) * (1/10MHz) = 1.2us -> 833kbps... adjust for actual clock
```

## Error Handling

```c
uint8_t error_code = CAN_GetLastErrorCode(CAN1);
if(error_code != CAN_ErrorCode_NoErr)
{
    // Handle error
    // CAN_ErrorCode_StuffErr, CAN_ErrorCode_FormErr, etc.
}

uint8_t tx_err = CAN_GetLSBTransmitErrorCounter(CAN1);
uint8_t rx_err = CAN_GetReceiveErrorCounter(CAN1);
```
