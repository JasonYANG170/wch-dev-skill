/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ���ڳ�ʼ�����
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __UART_H__
#define __UART_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
/* UART1相关 */
#define   UART1_INTERRUPT                           //定义是否开启串口1中断
#define   UART1_BUAD           115200               //串口1波特率配置
/* UART2相关 */
#define   UART2_INTERRUPT                           //定义是否开启串口2中断
#define   UART2_BUAD           115200               //串口2波特率配置
/* UART3相关 */
//#define   UART3_INTERRUPT                           //定义是否开启串口3中断
#define   UART3_BUAD           115200               //串口2波特率配置
/* 外部调用子函数 */
extern void  CH549UART1Init();                      //UART1初始化函数
extern void  CH549UART1Alter();                     //串口1引脚映射
extern UINT8 CH549UART1RcvByte( );                  //UART1接收一个字节
extern void  CH549UART1SendByte(UINT8 SendDat);     //UART1发送一个字节
extern void  CH549UART2Init();                      //UART2初始化函数
extern UINT8 CH549UART2RcvByte( );                  //UART2接收一个字节
extern void  CH549UART2SendByte(UINT8 SendDat);     //UART2发送一个字节
extern void  CH549UART3Init();                      //UART3初始化函数
extern UINT8 CH549UART3RcvByte( );                  //UART3接收一个字节
extern void  CH549UART3SendByte(UINT8 SendDat);     //UART3发送一个字节
#endif
