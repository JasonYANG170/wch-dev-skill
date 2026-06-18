# Recipe: USB 3.0 Superspeed Device/Host (CH569)

## Overview

Configure USB 3.0 Superspeed (5Gbps) device or host mode on CH569.
CH561 and CH563 only support USB 2.0 High-Speed -- this recipe applies to CH569 only.

## USB 3.0 vs USB 2.0 on CH56x

| Feature | CH569 | CH569 |
|---------|-------------|-------|
| USB 2.0 HS (480Mbps) | Yes | Yes |
| USB 3.0 SS (5Gbps) | No | Yes |
| USBSS Host | No | Yes |
| USBSS Device | No | Yes |

## USB Peripheral Clock

```c
// Enable USB 3.0 clock (CH569 only)
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBSS);

// Also enable USB HS clock if using USB 2.0
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBHS);
```

## USB 3.0 Device Mode

The CH569EVT provides USB 3.0 examples in:
```
CH569EVT/EVT/EXAM/USBSS/
  USBD/       - USB 3.0 device examples
  USBH/       - USB 3.0 host examples
```

### Device Initialization Pattern

```c
#include "CH56x_common.h"

// USB descriptors (device, configuration, string, etc.)
// These are standard USB descriptors - see USB 3.0 spec

void USBSS_Device_Init(void)
{
    // Enable USBSS clock
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBSS);

    // Configure USB pins (VBUS, D+, D-, SSTX, SSRX)
    // Pin configuration is board-specific

    // Initialize USBSS device controller
    // Configure endpoints
    // Set up descriptor pointers
    // Enable USBSS PHY
    // Connect to host
}
```

### USB 3.0 Endpoint Configuration

USB 3.0 adds bulk streams and higher throughput compared to USB 2.0:

```c
// USB 3.0 endpoints support:
// - Control (EP0)
// - Bulk IN/OUT (high throughput)
// - Interrupt IN/OUT
// - Isochronous IN/OUT

// USB 3.0 specific features:
// - Bulk streams (up to 65536 streams per endpoint)
// - Increased max packet size (1024 for bulk)
// - Link power management (U1, U2, U3 states)
```

### USB 3.0 Descriptor Requirements

USB 3.0 devices must provide additional descriptors:

```c
// BOS Descriptor (Binary Object Store) - required for USB 3.0
// USB 2.0 Extension descriptor
// SuperSpeed Device Capability descriptor
// Container ID descriptor

// Endpoint Companion descriptor - required for each SS endpoint
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bMaxBurst;        // 0-15 (number of packets per burst)
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
} USB_SS_EndpointCompanionDescriptor;
```

## USB 3.0 Host Mode

```c
void USBSS_Host_Init(void)
{
    // Enable USBSS clock
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBSS);

    // Initialize USBSS host controller
    // Enable port power
    // Wait for device connection
    // Reset port
    // Enumerate device
    // Configure endpoints
}
```

## USB 2.0 Fallback

For applications that need to work on CH569 as well:

```c
void USB_Init(void)
{
#if defined(CH569)
    // Use USB 3.0
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBSS);
    USBSS_Device_Init();
#else
    // Fall back to USB 2.0
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBHS);
    USBHS_Device_Init();
#endif
}
```

## USB Interrupts

```c
void USBSS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBSS_IRQHandler(void)
{
    // Handle USB 3.0 events:
    // - Setup packet received
    // - Data transfer complete
    // - Port status change
    // - Link state change
    // - Suspend/resume
}

// Enable interrupt
PFIC_EnableIRQ(USBSS_IRQn);
```

## Notes

- USB 3.0 requires CH569. CH569 only have USB 2.0 High-Speed.
- USB 3.0 PHY requires specific hardware connections (SSTX+/SSTX-, SSRX+/SSRX-)
- Use the official WCH USBSS examples as a starting point
- USB 3.0 significantly increases throughput but also increases code complexity
- The USBSS peripheral clock must be enabled before any USBSS register access
- For simple USB applications, USB 2.0 High-Speed may be sufficient and simpler
