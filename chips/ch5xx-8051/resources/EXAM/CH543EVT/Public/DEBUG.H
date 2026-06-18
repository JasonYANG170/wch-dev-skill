/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.H
* Author             : WCH
* Version            : V1.3
* Date               : 2023/5/30
* Description        : CH5XX DEBUG Interface
                     (1)、主频设置;
                     (2)、us\ms基本延时函数;										 
                     (3)、串口0输出打印信息,波特率可调;            				   
                     (4)、看门狗初始化和赋值函数;    										 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



#ifndef	__DEBUG_H__
#define __DEBUG_H__

/*******************************************************************************/
/* 头文件包含 */
#include <absacc.h>
#include <stdio.h>
#include <string.h>
#include <intrins.h>
#include ".\Public\CH543.H"

/*******************************************************************************/
/* 相关宏定义 */

/* 函数返回值 */
#ifndef  SUCCESS
#define  SUCCESS		1
#endif
#ifndef  FAIL
#define  FAIL			0
#endif

/* 定时器起始 */
#ifndef  START
#define  START			1
#endif
#ifndef  STOP
#define  STOP			0
#endif

/* 调试开关 */
#ifndef  DE_PRINTF                       
#define  DE_PRINTF		1
#endif

/* 系统主频24MHz */
/* 注: 48MHz主频仅对定制芯片使用 */
#define	 FREQ_SYS		24000000

/* PD接收主频24MHz */
#ifndef  UPD_TMR_RX
#define	UPD_TMR_RX = UPD_TMR_RX_24M;
#define	UPD_TMR_TX = UPD_TMR_TX_24M;
#endif

/* 快速唤醒 */
#define	 WAKEUP_FAST	0				         

/* 波特率 */
#ifndef  UART0BUAD
#define  UART0BUAD     	115200
#endif

/* 求最小值 */
#ifndef MIN
#define MIN( a, b )		( ( ( a ) <= ( b ) ) ? ( a ) : ( b ) )
#endif

/*******************************************************************************/
/* 函数外扩 */
extern void CfgFsys( void );                        							/* CH543时钟选择和配置 */
extern void mDelayuS( UINT16 n );              									/* 以uS为单位延时 */
extern void	mDelaymS( UINT16 n );              									/* 以mS为单位延时 */
extern void CH543UART0Alter( void );                 							/* CH543串口0引脚映射到P3.0/P3.1 */
extern void	mInitSTDIO( void );                     							/* T1作为波特率发生器 */
extern void CH543SoftReset( void );                 							/* CH543软复位 */
extern void CH543WDTModeSelect( UINT8 mode );	   								/* 看门狗模式选择 */
extern void CH543WDTFeed( UINT8 tim );           								/* 看门狗喂狗 */
	
#endif

