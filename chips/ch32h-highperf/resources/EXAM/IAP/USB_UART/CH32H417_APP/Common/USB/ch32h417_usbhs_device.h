/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_usbhs_device.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/09/18
 * Description        : header file of ch32h417_usbhs_device.c
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH32H417_USBHS_DEVICE_H__
#define __CH32H417_USBHS_DEVICE_H__

/*******************************************************************************/
/* Header File */
#include "ch32h417_conf.h"
#include "string.h"
#include "usb_desc.h"
#include "ch32h417_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Macro Definition */

/* General */
#define pUSBHS_SetupReqPak            ((PUSB_SETUP_REQ)USBHS_EP0_Buf)

#define USBHSD_UEP_RXDMA_BASE         0x40030024
#define USBHSD_UEP_TXDMA_BASE         0x40030040 
#define USBHSD_UEP_TXLEN_BASE         0x400300A0
#define USBHSD_UEP_TXCTL_BASE         0x400300A2
#define USBHSD_UEP_TX_EN( N )         ( (uint16_t)( 0x01 << N ) )
#define USBHSD_UEP_RX_EN( N )         ( (uint16_t)( 0x01 << N ) )

#define DEF_UEP_DMA_LOAD              0 /* Direct the DMA address to the data to be processed */
#define DEF_UEP_CPY_LOAD              1 /* Use memcpy to move data to a buffer */
#define USBHSD_UEP_RXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_RXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TXCTRL( N )        ( *((volatile uint8_t *)( USBHSD_UEP_TXCTL_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TLEN( N )          ( *((volatile uint16_t *)( USBHSD_UEP_TXLEN_BASE + ( N - 1 ) * 0x04 ) ) )


/******************************************************************************/
/* Variable Declaration */

extern const uint8_t *pUSBHS_Descr;

/* Setup Request */
extern volatile uint8_t  USBHS_SetupReqCode;
extern volatile uint8_t  USBHS_SetupReqType;
extern volatile uint16_t USBHS_SetupReqValue;
extern volatile uint16_t USBHS_SetupReqIndex;
extern volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
extern volatile uint8_t  USBHS_DevConfig;
extern volatile uint8_t  USBHS_DevAddress;
extern volatile uint8_t  USBHS_DevSleepStatus;
extern volatile uint8_t  USBHS_DevEnumStatus;

/* HID Class Command */
extern volatile uint8_t  USBHS_DevSpeed;
extern volatile uint16_t USBHS_DevMaxPackLen;

/* Endpoint Buffer */
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP0_Buf[ ];
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP2_Rx_Buf[ ];

/********************************************************************************/
/* Function Declaration */
extern void USBHS_Device_Endp_Init ( void );
extern void USBHS_Device_Init ( FunctionalState sta );

#ifdef __cplusplus
}
#endif

#endif


