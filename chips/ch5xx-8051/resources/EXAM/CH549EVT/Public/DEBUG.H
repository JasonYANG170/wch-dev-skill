/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.H
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH5XX DEBUG Interface
                     (1)、主频设置;
                     (2)、us\ms基本延时函数;										 
                     (3)、串口0输出打印信息，波特率可调;            				   
                     (4)、看门狗初始化和赋值函数;     										 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>

//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#ifndef  DE_PRINTF                       //调试开关
#define  DE_PRINTF     1
#endif

#define	 FREQ_SYS	     24000000	         //系统主频24MHz。注: 48MHz仅对定制芯片使用
#define  OSC_EN_XT     0                 //外部晶振使能，默认开启内部晶振
#define  MCU_WORK_AT_3V3    0			      //MCU工作电压：1-3.3V;0-5V. 注意: 请根据MCU实际工作电压进行设置

#ifndef  UART0BUAD
#define  UART0BUAD          115200
#endif

#ifndef MIN
#define MIN(a,b)                (((a) <= (b)) ? (a) : (b))
#endif


void	CfgFsys( );                        // CH549时钟选择和配置
void	mDelayuS( UINT16 n );              // 以uS为单位延时
void	mDelaymS( UINT16 n );              // 以mS为单位延时
void  CH559UART0Alter();                 // CH549串口0引脚映射到P0.2/P0.3
void	mInitSTDIO( );                     // T1作为波特率发生器
void  CH549SoftReset( );                 // CH549软复位
void  CH549WDTModeSelect(UINT8 mode);	   // 看门狗模式选择
void  CH549WDTFeed(UINT8 tim);           // 喂狗
	
#endif

