/********************************** (C) COPYRIGHT *******************************
 * File Name          : composite_km_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/18
 * Description        : All descriptors for the keyboard and mouse composite device.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "PD_User.h"

/*******************************************************************************/
/* Header File */
#include "usbd_desc.h"

/*******************************************************************************/
/* Device Descriptor */
const uint8_t MyDevDescr[ ] =
{
    0x12,                                                   // bLength
    0x01,                                                   // bDescriptorType
    0x10, 0x02,                                             // bcdUSB
    0x11,                                                   // bDeviceClass
    0x00,                                                   // bDeviceSubClass
    0x00,                                                   // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,                                     // bMaxPacketSize0
    (uint8_t)DEF_USB_VID, (uint8_t)( DEF_USB_VID >> 8 ),    // idVendor
    (uint8_t)DEF_USB_PID, (uint8_t)( DEF_USB_PID >> 8 ),    // idProduct
    0x00, DEF_IC_PRG_VER,                                   // bcdDevice
    0x01,                                                   // iManufacturer
    0x02,                                                   // iProduct
    0x03,                                                   // iSerialNumber
    0x01,                                                   // bNumConfigurations
};

/* Configuration Descriptor Set */
const uint8_t MyCfgDescr[ ] =
{
    /* Configuration Descriptor */
    0x09,                                                   // bLength
    0x02,                                                   // bDescriptorType
    0x19, 0x00,                                             // wTotalLength
    0x01,                                                   // bNumInterfaces
    0x01,                                                   // bConfigurationValue
    0x00,                                                   // iConfiguration
    0xC0,                                                   // bmAttributes: Bus Powered; Remote Wakeup
    0x32,                                                   // MaxPower: 100mA

    /* Interface Descriptor (Keyboard) */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType
    0x00,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting
    0x01,                                                   // bNumEndpoints
    0xEF,                                                   // bInterfaceClass
    0x02,                                                   // bInterfaceSubClass
    0x01,                                                   // bInterfaceProtocol: Keyboard
    0x00,                                                   // iInterface

    /* Endpoint Descriptor  */
    0x07,                                                                        // bLength
    0x05,                                                                        // bDescriptorType (Endpoint)
    0x81,                                                                        // bEndpointAddress (IN/D2H)
    0x03,                                                                        // bmAttributes (Bulk)             //Bulk端点
    0x08, 0x00,                                                                  // wMaxPacketSize 512
    0x64,                                                                        // bInterval 0 (unit depends on device speed)
};

/* Qualifier Descriptor */
const uint8_t  MyQuaDescr[ ] =
{
    0x0A,                                                   // bLength
    0x06,                                                   // bDescriptorType
    0x01, 0x02,                                             // bcdUSB                   USB Version 2.01
    0x11,                                                   // bDeviceClass             Billboard
    0x00,                                                   // bDeviceSubClass
    0x00,                                                   // bDeviceProtocol
    0x40,                                                   // bMaxPacketSize0
    0x01,                                                   // bNumConfigurations
    0x00                                                    // bReserved
};


#define DEF_USB_BB_TYPE         DEF_USB_BB_DP_ALT

#define DEF_USB_BB_USB4         0x1
#define DEF_USB_BB_TBT3         0x2
#define DEF_USB_BB_DP_ALT       0x3

const uint8_t MyBOSDescr[ ] =
{

    0x05,           //bLength           描述符长度
    0x0F,           //bDescriptorType   BOS描述符类型
    0x3D, 0x00,    //wTotalLength      集合长度 
    0x02,           //bNumDeviceCaps    在BOS中独立的设备功能描述符的数量


    /* Billboard Capability Descriptor */         //广告牌功能描述符
    0x30,           //bLength.
    0x10,           //bDescriptorType             //0x10      设备功能描述符
    0x0D,           //bDevCapabilityType          //0x0D      Billboard
    0x01,           //iAddtionalInfoURL           //UAL字符串描述符索引，获取更详细的信息
    0x01,           //bNumber Of Alternate Or USB4Modes         Alternate Or USB4Modes数量
    0x01,           //bPreferred Alternate Or USB4Mode          AUM索引
    0x00, 0x80,     //VCONN Power
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //bmConfigured          //00未指定错误     01AUM配置未尝试或退出   10AUM配置尝试但未进入    11AUM配置成功
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x21, 0x01,     //bcdVersion
    0x02,           //bAdditionalFailureInfo            //bit0：设备容器因缺乏电源而失败     bit1：设备容器因缺乏USB-PD通信而失败，仅在首选字段bmConfigured未设置11b时有效
    0x00,           //bReserved

#if ( DEF_USB_BB_TYPE == DEF_USB_BB_USB4 )
    /* USB设备功能可能受限,请确保电脑支持所连接的设备。*/         //USB Billboard Revision 1.2.2  P17
    0x00, 0xFF,                 //wSVID[0]
    0x00,                       //bAlternate Or SB4Mode[0]
    0x10,  //0x10,              //iAlternate Or USB4ModeString[0]

    /* Billboard AUM Capability Descriptor */
    /* USB设备功能可能受限,请确保电脑支持所连接的设备。*/         //USB Billboard Revision 1.2.2  P17
    0x08,                       //bLength
    0x10,                       //bDescriptorType
    0x0f,                       //bDevCapabilityType
    0x00,                       //bIndex
    0x00, 0xe0, 0x45, 0x20,     //dwAlternateModeVdo
#elif ( DEF_USB_BB_TYPE == DEF_USB_BB_TBT3 )
    /* Thunderbolt设备功能可能受限,请确保电脑支持所连接的Thunderbolt设备。*/
    0x87, 0x80,                 //wSVID[1]
    0x00,                       //bAlternate Or USB4Mode[1]
    0x11,                       //iAlternate Or USB4ModeString[1]

    /* Thunderbolt设备功能可能受限,请确保电脑支持所连接的Thunderbolt设备。*/
    0x08,                       //bLength
    0x10,                       //bDescriptorType
    0x0f,                       //bDevCapabilityType
    0x00,                       //bIndex
    0x10, 0x00, 0x00, 0x00,     //dwAlternateModeVdo
#elif ( DEF_USB_BB_TYPE == DEF_USB_BB_DP_ALT )
    /* 显示器连接可能受限,请确保电脑支持所连接的DisplayPort设备。*/       //DisplayPort
    0x01, 0xFF,                 //wSVID[3]
    0x00,                       //bAlternate Or USB4Mode[3]
    0x11, //0x13,               //iAlternate Or USB4ModeString[3]

    /* 显示器连接可能受限,请确保电脑支持所连接的DisplayPort设备。*/       //DisplayPort
    0x08,                       //bLength
    0x10,                       //bDescriptorType
    0x0f,                       //bDevCapabilityType
    0x01,                       //bIndex
    0xC5, 0x00, 0x0C, 0x00,     //dwAlternateModeVdo
#endif

};


const uint8_t iAlternateDescr[] =
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

/* Language Descriptor */
const uint8_t MyLangDescr[ ] =
{
    0x04,
    0x03,
    0x09,
    0x04
};

/* Manufacturer Descriptor */
const uint8_t MyManuInfo[ ] =
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
    0x2A,
    0x03,
    'U', 0,
    'S', 0,
    'B', 0,
    ' ', 0,
    'B', 0,
    'I', 0,
    'L', 0,
    'L', 0,
    'B', 0,
    'O', 0,
    'A', 0,
    'R', 0,
    'D', 0,
    ' ', 0,
    'D', 0,
    'E', 0,
    'V', 0,
    'I', 0,
    'C', 0,
    'E', 0,
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