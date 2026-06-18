/********************************** (C) COPYRIGHT *******************************
* File Name          : Type_C.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-C使用
                       主模式 设备正反插检测、充电电流通知从设备
                       从模式 检测主机供电能力
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __TYPE_C_H__
#define __TYPE_C_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define     TYPE_C_DFP
//#define     TYPE_C_UFP
sbit VCTL = P1^1;
#define bVCTL 0x02
/* CC½Å¼ì²âµ½µÄADCÖµÅÐ¶ÏDFPµÄ¹©µçÄÜÁ¦ */
#define  DufaultPowerMin  (342-137)
#define  DufaultPowerMax  (342+158)
#define  Power1_5AMin     (771-198)
#define  Power1_5AMax     (771+180)
#define  Power3_0AMin     (1383-310)
#define  Power3_0AMax     (1383+288)
/* DFP Ïà¹Ø¶¨Òå */
#ifdef TYPE_C_DFP
#define DFP_DisableRpUCC1()     {USB_C_CTRL = ~(bUCC1_PU1_EN | bUCC1_PU0_EN);}
#define DFP_DefaultPowerUCC1()  {USB_C_CTRL = USB_C_CTRL & ~bUCC1_PU1_EN | bUCC1_PU0_EN;}
#define DFP_1_5APowerUCC1()     {USB_C_CTRL = USB_C_CTRL | bUCC1_PU1_EN & ~bUCC1_PU0_EN; }
#define DFP_3_0APowerUCC1()     {USB_C_CTRL |= bUCC1_PU1_EN | bUCC1_PU0_EN;}
#define DFP_DisableRpUCC2()     {USB_C_CTRL = ~(bUCC2_PU1_EN | bUCC2_PU0_EN);}
#define DFP_DefaultPowerUCC2()  {USB_C_CTRL = USB_C_CTRL & ~bUCC2_PU1_EN | bUCC2_PU0_EN;}
#define DFP_1_5APowerUCC2()     {USB_C_CTRL = USB_C_CTRL | bUCC2_PU1_EN & ~bUCC2_PU0_EN;}
#define DFP_3_0APowerUCC2()     {USB_C_CTRL |= bUCC2_PU1_EN | bUCC2_PU0_EN;}
#define UCC_Connect_Vlaue       Power3_0AMax
#define UCC_DISCONNECT          0x00                                                       //Éè±¸Î´Á¬½Ó
#define UCC1_CONNECT            0x01                                                       //ÕýÏòÁ¬½Ó
#define UCC2_CONNECT            0x02                                                       //·´ÏòÁ¬½Ó
#define UCC_CONNECT             0x03                                                       //UFP C1ºÍC2¶¼¿ªÆô5.1Kµç×è
extern void  TypeC_DFP_Init( UINT8 Power );                                                //UPF³õÊ¼»¯,ÅäÖÃÊä³öµçÁ÷ÄÜÁ¦
extern UINT8 TypeC_DFP_Insert( void );                                                     //DPF¼ì²âUFPÕý²å£¬·´²åÒÔ¼°Î´²åÈëºÍÒÑ²åÈë
#endif
/* UFP Ïà¹Ø¶¨Òå */
#ifdef TYPE_C_UFP
#define UPF_CC1RdCfg(SS)        (USB_C_CTRL = SS ? (USB_C_CTRL|bUCC1_PD_EN) : 0)           //UCC1 ÏÂÀ­Rd ¿ªÆô»ò¹Ø±Õ
#define UPF_CC2RdCfg(SS)        (USB_C_CTRL = SS ? (USB_C_CTRL|bUCC2_PD_EN) : 0)           //UCC2 ÏÂÀ­Rd ¿ªÆô»ò¹Ø±Õ
#define UPF_PD_Normal           0x01                                                       //Ä¬ÈÏ¹©µçÄÜÁ¦500mA
#define UPF_PD_1A5              0x02                                                       //¹©µçÄÜÁ¦1.5mA
#define UPF_PD_3A               0x03                                                       //¹©µçÄÜÁ¦3mA
#define UPD_PD_DISCONNECT       0xff                                                       //Éè±¸Î´Á¬½Ó
extern void  TypeC_UPF_PDInit( void );                                                     //Type-C UPF³õÊ¼»¯
extern UINT8 TypeC_UPF_PDCheck();                                                          //Type-C UPF¼ì²âDPF¹©µçÄÜÁ¦
#endif
#endif
