/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_usb20.h
* Author             : WCH
* Version            : V1.1
* Date               : 2023/02/16
*
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef USB20_CH56X_USB20_H_
#define USB20_CH56X_USB20_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Global define */
#define U20_MAXPACKET_LEN       512
#define U20_UEP0_MAXSIZE        64

#define UsbSetupBuf     ((PUSB_SETUP)USBHS_EP0_Buf)//endpoint 0

// 00: OUT, 01:SOF, 10:IN, 11:SETUP
#define PID_OUT     0
#define PID_SOF     1
#define PID_IN      2

#define USB_DESCR_LANGID_STRING    0x00
#define USB_DESCR_VENDOR_STRING    0x01
#define USB_DESCR_PRODUCT_STRING   0x02
#define USB_DESCR_SERIAL_STRING    0x03
#define USB_DESCR_OS_STRING        0xee

#define USB_DESCR_TYP_BOS       0x0f
#define USB_DESCR_UNSUPPORTED   0xffff
#define INVALID_REQ_CODE 0xFF

typedef struct __attribute__((packed))
{
   UINT8 dev_speed;
   UINT8 dev_addr;
   UINT8 dev_config_value;
   UINT8 dev_sleep_status;
   UINT8 dev_enum_status;
}DevInfo_Typedef;

typedef struct __PACKED
{
    UINT8 bRequestType;
    UINT8 bRequest;
    UINT8 wValueL;
    UINT8 wValueH;
    UINT8 wIndexL;
    UINT8 wIndexH;
    UINT16 wLength;
} *PUSB_SETUP;

//#define PID_SETUP	3
extern const UINT8 hs_device_descriptor[];
extern const UINT8 hs_config_descriptor[];
extern const UINT8 hs_string_descriptor0[];
extern const UINT8 hs_string_descriptor1[];
extern const UINT8 hs_string_descriptor2[];
extern const UINT8 hs_bos_descriptor[];

void   USB20_Device_Init ( FunctionalState sta );
UINT16 U20_NonStandard_Request_Deal();
UINT16 U20_Standard_Request_Deal();
UINT16 U20_Endp0_IN_Callback(void);


#ifdef __cplusplus
}
#endif

#endif

