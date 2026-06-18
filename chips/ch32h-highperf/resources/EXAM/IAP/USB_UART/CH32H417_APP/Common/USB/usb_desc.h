/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/07/31
 * Description        : header file of usb_desc.c
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __USB_DESC_H
#define __USB_DESC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "ch32h417_conf.h"

#define DEF_USB_VID                  0x1A86
#define DEF_USB_PID_VENDOR           0x55E1
#define DEF_USB_PID_HID              0xFE18

#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00

/* Endpoint Number */
#define DEF_UEP0                      0x00
#define DEF_UEP1                      0x01
#define DEF_UEP2                      0x02
#define DEF_UEP3                      0x03
#define DEF_UEP4                      0x04
#define DEF_UEP5                      0x05
#define DEF_UEP6                      0x06
#define DEF_UEP7                      0x07
#define DEF_UEP8                      0x08
#define DEF_UEP9                      0x09
#define DEF_UEP10                     0x0A
#define DEF_UEP11                     0x0B
#define DEF_UEP12                     0x0C
#define DEF_UEP13                     0x0D
#define DEF_UEP14                     0x0E
#define DEF_UEP15                     0x0F


/* usb device endpoint size define */
#define DEF_USBD_UEP0_SIZE          64     /* usb hs/fs device end-point 0 size */
#define DEF_IAP_USB_PACK_SIZE       64


#define DEF_USBD_DEVICE_DESC_LEN     ((uint8_t)MyDevDescr[0])
#define DEF_USBD_CONFIG_DESC_LEN     ((uint16_t)MyCfgDescr[2] + (uint16_t)(MyCfgDescr[3] << 8))
#define DEF_USBD_REPORT_DESC_LEN     34
#define DEF_USBD_LANG_DESC_LEN       ((uint16_t)MyLangDescr[0])
#define DEF_USBD_MANU_DESC_LEN       ((uint16_t)MyManuInfo[0])
#define DEF_USBD_PROD_DESC_LEN       ((uint16_t)MyProdInfo[0])
#define DEF_USBD_SN_DESC_LEN         ((uint16_t)MySerNumInfo[0])

extern const uint8_t MyDevDescr[ ];
extern const uint8_t MyCfgDescr[ ];
extern const uint8_t MyLangDescr[ ];
extern const uint8_t MyManuInfo[ ];
extern const uint8_t MyProdInfo[ ];
extern const uint8_t MySerNumInfo[ ];
extern const uint8_t MyHIDReportDesc[ ];
#ifdef __cplusplus
}
#endif

#endif

