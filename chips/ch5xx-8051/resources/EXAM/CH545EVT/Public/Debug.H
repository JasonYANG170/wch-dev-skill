/********************************** (C) COPYRIGHT *******************************
* File Name          : Debug.H
* Author             : WCH
* Version            : V1.4
* Date               : 2021/12/15
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

#ifndef NO_XSFR_DEFINE
#include "CH545X.H"
#else
#include "CH545.H"
#endif


//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS    0
#endif
#ifndef  FAIL
#define  FAIL       0xFF
#endif

#ifndef MIN
#define MIN(a,b)                (((a) <= (b)) ? (a) : (b))
#endif


#define	 FREQ_SYS	                       24000000	          //系统主频24MHz
#define  OSC_EN_XT                       0                    //外部晶振使能，默认开启内部晶振
#ifndef  UART0_BUAD
#define  UART0_BUAD                      115200
#define  UART1_BUAD                      57600
#endif
#ifndef  DE_PRINTF                                            //程序Pirntf输出开启
#define  DE_PRINTF                      1
#endif

void CfgFsys(void);                                           //CH545时钟选择和配置
void mDelayuS(UINT16 n);                                 //以uS为单位延时
void mDelaymS(UINT16 n);                                 //以mS为单位延时
void mInitSTDIO(void);                                        //T1作为波特率发生器
void CH545UART0Alter(void);                                    //CH545串口0引脚映射到P0.2/P0.3
void CH545UART0SendByte(UINT8 SendDat);                    //CH545UART0发送一个字节
UINT8 CH545UART0RcvByte(void);                                //CH545UART0查询方式接收一个字节

void CH545WDTModeSelect(UINT8 mode);                       //CH545看门狗模式设置 
void CH545WDTFeed(UINT8 tim);                              //CH545看门狗喂狗
void CH545SoftReset(void);                                    //CH545软复位
  
#endif
