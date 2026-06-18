# Recipe: CAN Bus Communication

## When to Use
User wants to implement CAN bus communication for industrial control, automotive, or sensor networks.

## API Reference (from ch32v20x_can.h)

### CAN Init Structure
```c
typedef struct {
    uint16_t CAN_Prescaler;     // Baud rate prescaler (1-1024)
    uint8_t CAN_Mode;           // CAN_Mode_Normal, _LoopBack, _Silent, _Silent_LoopBack
    uint8_t CAN_SJW;            // Sync jump width: CAN_SJW_1tq .. _4tq
    uint8_t CAN_BS1;            // Bit segment 1: CAN_BS1_1tq .. _16tq
    uint8_t CAN_BS2;            // Bit segment 2: CAN_BS2_1tq .. _8tq
    FunctionalState CAN_TTCM;   // Time triggered communication
    FunctionalState CAN_ABOM;   // Automatic bus-off management
    FunctionalState CAN_AWUM;   // Automatic wake-up mode
    FunctionalState CAN_NART;   // No automatic retransmission
    FunctionalState CAN_RFLM;   // Receive FIFO locked mode
    FunctionalState CAN_TXFP;   // Transmit FIFO priority
} CAN_InitTypeDef;
```

### CAN Filter Init Structure
```c
typedef struct {
    uint16_t CAN_FilterIdHigh;          // Filter ID high (MSBs)
    uint16_t CAN_FilterIdLow;           // Filter ID low (LSBs)
    uint16_t CAN_FilterMaskIdHigh;      // Filter mask high
    uint16_t CAN_FilterMaskIdLow;       // Filter mask low
    uint16_t CAN_FilterFIFOAssignment;  // CAN_Filter_FIFO0 or _FIFO1
    uint8_t CAN_FilterNumber;           // Filter index (0-13)
    uint8_t CAN_FilterMode;             // CAN_FilterMode_IdMask or _IdList
    uint8_t CAN_FilterScale;            // CAN_FilterScale_16bit or _32bit
    FunctionalState CAN_FilterActivation; // ENABLE or DISABLE
} CAN_FilterInitTypeDef;
```

### CAN Tx/Rx Message
```c
typedef struct {
    uint32_t StdId;    // Standard ID (0-0x7FF)
    uint32_t ExtId;    // Extended ID (0-0x1FFFFFFF)
    uint8_t IDE;       // CAN_Id_Standard or CAN_Id_Extended
    uint8_t RTR;       // CAN_RTR_Data or CAN_RTR_Remote
    uint8_t DLC;       // Data length (0-8)
    uint8_t Data[8];   // Data bytes
} CanTxMsg;

typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
    uint8_t FMI;       // Filter match index
} CanRxMsg;
```

### Key Functions
```c
uint8_t    CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct);
void       CAN_FilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct);
uint8_t    CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage);
uint8_t    CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox);
void       CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage);
void       CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber);
uint8_t    CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber);
void       CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState);
```

### CAN Bit Timing Calculation
```
CAN_BaudRate = APB1_Clock / (Prescaler * (1 + BS1 + BS2))
Example: 36MHz / (18 * (1 + 5 + 2)) = 250kbps
```

## Example: CAN1 Loopback Test

```c
#include "ch32v20x.h"

void CAN1_Loopback_Init(void)
{
    CAN_InitTypeDef CAN_InitStruct;
    CAN_FilterInitTypeDef CAN_FilterStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // CAN1 TX - PA12 (AF push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CAN1 RX - PA11 (floating input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CAN init (250kbps @ 36MHz APB1)
    CAN_InitStruct.CAN_Prescaler = 18;
    CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack;
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_5tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq;
    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = DISABLE;
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;
    CAN_Init(CAN1, &CAN_InitStruct);

    // Filter: accept all messages
    CAN_FilterStruct.CAN_FilterNumber = 0;
    CAN_FilterStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterStruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterStruct.CAN_FilterIdHigh = 0x0000;
    CAN_FilterStruct.CAN_FilterIdLow = 0x0000;
    CAN_FilterStruct.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterStruct.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterStruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterStruct);
}

uint8_t CAN1_SendMessage(uint32_t id, uint8_t *data, uint8_t len)
{
    CanTxMsg TxMessage;
    uint8_t mailbox;

    TxMessage.StdId = id;
    TxMessage.ExtId = 0;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = len;
    for(int i = 0; i < len; i++) TxMessage.Data[i] = data[i];

    mailbox = CAN_Transmit(CAN1, &TxMessage);
    if(mailbox == CAN_TxStatus_NoMailBox) return 0xFF;

    // Wait for transmission
    while(CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok);
    return mailbox;
}

uint8_t CAN1_ReceiveMessage(CanRxMsg *RxMessage)
{
    if(CAN_MessagePending(CAN1, CAN_FIFO0) == 0) return 0;

    CAN_Receive(CAN1, CAN_FIFO0, RxMessage);
    CAN_FIFORelease(CAN1, CAN_FIFO0);
    return 1;
}
```

## CAN Pin Remapping

| CAN | Default Pins | Remap1 | Remap2 |
|-----|-------------|--------|--------|
| CAN1 | PA11(RX)/PA12(TX) | PB8(RX)/PB9(TX) | -- |

## Pitfalls
- **Filter must be configured** -- without a filter, no messages are received
- **Baud rate must match** -- all nodes on the bus must use identical bit timing
- **CAN1 on APB1** -- clock is typically 36MHz (not 72MHz)
- **Check mailbox availability** -- `CAN_Transmit` returns `CAN_TxStatus_NoMailBox` if all 3 mailboxes are full
- **FIFO release required** -- call `CAN_FIFORelease()` after `CAN_Receive()` to free the FIFO slot
