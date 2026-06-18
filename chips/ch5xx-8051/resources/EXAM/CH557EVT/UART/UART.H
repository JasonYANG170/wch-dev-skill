/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 UART
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef	__UART_H__
#define __UART_H__

#include "DEBUG.H"

/* UART1相关 */
#define   UART1_INTERRUPT                           //定义是否开启串口1中断

/* 外部调用子函数 */
void CH557UART1Init();                      //UART1初始化函数
void CH557UART1Alter();                     //串口1引脚映射
UINT8 CH557UART1RcvByte();                  //UART1接收一个字节
void CH557UART1SendByte(UINT8 SendDat);     //UART1发送一个字节

#endif
