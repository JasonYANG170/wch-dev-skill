# USB Device

> **Summary**: Configure USB Full-Speed device mode (CDC, HID, Vendor) on CH32X/CH6xx chips.

## Trigger Intent

- "USB device"
- "USB CDC serial"
- "USB HID"
- "USB vendor device"
- "USB composite"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` + `ch32x035_usb.h` |
| Clock | `RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE)` |
| Pins | PA11 (D-), PA12 (D+) |
| Reference | `CH32X035EVT/EVT/EXAM/USB/` |

## Step-by-Step

### USB Device Initialization Overview

The CH32X035 USB FS controller uses register-level programming (not a high-level library). The USB driver in `ch32x035_usb.h` provides struct definitions and register bit macros.

```c
#include "ch32x035.h"
#include "ch32x035_usb.h"
#include "debug.h"

// USB endpoint buffer addresses
#define DEF_UEP0_SIZE    64
#define DEF_UEP1_SIZE    64
#define DEF_UEP2_SIZE    64

// Endpoint buffer start address (in USB SRAM)
uint8_t *pUEP0_RAM_Addr;
uint8_t *pUEP1_RAM_Addr;
uint8_t *pUEP2_RAM_Addr;

void USB_Device_Init(void) {
    // Enable USB FS clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE);

    // Configure USB pins (PA11=D-, PA12=D+) - internal pull-up
    // No external GPIO init needed for USB device mode

    // Reset USB SIE
    R8_USB_CTRL = USBFS_UC_RESET_SIE;
    Delay_Us(100);
    R8_USB_CTRL = 0;

    // Set device address (0 for initial enumeration)
    R8_USB_DEV_AD = 0;

    // Configure endpoint 0 (control)
    R8_UEP4_1_MOD = 0;  // EP0: single 64-byte buffer for RX/TX
    R16_UEP0_DMA = (uint16_t)(uint32_t)pUEP0_RAM_Addr;

    // Enable USB device pull-up
    R8_USB_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;

    // Enable interrupts
    R8_USB_INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_TRANSFER | USBFS_UIE_BUS_RST;

    // Enable USB port
    R8_UDEV_CTRL = USBFS_UD_PORT_EN;
}
```

### USB Interrupt Handler

```c
__attribute__((interrupt("WCH-Interrupt-fast")))
void USB_LP_CAN1_RX0_IRQHandler(void) {
    uint8_t int_flag = R8_USB_INT_FG;

    if(int_flag & USBFS_UIF_BUS_RST) {
        // Bus reset
        R8_USB_DEV_AD = 0;
        R8_UEP_TX_CTRL = USBFS_UEP_T_RES_NAK;
        R8_UEP_RX_CTRL = USBFS_UEP_R_RES_ACK;
        R8_USB_INT_FG = USBFS_UIF_BUS_RST;
    }

    if(int_flag & USBFS_UIF_TRANSFER) {
        uint8_t token = R8_USB_INT_ST & USBFS_UIS_TOKEN_MASK;
        uint8_t ep_num = R8_USB_INT_ST & USBFS_UIS_ENDP_MASK;

        switch(token) {
            case USBFS_UIS_TOKEN_SETUP:
                // Handle SETUP packet
                break;
            case USBFS_UIS_TOKEN_IN:
                // Handle IN transfer complete
                break;
            case USBFS_UIS_TOKEN_OUT:
                // Handle OUT transfer complete
                break;
        }
        R8_USB_INT_FG = USBFS_UIF_TRANSFER;
    }

    if(int_flag & USBFS_UIF_SUSPEND) {
        R8_USB_INT_FG = USBFS_UIF_SUSPEND;
    }
}
```

### USB Descriptor Example (CDC)

```c
// Device Descriptor
const uint8_t CDC_DeviceDesc[] = {
    18,                         // bLength
    USB_DESCR_TYP_DEVICE,       // bDescriptorType
    0x00, 0x02,                 // bcdUSB = 2.0
    0x02,                       // bDeviceClass = CDC
    0x00,                       // bDeviceSubClass
    0x00,                       // bDeviceProtocol
    DEF_UEP0_SIZE,              // bMaxPacketSize0
    0x86, 0x1A,                 // idVendor (WCH)
    0x35, 0x57,                 // idProduct
    0x00, 0x01,                 // bcdDevice
    1, 2, 3,                    // iManufacturer, iProduct, iSerialNumber
    1                           // bNumConfigurations
};

// Configuration Descriptor
const uint8_t CDC_ConfigDesc[] = {
    // Configuration descriptor
    9, USB_DESCR_TYP_CONFIG,
    (9+9+5+5+4+5+7+7) & 0xFF, ((9+9+5+5+4+5+7+7) >> 8) & 0xFF,
    2,    // bNumInterfaces
    1,    // bConfigurationValue
    0,    // iConfiguration
    0x80, // bmAttributes (bus-powered)
    0x32, // MaxPower = 100mA

    // Interface 0: CDC Communication
    9, USB_DESCR_TYP_INTERF,
    0, 0, 1, 0x02, 0x02, 0x01, 0,

    // CDC Functional Descriptors (header, call management, ACM, union)
    5, 0x24, 0x00, 0x10, 0x01,  // Header
    5, 0x24, 0x01, 0x00, 1,     // Call Management
    4, 0x24, 0x02, 0x02,        // ACM
    5, 0x24, 0x06, 0, 1,        // Union (master=0, slave=1)

    // Endpoint: CDC Notification (IN)
    7, USB_DESCR_TYP_ENDP,
    0x81, 0x03, 0x08, 0x00, 0x40,  // EP1 IN, Interrupt, 8 bytes, 64ms

    // Interface 1: CDC Data
    9, USB_DESCR_TYP_INTERF,
    1, 0, 2, 0x0A, 0x00, 0x00, 0,

    // Endpoint: Data OUT (EP2)
    7, USB_DESCR_TYP_ENDP,
    0x02, 0x02, 0x40, 0x00, 0x00,  // EP2 OUT, Bulk, 64 bytes

    // Endpoint: Data IN (EP2)
    7, USB_DESCR_TYP_ENDP,
    0x82, 0x02, 0x40, 0x00, 0x00,  // EP2 IN, Bulk, 64 bytes
};
```

## USB Endpoint Reference

| Endpoint | Direction | Type | Buffer Mode |
|----------|-----------|------|-------------|
| EP0 | IN/OUT | Control | Single 64B (shared RX/TX) |
| EP1 | IN | Interrupt | 64B TX |
| EP2 | OUT/IN | Bulk | 64B RX + 64B TX |
| EP3 | OUT/IN | Bulk/ISO | 64B RX + 64B TX |

## Common Errors

- Forgetting `RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE)` -- USB not functional
- Endpoint buffer address must be in USB SRAM -- not regular SRAM
- Device address must be set to 0 after bus reset
- SETUP packet must be processed before responding with data
- Not handling all standard requests (GET_DESCRIPTOR, SET_ADDRESS, SET_CONFIGURATION) -- enumeration fails
