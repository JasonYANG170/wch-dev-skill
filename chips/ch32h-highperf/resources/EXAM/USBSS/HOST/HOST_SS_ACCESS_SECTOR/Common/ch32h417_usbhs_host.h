/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbhs_host.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/23
* Description        : This file contains the headers of the ch32h417_usbhs handlers.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_USBHS_HOST_H__
#define __CH32H417_USBHS_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"

extern void USBHS_RCC_Init( FunctionalState sta );
extern void USBHS_Host_Init( FunctionalState sta );
extern void USBHSH_CheckRootHubPortStatus( void );

#ifdef __cplusplus
}
#endif

#endif