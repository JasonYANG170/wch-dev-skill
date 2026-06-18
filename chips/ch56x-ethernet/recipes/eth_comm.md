# Recipe: Ethernet Communication

## Overview

Configure and use the integrated 10/100M Ethernet MAC on CH56x chips with DMA
descriptors for frame transmission and reception. Requires an external PHY chip
(e.g., CH182, RTL8201, LAN8720).

## Hardware Requirements

- CH569 evaluation board with Ethernet PHY
- External PHY connected via MII/RMII interface
- 25MHz crystal or clock for PHY
- Ethernet RJ45 connector with magnetics

## Ethernet Architecture

```
CH569 MAC  <---MII/RMII--->  External PHY  <---MDI--->  RJ45
   |                              |
   |  MDC/MDIO (management)       |
   +------------------------------+
   |
   +--- DMA TX Descriptor Chain ---> TX Buffers (in RAMX)
   +--- DMA RX Descriptor Chain ---> RX Buffers (in RAMX)
```

## Pin Connections (Typical)

| Function | Pin | Description |
|----------|-----|-------------|
| MDC | PB8 | Management clock to PHY |
| MDIO | PB9 | Management data I/O |
| TXD0-3 | PB12-PB15 | Transmit data |
| TXEN | PB16 | Transmit enable |
| RXD0-3 | PB17-PB20 | Receive data |
| RXDV | PB21 | Receive data valid |
| RXCLK | PB22 | Receive clock |
| TXCLK | PB23 | Transmit clock |

(Pin assignments vary by board; check schematic.)

## DMA Buffer Configuration

```c
// CRITICAL: DMA buffers must be in RAMX (0x20020000+)
// DMA cannot access regular RAM at 0x20000000

#define ETH_TXBUFNB     4       // Number of TX buffers
#define ETH_RXBUFNB     4       // Number of RX buffers
#define ETH_MAX_PACKET_SIZE  1518  // Max Ethernet frame size

// Place in .dmadata section (maps to RAMX)
__attribute__((aligned(4), section(".dmadata")))
uint8_t Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];

__attribute__((aligned(4), section(".dmadata")))
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE];

// DMA descriptors (also in RAMX)
__attribute__((aligned(4), section(".dmadata")))
ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];

__attribute__((aligned(4), section(".dmadata")))
ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];
```

## PHY Initialization

```c
// Read PHY register
uint16_t phy_val = ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BCR);

// Write PHY register
ETH_WritePHYRegister(PHY_ADDRESS, PHY_BCR, PHY_Reset);

// Wait for auto-negotiation complete
uint16_t bsr;
do {
    bsr = ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR);
} while (!(bsr & PHY_AutoNego_Complete));

// Check link status
if (bsr & PHY_Linked_Status) {
    // Link is up
}

// Read speed and duplex from PHY status register
uint16_t sr = ETH_ReadPHYRegister(PHY_ADDRESS, PHY_SR);
if (sr & PHY_DUPLEX_STATUS) {
    // Full duplex
} else {
    // Half duplex
}
uint16_t speed = (sr & PHY_SPEED_STATUS);
```

## Complete Ethernet Initialization

```c
#include "CH56x_common.h"

#define PHY_ADDRESS     0x01    // PHY address (depends on hardware)
#define ETH_TXBUFNB     4
#define ETH_RXBUFNB     4
#define ETH_MAX_PACKET_SIZE  1518

__attribute__((aligned(4), section(".dmadata")))
uint8_t Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];

__attribute__((aligned(4), section(".dmadata")))
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE];

__attribute__((aligned(4), section(".dmadata")))
ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];

__attribute__((aligned(4), section(".dmadata")))
ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];

ETH_InitTypeDef ETH_InitStruct;

void ETH_GPIO_Config(void)
{
    // Configure MDC, MDIO, and RMII/MII pins
    // Pin remap for Ethernet functions
    // (Board-specific - check schematic)
    GPIOPinRemap(ENABLE, ...);
}

void ETH_BSP_Config(void)
{
    // Enable Ethernet peripheral clock
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_ETH);

    // Configure GPIO pins
    ETH_GPIO_Config();

    // Software reset
    ETH_SoftwareReset();
    while (ETH_GetSoftwareResetStatus() == SET);

    // Initialize ETH structure with defaults
    ETH_StructInit(&ETH_InitStruct);

    // Customize settings
    ETH_InitStruct.ETH_Speed = ETH_Speed_100M;
    ETH_InitStruct.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStruct.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStruct.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    ETH_InitStruct.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStruct.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStruct.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;

    // Apply configuration (includes PHY init via MII)
    // May need custom ETH_Init() call depending on library version

    // Setup DMA descriptor chains
    ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

    // Enable MAC TX/RX and DMA
    ETH_Start();
}
```

## Sending Ethernet Frames

```c
uint8_t tx_frame[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination MAC (broadcast)
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  // Source MAC
    0x08, 0x00,                            // EtherType: IPv4
    // ... payload ...
};

// Find an available TX descriptor
ETH_DMADESCTypeDef *DmaTxDesc = DMATxDscrTab;
while (DmaTxDesc->Status & ETH_DMATxDesc_OWN) {
    // Descriptor owned by DMA, wait or try next
}

// Copy frame to TX buffer
memcpy((void *)DmaTxDesc->Buffer1Addr, tx_frame, sizeof(tx_frame));

// Set frame length and ownership
DmaTxDesc->ControlBufferSize = sizeof(tx_frame);
DmaTxDesc->Status |= ETH_DMATxDesc_OWN | ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

// Poll demand to trigger DMA
ETH->DMATPDR = 0;
```

## Receiving Ethernet Frames

```c
void poll_eth_rx(void)
{
    ETH_DMADESCTypeDef *DmaRxDesc = DMARxDscrTab;

    // Check if DMA has filled a descriptor
    while (!(DmaRxDesc->Status & ETH_DMARxDesc_OWN)) {
        if (DmaRxDesc->Status & ETH_DMARxDesc_LS) {
            // Last segment - frame is complete
            uint32_t frame_len = (DmaRxDesc->Status & ETH_DMARxDesc_FL) >> 16;
            uint8_t *frame_data = (uint8_t *)DmaRxDesc->Buffer1Addr;

            // Process received frame
            process_eth_frame(frame_data, frame_len);
        }

        // Return descriptor to DMA
        DmaRxDesc->Status = ETH_DMARxDesc_OWN;

        // Next descriptor
        DmaRxDesc = (ETH_DMADESCTypeDef *)DmaRxDesc->Buffer2NextDescAddr;
    }

    // Resume DMA reception
    ETH->DMARPDR = 0;
}
```

## Ethernet Interrupt

```c
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void)
{
    // Check DMA receive interrupt
    if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R)) {
        ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        poll_eth_rx();
    }

    // Check DMA transmit interrupt
    if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_T)) {
        ETH_DMAClearITPendingBit(ETH_DMA_IT_T);
        // TX complete
    }
}

// Enable interrupt in init:
ETH_DMAITConfig(ETH_DMA_IT_R | ETH_DMA_IT_T, ENABLE);
PFIC_EnableIRQ(ETH_IRQn);
```

## WCHNET TCP/IP Stack

WCH provides a pre-compiled TCP/IP stack library (WCHNET) for CH56x.
The ETH examples in CH569EVT use this library for TCP, UDP, DHCP, DNS:

```
CH569EVT/EVT/EXAM/ETH/
  TCPClient/     - TCP client example
  TCPServer/     - TCP server example
  UDPClient/     - UDP client example
  UDPServer/     - UDP server example
  DHCP/          - DHCP client example
  DNS/           - DNS resolver example
  IPRaw_PING/    - Raw IP ping example
  MAC_RAW/       - Raw MAC frame example
  ETH_SRC/       - Ethernet source example
```

## Notes

- DMA buffers MUST be in RAMX (0x20020000+), not regular RAM
- PHY address depends on hardware (check PHY datasheet and board schematic)
- Ethernet MDC clock = HCLK/42 (at 120MHz, MDC = ~2.86MHz)
- Max frame size: 1518 bytes (standard Ethernet)
- For TCP/IP, use the WCHNET library or implement your own stack
- RMII uses fewer pins than MII but requires 50MHz reference clock
