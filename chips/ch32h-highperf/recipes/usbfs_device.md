# USB Full-Speed Device

## Overview

CH32H417 has a USBFS (USB Full-Speed) peripheral supporting USB 2.0 Full-Speed (12 Mbps) device and host modes. Includes 8 endpoint buffers, configurable FIFO, and DMA support. The USBFS examples in the EVT directory include both DEVICE and HOST configurations.

## Key API Functions

```c
// USBFS Device functions (from usb_desc.c / usb_endp.c)
void USB_Init(void);
void USB_Suspend(void);
void USB_Resume(void);
void EP1_IN_Callback(void);
void EP2_IN_Callback(void);
void EP3_OUT_Callback(void);
// Standard USB request handling via USB_LP_IRQHandler
```

## USBFS Device Example Structure

The USBFS device example follows the standard USB device framework:

```
USBFS/DEVICE/
  APP/
    include/
      config.h         # USB configuration
      usb_desc.h       # Descriptor definitions
    src/
      main.c           # Main application
      usb_desc.c       # USB descriptors
      usb_endp.c       # Endpoint callbacks
      usb_prop.c       # USB property handling
      hw_config.c      # Hardware configuration
  Profile/             # USB class implementations
  StdPeriphDriver/     # Peripheral drivers
```

## USBFS Device Initialization

```c
#include "ch32h417.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"

void USBFS_Device_Init(void)
{
    // Enable USBFS clock
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_USBFS, ENABLE);

    // Configure USB pins (PA11=DM, PA12=DP)
    // Typically handled by the USB library

    // Initialize USB device
    USB_Init();

    // Set device address (handled by USB library after enumeration)
}
```

## Endpoint Configuration

```c
// Endpoint 1 IN - Send data to host
void EP1_IN_Callback(void)
{
    // Called when EP1 IN transfer completes
    // Prepare next data packet if needed
}

// Endpoint 3 OUT - Receive data from host
void EP3_OUT_Callback(void)
{
    // Called when EP3 OUT data is received
    uint8_t buffer[64];
    uint16_t length = GetEPRxCount(ENDP3);
    PMAToUserBufferCopy(buffer, ENP3_RXADDR, length);
}
```

## USBFS Clock Configuration

```c
// USBFS requires 48MHz clock
// Configure from PLL or HSE
void USB_Clock_Init(void)
{
    // RCC configuration to provide 48MHz to USBFS
    // This is typically done in system_ch32h417.c
}
```

## Important Notes

- USBFS uses PA11 (D-) and PA12 (D+) by default
- Requires 48MHz clock source for USB operation
- The USB library handles enumeration, standard requests, and state machine
- Endpoint buffers are allocated in the USB SRAM (PMA - Packet Memory Area)
- For custom USB classes, modify `usb_prop.c` and `usb_desc.c`
- Interrupt: `USB_LP_IRQHandler` for low-priority USB events
