# USB Power Delivery Configuration

> **Summary**: Configure USB-PD source, sink, or DRP (Dual-Role Power) applications on CH32X/CH6xx chips.

## Trigger Intent

- "USB-PD"
- "Power Delivery"
- "PD source"
- "PD sink"
- "PDO configuration"
- "USB-C charging"
- "PD negotiation"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` + `ch32x035_usbpd.h` |
| Clock | `RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBPD, ENABLE)` |
| Pins | PC14 (CC1), PC15 (CC2) -- CH32X035 |
| Reference | `CH32X035EVT/EVT/EXAM/USBPD/` |

## Step-by-Step

### USB-PD Peripheral Initialization (CH32X035)

```c
#include "ch32x035.h"
#include "ch32x035_usbpd.h"
#include "debug.h"

void USBPD_Init(void) {
    // Enable USBPD clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBPD, ENABLE);

    // Configure CC pins for PD communication
    // CC1 (PC14): pull-down + comparator at 0.22V
    USBPD->PORT_CC1 = CC_PD | CC_CMP_22;

    // CC2 (PC15): pull-down + comparator at 0.22V
    USBPD->PORT_CC2 = CC_PD | CC_CMP_22;

    // For VDD = 3.3V direct (no LDO limit):
    USBPD->PORT_CC1 |= USBPD_PHY_V33;
    USBPD->PORT_CC2 |= USBPD_PHY_V33;

    // Configure BMC timer values (at 48MHz system clock)
    // TX timer: 80-1 = 79 (for 600kHz BMC at 48MHz)
    // RX timer: 120-1 = 119

    // Enable PD input filter
    USBPD->CONFIG = PD_FILT_ED;

    // Enable interrupts
    USBPD->CONFIG |= IE_RX_ACT | IE_RX_RESET | IE_TX_END | IE_PD_IO;

    // Select CC channel (0=CC1, 1=CC2)
    USBPD->CONFIG &= ~CC_SEL;  // Select CC1 initially
}
```

### PD Source Implementation

```c
// Global PD control structure
PD_CONTROL pd_ctrl;
uint8_t pd_rx_buffer[34];  // Max PD message size
uint8_t pd_tx_buffer[34];

// Source PDO: 5V@3A, 9V@3A, 12V@2.5A, 15V@2A, 20V@1.5A
const uint32_t source_pdo[] = {
    (2 << 30) | (150 << 10) | (300),           // PDO1: Fixed 5V, 3A
    (2 << 30) | (180 << 10) | (300),           // PDO2: Fixed 9V, 3A
    (2 << 30) | (240 << 10) | (250),           // PDO3: Fixed 12V, 2.5A
    (2 << 30) | (300 << 10) | (200),           // PDO4: Fixed 15V, 2A
    (2 << 30) | (400 << 10) | (150),           // PDO5: Fixed 20V, 1.5A
};

void PD_SendSourceCap(void) {
    _Message_Header header;
    header.Data = 0;
    header.Message_Header.MsgType = DEF_TYPE_SRC_CAP;
    header.Message_Header.PDRole = 1;    // DFP
    header.Message_Header.PRRole = 1;    // Source
    header.Message_Header.SpecRev = DEF_PD_REVISION_20;
    header.Message_Header.NumDO = 5;     // 5 PDOs
    header.Message_Header.MsgID = pd_ctrl.Msg_ID;

    // Build message
    pd_tx_buffer[0] = header.Data & 0xFF;
    pd_tx_buffer[1] = (header.Data >> 8) & 0xFF;
    for(int i = 0; i < 5; i++) {
        pd_tx_buffer[2 + i*4 + 0] = (source_pdo[i]) & 0xFF;
        pd_tx_buffer[2 + i*4 + 1] = (source_pdo[i] >> 8) & 0xFF;
        pd_tx_buffer[2 + i*4 + 2] = (source_pdo[i] >> 16) & 0xFF;
        pd_tx_buffer[2 + i*4 + 3] = (source_pdo[i] >> 24) & 0xFF;
    }

    // Send via USBPD peripheral
    // (Implementation depends on specific USBPD TX method)
}

void PD_SendAccept(void) {
    _Message_Header header;
    header.Data = 0;
    header.Message_Header.MsgType = DEF_TYPE_ACCEPT;
    header.Message_Header.PDRole = 1;
    header.Message_Header.PRRole = 1;
    header.Message_Header.SpecRev = DEF_PD_REVISION_20;
    header.Message_Header.NumDO = 0;
    header.Message_Header.MsgID = pd_ctrl.Msg_ID;

    pd_tx_buffer[0] = header.Data & 0xFF;
    pd_tx_buffer[1] = (header.Data >> 8) & 0xFF;
}

void PD_SendPS_RDY(void) {
    _Message_Header header;
    header.Data = 0;
    header.Message_Header.MsgType = DEF_TYPE_PS_RDY;
    header.Message_Header.PDRole = 1;
    header.Message_Header.PRRole = 1;
    header.Message_Header.SpecRev = DEF_PD_REVISION_20;
    header.Message_Header.NumDO = 0;
    header.Message_Header.MsgID = pd_ctrl.Msg_ID;

    pd_tx_buffer[0] = header.Data & 0xFF;
    pd_tx_buffer[1] = (header.Data >> 8) & 0xFF;
}
```

### PD Sink Implementation

```c
// Sink PDO: 5V@3A, 9V@2A, 12V@1.5A, 15V@1.2A, 20V@1A
const uint32_t sink_pdo[] = {
    (2 << 30) | (150 << 10) | (300),           // 5V, 3A
    (2 << 30) | (180 << 10) | (200),           // 9V, 2A
    (2 << 30) | (240 << 10) | (150),           // 12V, 1.5A
    (2 << 30) | (300 << 10) | (120),           // 15V, 1.2A
    (2 << 30) | (400 << 10) | (100),           // 20V, 1A
};

void PD_SendRequest(uint8_t pdo_index) {
    _Message_Header header;
    header.Data = 0;
    header.Message_Header.MsgType = DEF_TYPE_REQUEST;
    header.Message_Header.PDRole = 0;    // UFP
    header.Message_Header.PRRole = 0;    // Sink
    header.Message_Header.SpecRev = DEF_PD_REVISION_20;
    header.Message_Header.NumDO = 1;
    header.Message_Header.MsgID = pd_ctrl.Msg_ID;

    // Build RDO (Request Data Object)
    uint32_t rdo = (pdo_index + 1) << 28;  // Object position
    rdo |= (sink_pdo[pdo_index] & 0x3FF);  // Operating current

    pd_tx_buffer[0] = header.Data & 0xFF;
    pd_tx_buffer[1] = (header.Data >> 8) & 0xFF;
    pd_tx_buffer[2] = rdo & 0xFF;
    pd_tx_buffer[3] = (rdo >> 8) & 0xFF;
    pd_tx_buffer[4] = (rdo >> 16) & 0xFF;
    pd_tx_buffer[5] = (rdo >> 24) & 0xFF;
}
```

### PD State Machine (Simplified)

```c
void PD_StateMachine(void) {
    switch(pd_ctrl.PD_State) {
        case STA_IDLE:
            // Wait for connection detection
            break;

        case STA_SRC_CONNECT:
            // Source connected, send Source_Capabilities
            PD_SendSourceCap();
            pd_ctrl.PD_State = STA_RX_REQ_WAIT;
            break;

        case STA_RX_REQ:
            // Request received, validate and respond
            if(/* request valid */) {
                PD_SendAccept();
                // Adjust voltage/current output
                pd_ctrl.PD_State = STA_ADJ_VOL;
            } else {
                PD_SendReject();
                pd_ctrl.PD_State = STA_SRC_CONNECT;
            }
            break;

        case STA_ADJ_VOL:
            // Voltage adjustment complete
            PD_SendPS_RDY();
            pd_ctrl.PD_State = STA_IDLE;
            break;

        case STA_TX_SOFTRST:
            // Send soft reset
            PD_SendSoftReset();
            pd_ctrl.PD_State = STA_SRC_CONNECT;
            break;

        case STA_TX_HRST:
            // Send hard reset
            PD_SendHardReset();
            pd_ctrl.PD_State = STA_SRC_CONNECT;
            break;

        default:
            break;
    }
}
```

### USBPD Interrupt Handler

```c
volatile uint8_t pd_rx_flag = 0;
volatile uint8_t pd_tx_flag = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
void USBPD_IRQHandler(void) {
    uint8_t status = USBPD->STATUS;

    if(status & IF_RX_ACT) {
        // Receive complete - copy data from USBPD buffer
        pd_rx_flag = 1;
        USBPD->STATUS = IF_RX_ACT;  // Clear flag
    }

    if(status & IF_TX_END) {
        // Transmit complete
        pd_tx_flag = 1;
        USBPD->STATUS = IF_TX_END;  // Clear flag
    }

    if(status & IF_RX_RESET) {
        // PD reset received
        USBPD->STATUS = IF_RX_RESET;
        pd_ctrl.PD_State = STA_PHY_RST;
    }

    if(status & BUF_ERR) {
        // Buffer error
        USBPD->STATUS = BUF_ERR;
    }
}
```

### CC Channel Selection and Detection

```c
// Detect which CC line has a connection
uint8_t PD_DetectCC(void) {
    // Try CC1
    USBPD->CONFIG &= ~CC_SEL;  // Select CC1
    Delay_Us(10);
    uint8_t cc1_level = USBPD->PORT_CC1 & PA_CC_AI;

    // Try CC2
    USBPD->CONFIG |= CC_SEL;   // Select CC2
    Delay_Us(10);
    uint8_t cc2_level = USBPD->PORT_CC2 & PA_CC_AI;

    if(cc1_level && !cc2_level) return DEF_PD_CC1;
    if(!cc1_level && cc2_level) return DEF_PD_CC2;
    return 0xFF;  // No connection
}
```

## PD Message Type Reference

### Control Messages

| Type | Value | Description | Sender |
|------|-------|-------------|--------|
| GoodCRC | 0x01 | Acknowledge | Source, Sink |
| Accept | 0x03 | Accept request | Source, Sink |
| Reject | 0x04 | Reject request | Source, Sink |
| PS_RDY | 0x06 | Power supply ready | Source, Sink |
| DR_Swap | 0x09 | Data role swap | Source, Sink |
| PR_Swap | 0x0A | Power role swap | Source, Sink |
| Soft_Reset | 0x0D | Soft reset | Source, Sink |
| Hard_Reset | -- | Hard reset (PHY level) | Source, Sink |

### Data Messages

| Type | Value | Description | Sender |
|------|-------|-------------|--------|
| Source_Capabilities | 0x01 | List source PDOs | Source |
| Request | 0x02 | Request specific PDO | Sink |
| Sink_Capabilities | 0x04 | List sink PDOs | Sink |
| Vendor_Defined | 0x0F | VDM messages | Source, Sink |

## PDO Format Reference

### Fixed Supply PDO
```
Bits [31:30] = 0b00 (Fixed Supply)
Bits [29]    = Dual-Role Power
Bits [27:25] = USB Suspend Supported, Externally Powered, etc.
Bits [21:20] = Peak Current
Bits [19:10] = Voltage (in 50mV units, e.g., 100 = 5V, 180 = 9V, 400 = 20V)
Bits [9:0]   = Max Current (in 10mA units, e.g., 300 = 3A)
```

### PPS (Programmable Power Supply) PDO
```
Bits [31:30] = 0b11 (Augmented PDO)
Bits [29:28] = 0b00 (PPS)
Bits [27:24] = PPS Power Limited
Bits [21:17] = Max Voltage (in 100mV units)
Bits [15:8]  = Min Voltage (in 100mV units)
Bits [6:0]   = Max Current (in 50mA units)
```

## BMC Timer Values

| System Clock | UPD_TMR_TX | UPD_TMR_RX | Notes |
|-------------|------------|------------|-------|
| 48MHz | 80-1 | 120-1 | `UPD_TMR_TX_48M`, `UPD_TMR_RX_48M` |
| 24MHz | 40-1 | 60-1 | `UPD_TMR_TX_24M`, `UPD_TMR_RX_24M` |
| 12MHz | 20-1 | 30-1 | `UPD_TMR_TX_12M`, `UPD_TMR_RX_12M` |

## Common Errors

- Not configuring CC pin comparator thresholds -- no PD communication
- Wrong `USBPD_PHY_V33` bit for VDD voltage -- PHY damage or no communication
- BMC timer values wrong for system clock frequency -- protocol errors
- Not handling GoodCRC after sending messages -- partner assumes message lost
- Forgetting to increment Msg_ID after each message -- protocol violation
- CC channel not properly selected -- communication on wrong pin
- Hard Reset requires PHY-level BMC signaling, not just message sending
