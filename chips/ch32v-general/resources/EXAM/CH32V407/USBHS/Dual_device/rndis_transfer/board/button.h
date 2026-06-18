/********************************** (C) COPYRIGHT *******************************
* File Name          : button.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Button driver for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __BUTTON_H_
#define __BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_define.h"

/* @function declaration */
usb_rst_e btn_init(void);
uint32_t btn_scan(void);

#ifdef __cplusplus
}
#endif

#endif
