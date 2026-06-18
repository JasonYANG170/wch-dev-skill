/********************************** (C) COPYRIGHT *******************************
* File Name          : descriptor.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/11/01
* Description        : Usb descriptor.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "class/hid/hid.h"

#include "descriptor.h"

/* @global */
const desc_device_t device_desc =
{
    .bLength = 0x12,
    .bDescriptorType = 0x01,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x02,
    .bDeviceSubClass = 0x02,
    .bDeviceProtocol = 0x01,
    .bMaxPacketSize0 = 0x40,
    .idVendor = 0x1A86,
    .idProduct = 0xFE50,
    .bcdDevice = USB_DEVICE_DRIVER_VERSION,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x00,
    .bNumConfigurations = 0x01,
};

const desc_qua_t qua_desc =
{
    .bLength = 0x0A,
    .bDescriptorType = 0x06,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x02,
    .bDeviceSubClass = 0x02,
    .bDeviceProtocol = 0x01,
    .bMaxPacketSize0 = 0x40,
    .bNumConfigurations = 0x01,
    .bReserved = 0x00,
};

const uint8_t config_hs_desc[] =
{
    0x09,                                       // bLength
    0x02,                                       // bDescriptorType (Configuration)
    U16_TO_U8S_LE(CONFIG_DESC_SIZE),            // wTotalLength 67
    0x02,                                       // bNumInterfaces 2
    0x01,                                       // bConfigurationValue
    0x00,                                       // iConfiguration (String Index)
    0x80,                                       // bmAttributes
    0x32,                                       // bMaxPower 100mA

    0x09,                                       // bLength
    0x04,                                       // bDescriptorType
    0x00,                                       // bInterfaceNumber
    0x00,                                       // bAlternateSetting
    0x01,                                       // bNumEndpoints
    0x02,                                       // bInterfaceClass
    0x02,                                       // bInterfaceSubClass
    0x01,                                       // bInterfaceProtocol
    0x00,                                       // iInterface

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x00,                                       // bDescriptorSubtype
    0x10, 0x01,                                 // bcdCDC

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x01,                                       // bDescriptorSubtype
    0x00,                                       // bmCapabilities
    0x03,                                       // bDataInterface

    0x04,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x02,                                       // bDescriptorSubtype
    0x02,                                       // bmCapabilities

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x06,                                       // bDescriptorSubtype
    0x02,                                       // bControlInterface
    0x03,                                       // bSubordinateInterface0

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x81,                                       // bEndpointAddress
    0x03,                                       // bmAttributes
    U16_TO_U8S_LE(8),                           // wMaxPacketSize
    0x01,                                       // bInterval

    0x09,                                       // bLength
    0x04,                                       // bDescriptorType
    0x01,                                       // bInterfaceNumber
    0x00,                                       // bAlternateSetting
    0x02,                                       // bNumEndpoints
    0x0A,                                       // bInterfaceClass
    0x00,                                       // bInterfaceSubClass
    0x00,                                       // bInterfaceProtocol
    0x00,                                       // iInterface

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x82,                                       // bEndpointAddress
    0x02,                                       // bmAttributes
    U16_TO_U8S_LE(512),                         // wMaxPacketSize
    0x00,                                       // bInterval

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x03,                                       // bEndpointAddress
    0x02,                                       // bmAttributes
    U16_TO_U8S_LE(512),                         // wMaxPacketSize
    0x00,                                       // bInterval
};

const uint8_t config_fs_desc[] =
{
    0x09,                                       // bLength
    0x02,                                       // bDescriptorType (Configuration)
    U16_TO_U8S_LE(CONFIG_DESC_SIZE),            // wTotalLength 67
    0x02,                                       // bNumInterfaces 2
    0x01,                                       // bConfigurationValue
    0x00,                                       // iConfiguration (String Index)
    0x80,                                       // bmAttributes
    0x32,                                       // bMaxPower 100mA

    0x09,                                       // bLength
    0x04,                                       // bDescriptorType
    0x00,                                       // bInterfaceNumber
    0x00,                                       // bAlternateSetting
    0x01,                                       // bNumEndpoints
    0x02,                                       // bInterfaceClass
    0x02,                                       // bInterfaceSubClass
    0x01,                                       // bInterfaceProtocol
    0x00,                                       // iInterface

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x00,                                       // bDescriptorSubtype
    0x10, 0x01,                                 // bcdCDC

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x01,                                       // bDescriptorSubtype
    0x00,                                       // bmCapabilities
    0x03,                                       // bDataInterface

    0x04,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x02,                                       // bDescriptorSubtype
    0x02,                                       // bmCapabilities

    0x05,                                       // bFunctionLength
    0x24,                                       // bDescriptorType
    0x06,                                       // bDescriptorSubtype
    0x02,                                       // bControlInterface
    0x03,                                       // bSubordinateInterface0

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x81,                                       // bEndpointAddress
    0x03,                                       // bmAttributes
    U16_TO_U8S_LE(8),                           // wMaxPacketSize
    0x01,                                       // bInterval

    0x09,                                       // bLength
    0x04,                                       // bDescriptorType
    0x01,                                       // bInterfaceNumber
    0x00,                                       // bAlternateSetting
    0x02,                                       // bNumEndpoints
    0x0A,                                       // bInterfaceClass
    0x00,                                       // bInterfaceSubClass
    0x00,                                       // bInterfaceProtocol
    0x00,                                       // iInterface

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x82,                                       // bEndpointAddress
    0x02,                                       // bmAttributes
    U16_TO_U8S_LE(64),                          // wMaxPacketSize
    0x00,                                       // bInterval

    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x03,                                       // bEndpointAddress
    0x02,                                       // bmAttributes
    U16_TO_U8S_LE(64),                          // wMaxPacketSize
    0x00,                                       // bInterval
};

static uint8_t _other_speed_desc[CONFIG_DESC_SIZE];

/* @function declaration */
void *_get_string_desc(uint8_t index, size_t *size);

usb_rst_e get_device_desc(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    switch (req->wValue >> 8)
    {
    case USB_DESC_DEVICE:
        *buf = (void *)&device_desc;
        *size = USB_MIN(req->wLength, sizeof(device_desc));
        return USB_RST_OK;

    case USB_DESC_CONFIGURATION:
        if (usbd_get_speed(h) == USB_SPEED_HIGH)
        {
            *buf = (void *)config_hs_desc;
            *size = USB_MIN(req->wLength, CONFIG_DESC_SIZE);
        }
        else
        {
            *buf = (void *)config_fs_desc;
            *size = USB_MIN(req->wLength, CONFIG_DESC_SIZE);
        }
        return USB_RST_OK;

    case USB_DESC_STRING:
        *buf = _get_string_desc(req->wValue & 0xFF, size);
        return *buf ? USB_RST_OK : USB_RST_FAILED;

    case USB_DESC_DEVICE_QUALIFIER:
        *buf = (void *)&qua_desc;
        *size = USB_MIN(req->wLength, sizeof(qua_desc));
        return USB_RST_OK;

    case USB_DESC_OTHER_SPEED_CONFIG:
        if (usbd_get_speed(h) == USB_SPEED_HIGH)
        {
            memcpy(_other_speed_desc, config_fs_desc, sizeof(_other_speed_desc));
        }
        else
        {
            memcpy(_other_speed_desc, config_hs_desc, sizeof(_other_speed_desc));
        }
        _other_speed_desc[1] = USB_DESC_OTHER_SPEED_CONFIG;
        *buf = (void *)_other_speed_desc;
        *size = USB_MIN(req->wLength, CONFIG_DESC_SIZE);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

void *_get_string_desc(uint8_t index, size_t *size)
{
    size_t desc_size;
    static uint16_t desc_buf[64];

    switch (index)
    {
    case 0x00:
    {
        desc_size = 1;
        desc_buf[1] = 0x0409;
        break;
    }

    default:
    {
        uint8_t const *string_array[] =
        {
            "",
            "wch.cn",
            "Dual CDC-ACM Transfer Demo",
        };

        if (index >= sizeof(string_array) / sizeof(uint8_t const *)) return NULL;

        uint8_t const *string = string_array[index];
        desc_size = USB_MIN(strlen(string), sizeof(desc_buf) / sizeof(desc_buf[0]) - 1);
        for (uint8_t i = 0; i < desc_size; i++)
        {
            desc_buf[1 + i] = string[i];
        }
        break;
    }
    }

    *size = 2 + 2 * desc_size;
    desc_buf[0] = (uint16_t)((USB_DESC_STRING << 8) | *size);
    return (uint8_t *)desc_buf;
}
