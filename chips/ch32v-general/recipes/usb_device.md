# Recipe: USB Device

## When to Use
User wants to implement USB device functionality (CDC serial, HID, vendor-defined).

## Overview
CH32V chips have a built-in USB 2.0 full-speed device peripheral. The USB driver is provided as a library with endpoint management.

## Key Concepts
- USB peripheral clock: 48MHz (from PLL or HSE)
- Endpoint 0: Control (mandatory)
- Endpoints 1-7: Available for bulk/interrupt/isochronous
- Double buffering available on some endpoints

## Example: USB CDC (Virtual COM Port)

```c
#include "ch32v20x.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_istr.h"

// USB clock configuration (48MHz required)
void USB_Clock_Init(void)
{
    // Select USB clock source
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

// USB interrupt handler
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}

// Send data via USB CDC
uint16_t CDC_Send_Data(uint8_t *buf, uint16_t len)
{
    UserToPMABufferCopy(buf, ENDP1_TXADDR, len);
    SetEPTxCount(ENDP1, len);
    SetEPTxValid(ENDP1);
    return len;
}

// Receive data from USB CDC
uint16_t CDC_Receive_Data(uint8_t *buf, uint16_t max_len)
{
    uint16_t len = GetEPRxCount(ENDP3) & 0x3FF;
    if(len > max_len) len = max_len;
    PMAToUserBufferCopy(buf, ENDP3_RXADDR, len);
    SetEPRxValid(ENDP3);
    return len;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();

    USB_Clock_Init();
    USB_Init();

    while(1)
    {
        // Process USB events
        if(bDeviceState == CONFIGURED)
        {
            // USB is configured and ready
            // Send/receive data as needed
        }
    }
}
```

## Example: USB HID Keyboard

```c
// HID report descriptor (keyboard)
const uint8_t HID_ReportDescriptor[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x05, 0x07,  // Usage Page (Key Codes)
    0x19, 0xE0,  // Usage Minimum (224)
    0x29, 0xE7,  // Usage Maximum (231)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x01,  // Logical Maximum (1)
    0x75, 0x01,  // Report Size (1)
    0x95, 0x08,  // Report Count (8)
    0x81, 0x02,  // Input (Data, Variable, Absolute)
    0x95, 0x01,  // Report Count (1)
    0x75, 0x08,  // Report Size (8)
    0x81, 0x01,  // Input (Constant)
    0x95, 0x05,  // Report Count (5)
    0x75, 0x01,  // Report Size (1)
    0x05, 0x08,  // Usage Page (LEDs)
    0x19, 0x01,  // Usage Minimum (1)
    0x29, 0x05,  // Usage Maximum (5)
    0x91, 0x02,  // Output (Data, Variable, Absolute)
    0x95, 0x01,  // Report Count (1)
    0x75, 0x03,  // Report Size (3)
    0x91, 0x01,  // Output (Constant)
    0xC0         // End Collection
};

// Send key press
void HID_Send_Key(uint8_t modifier, uint8_t keycode)
{
    uint8_t report[8] = {modifier, 0, keycode, 0, 0, 0, 0, 0};
    UserToPMABufferCopy(report, ENDP1_TXADDR, 8);
    SetEPTxCount(ENDP1, 8);
    SetEPTxValid(ENDP1);
}

// Send key release
void HID_Send_KeyRelease(void)
{
    uint8_t report[8] = {0};
    UserToPMABufferCopy(report, ENDP1_TXADDR, 8);
    SetEPTxCount(ENDP1, 8);
    SetEPTxValid(ENDP1);
}
```

## USB Pin Assignment

| Signal | Pin | Description |
|--------|-----|-------------|
| USB_D- | PA11 | USB data minus |
| USB_D+ | PA10 (or PA12) | USB data plus |
| USB_DP | PA12 | External pull-up (if needed) |

## Pitfalls
- **48MHz clock required** -- USB needs precise 48MHz; PLL must be configured correctly
- **Pull-up resistor** -- some designs need external 1.5K pull-up on D+
- **Endpoint buffer allocation** -- PMA (Packet Memory Area) is limited; plan carefully
- **USB interrupt priority** -- should be high priority for reliable operation
- **Enumeration timeout** -- host expects response within specific time windows
