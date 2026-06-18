/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_inf.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/07/31
 * Description        : header file of usb_desc.c
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __USB_INF_H
#define __USB_INF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "ch32h417_conf.h"
#include "ch32h417_usbfs_device.h"
#include "ch32h417_usbhs_device.h"

/******************************************************************************/
#define DEF_USB_IAP_MODE_VENDOR     0                                           // Vendor mode, requires driver installation
#define DEF_USB_IAP_MODE_HID        1                                           // HID mode, driver-free on most operating systems
#define DEF_USB_IAP_MODE            DEF_USB_IAP_MODE_HID                        // Select the USB IAP device type

/******************************************************************************/
#define DEF_USBFS_PORT_ENABLE       1                                           // Enable USBFS interface
#define DEF_USBHS_PORT_ENABLE       1                                           // Enable USBHS interface

/******************************************************************************/
#define DEF_USB_PORT_FS             0
#define DEF_USB_PORT_HS             1

extern void USB_Init(FunctionalState sta);
extern void USB_Pack_Out_Callback(uint8_t usb_port, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif

