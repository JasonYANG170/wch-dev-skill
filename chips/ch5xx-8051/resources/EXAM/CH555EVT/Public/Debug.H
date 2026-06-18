/********************************** (C) COPYRIGHT *******************************
* File Name          : Debug.H
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/03
* Description        : This file contains all the functions prototypes for UART
*                      Printf , Delay functions.									 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include <absacc.h>
#include <stdio.h>
#include <string.h>
#include <intrins.h>
#include "CH555.H"


#ifndef  SUCCESS
#define  SUCCESS    0
#endif
#ifndef  FAIL
#define  FAIL       0xFF
#endif


#define	 FREQ_SYS	                       24000000	          //系统主频24MHz
#define  OSC_EN_XT                       0                    //外部晶振使能控制
#ifndef  UART0_BUAD
#define  UART0_BUAD                      115200
#define  UART1_BUAD                      57600
#endif
#ifndef  DE_PRINTF                                            //调试打印
#define  DE_PRINTF                      1
#endif

#ifndef MIN
#define MIN(a,b)                (((a) <= (b)) ? (a) : (b))
#endif

void CfgFsys(void);                                           //CH5555Ê±ÖÓÑ¡ÔñºÍÅäÖĂ
void mDelayuS(UINT16 n);                                 //̉ÔuSÎªµ¥Î»ÑÓÊ±
void mDelaymS(UINT16 n);                                 //̉ÔmSÎªµ¥Î»ÑÓÊ±
void mInitSTDIO(void);                                        //T1×÷Îª²῭ØÂÊ·¢ÉúÆ÷
void CH555UART0Alter(void);                                    //CH555´®¿Ú0̉ư½ÅÓ³Éäµ½P0.2/P0.3
void CH555UART0SendByte(UINT8 SendDat);                    //CH555UART0·¢ËÍ̉»¸ö×Ö½Ú
UINT8 CH555UART0RcvByte(void);                                //CH555UART0²éÑ¯·½Ê½½ÓÊỞ»¸ö×Ö½Ú

void CH555WDTModeSelect(UINT8 mode);                       //CH555¿´ĂÅ¹·Ä£Ê½ÉèÖĂ 
void CH555WDTFeed(UINT8 tim);                              //CH555¿´ĂÅ¹·Î¹¹·
void CH555SoftReset(void);                                    //CH555Èí¸´Î»
  
#endif
