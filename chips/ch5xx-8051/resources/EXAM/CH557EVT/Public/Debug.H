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
#include "CH557X.H"


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

void CfgFsys(void);                                           //系统初始化配置
void mDelayuS(UINT16 n);                                 
void mDelaymS(UINT16 n);                                 
void mInitSTDIO(void);                                        //串口初始化
void CH557UART0Alter(void);                                    //UART0引脚重映射P0.2/P0.3
void CH557UART0SendByte(UINT8 SendDat);                    
UINT8 CH557UART0RcvByte(void);                                

void CH557WDTModeSelect(UINT8 mode);                       
void CH557WDTFeed(UINT8 tim);                             
void CH557SoftReset(void);                                  
  
#endif
