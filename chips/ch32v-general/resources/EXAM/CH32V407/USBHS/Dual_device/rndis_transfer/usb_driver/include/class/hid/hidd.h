/********************************** (C) COPYRIGHT *******************************
* File Name          : hidd.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/08/21
* Description        : Hid device class define.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __HIDD_H_
#define __HIDD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "usb_define.h"
#include "class/hid/hid.h"

/* @struct */
typedef struct
{
    uint8_t itf_num;

    usb_endp_t ep_in;
    usb_endp_t ep_out;

    uint8_t itf_protocol;
    uint8_t protocol_mode;
    uint8_t idle_rate;
} hidd_interface_t;

#ifdef __cplusplus
}
#endif

#endif
