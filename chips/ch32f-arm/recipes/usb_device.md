# Recipe: USB Device

## Overview

Configure USB device mode on CH32F10x/CH32F20x using the built-in USB peripheral.

## USB Peripheral Overview

CH32F10x/CH32F20x have a built-in USB 2.0 Full-Speed device/host controller. The USB peripheral uses a register-based API (not the STM32 USB OTG library).

## Key Registers

| Register | Address | Description |
|----------|---------|-------------|
| R8_USB_CTRL | 0x40023400 | USB base control |
| R8_UDEV_CTRL | 0x40023401 | USB device physical port control |
| R8_USB_INT_EN | 0x40023402 | USB interrupt enable |
| R8_USB_DEV_AD | 0x40023403 | USB device address |
| R8_USB_INT_FG | 0x40023406 | USB interrupt flag |
| R8_USB_INT_ST | 0x40023407 | USB interrupt status |
| R8_USB_RX_LEN | 0x40023408 | USB receiving length |
| R16_UEP0_DMA | 0x40023410 | Endpoint 0 DMA buffer address |
| R8_UEP0_T_LEN | 0x40023420 | Endpoint 0 transmit length |
| R8_UEP0_CTRL | 0x40023422 | Endpoint 0 control |

## Key Functions

```c
// Initialization
void USB_DeviceInit(void);
void USB_Device_Endp_Init(void);

// Transfer processing
void USB_DevTransProcess(void);

// Endpoint callbacks (implement these)
void DevEP1_OUT_Deal(UINT8 l);
void DevEP2_OUT_Deal(UINT8 l);
void DevEP3_OUT_Deal(UINT8 l);
void DevEP4_OUT_Deal(UINT8 l);
void DevEP1_IN_Deal(UINT8 l);
void DevEP2_IN_Deal(UINT8 l);
void DevEP3_IN_Deal(UINT8 l);
void DevEP4_IN_Deal(UINT8 l);
```

## Endpoint Control Bits

| Bit | Description |
|-----|-------------|
| RB_UEP_R_TOG | Expected data toggle for receiving (0=DATA0, 1=DATA1) |
| RB_UEP_T_TOG | Prepared data toggle for transmitting (0=DATA0, 1=DATA1) |
| RB_UEP_AUTO_TOG | Auto toggle after successful transfer |
| MASK_UEP_R_RES | Receiving handshake response (ACK/NAK/STALL) |
| MASK_UEP_T_RES | Transmitting handshake response (ACK/NAK/STALL) |

## Example: USB HID Device

```c
#include "debug.h"
#include "ch32f10x_usb.h"

// USB descriptor data (place in USB RAM or regular RAM)
UINT8 DevDesc[] = { /* USB Device Descriptor */ };
UINT8 CfgDesc[] = { /* USB Configuration Descriptor */ };
UINT8 HIDDesc[] = { /* USB HID Report Descriptor */ };
UINT8 StrDesc[] = { /* USB String Descriptors */ };

// Endpoint buffers
UINT8 EP0_Databuf[64];
UINT8 EP1_Databuf[128];

void USB_IRQHandler(void)
{
    // Process USB interrupt
    USB_DevTransProcess();
}

void DevEP1_OUT_Deal(UINT8 len)
{
    // Handle data received on EP1
    UINT8* data = pEP1_OUT_DataBuf;
    // Process received data...

    // Re-enable EP1 OUT
    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
}

void DevEP1_IN_Deal(UINT8 len)
{
    // EP1 IN transfer complete
    // Can prepare next transfer here
}

void USB_HID_SendReport(UINT8* report, UINT8 len)
{
    // Copy report to EP1 IN buffer
    for(UINT8 i = 0; i < len; i++)
    {
        pEP1_IN_DataBuf[i] = report[i];
    }

    // Set transmit length and trigger
    R8_UEP1_T_LEN = len;
    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // Set endpoint buffer addresses
    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;

    // Initialize USB device
    USB_DeviceInit();

    // Enable USB interrupt
    NVIC_EnableIRQ(USB_IRQn);

    printf("USB HID Device Started\r\n");

    while(1)
    {
        // Send HID report periodically
        UINT8 report[2] = {0x00, 0x00};
        USB_HID_SendReport(report, 2);
        Delay_Ms(1000);
    }
}
```

## USB Interrupt Flags

| Flag | Description |
|------|-------------|
| RB_UIF_BUS_RST | Bus reset event (device mode) |
| RB_UIF_TRANSFER | Transfer completion |
| RB_UIF_SUSPEND | Suspend/resume event |
| RB_UIF_FIFO_OV | FIFO overflow |

## Token Types

| Token | Value | Description |
|-------|-------|-------------|
| UIS_TOKEN_OUT | 0x00 | OUT token (host to device) |
| UIS_TOKEN_SOF | 0x10 | SOF token |
| UIS_TOKEN_IN | 0x20 | IN token (device to host) |
| UIS_TOKEN_SETUP | 0x30 | SETUP token |

## Note

The CH32F10x/CH32F20x USB implementation is different from STM32 USB OTG. Refer to the official WCH USB examples in `EVT/EXAM/USB/` for complete implementation details including descriptor tables and class-specific handling.
