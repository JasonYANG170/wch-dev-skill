/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/26
 * Description        : usb device descriptor,configuration descriptor,
 *                      string descriptors and other descriptors.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "usb_desc.h"
#include "hardware.h"
#include "usb_inf.h"

#if (DEF_USB_APP_MODE == DEF_USB_APP_MODE_VENDOR)
/* Device Descriptor */
const uint8_t  MyDevDescr[ ] =
{
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x00, 0x02, // bcdUSB 2.00
    0xFF,       // bDeviceClass
    0xFF,       // bDeviceSubClass
    0xFF,       // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,   // bMaxPacketSize0 64
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),
    (uint8_t)DEF_USB_PID_VENDOR, (uint8_t)(DEF_USB_PID_VENDOR >> 8),
    (uint8_t)DEF_VERSION, (uint8_t)(DEF_VERSION >> 8),
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x00,       // iSerialNumber (String Index)
    0x01,       // bNumConfigurations 1
};

/* Configuration Descriptor(high speed)  */
const uint8_t  MyCfgDescr[ ] =
{
    0x09,        // bLength
    0x02,        // bDescriptorType (Configuration)
    0x20, 0x00,  // wTotalLength 60
    0x01,        // bNumInterfaces 1
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration (String Index)
    0x80,        // bmAttributes
    0x32,        // bMaxPower 100mA

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x02,        // bNumEndpoints 6
    0xFF,        // bInterfaceClass
    0xFF,        // bInterfaceSubClass
    0xFF,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x82,        // bEndpointAddress (IN/D2H)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_IAP_USB_PACK_SIZE, (uint8_t)( DEF_IAP_USB_PACK_SIZE >> 8 ),  // wMaxPacketSize 512
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x02,        // bEndpointAddress (OUT/H2D)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_IAP_USB_PACK_SIZE, (uint8_t)( DEF_IAP_USB_PACK_SIZE >> 8 ),  // wMaxPacketSize 512
    0x00,        // bInterval 0 (unit depends on device speed)
};

#else
/* HID Mode */
/* Device Descriptor */
const uint8_t  MyDevDescr[ ] =
{
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x00, 0x02, // bcdUSB 2.00
    0xFF,       // bDeviceClass
    0xFF,       // bDeviceSubClass
    0xFF,       // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,   // bMaxPacketSize0 64
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),
    (uint8_t)DEF_USB_PID_HID, (uint8_t)(DEF_USB_PID_HID >> 8),
    (uint8_t)DEF_VERSION, (uint8_t)(DEF_VERSION >> 8),
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x00,       // iSerialNumber (String Index)
    0x01,       // bNumConfigurations 1
};

/* Configuration Descriptor(high speed)  */
const uint8_t  MyCfgDescr[ ] =
{
    /* Configuration Descriptor */
    0x09,                                       // bLength
    0x02,                                       // bDescriptorType
    0x29, 0x00,                                 // wTotalLength
    0x01,                                       // bNumInterfaces
    0x01,                                       // bConfigurationValue
    0x03,                                       // iConfiguration (String Index)
    0x80,                                       // bmAttributes Remote Wakeup
    0x23,                                       // bMaxPower 70mA

    /* Interface Descriptor */
    0x09,                                       // bLength
    0x04,                                       // bDescriptorType (Interface)
    0x00,                                       // bInterfaceNumber 0
    0x00,                                       // bAlternateSetting
    0x02,                                       // bNumEndpoints 2
    0x03,                                       // bInterfaceClass
    0x00,                                       // bInterfaceSubClass
    0x00,                                       // bInterfaceProtocol
    0x00,                                       // iInterface (String Index)

    /* HID Descriptor */
    0x09,                                       // bLength
    0x21,                                       // bDescriptorType
    0x11, 0x01,                                 // bcdHID
    0x00,                                       // bCountryCode
    0x01,                                       // bNumDescriptors
    0x22,                                       // bDescriptorType
    (uint8_t)DEF_USBD_REPORT_DESC_LEN, 
    (uint8_t)(DEF_USBD_REPORT_DESC_LEN >> 8),   // wDescriptorLength

    /* Endpoint Descriptor */
    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x02,                                       // bEndpointAddress: OUT Endpoint 2
    0x03,                                       // bmAttributes
    (uint8_t)DEF_IAP_USB_PACK_SIZE, 
    (uint8_t)( DEF_IAP_USB_PACK_SIZE >> 8 ),    // wMaxPacketSize
    0x01,                                       // bInterval

    /* Endpoint Descriptor */
    0x07,                                       // bLength
    0x05,                                       // bDescriptorType
    0x82,                                       // bEndpointAddress: IN Endpoint 2
    0x03,                                       // bmAttributes
    (uint8_t)DEF_IAP_USB_PACK_SIZE, 
    (uint8_t)( DEF_IAP_USB_PACK_SIZE >> 8 ),    // wMaxPacketSize
    0x01,                                       // bInterval
};

/* HID Report Descriptor */
const uint8_t  MyHIDReportDesc[ ] =
{
    0x06, 0x00, 0xFF,                           // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,                                 // Usage (0x01)
    0xA1, 0x01,                                 // Collection (Application)
    0x09, 0x02,                                 //   Usage (0x02)
    0x26, 0xFF, 0x00,                           //   Logical Maximum (255)
    0x75, 0x08,                                 //   Report Size (8)
    0x15, 0x00,                                 //   Logical Minimum (0)
    0x95, 0x40,                                 //   Report Count (64)
    0x81, 0x06,                                 //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x02,                                 //   Usage (0x02)
    0x15, 0x00,                                 //   Logical Minimum (0)
    0x26, 0xFF, 0x00,                           //   Logical Maximum (255)
    0x75, 0x08,                                 //   Report Size (8)
    0x95, 0x40,                                 //   Report Count (64)
    0x91, 0x06,                                 //   Output (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                                       // End Collection
};
#endif
/* Language Descriptor */
const uint8_t  MyLangDescr[] =
{
    0x04, 0x03, 0x09, 0x04
};

/* Manufacturer Descriptor */
const uint8_t  MyManuInfo[ ] =
{
    0x0E, 
    0x03, 
    'w', 0, 
    'c', 0, 
    'h', 0, 
    '.', 0, 
    'c', 0, 
    'n', 0
};

/* Product Information */
const uint8_t MyProdInfo[ ]  =
{
    26 , 
    0x03, 
    'C', 0, 
    'H', 0, 
    '3', 0, 
    '2', 0, 
    'H', 0, 
    '4', 0, 
    '1', 0, 
    '7', 0,
    ' ', 0, 
    'A', 0, 
    'P', 0, 
    'P', 0, 
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[ ] =
{
    0x16, 
    0x03, 
    '0', 0, 
    '1', 0, 
    '2', 0, 
    '3', 0, 
    '4', 0, 
    '5', 0, 
    '6', 0, 
    '7', 0, 
    '8', 0, 
    '9', 0
};
