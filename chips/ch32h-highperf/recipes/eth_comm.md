# Ethernet Communication

## Overview

CH32H417 has an integrated 10/100M Ethernet MAC with an internal PHY. It supports MII and RMII interfaces, DMA descriptors for TX/RX, checksum offload, and PTP (Precision Time Protocol).

## Key API Functions

```c
void ETH_DeInit(void);
void ETH_StructInit(ETH_InitTypeDef* ETH_InitStruct);
void ETH_SoftwareReset(void);
FlagStatus ETH_GetSoftwareResetStatus(void);
FlagStatus ETH_GetlinkStaus(void);
void ETH_Start(void);
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength);
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);
uint32_t ETH_GetRxPktSize(void);
void ETH_DropRxPkt(void);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
void ETH_MACTransmissionCmd(FunctionalState NewState);
void ETH_MACReceptionCmd(FunctionalState NewState);
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);
void ETH_GetMACAddress(uint32_t MacAddr, uint8_t *Addr);
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState);
void RGMII_TXC_Delay(uint8_t clock_polarity, uint8_t delay_time);
```

## Ethernet Initialization Example

```c
#include "ch32h417.h"
#include "ch32h417_eth.h"

// MAC address
uint8_t MAC_ADDR[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

// TX/RX buffers
#define ETH_TX_BUF_NUM 4
#define ETH_RX_BUF_NUM 4
#define ETH_BUF_SIZE   1536

uint8_t ETH_TX_Buf[ETH_TX_BUF_NUM][ETH_BUF_SIZE] __attribute__((aligned(4)));
uint8_t ETH_RX_Buf[ETH_RX_BUF_NUM][ETH_BUF_SIZE] __attribute__((aligned(4)));
ETH_DMADESCTypeDef ETH_TX_Desc[ETH_TX_BUF_NUM];
ETH_DMADESCTypeDef ETH_RX_Desc[ETH_RX_BUF_NUM];

void ETH_Init_Example(void)
{
    ETH_InitTypeDef ETH_InitStruct = {0};

    // Enable clocks
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH, ENABLE);

    // Configure GPIO for ETH (RMII mode)
    // ... GPIO configuration for RMII pins ...

    // ETH configuration
    ETH_InitStruct.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStruct.ETH_Speed = ETH_Speed_100M;
    ETH_InitStruct.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStruct.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStruct.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStruct.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStruct.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    ETH_InitStruct.ETH_StoreForward = ETH_StoreForward_Enable;
    ETH_InitStruct.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    ETH_InitStruct.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;

    // Note: ETH_Init() is typically called with full configuration
    // Refer to the ETH example project for complete initialization

    // Set MAC address
    ETH_MACAddressConfig(ETH_MAC_Address0, MAC_ADDR);

    // Initialize DMA descriptors
    ETH_DMATxDescChainInit(ETH_TX_Desc, (uint8_t*)ETH_TX_Buf, ETH_TX_BUF_NUM);
    ETH_DMARxDescChainInit(ETH_RX_Desc, (uint8_t*)ETH_RX_Buf, ETH_RX_BUF_NUM);

    // Enable DMA interrupts
    ETH_DMAITConfig(ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);

    // Start Ethernet
    ETH_Start();
}
```

## Send Ethernet Frame

```c
uint32_t ETH_SendPacket(uint8_t *pkt, uint16_t len)
{
    return ETH_HandleTxPkt(pkt, len);
}
```

## Receive Ethernet Frame

```c
uint16_t ETH_RecvPacket(uint8_t *pkt)
{
    uint32_t len = ETH_GetRxPktSize();
    if(len > 0)
    {
        ETH_HandleRxPkt(pkt);
        return (uint16_t)len;
    }
    return 0;
}
```

## PHY Register Access

```c
// Read PHY register
uint16_t phy_val = ETH_ReadPHYRegister(0x01, PHY_BSR);  // Read Basic Status

// Check link status
if(phy_val & PHY_Linked_Status)
{
    // Link is up
}

// Write PHY register
ETH_WritePHYRegister(0x01, PHY_BCR, PHY_Reset);  // Reset PHY
```

## DMA Descriptor Structure

```c
typedef struct {
    uint32_t volatile Status;       // Status bits
    uint32_t ControlBufferSize;     // Control and buffer sizes
    uint32_t Buffer1Addr;           // Buffer 1 address
    uint32_t Buffer2NextDescAddr;   // Buffer 2 or next descriptor
} ETH_DMADESCTypeDef;
```

## Common PHY Registers

| Register | Description |
|----------|-------------|
| `PHY_BCR` (0x00) | Basic Control Register |
| `PHY_BSR` (0x01) | Basic Status Register |
| `PHY_PHYIDR1` (0x02) | PHY Identifier 1 |
| `PHY_PHYIDR2` (0x03) | PHY Identifier 2 |
| `PHY_ANAR` (0x04) | Auto-Negotiation Advertisement |
| `PHY_PAG_SEL` (0x1F) | Page Select |

## Ethernet Flags

| Flag | Description |
|------|-------------|
| `ETH_DMA_FLAG_R` | Receive flag |
| `ETH_DMA_FLAG_T` | Transmit flag |
| `ETH_DMA_FLAG_FBE` | Fatal bus error |
| `ETH_DMA_FLAG_NIS` | Normal interrupt summary |
| `ETH_DMA_FLAG_AIS` | Abnormal interrupt summary |

## PTP (Precision Time Protocol)

```c
// Enable PTP timestamping
ETH_PTPTimeStampCmd(ENABLE);

// Set PTP sub-second increment
ETH_SetPTPSubSecondIncrement(0x40);

// Initialize PTP timestamp
ETH_InitializePTPTimeStamp();

// Update PTP time
ETH_SetPTPTimeStampUpdate(ETH_PTP_PositiveTime, seconds, subseconds);
```

## References

- See `CH32H417EVT/EVT/EXAM/ETH/` for complete examples
- ETH header: `SRC/Peripheral/inc/ch32h417_eth.h`
