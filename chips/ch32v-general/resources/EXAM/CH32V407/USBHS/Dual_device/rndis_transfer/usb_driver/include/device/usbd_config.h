/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_config.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/08/13
* Description        : Usb device config headfile.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBD_CONFIG_H_
#define __USBD_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @define */
#define USBD_CTRL_TABLE_MAX_ITEMS       32
#define USBD_ITF_TABLE_MAX_ITEMS        16

#define USBD_DRIVER_LOG_INFO
#define USBD_DRIVER_LOG_WARNING
#define USBD_DRIVER_LOG_ERROR

#ifdef __cplusplus
}
#endif

#endif
