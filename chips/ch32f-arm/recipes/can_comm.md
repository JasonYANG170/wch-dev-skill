# Recipe: CAN Communication

## Overview

Configure CAN bus for communication on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void CAN_DeInit(CAN_TypeDef* CANx);
uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct);
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct);
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);

// Transmit
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage);
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox);
void CAN_CancelTransmit(CAN_TypeDef* CANx, uint8_t Mailbox);

// Receive
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage);
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber);
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber);

// Interrupt
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState);
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG);
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG);
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT);
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT);
```

## CAN Init Structure

```c
typedef struct {
    uint16_t CAN_Prescaler;      // Time quantum prescaler (1-1024)
    uint8_t CAN_Mode;            // CAN_Mode_Normal, _LoopBack, _Silent, _Silent_LoopBack
    uint8_t CAN_SJW;             // Synchronization jump width (1-4 time quanta)
    uint8_t CAN_BS1;             // Bit segment 1 (1-16 time quanta)
    uint8_t CAN_BS2;             // Bit segment 2 (1-8 time quanta)
    FunctionalState CAN_TTCM;    // Time triggered communication mode
    FunctionalState CAN_ABOM;    // Automatic bus-off management
    FunctionalState CAN_AWUM;    // Automatic wake-up mode
    FunctionalState CAN_NART;    // No automatic retransmission
    FunctionalState CAN_RFLM;    // Receive FIFO locked mode
    FunctionalState CAN_TXFP;    // Transmit FIFO priority
} CAN_InitTypeDef;
```

## CAN Filter Init Structure

```c
typedef struct {
    uint16_t CAN_FilterIdHigh;          // Filter ID high (MSBs)
    uint16_t CAN_FilterIdLow;           // Filter ID low (LSBs)
    uint16_t CAN_FilterMaskIdHigh;      // Filter mask high
    uint16_t CAN_FilterMaskIdLow;       // Filter mask low
    uint16_t CAN_FilterFIFOAssignment;  // CAN_Filter_FIFO0 or _FIFO1
    uint8_t CAN_FilterNumber;           // Filter number (0-13)
    uint8_t CAN_FilterMode;             // CAN_FilterMode_IdMask or _IdList
    uint8_t CAN_FilterScale;            // CAN_FilterScale_16bit or _32bit
    FunctionalState CAN_FilterActivation; // ENABLE or DISABLE
} CAN_FilterInitTypeDef;
```

## CAN Message Structures

```c
typedef struct {
    uint32_t StdId;     // Standard ID (0-0x7FF)
    uint32_t ExtId;     // Extended ID (0-0x1FFFFFFF)
    uint8_t IDE;        // CAN_Id_Standard or CAN_Id_Extended
    uint8_t RTR;        // CAN_RTR_Data or CAN_RTR_Remote
    uint8_t DLC;        // Data length (0-8)
    uint8_t Data[8];    // Data bytes
} CanTxMsg;

typedef struct {
    uint32_t StdId;     // Standard ID
    uint32_t ExtId;     // Extended ID
    uint8_t IDE;        // ID type
    uint8_t RTR;        // Frame type
    uint8_t DLC;        // Data length
    uint8_t Data[8];    // Data bytes
    uint8_t FMI;        // Filter match index
} CanRxMsg;
```

## Baud Rate Calculation

```
CAN_BaudRate = APB1_Clock / (Prescaler * (1 + BS1 + BS2))
```

Example: 36MHz APB1 clock, 500kbps:
- Prescaler = 4, BS1 = CAN_BS1_6tq (6), BS2 = CAN_BS2_5tq (5)
- BaudRate = 36MHz / (4 * (1 + 6 + 5)) = 36MHz / 48 = 750kbps (not exact)

For 500kbps: Prescaler = 9, BS1 = 6, BS2 = 5
- BaudRate = 36MHz / (9 * 12) = 333kbps (still not exact)

Better: Prescaler = 6, BS1 = 7, BS2 = 6
- BaudRate = 36MHz / (6 * 14) = 428kbps

For 1Mbps: Prescaler = 3, BS1 = 6, BS2 = 5
- BaudRate = 36MHz / (3 * 12) = 1Mbps

## Example: CAN1 Loopback Test

```c
#include "debug.h"

void CAN1_Loopback_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    CAN_InitTypeDef CAN_InitStructure = {0};
    CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA11 = CAN1_RX (floating input)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA12 = CAN1_TX (AF push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CAN configuration
    CAN_InitStructure.CAN_Prescaler = 6;
    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;  // Loopback for testing
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_Init(CAN1, &CAN_InitStructure);

    // Filter configuration (accept all)
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}

uint8_t CAN1_SendMessage(uint32_t id, uint8_t* data, uint8_t len)
{
    CanTxMsg TxMessage;
    uint8_t mailbox;

    TxMessage.StdId = id;
    TxMessage.ExtId = 0;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = len;
    for(uint8_t i = 0; i < len; i++)
    {
        TxMessage.Data[i] = data[i];
    }

    mailbox = CAN_Transmit(CAN1, &TxMessage);
    if(mailbox == CAN_TxStatus_NoMailBox)
    {
        return 1;  // No mailbox available
    }

    // Wait for transmission
    uint32_t timeout = 100000;
    while(CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok)
    {
        if(--timeout == 0) return 2;
    }
    return 0;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    CAN1_Loopback_Init();
    printf("CAN Loopback TEST\r\n");

    while(1)
    {
        uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        CAN1_SendMessage(0x123, data, 8);

        // Check for received message
        if(CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
        {
            CanRxMsg RxMessage;
            CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
            printf("Received ID=0x%03X DLC=%d Data=", RxMessage.StdId, RxMessage.DLC);
            for(uint8_t i = 0; i < RxMessage.DLC; i++)
            {
                printf("%02X ", RxMessage.Data[i]);
            }
            printf("\r\n");
        }
        Delay_Ms(1000);
    }
}
```

## CAN Remapping

```c
// Remap CAN1 to PB8/PB9
GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

// Remap CAN1 to PD0/PD1
GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
```
