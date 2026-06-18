/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 串口初始化相关
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __UART_H__
#define __UART_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************/
/* 函数外扩 */
extern UINT8 CH543UART0RcvByte( void );                 					 	/* UART0接收一个字节 */
extern void  CH543UART0SendByte( UINT8 SendDat );     							/* UART0发送一个字节 */

#endif
