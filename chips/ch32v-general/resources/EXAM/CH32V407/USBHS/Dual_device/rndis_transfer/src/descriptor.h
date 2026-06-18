/********************************** (C) COPYRIGHT *******************************
* File Name          : descriptor.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/10
* Description        : Usb descriptor.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __DESCRIPTOR_H_
#define __DESCRIPTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "device/usbd_driver.h"

/* @define */
#define CONFIG_DESC_SIZE                    67

/* @extern */
extern const desc_device_t device_desc;
extern const desc_qua_t qua_desc;
extern const uint8_t config_hs_desc[];
extern const uint8_t config_fs_desc[];

/* @function declaration */
usb_rst_e get_device_desc(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);

#ifdef __cplusplus
}
#endif

#endif
