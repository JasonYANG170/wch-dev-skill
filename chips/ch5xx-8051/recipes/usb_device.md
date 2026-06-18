# USB Device

> **Summary**: Configure USB device mode on CH5xx 8051 chips -- endpoint setup, DMA buffers, enumeration, HID/CDC class implementation.

## Trigger Intent

- "USB device setup"
- "USB HID device"
- "USB CDC serial"
- "USB endpoint configuration"
- "USB enumeration"
- "USB bulk transfer"

## USB Resources

| Chip | USB Mode | Endpoints |
|---|---|---|
| CH552 | Device only | EP0-EP3 (+EP4) |
| CH551 | Device only | EP0-EP3 |
| CH554 | Device + Host | EP0-EP3 |
| CH555 | Device + Host | EP0-EP3 |
| CH559 | Device + Host | EP0-EP3 (+EP4) |
| CH543 | Device + Host | EP0-EP3 |
| CH549 | Device + Host | EP0-EP3 |

## USB Device Registers

| Register | Address | Purpose |
|---|---|---|
| UDEV_CTRL | 0xD1 | Physical port control |
| USB_CTRL | 0xE2 | Base control (enable, pullup, reset) |
| USB_DEV_AD | 0xE3 | Device address (0-127) |
| USB_INT_EN | 0xE1 | Interrupt enable |
| USB_INT_FG | 0xD8 | Interrupt flags |
| USB_INT_ST | 0xD9 | (RO) Interrupt status |
| USB_MIS_ST | 0xDA | (RO) Miscellaneous status |
| USB_RX_LEN | 0xDB | (RO) Received data length |
| UEP0_CTRL | 0xDC | Endpoint 0 control |
| UEP0_T_LEN | 0xDD | EP0 transmit length |
| UEP1_CTRL | 0xD2 | Endpoint 1 control |
| UEP1_T_LEN | 0xD3 | EP1 transmit length |
| UEP2_CTRL | 0xD4 | Endpoint 2 control |
| UEP2_T_LEN | 0xD5 | EP2 transmit length |
| UEP3_CTRL | 0xD6 | Endpoint 3 control |
| UEP3_T_LEN | 0xD7 | EP3 transmit length |
| UEP4_CTRL | 0xDE | Endpoint 4 control |
| UEP4_T_LEN | 0xDF | EP4 transmit length |

### DMA Buffer Address Registers

| Register | Address | Purpose |
|---|---|---|
| UEP0_DMA_L/H | 0xEC/0xED | EP0 buffer start address |
| UEP1_DMA_L/H | 0xEE/0xEF | EP1 buffer start address |
| UEP2_DMA_L/H | 0xE4/0xE5 | EP2 buffer start address |
| UEP3_DMA_L/H | 0xE6/0xE7 | EP3 buffer start address |

### Endpoint Mode Registers

| Register | Address | Purpose |
|---|---|---|
| UEP4_1_MOD | 0xEA | EP4 and EP1 buffer mode |
| UEP2_3_MOD | 0xEB | EP2 and EP3 buffer mode |

### USB_INT_FG Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | U_IS_NAK | (RO) NAK received |
| 6 | U_TOG_OK | (RO) Toggle OK |
| 5 | U_SIE_FREE | (RO) SIE free |
| 4 | UIF_FIFO_OV | FIFO overflow flag |
| 2 | UIF_SUSPEND | Suspend/resume flag |
| 1 | UIF_TRANSFER | Transfer complete flag |
| 0 | UIF_BUS_RST | Bus reset flag |

### USB_INT_ST Bits

| Bits | Name | Purpose |
|---|---|---|
| [7] | bUIS_IS_NAK | NAK received |
| [6] | bUIS_TOG_OK | Toggle OK |
| [5:4] | MASK_UIS_TOKEN | Token: 00=OUT, 01=SOF, 10=IN, 11=SETUP |
| [3:0] | MASK_UIS_ENDP | Endpoint number |

### USB_CTRL Bits

| Bits | Purpose |
|---|---|
| bUC_DEV_PU_EN | Device enable + internal pullup |
| bUC_INT_BUSY | Auto-NAK during interrupt |
| bUC_RESET_SIE | Force SIE reset |
| bUC_CLR_ALL | Clear FIFO and counters |
| bUC_DMA_EN | DMA enable |

### Endpoint Control Bits

| Bit | Name | Purpose |
|---|---|---|
| bUEP_R_TOG | Expected RX toggle (0=DATA0, 1=DATA1) |
| bUEP_T_TOG | Prepared TX toggle |
| bUEP_AUTO_TOG | Auto toggle after transfer |
| MASK_UEP_R_RES | RX handshake: 00=ACK, 10=NAK, 11=STALL |
| MASK_UEP_T_RES | TX handshake: 00=ACK, 10=NAK, 11=STALL |

### Endpoint Buffer Mode (UEP4_1_MOD / UEP2_3_MOD)

| RX_EN | TX_EN | BUF_MOD | Buffer Layout |
|---|---|---|---|
| 0 | 0 | x | Disabled |
| 1 | 0 | 0 | 64B RX buffer |
| 0 | 1 | 0 | 64B TX buffer |
| 1 | 1 | 0 | 64B RX + 64B TX = 128B |
| 1 | 0 | 1 | Dual 64B RX (toggle) = 128B |
| 0 | 1 | 1 | Dual 64B TX (toggle) = 128B |
| 1 | 1 | 1 | Dual RX + Dual TX = 256B |

## Step-by-Step

### 1. USB Device Init

```c
UINT8 xdata EP0_BUF[64] _at_ 0x0000;   // EP0 buffer at xdata start
UINT8 xdata EP1_BUF[64] _at_ 0x0040;   // EP1 buffer
UINT8 xdata EP2_BUF[64] _at_ 0x0080;   // EP2 buffer

void USB_DeviceInit(void)
{
    // Set USB pins to USB function
    PIN_FUNC |= bUSB_IO_EN;

    // Enable USB device with internal pullup
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;

    // Set device address (0 for enumeration)
    USB_DEV_AD = 0x00;

    // Configure endpoint 0 (control)
    UEP0_DMA_L = (UINT16)EP0_BUF & 0xFF;
    UEP0_DMA_H = (UINT16)EP0_BUF >> 8;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

    // Configure endpoint 1 (e.g., bulk IN)
    UEP1_DMA_L = (UINT16)EP1_BUF & 0xFF;
    UEP1_DMA_H = (UINT16)EP1_BUF >> 8;
    UEP4_1_MOD = bUEP1_TX_EN;           // EP1 TX only
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;

    // Enable USB interrupts
    USB_INT_EN = bUIE_TRANSFER | bUIE_BUS_RST | bUIE_SUSPEND;
    USB_INT_FG = 0xFF;                    // Clear all flags
    IE_USB = 1;
    EA = 1;
}
```

### 2. USB Interrupt Handler (EP0 Control Transfer)

```c
void USB_ISR(void) interrupt INT_NO_USB using 1
{
    if (UIF_BUS_RST) {
        UIF_BUS_RST = 0;
        USB_DEV_AD = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    }

    if (UIF_TRANSFER) {
        UIF_TRANSFER = 0;

        UINT8 token = USB_INT_ST & MASK_UIS_TOKEN;
        UINT8 ep = USB_INT_ST & MASK_UIS_ENDP;

        if (ep == 0) {  // Endpoint 0
            if (token == UIS_TOKEN_SETUP) {
                // Handle SETUP packet
                USB_HandleSetup();
            } else if (token == UIS_TOKEN_IN) {
                // EP0 IN complete
                USB_HandleEP0In();
            } else if (token == UIS_TOKEN_OUT) {
                // EP0 OUT complete
                USB_HandleEP0Out();
            }
        }
    }

    if (UIF_SUSPEND) {
        UIF_SUSPEND = 0;
        // Handle suspend/resume
    }
}
```

### 3. EP0 Setup Packet Handling

```c
typedef struct {
    UINT8 bmRequestType;
    UINT8 bRequest;
    UINT16 wValue;
    UINT16 wIndex;
    UINT16 wLength;
} SETUP_PKT;

void USB_HandleSetup(void)
{
    SETUP_PKT *pkt = (SETUP_PKT *)EP0_BUF;

    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;

    if (pkt->bmRequestType & 0x80) {  // Device-to-Host
        // Standard requests
        switch(pkt->bRequest) {
            case 0x06:  // GET_DESCRIPTOR
                USB_GetDescriptor(pkt);
                break;
            case 0x05:  // SET_ADDRESS
                USB_DEV_AD = pkt->wValue & 0x7F;
                UEP0_T_LEN = 0;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_ACK;
                break;
        }
    } else {  // Host-to-Device
        switch(pkt->bRequest) {
            case 0x09:  // SET_CONFIGURATION
                UEP0_T_LEN = 0;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_ACK;
                break;
        }
    }
}
```

### 4. Send Data on EP1 (Bulk IN)

```c
void USB_EP1_SendData(PUINT8 buf, UINT8 len)
{
    UINT8 i;
    for (i = 0; i < len; i++) {
        EP1_BUF[i] = buf[i];
    }
    UEP1_T_LEN = len;
    UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}
```

### 5. Receive Data on EP2 (Bulk OUT)

```c
void USB_HandleEP2Out(void)
{
    if (USB_INT_ST & bUIS_TOG_OK) {
        UINT8 len = USB_RX_LEN;
        // Process EP2_BUF[0..len-1]
        // Re-enable reception
        UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
    }
}
```

### 6. USB Suspend / Remote Wakeup

```c
// Check suspend status
if (USB_MIS_ST & bUMS_SUSPEND) {
    // USB suspended, enter low power
    PCON |= PD;  // Power down
}

// Remote wakeup
void USB_Wakeup(void)
{
    UDEV_CTRL |= bUD_GP_BIT;  // Force bus activity
    mDelaymS(2);
    UDEV_CTRL &= ~bUD_GP_BIT;
}
```

## Descriptor Example (HID)

```c
code UINT8 DevDesc[] = {
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x10, 0x01, // bcdUSB (1.1)
    0x00,       // bDeviceClass
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    0x40,       // bMaxPacketSize0 (64)
    0x86, 0x1A, // idVendor (0x1A86 = WCH)
    0x37, 0x55, // idProduct (0x5537)
    0x00, 0x01, // bcdDevice
    0x01,       // iManufacturer
    0x02,       // iProduct
    0x03,       // iSerialNumber
    0x01        // bNumConfigurations
};
```

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Device not enumerated | Pullup not enabled | Set bUC_DEV_PU_EN in USB_CTRL |
| EP0 not responding | DMA buffer address wrong | Check UEP0_DMA_L/H values |
| Data corruption | Buffer overflow | Ensure USB_RX_LEN < buffer size |
| Stall on control transfer | Setup packet not handled | Check bmRequestType direction bit |
| No USB interrupts | IE_USB=0 or EA=0 | Enable USB and global interrupts |
| Host can't detect device | USB pins not enabled | Set bUSB_IO_EN in PIN_FUNC |
