/********************************** (C) COPYRIGHT *******************************
* File Name          : led.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Led driver for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __LED_H_
#define __LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_define.h"

/* @function declaration */
usb_rst_e led_init(void);
void led_ctrl(uint32_t bit);

#ifdef __cplusplus
}
#endif

#endif
