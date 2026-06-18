# Recipe: Ethernet Communication (Register-Based)

## Overview

Configure and use the integrated 10/100M Ethernet MAC on CH561/CH563 with DMA
descriptors for frame transmission and reception. Requires an external PHY chip
(e.g., CH182, RTL8201, LAN8720). Uses CH561NET TCP/IP stack library.

## Hardware Requirements

- CH561 or CH563 evaluation board with Ethernet PHY
- External PHY connected via MII/RMII interface
- 25MHz crystal or clock for PHY
- Ethernet RJ45 connector with magnetics

## Ethernet Architecture

```
CH561/CH563 MAC  <---MII/RMII--->  External PHY  <---MDI--->  RJ45
   |                                    |
   |  MDC/MDIO (management)             |
   +------------------------------------+
   |
   +--- DMA TX Descriptor Chain ---> TX Buffers (in SRAM)
   +--- DMA RX Descriptor Chain ---> RX Buffers (in SRAM)
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

## Key Difference from CH569 (RISC-V)

CH561/CH563 use ARM7TDMI core. Key differences:
- No `PWR_PeriphClkCfg()` -- use `R8_SLP_CLK_OFF0` register directly
- No `ETH_StructInit()` / `ETH_Init()` library -- register-based init
- No `__attribute__((interrupt("WCH-Interrupt-fast")))` -- use `__irq` keyword
- No `.dmadata` section -- DMA buffers in regular DATA SRAM
- No PFIC -- ARM NVIC for interrupt control
- Flash erase unit is 4KB (not 256 bytes)
- CH563 has additional USB SFR registers

## Ethernet Register Base

```c
// Ethernet MAC registers are memory-mapped at 0x00400000 + offset
// Key registers (from CH561SFR.H):
// R32_ETH_MAC_CFG      - MAC configuration
// R32_ETH_MAC_FRAME_FLT - MAC frame filter
// R32_ETH_MAC_ADDR0_H  - MAC address high
// R32_ETH_MAC_ADDR0_L  - MAC address low
// R32_ETH_DMA_BUS_MOD  - DMA bus mode
// R32_ETH_DMA_TX_DESC  - TX descriptor list address
// R32_ETH_DMA_RX_DESC  - RX descriptor list address
// R32_ETH_DMA_OP_MODE  - DMA operation mode
// R32_ETH_DMA_INT_EN   - DMA interrupt enable
// R32_ETH_DMA_INT_FLAG - DMA interrupt flag
// R32_ETH_MII_ADDR     - MII address (PHY access)
// R32_ETH_MII_DATA     - MII data (PHY access)
```

## PHY Access via MII

```c
// Read PHY register
UINT16 ETH_ReadPHYRegister(UINT8 phy_addr, UINT8 reg_addr)
{
    R32_ETH_MII_ADDR = (phy_addr << 11) | (reg_addr << 6) | RB_ETH_MII_BUSY;
    while (R32_ETH_MII_ADDR & RB_ETH_MII_BUSY);
    return (UINT16)R32_ETH_MII_DATA;
}

// Write PHY register
void ETH_WritePHYRegister(UINT8 phy_addr, UINT8 reg_addr, UINT16 value)
{
    R32_ETH_MII_DATA = value;
    R32_ETH_MII_ADDR = (phy_addr << 11) | (reg_addr << 6) |
                       RB_ETH_MII_W | RB_ETH_MII_BUSY;
    while (R32_ETH_MII_ADDR & RB_ETH_MII_BUSY);
}
```

## DMA Descriptor Structure

```c
// DMA descriptor (must be word-aligned)
typedef struct {
    UINT32 Status;          // Status bits
    UINT32 ControlBufferSize; // Buffer size and control
    UINT32 Buffer1Addr;     // Buffer 1 address
    UINT32 Buffer2NextDescAddr; // Buffer 2 or next descriptor
} ETH_DMADESCTypeDef;

// Status bits for TX descriptor
#define ETH_DMATxDesc_OWN   0x80000000  // Owned by DMA
#define ETH_DMATxDesc_LS    0x20000000  // Last segment
#define ETH_DMATxDesc_FS    0x10000000  // First segment

// Status bits for RX descriptor
#define ETH_DMARxDesc_OWN   0x80000000  // Owned by DMA
#define ETH_DMARxDesc_LS    0x00000200  // Last segment
#define ETH_DMARxDesc_FS    0x00000100  // First segment
#define ETH_DMARxDesc_FL    0x3FFF0000  // Frame length mask
```

## DMA Buffer Configuration

```c
#define ETH_TXBUFNB     4       // Number of TX buffers
#define ETH_RXBUFNB     4       // Number of RX buffers
#define ETH_MAX_PACKET_SIZE  1518  // Max Ethernet frame size

// Buffers in DATA SRAM (CH561/CH563 DMA can access DATA SRAM)
UINT8 Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__((aligned(4)));
UINT8 Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__((aligned(4)));

// DMA descriptors (word-aligned)
ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB] __attribute__((aligned(4)));
ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB] __attribute__((aligned(4)));
```

## Complete Ethernet Initialization

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

#define PHY_ADDRESS     0x01    // PHY address (depends on hardware)

void ETH_Init(void)
{
    // Enable Ethernet peripheral clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_ETH;

    // Configure GPIO pins for Ethernet (board-specific)
    // MDC=PB8, MDIO=PB9, TXD0-3=PB12-15, TXEN=PB16
    // RXD0-3=PB17-20, RXDV=PB21, RXCLK=PB22, TXCLK=PB23

    // Software reset
    R32_ETH_DMA_BUS_MODE |= RB_ETH_DMA_SWR;
    while (R32_ETH_DMA_BUS_MODE & RB_ETH_DMA_SWR);

    // Set MAC address
    R32_ETH_MAC_ADDR0_H = (my_mac[5] << 8) | my_mac[4];
    R32_ETH_MAC_ADDR0_L = (my_mac[3] << 24) | (my_mac[2] << 16) |
                           (my_mac[1] << 8) | my_mac[0];

    // MAC configuration: enable TX, RX, full duplex, 100M
    R32_ETH_MAC_CFG = RB_ETH_MAC_TE | RB_ETH_MAC_RE |
                      RB_ETH_MAC_DM | RB_ETH_MAC_FES;

    // Setup DMA descriptor chains
    ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

    // Enable DMA interrupts
    R32_ETH_DMA_INT_EN = RB_ETH_DMA_IE_RIE | RB_ETH_DMA_IE_TIE;

    // Enable DMA TX and RX
    R32_ETH_DMA_OP_MODE = RB_ETH_DMA_ST | RB_ETH_DMA_SR;
}

void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *desc, UINT8 *buf, UINT32 count)
{
    for (UINT32 i = 0; i < count; i++) {
        desc[i].Status = ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
        desc[i].ControlBufferSize = 0;
        desc[i].Buffer1Addr = (UINT32)&buf[i * ETH_MAX_PACKET_SIZE];
        desc[i].Buffer2NextDescAddr = (UINT32)&desc[i + 1];
    }
    desc[count - 1].Buffer2NextDescAddr = (UINT32)&desc[0]; // Ring
}

void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *desc, UINT8 *buf, UINT32 count)
{
    for (UINT32 i = 0; i < count; i++) {
        desc[i].Status = ETH_DMARxDesc_OWN;
        desc[i].ControlBufferSize = ETH_MAX_PACKET_SIZE;
        desc[i].Buffer1Addr = (UINT32)&buf[i * ETH_MAX_PACKET_SIZE];
        desc[i].Buffer2NextDescAddr = (UINT32)&desc[i + 1];
    }
    desc[count - 1].Buffer2NextDescAddr = (UINT32)&desc[0]; // Ring
}
```

## Sending Ethernet Frames

```c
UINT8 tx_frame[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination MAC (broadcast)
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  // Source MAC
    0x08, 0x00,                            // EtherType: IPv4
    // ... payload ...
};

void ETH_SendFrame(UINT8 *data, UINT16 len)
{
    // Find an available TX descriptor
    ETH_DMADESCTypeDef *desc = &DMATxDscrTab[tx_desc_idx];
    while (desc->Status & ETH_DMATxDesc_OWN) {
        // Descriptor owned by DMA, wait
    }

    // Copy frame to TX buffer
    memcpy((void *)desc->Buffer1Addr, data, len);

    // Set frame length and ownership
    desc->ControlBufferSize = len;
    desc->Status |= ETH_DMATxDesc_OWN | ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

    // Poll demand to trigger DMA
    R32_ETH_DMA_TX_POLL = 0;

    tx_desc_idx = (tx_desc_idx + 1) % ETH_TXBUFNB;
}
```

## Receiving Ethernet Frames

```c
void ETH_PollRx(void)
{
    ETH_DMADESCTypeDef *desc = &DMARxDscrTab[rx_desc_idx];

    // Check if DMA has filled a descriptor
    while (!(desc->Status & ETH_DMARxDesc_OWN)) {
        if (desc->Status & ETH_DMARxDesc_LS) {
            // Last segment - frame is complete
            UINT32 frame_len = (desc->Status & ETH_DMARxDesc_FL) >> 16;
            UINT8 *frame_data = (UINT8 *)desc->Buffer1Addr;

            // Process received frame
            process_eth_frame(frame_data, frame_len);
        }

        // Return descriptor to DMA
        desc->Status = ETH_DMARxDesc_OWN;

        // Next descriptor
        rx_desc_idx = (rx_desc_idx + 1) % ETH_RXBUFNB;
        desc = &DMARxDscrTab[rx_desc_idx];
    }

    // Resume DMA reception
    R32_ETH_DMA_RX_POLL = 0;
}
```

## Ethernet Interrupt

```c
__irq void IRQ_Handler(void)
{
    if (R8_INT_FLAG_0 & RB_IF_ETH) {
        UINT32 dma_flag = R32_ETH_DMA_INT_FLAG;

        // Check DMA receive interrupt
        if (dma_flag & RB_ETH_DMA_IF_RI) {
            R32_ETH_DMA_INT_FLAG = RB_ETH_DMA_IF_RI;
            ETH_PollRx();
        }

        // Check DMA transmit interrupt
        if (dma_flag & RB_ETH_DMA_IF_TI) {
            R32_ETH_DMA_INT_FLAG = RB_ETH_DMA_IF_TI;
            // TX complete
        }
    }
}

// Enable interrupt in init:
R8_INT_EN_IRQ_0 |= RB_IE_IRQ_ETH;
R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;
```

## CH561NET TCP/IP Stack

WCH provides a pre-compiled TCP/IP stack library (CH561NET) for CH561/CH563.
The NET examples use this library for TCP, UDP, DHCP, DNS:

```
CH561EVT/EXAM/NET/
  TCP_CLIENT/     - TCP client example
  TCP_SERVER/     - TCP server example
  UDP_CLIENT/     - UDP client example
  UDP_SERVER/     - UDP server example
  DHCP_CLIENT/    - DHCP client example
  DNS/            - DNS resolver example
  IP_RAW/         - Raw IP example
  PING/           - Ping example
  NET_CFG/        - Network configuration
  NET_MAC/        - MAC address configuration

CH563EVT/EXAM/NET/
  (Same as CH561 plus additional examples)
  FTP_CLIENT/     - FTP client
  FTP_SERVER/     - FTP server
  WEB_SERVER/     - Web server
  TCP_MQTT/       - MQTT client
  MAIL/           - Email client
  TFTP_CLIENT/    - TFTP client
  MULTICAST/      - Multicast
  UPNP/           - UPnP
```

## Notes

- DMA buffers must be word-aligned and in DATA SRAM
- PHY address depends on hardware (check PHY datasheet and board schematic)
- Ethernet MDC clock = HCLK/42 (at 100MHz, MDC = ~2.38MHz)
- Max frame size: 1518 bytes (standard Ethernet)
- For TCP/IP, use the CH561NET library or implement your own stack
- RMII uses fewer pins than MII but requires 50MHz reference clock
- CH561 has fewer NET examples than CH563
