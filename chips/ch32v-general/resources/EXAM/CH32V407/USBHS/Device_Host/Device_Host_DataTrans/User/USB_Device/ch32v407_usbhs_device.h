/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v407_usbhs_device.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/01/19
 * Description        : header file of ch32v407_usbhs_device.c
 *********************************************************************************
 * Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH32V407_USBHS_DEVICE_H__
#define __CH32V407_USBHS_DEVICE_H__

/*******************************************************************************/
/* Header File */
#include "string.h"
// #include "usb_desc.h"
#include "ch32v4x7_conf.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Macro Definition */

/* USB Device Descriptor, Device Serial Number(bcdDevice) */
#define DEF_IC_PRG_VER                DEF_FILE_VERSION

/* USB Device Endpoint Size */
#define DEF_USBD_UEP0_SIZE            64     /* usb hs/fs device end-point 0 size */
/* HS */
#define DEF_USBD_HS_PACK_SIZE         512    /* usb hs device max bluk/int pack size */
#define DEF_USBD_HS_ISO_PACK_SIZE     1024   /* usb hs device max iso pack size */
/* FS */
#define DEF_USBD_FS_PACK_SIZE         64     /* usb fs device max bluk/int pack size */
#define DEF_USBD_FS_ISO_PACK_SIZE     1023   /* usb fs device max iso pack size */
/* LS */
#define DEf_USBD_LS_UEP0_SIZE         8      /* usb ls device end-point 0 size */
#define DEF_USBD_LS_PACK_SIZE         64     /* usb ls device max int pack size */

/* USB Device Endpoint1-6 Size */
/* HS */
#define DEF_USB_EP1_HS_SIZE           DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP2_HS_SIZE           DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP3_HS_SIZE           DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP4_HS_SIZE           DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP5_HS_SIZE           DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP6_HS_SIZE           DEF_USBD_HS_PACK_SIZE
/* FS */
#define DEF_USB_EP1_FS_SIZE           DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP2_FS_SIZE           DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP3_FS_SIZE           DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP4_FS_SIZE           DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP5_FS_SIZE           DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP6_FS_SIZE           DEF_USBD_FS_PACK_SIZE
/* LS */
/* ... */
/* USB Device Descriptor Length */
/* Note: If a descriptor does not exist, set the length to 0. */
#define DEF_USBD_DEVICE_DESC_LEN      ( (uint16_t)MyDevDescr[ 0 ] )
#define DEF_USBD_CONFIG_DESC_LEN      ( (uint16_t)MyCfgDescr[ 2 ] + ( (uint16_t)MyCfgDescr[ 3 ] << 8 ) )
#define DEF_USBD_LANG_DESC_LEN        ( (uint16_t)MyLangDescr[ 0 ] )
#define DEF_USBD_MANU_DESC_LEN        ( (uint16_t)MyManuInfo[ 0 ] )
#define DEF_USBD_PROD_DESC_LEN        ( (uint16_t)MyProdInfo[ 0 ] )
#define DEF_USBD_SN_DESC_LEN          ( (uint16_t)MySerNumInfo[ 0 ] )
#define DEF_USBD_QUALFY_DESC_LEN      ( (uint16_t)MyQuaDesc[ 0 ])
#define DEF_USBD_BOS_DESC_LEN         0
#define DEF_USBD_FS_OTH_DESC_LEN      0
#define DEF_USBD_HS_OTH_DESC_LEN      0

#define DEF_DEVICE_DESC_LEN 18
#define DEF_DESC_LEN 256
#define REPORT_DESC_NUM 2
#define DEF_CONFIG_DESC_LEN    DEF_DESC_LEN
#define DEF_REPORT_DESC_LEN    DEF_DESC_LEN
#define DEF_STRING_DESC_LEN    DEF_DESC_LEN
/* General */
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
#define USBHSD_UEP_RXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) )
#define USBHSD_UEP_TXCTRL( N )        ( *((volatile uint8_t *)( USBHSD_UEP_TXCTL_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) )
#define USBHSD_UEP_TLEN( N )          ( *((volatile uint16_t *)( USBHSD_UEP_TXLEN_BASE + ( N - 1 ) * 0x04 ) ) )


/* USB SPEED TYPE */
#define USBHS_SPEED_TYPE_MASK         ((uint8_t)(0x03))
#define USBHS_SPEED_LOW               ((uint8_t)(0x02))
#define USBHS_SPEED_FULL              ((uint8_t)(0x00))
#define USBHS_SPEED_HIGH              ((uint8_t)(0x01))

/******************************************************************************/
/* Variable Declaration */

/* Ringbuffer define  */
#define DEF_Ring_Buffer_Max_Blks      16
#define DEF_RING_BUFFER_SIZE          (DEF_Ring_Buffer_Max_Blks*DEF_USBD_HS_PACK_SIZE)
#define DEF_RING_BUFFER_REMINE        4
#define DEF_RING_BUFFER_RESTART       12
/* Ring Buffer typedef */
typedef struct __attribute__((packed)) _RING_BUFF_COMM
{
    volatile uint8_t  LoadPtr;
    volatile uint8_t  DealPtr;
    volatile uint8_t  RemainPack;
    volatile uint8_t  StopFlag;
    volatile uint16_t PackLen[DEF_Ring_Buffer_Max_Blks];
} RING_BUFF_COMM, *pRING_BUFF_COMM;

/* Ringbuffer variables */
extern RING_BUFF_COMM  RingBuffer_Comm;
extern __attribute__ ((aligned(4))) uint8_t  Data_Buffer[ ];

extern volatile uint8_t KB_LED_Cur_Status;


extern volatile uint8_t  USBHS_Endp_Busy[ DEF_UEP_NUM ];
/* SetUp Request Values */
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
extern __attribute__ ((aligned(16))) uint8_t USBHS_EP0_Buf[ ];
extern __attribute__ ((aligned(16))) uint8_t USBHS_EP2_Tx_Buf[ ];
extern  volatile uint8_t  USBHS_Endp_Busy[ DEF_UEP_NUM ];
/* Descriptor Declaration */
extern uint8_t MyDevDescr[ DEF_DEVICE_DESC_LEN ];
extern uint8_t MyCfgDescr[ DEF_CONFIG_DESC_LEN];
extern uint8_t KeyRepDesc[REPORT_DESC_NUM][ DEF_REPORT_DESC_LEN];
extern uint8_t KeyRepDesc_Len[REPORT_DESC_NUM];
extern uint8_t MyLangDescr[DEF_STRING_DESC_LEN ];
extern uint8_t MyManuInfo[ DEF_STRING_DESC_LEN];
extern uint8_t MyProdInfo[DEF_STRING_DESC_LEN ];
extern uint8_t MySerNumInfo[DEF_STRING_DESC_LEN ];
/********************************************************************************/
/* Function Declaration */

extern uint8_t USBHS_Endp_DataUp( uint8_t endp, uint8_t *pbuf, uint16_t len);
extern void USBHS_Device_Endp_Init ( void );
extern void USBHS_Device_Init ( FunctionalState sta );
extern void USBHS_Send_Resume(void);
extern void USBHSD_RCC_Init(FunctionalState sta);
#ifdef __cplusplus
}
#endif

#endif


