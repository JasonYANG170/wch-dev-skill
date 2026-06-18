/********************************** (C) COPYRIGHT *******************************
* File Name          : usart.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/12
* Description        : Usart driver for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USART_H_
#define __USART_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_define.h"
#include "class/cdc/cdc.h"

/* @function declaration */
usb_rst_e usart_init(void);
usb_rst_e usart_config(cdc_line_coding_t *line_coding);
usb_rst_e usart_send(void *buf, size_t size);
size_t usart_receive(void *buf, size_t max_size);

#ifdef __cplusplus
}
#endif

#endif
