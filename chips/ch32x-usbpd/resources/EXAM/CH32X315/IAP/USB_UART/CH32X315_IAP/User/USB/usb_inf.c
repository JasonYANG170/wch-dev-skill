#include "debug.h"
#include "usb_inf.h"
#include "iap.h"
#include "usb_desc.h"

__attribute__((aligned(4))) uint8_t USB_In_Buf[DEF_IAP_USB_PACK_SIZE] = {0};

/**
 * @brief  Initialize USB ports (USBFS/USBHS).
 * 
 * This function enables or disables the USBFS and USBHS device ports 
 * depending on the given state. For USBHS, it also disables the SWD 
 * (Serial Wire Debug) to free up the corresponding pins.
 * 
 * @param sta  ENABLE to initialize USB ports, DISABLE to deinitialize them.
 */
void USB_Init(FunctionalState sta)
{
    if (sta == ENABLE) 
    {
        /* Enable USBHS Port */
        USBHS_Device_Init(ENABLE);
    }
    else
    {
        /* Enable USBHS Port */
        USBHS_Device_Init(DISABLE);
    }
}

/**
 * @brief  Send a USB data packet to the host.
 * 
 * This function transmits data through the specified USB port (USBFS or USBHS).
 * It copies the data into the endpoint buffer and updates the endpoint 
 * transmission control registers. The packet length depends on the 
 * selected USB IAP mode (HID or Vendor).
 * @param len       Length of the data to send (ignored in HID mode).
 * 
 * @return Always returns 0.
 */
uint8_t USB_Pack_In(uint8_t len)
{
        USBHSD->UEP2_TX_DMA = (uint32_t)USB_In_Buf;
#if (DEF_USB_IAP_MODE == DEF_USB_IAP_MODE_HID)
        USBHSD->UEP2_TX_LEN = DEF_IAP_USB_PACK_SIZE;
#else
        USBHSD->UEP2_TX_LEN = len;
#endif
        USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;
    return 0;
}

/**
 * @brief  Callback function for receiving USB data packets.
 * 
 * This function is triggered when a packet is received from the host.  
 * It copies the received data into the IAP processing buffer, processes 
 * the command, and if necessary, sends a response back to the host.
 * 
 * @param usb_port  USB port used (DEF_USB_PORT_FS or DEF_USB_PORT_HS).
 * @param len       Length of the received data.
 */
void USB_Pack_Out_Callback(uint8_t len)
{
    uint8_t s;
    uint8_t *pbuf_out;
    pbuf_out = USBHS_EP2_Rx_Buf;

    memcpy(IAP_Deal_Buf,pbuf_out,len);
    s = RecData_Deal();

    if (s != ERR_End) 
    {
        memset(USB_In_Buf,0,sizeof(USB_In_Buf));
        USB_In_Buf[0] = 0x00;
        USB_In_Buf[1] = s ? 0x01 : 0x00;
        USB_Pack_In(2);
    }
}