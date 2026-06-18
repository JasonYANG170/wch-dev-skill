# USB-PD Configuration

## Overview

CH32H417 has an integrated USB-PD (Power Delivery) controller supporting PD 3.0. It handles BMC (Biphase Mark Coding) communication on CC1/CC2 pins, supporting source and sink roles with voltage/current negotiation.

## USBPD Register Definitions

The USBPD peripheral is controlled through these registers:

| Register | Description |
|----------|-------------|
| `CONFIG` | PD configuration (filter, DMA, interrupts) |
| `CONTROL` | TX/RX control |
| `TX_SEL` | TX preamble selection |
| `STATUS` | Status flags |
| `PORT_CC1` | CC1 port configuration |
| `PORT_CC2` | CC2 port configuration |

## Key Configuration Bits

```c
// CONFIG register
#define PD_FILT_ED    (1<<0)   // PD pin input filter enable
#define PD_ALL_CLR    (1<<1)   // Clear all interrupt flags
#define CC_SEL        (1<<2)   // Select PD communication port (0=CC1, 1=CC2)
#define PD_DMA_EN     (1<<3)   // Enable DMA for USBPD
#define PD_RST_EN     (1<<4)   // PD mode reset command enable
#define IE_PD_IO      (1<<10)  // PD IO interrupt enable
#define IE_RX_BIT     (1<<11)  // Receive bit interrupt enable
#define IE_RX_BYTE    (1<<12)  // Receive byte interrupt enable
#define IE_RX_ACT     (1<<13)  // Receive completion interrupt enable
#define IE_RX_RESET   (1<<14)  // Reset interrupt enable
#define IE_TX_END     (1<<15)  // Transfer completion interrupt enable

// CONTROL register
#define PD_TX_EN      (1<<0)   // USBPD transceiver mode and transmit enable
#define BMC_START     (1<<1)   // BMC send start signal
#define DATA_FLAG     (1<<5)   // Cache data valid flag bit

// CC port configuration
#define CC_PD         (1<<1)   // CC port pull-down resistor enable
#define CC_PU_330     (1<<2)   // CC port pull-up 330uA
#define CC_PU_180     (2<<2)   // CC port pull-up 180uA
#define CC_PU_80      (3<<2)   // CC port pull-up 80uA
#define CC_CMP_22     (2<<5)   // CC comparator 0.22V
#define CC_CMP_45     (3<<5)   // CC comparator 0.45V
#define CC_CMP_66     (5<<5)   // CC comparator 0.66V
#define CC_CMP_95     (6<<5)   // CC comparator 0.95V
#define CC_CMP_123    (7<<5)   // CC comparator 1.23V
```

## PD Message Types

```c
// Control Message Types
#define DEF_TYPE_RESERVED          0x00
#define DEF_TYPE_GOODCRC           0x01
#define DEF_TYPE_GOTOMIN           0x02
#define DEF_TYPE_ACCEPT            0x03
#define DEF_TYPE_REJECT            0x04
#define DEF_TYPE_PING              0x05
#define DEF_TYPE_PS_RDY            0x06
#define DEF_TYPE_GET_SRC_CAP       0x07
#define DEF_TYPE_GET_SNK_CAP       0x08
#define DEF_TYPE_DR_SWAP           0x09
#define DEF_TYPE_PR_SWAP           0x0A
#define DEF_TYPE_VCONN_SWAP        0x0B
#define DEF_TYPE_WAIT              0x0C
#define DEF_TYPE_SOFT_RESET        0x0D
#define DEF_TYPE_DATA_RESET        0x0E
#define DEF_TYPE_DATA_RESET_COMP   0x0F
#define DEF_TYPE_NOT_SUPPORTED     0x10
#define DEF_TYPE_GET_SRC_CAP_EXT   0x11
#define DEF_TYPE_GET_STATUS        0x12
#define DEF_TYPE_FR_SWAP           0x13
#define DEF_TYPE_GET_PPS_STATUS    0x14
#define DEF_TYPE_GET_COUNTRY_CODES 0x15

// Data Message Types
#define DEF_TYPE_SRC_CAP           0x01
#define DEF_TYPE_REQUEST           0x02
#define DEF_TYPE_BIST              0x03
#define DEF_TYPE_SNK_CAP           0x04
#define DEF_TYPE_BAT_STATUS        0x05
#define DEF_TYPE_ALERT             0x06
#define DEF_TYPE_GET_COUNTRY_INFO  0x07
#define DEF_TYPE_VENDOR_DEFINED    0x0F
```

## Basic PD Sink Example

```c
#include "ch32h417.h"
#include "ch32h417_usbpd.h"

void USBPD_Sink_Init(void)
{
    // Enable USBPD peripheral clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USBPD, ENABLE);

    // Configure CC1 pin (PC14) and CC2 pin (PC15)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // CC pins are typically configured by the USBPD peripheral

    // Select CC2 port for PD communication
    USBPD->CONFIG |= CC_SEL;

    // Enable input filter
    USBPD->CONFIG |= PD_FILT_ED;

    // Configure CC2 as sink (pull-down)
    USBPD->PORT_CC2 = CC_PD | CC_CMP_45;  // Pull-down, 0.45V threshold

    // Enable interrupts
    USBPD->CONFIG |= IE_RX_ACT | IE_RX_RESET | IE_TX_END;
}

void USBPD_SendMessage(uint8_t *msg, uint16_t len)
{
    // Copy message to USBPD TX buffer
    for(uint16_t i = 0; i < len; i++)
    {
        USBPD->BUFFER[i] = msg[i];
    }

    // Start transmission
    USBPD->CONTROL = PD_TX_EN | BMC_START;

    // Wait for TX complete
    while(!(USBPD->STATUS & IF_TX_END));
    USBPD->STATUS |= IF_TX_END;  // Clear flag
}
```

## USBPD Interrupt Handler

```c
void USBPD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBPD_IRQHandler(void)
{
    uint8_t status = USBPD->STATUS;

    if(status & IF_RX_ACT)
    {
        // Received a complete PD message
        // Process message...
        USBPD->STATUS |= IF_RX_ACT;
    }

    if(status & IF_RX_RESET)
    {
        // Received hard reset
        // Handle reset...
        USBPD->STATUS |= IF_RX_RESET;
    }

    if(status & IF_TX_END)
    {
        // Transmission complete
        USBPD->STATUS |= IF_TX_END;
    }
}
```

## PHY Voltage Configuration

```c
// For 3.3V VDD applications
USBPD->PORT_CC1 |= USBPD_PHY_V33;
USBPD->PORT_CC2 |= USBPD_PHY_V33;

// For VDD > 4V (use internal LDO)
USBPD->PORT_CC1 &= ~USBPD_PHY_V33;
USBPD->PORT_CC2 &= ~USBPD_PHY_V33;
```

## High Threshold Input Mode

```c
// Reduce I/O power consumption during PD communication
USBPD->PORT_CC1 |= USBPD_IN_HVT;  // 2.2V typical threshold
USBPD->PORT_CC2 |= USBPD_IN_HVT;
```

## References

- See `CH32H417EVT/EVT/EXAM/USBPD/` for complete examples
- USBPD header: `SRC/Peripheral/inc/ch32h417_usbpd.h`
