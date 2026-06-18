/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v407_usbhs_host.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/01/19
* Description        : header file of ch32v407_usbhs_device.c
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V407_USBHS_HOST_H__
#define __CH32V407_USBHS_HOST_H__

/*******************************************************************************/
/* Header File */
#include "ch32v4x7_conf.h"
#include "ch32v4x7_usb.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USBHS_CONTROLLER

#define USBHS_CONTROLLER_1      1
#define USBHS_CONTROLLER_2      2

#define USBHS_CONTROLLER        USBHS_CONTROLLER_1

#if (USBHS_CONTROLLER == USBHS_CONTROLLER_1)
#define USBHSH                  USBHS1H
#define RCC_HBPeriph_USBHS      RCC_HBPeriph_USBHS1
#define RCC_UTMIcmd(x)          RCC_UTMI1cmd(x)
#endif

#if (USBHS_CONTROLLER == USBHS_CONTROLLER_2)
#define USBHSH                  USBHS2H
#define RCC_HBPeriph_USBHS      RCC_HBPeriph_USBHS2
#define RCC_UTMIcmd(x)          RCC_UTMI2cmd(x)
#endif

#endif

/* Variable Declaration */
extern __attribute__((aligned(4))) uint8_t  RxBuffer[ ];
extern __attribute__((aligned(4))) uint8_t  TxBuffer[ ];

/* USB Setup Request */
#define pUSBHS_SetupRequest        ( (PUSB_SETUP_REQ)TxBuffer )

/* USB Buffer Size */
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE          8          // default maximum packet size for endpoint 0
#endif
#ifndef USBHS_MAX_PACKET_SIZE
#define USBHS_MAX_PACKET_SIZE      64
#endif

/*******************************************************************************/
/* Function Declaration */
extern void USBHS_Host_Init( FunctionalState sta );
extern uint8_t USBHSH_CheckRootHubPortStatus( uint8_t status );
extern uint8_t USBHSH_CheckRootHubPortEnable( void );
extern uint8_t USBHSH_CheckRootHubPortSpeed( void );
extern void USBHSH_SetSelfAddr( uint8_t addr );
extern void USBHSH_ResetRootHubPort( void );
extern uint8_t USBHSH_EnableRootHubPort( uint8_t *pspeed );
extern uint8_t USBHSH_Transact( uint8_t endp_pid, uint16_t endp_tog, uint32_t timeout );
extern uint8_t USBHSH_CtrlTransfer( uint8_t ep0_size, uint8_t *pbuf, uint16_t *plen );
extern uint8_t USBHSH_GetDeviceDescr( uint8_t *pep0_size, uint8_t *pbuf );
extern uint8_t USBHSH_GetConfigDescr( uint8_t ep0_size, uint8_t *pbuf, uint16_t buf_len, uint16_t *pcfg_len );
extern uint8_t USBHSH_GetStrDescr( uint8_t ep0_size, uint8_t str_num, uint8_t *pbuf );
extern uint8_t USBHSH_SetUsbAddress( uint8_t ep0_size, uint8_t addr );
extern uint8_t USBHSH_SetUsbConfig( uint8_t ep0_size, uint8_t cfg_val );
extern uint8_t USBHSH_ClearEndpStall( uint8_t ep0_size, uint8_t endp_num );
extern uint8_t USBHSH_GetEndpData( uint8_t endp_num, uint16_t *pendp_tog, uint8_t *pbuf, uint16_t *plen );
extern uint8_t USBHSH_SendEndpData( uint8_t endp_num, uint16_t *pendp_tog, uint8_t *pbuf, uint16_t len );

#ifdef __cplusplus
}
#endif

#endif


