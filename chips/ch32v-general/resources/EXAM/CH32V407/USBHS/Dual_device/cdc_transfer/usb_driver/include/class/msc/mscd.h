/********************************** (C) COPYRIGHT *******************************
* File Name          : mscd.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/10/30
* Description        : Msc device class define.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __MSCD_H_
#define __MSCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

#include "class/msc/msc.h"

/* @enum */
typedef enum
{
    MSC_STAGE_READY,
    MSC_STAGE_COMMAND,
    MSC_STAGE_DATA,
    MSC_STAGE_STATUS,
    MSC_STAGE_ERROR,
} msc_stage_e;

/* @struct */
typedef struct
{
    uint8_t itf_num;

    uint8_t ep_in;
    uint8_t ep_out;

    uint8_t ready;
    uint8_t stage;
    uint8_t dir;
    uint32_t total;
    uint32_t offset;
    void *buf_ptr;

    __attribute__((aligned(4))) msc_cbw_t cbw;
    __attribute__((aligned(4))) msc_csw_t csw;
} mscd_interface_t;

#ifdef __cplusplus
}
#endif

#endif
