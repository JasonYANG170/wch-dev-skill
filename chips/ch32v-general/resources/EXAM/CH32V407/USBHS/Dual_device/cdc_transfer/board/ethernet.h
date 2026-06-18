/********************************** (C) COPYRIGHT *******************************
* File Name          : ethernet.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/01/19
* Description        : Ethernet driver for ch32v307.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __ETHERNET_H_
#define __ETHERNET_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "ch32v4x7_eth.h"

#include "usb_define.h"

/* @function declaration */
usb_rst_e ethernet_init(void);
void ethernet_task(void);
void ethernet_get_mac(uint8_t *mac);
void ethernet_mac_filter(uint8_t filter);
usb_bool_t ethernet_is_linked(void);
uint32_t ethernet_get_link_speed(void);
usb_rst_e ethernet_send(void *buf, size_t size);
usb_rst_e ethernet_receive(void *buf, size_t *size);
void ethernet_flow_control(void);

#ifdef __cplusplus
}
#endif

#endif
