/********************************** (C) COPYRIGHT *******************************
* File Name          : board.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Board for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __BOARD_H_
#define __BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include "device/usbd_driver.h"

/* @define */
#define BOARD_USBDC_COUNT       2

/* @function declaration */
void board_init(void);
void board_delay_ms(uint32_t ms);
void board_get_mac(uint8_t *mac);
usbd_handle_t *board_usbd_init(uint8_t index, usb_bool_t interrupt);

#ifdef __cplusplus
}
#endif

#endif
