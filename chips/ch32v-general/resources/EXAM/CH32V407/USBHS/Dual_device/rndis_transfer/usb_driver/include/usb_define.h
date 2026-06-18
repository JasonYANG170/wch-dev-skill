/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_define.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/08/13
* Description        : Usb driver define.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USB_DEFINE_H_
#define __USB_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @define */
#define ENDP_DIR(endp)          ((endp) & 0x80)
#define ENDP_NUM(endp)          ((endp) & 0x7F)

#define USB_MIN(a, b)           ((a) < (b) ? (a) : (b))

#define U16_HIGH(_u16)          ((uint8_t)(((_u16) >> 8) & 0x00FF))
#define U16_LOW(_u16)           ((uint8_t)((_u16) & 0x00FF))
#define U16_TO_U8S_BE(_u16)     U16_HIGH(_u16), U16_LOW(_u16)
#define U16_TO_U8S_LE(_u16)     U16_LOW(_u16), U16_HIGH(_u16)

#define U32_BYTE3(_u32)         ((uint8_t) ((((uint32_t) _u32) >> 24) & 0x000000ff))
#define U32_BYTE2(_u32)         ((uint8_t) ((((uint32_t) _u32) >> 16) & 0x000000ff))
#define U32_BYTE1(_u32)         ((uint8_t) ((((uint32_t) _u32) >>  8) & 0x000000ff))
#define U32_BYTE0(_u32)         ((uint8_t) (((uint32_t)  _u32)        & 0x000000ff))

#define U32_TO_U8S_BE(_u32)     U32_BYTE3(_u32), U32_BYTE2(_u32), U32_BYTE1(_u32), U32_BYTE0(_u32)
#define U32_TO_U8S_LE(_u32)     U32_BYTE0(_u32), U32_BYTE1(_u32), U32_BYTE2(_u32), U32_BYTE3(_u32)

/* @typedef */
typedef uint8_t                 usb_bool_t;
typedef uint8_t                 usb_endp_t;

/* Return result enum type */
typedef enum
{
    USB_RST_OK,
    USB_RST_FAILED,
    USB_RST_NULL_PTR,
    USB_RST_NOT_ENABLE,
    USB_RST_NOT_OPEN,
    USB_RST_OVERFLOW,
    USB_RST_EXISTS,
    USB_RST_ERROR_PARAMETER,
    USB_RST_ENDP_BUSY,
    USB_RST_ENDP_STALL,
    USB_RST_UNKNOW_EVENT,
    USB_RST_UNSUPPORTED_ENDP,
    USB_RST_UNSUPPORTED_EVENT,
    USB_RST_UNSUPPORTED_BURST,
} usb_rst_e;

typedef enum
{
    USB_FALSE,
    USB_TRUE,
} usb_bool_e;

typedef enum
{
    USB_SPEED_FULL,
    USB_SPEED_LOW,
    USB_SPEED_HIGH,
    USB_SPEED_UNKNOW,
} usb_speed_e;

typedef enum
{
    ENDP_RESP_ACK,
    ENDP_RESP_NYET,
    ENDP_RESP_NAK,
    ENDP_RESP_STALL,
} endp_resp_e;

typedef enum
{
    ENDP_TOG_DATA0,
    ENDP_TOG_DATA1,
    ENDP_TOG_DATA2,
    ENDP_TOG_MDATA,
} endp_tog_e;

typedef enum
{
    ENDP_TYPE_NORMAL,
    ENDP_TYPE_ISO,
    ENDP_TYPE_BURST,
} endp_type_e;

typedef enum
{
    ENDP_STA_IDLE,
    ENDP_STA_BUSY,
    ENDP_STA_STALL,
} endp_sta_e;

typedef enum
{
    CTRL_STAGE_SETUP,
    CTRL_STAGE_DATA,
    CTRL_STAGE_STATUS,
} ctrl_stage_e;

typedef enum
{
    USB_REQ_CODE_GET_STATUS         = 0x00,
    USB_REQ_CODE_CLEAR_FEATURE      = 0x01,
    USB_REQ_CODE_SET_FEATURE        = 0x03,
    USB_REQ_CODE_SET_ADDRESS        = 0x05,
    USB_REQ_CODE_GET_DESCRIPTOR     = 0x06,
    USB_REQ_CODE_SET_DESCRIPTOR     = 0x07,
    USB_REQ_CODE_GET_CONFIGURATION  = 0x08,
    USB_REQ_CODE_SET_CONFIGURATION  = 0x09,
    USB_REQ_CODE_GET_INTERFACE      = 0x0A,
    USB_REQ_CODE_SET_INTERFACE      = 0x0B,
    USB_REQ_CODE_SYNCH_FRAME        = 0x0C,
} usb_req_code_e;

typedef enum
{
    USB_REQ_FEATURE_ENDPOINT_HALT,
    USB_REQ_FEATURE_REMOTE_WAKEUP,
    USB_REQ_FEATURE_TEST_MODE,
} usb_req_feature_e;

typedef enum
{
    USB_REQ_TYPE_STANDARD,
    USB_REQ_TYPE_CLASS,
    USB_REQ_TYPE_VENDOR,
    USB_REQ_TYPE_INVALID,
} usb_req_type_e;

typedef enum
{
    USB_REQ_RCPT_DEVICE,
    USB_REQ_RCPT_INTERFACE,
    USB_REQ_RCPT_ENDPOINT,
    USB_REQ_RCPT_OTHER,
} usb_req_recipient_e;

typedef enum
{
    USB_DESC_DEVICE                             = 0x01,
    USB_DESC_CONFIGURATION                      = 0x02,
    USB_DESC_STRING                             = 0x03,
    USB_DESC_INTERFACE                          = 0x04,
    USB_DESC_ENDPOINT                           = 0x05,
    USB_DESC_DEVICE_QUALIFIER                   = 0x06,
    USB_DESC_OTHER_SPEED_CONFIG                 = 0x07,
    USB_DESC_INTERFACE_POWER                    = 0x08,
    USB_DESC_OTG                                = 0x09,
    USB_DESC_DEBUG                              = 0x0A,
    USB_DESC_INTERFACE_ASSOCIATION              = 0x0B,

    USB_DESC_BOS                                = 0x0F,
    USB_DESC_DEVICE_CAPABILITY                  = 0x10,

    USB_DESC_FUNCTIONAL                         = 0x21,

    // Class Specific Descriptor
    USB_DESC_CS_DEVICE                          = 0x21,
    USB_DESC_CS_CONFIGURATION                   = 0x22,
    USB_DESC_CS_STRING                          = 0x23,
    USB_DESC_CS_INTERFACE                       = 0x24,
    USB_DESC_CS_ENDPOINT                        = 0x25,
} usb_desc_type_e;

/* @struct */

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    union
    {
        struct
        {
            uint8_t recipient : 5;
            uint8_t type : 2;
            uint8_t direction : 1;
        } bmRequestType_bit;

        uint8_t bmRequestType;
    };

    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_req_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} desc_device_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bReserved;
} desc_qua_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
