/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32x3x5_usbhs_device.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : USBHS2.0
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32X3x5_USBHS_DEVICE_H__
#define __CH32X3x5_USBHS_DEVICE_H__

#include "debug.h"
#include "string.h"
#include "ch32x3x5_usb.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif

/*******************************************************************************/
/* Macro Definition */
#ifndef USBHS_CONTROLLER

#define USBHS_CONTROLLER_1      1
#define USBHS_CONTROLLER_2      2

#define USBHS_CONTROLLER        USBHS_CONTROLLER_1

// #if (USBHS_CONTROLLER == USBHS_CONTROLLER_1)
// #define USBHSD                  USBHS1D
// #define USBHS_IRQn              USBHS1_IRQn
// #define RCC_HBPeriph_USBHS      RCC_HBPeriph_USBHS1
// #define RCC_UTMIcmd(x)          RCC_UTMI1cmd(x)
// #define USBHS_IRQHandler        USBHS1_IRQHandler

#define USBHSD_UEP_RXDMA_BASE         0x40024024
#define USBHSD_UEP_TXDMA_BASE         0x40024040
#define USBHSD_UEP_TXLEN_BASE         0x400240A0
#define USBHSD_UEP_TXCTL_BASE         0x400240A2
// #endif

#if (USBHS_CONTROLLER == USBHS_CONTROLLER_2)
#define USBHSD                  USBHS2D
#define USBHS_IRQn              USBHS2_IRQn
#define RCC_HBPeriph_USBHS      RCC_HBPeriph_USBHS2
#define RCC_UTMIcmd(x)          RCC_UTMI2cmd(x)
#define USBHS_IRQHandler        USBHS2_IRQHandler

#define USBHSD_UEP_RXDMA_BASE         0x40023424
#define USBHSD_UEP_TXDMA_BASE         0x40023440
#define USBHSD_UEP_TXLEN_BASE         0x400234A0
#define USBHSD_UEP_TXCTL_BASE         0x400234A2
#endif

#endif

/* Global define */
#define IF_FULL_SPEED       1

#define DEF_USBD_UEP0_SIZE     64
#if IF_FULL_SPEED
#define USBHS_MAX_PACK_SIZE 64
#else
#define USBHS_MAX_PACK_SIZE 64
#endif
#define pUSBHS_SetupReqPak            ((PUSB_SETUP_REQ)USBHS_EP0_Buf)

#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00
/* Endpoint Number */
#define DEF_UEP_BUSY                  0x01
#define DEF_UEP_FREE                  0x00
#define DEF_UEP_NUM                   16
#define DEF_UEP0                      0x00
#define DEF_UEP1                      0x01
#define DEF_UEP2                      0x02
#define DEF_UEP3                      0x03
#define DEF_UEP4                      0x04
#define DEF_UEP5                      0x05
#define DEF_UEP6                      0x06
#define DEF_UEP7                      0x07
#define DEF_UEP8                      0x08
#define DEF_UEP9                      0x09
#define DEF_UEP10                     0x0A
#define DEF_UEP11                     0x0B
#define DEF_UEP12                     0x0C
#define DEF_UEP13                     0x0D
#define DEF_UEP14                     0x0E
#define DEF_UEP15                     0x0F


#define TEST_ENABLE                   0x01
#define TEST_MASK                     0x0F

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

