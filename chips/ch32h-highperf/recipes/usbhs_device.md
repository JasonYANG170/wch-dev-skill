# USB High-Speed Device

## Overview

CH32H417 has a USBHS (USB High-Speed) peripheral supporting USB 2.0 High-Speed (480 Mbps) device and host modes. The USBHS peripheral is separate from USBFS and provides significantly higher throughput. It uses an external ULPI PHY or internal transceiver.

## Key API Functions

The USBHS device uses a similar API structure to USBFS but with high-speed specific configurations.

```c
// USBHS Device functions
void USBHS_Device_Init(void);
void USBHS_Suspend(void);
void USBHS_Resume(void);
// Endpoint callbacks similar to USBFS
```

## USBHS Device Example Structure

```
USBHS/DEVICE/
  APP/
    include/
      config.h
      usb_desc.h
    src/
      main.c
      usb_desc.c
      usb_endp.c
      usb_prop.c
      hw_config.c
  Profile/
  StdPeriphDriver/
```

## USBHS vs USBFS

| Feature | USBFS | USBHS |
|---------|-------|-------|
| Speed | 12 Mbps (Full-Speed) | 480 Mbps (High-Speed) |
| PHY | Internal | ULPI or Internal |
| Endpoint count | 8 | 16 |
| Buffer size | Small PMA | Larger configurable FIFO |
| Clock | 48 MHz | 480 MHz + 48 MHz |

## Important Notes

- USBHS requires careful clock configuration for 480MHz operation
- May use ULPI external PHY depending on board design
- The USBHS examples in the EVT directory provide complete device/host implementations
- Refer to the specific USBHS DEVICE and HOST example code for full implementation details
- Interrupt handling is similar to USBFS but uses USBHS-specific interrupt vectors
