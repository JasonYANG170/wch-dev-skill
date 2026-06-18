/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs1_dcp.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Usb high speed device controller 1 port for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBHS1_DC_H_
#define __USBHS1_DC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include "device/usbd_driver.h"

/* @define */
#define USBHS1_DCP_MAX_ENDPOINTS        8
#define USBHS1_DCP_ENDP0_SIZE           64

/* @extern */
extern usbd_handle_t usbhs1d_handle;

/* @function declaration */
usb_rst_e usbhs1_dch_init(usb_bool_t interrupt);

#ifdef __cplusplus
}
#endif

#endif
