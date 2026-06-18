/********************************** (C) COPYRIGHT  *******************************
 * File Name          : device_host_trans.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/04/21
 * Description        : This file contains all the functions prototypes for the USB
 *                      firmware library.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "usb_host_config.h"

#ifndef __DEVICE_HOST_TRANS_H
#define __DEVICE_HOST_TRANS_H

#ifndef USBHS_CONTROLLER

#define USBHS_CONTROLLER_1      1
#define USBHS_CONTROLLER_2      2

#define USBHS_CONTROLLER        USBHS_CONTROLLER_1

#if (USBHS_CONTROLLER == USBHS_CONTROLLER_1)
#define USBHSH                  USBHS1H
#define RCC_HBPeriph_USBHSH     RCC_HBPeriph_USBHS1
#define RCC_Host_UTMIcmd(x)     RCC_UTMI1cmd(x)
#define EXTI_HOST               EXTI_Line18

#define USBHSD                  USBHS2D
#define USBHS_IRQn              USBHS2_IRQn
#define RCC_HBPeriph_USBHSD     RCC_HBPeriph_USBHS2
#define RCC_Device_UTMIcmd(x)   RCC_UTMI2cmd(x)
#define USBHS_IRQHandler        USBHS2_IRQHandler
#define EXTI_DEVICE             EXTI_Line20

#define USBHSD_UEP_RXDMA_BASE   0x40023424
#define USBHSD_UEP_TXDMA_BASE   0x40023440
#define USBHSD_UEP_TXLEN_BASE   0x400234A0
#define USBHSD_UEP_TXCTL_BASE   0x400234A2
#endif

#if (USBHS_CONTROLLER == USBHS_CONTROLLER_2)
#define USBHSH                  USBHS2H
#define RCC_HBPeriph_USBHSH     RCC_HBPeriph_USBHS2
#define RCC_Host_UTMIcmd(x)     RCC_UTMI2cmd(x)
#define EXTI_HOST               EXTI_Line20

#define USBHSD                  USBHS1D
#define USBHS_IRQn              USBHS1_IRQn
#define RCC_HBPeriph_USBHSD     RCC_HBPeriph_USBHS1
#define RCC_Device_UTMIcmd(x)   RCC_UTMI1cmd(x)
#define USBHS_IRQHandler        USBHS1_IRQHandler
#define EXTI_DEVICE             EXTI_Line18

#define USBHSD_UEP_RXDMA_BASE   0x40024024
#define USBHSD_UEP_TXDMA_BASE   0x40024040
#define USBHSD_UEP_TXLEN_BASE   0x400240A0
#define USBHSD_UEP_TXCTL_BASE   0x400240A2
#endif
#endif

#define DEF_KEY_W                           0x1a
#define DEF_KEY_TAB                         0x2b
#define DEF_KEY_LEFT_POINT                  0x37
#define DEF_KEY_C                           0x06
#define DEF_KEY_H                           0x0b
#define DEF_KEY_N                           0x11
#define DEF_KEY_T                           0x17
#define DEF_KEY_ENTER                       0x28
#define DEF_KEY_F9                          0x42
#define DEF_KEY_WIN                         0x08
#define DEF_KEY_F2                          0x3b
#define DEF_KEY_F3                          0x3c
#define DEF_KEY_F4                          0x3d

#define DEF_KEY_RIGHT_ZERO                  0x62
#define DEF_KEY_RIGHT_ONE                   0x59

#define DEF_KEY_LEFT_ONE                    0x1e
#define DEF_KEY_LEFT_ZERO                   0x27

#define DEF_STATE_WAIT_WIN                  0x00
#define DEF_STATE_WAIT_F9                   0x01
#define DEF_STATE_WAIT_ALL_ZERO             0x02
#define DEF_STATE_UPLOAD_WCHCN              0x03


#define UPLOAD_TIME_OUT              1000

// extern volatile uint8_t s_upload; 
extern void USBH_MainDeal( void );
extern void USB_Sleep_Wakeup_CFG( void );
extern void MCU_Sleep_Wakeup_Operate( void );
#endif
