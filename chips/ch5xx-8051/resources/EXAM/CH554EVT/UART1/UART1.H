/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/07/25
* Description        : CH554 ����1�շ�  
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#define UART1_INTERRUPT   0          //定义是否使用接收中断接收数据
#define UART1_PINMAP      0          //定义串口1是否开启引脚映射。不开启选择P16(R)、P17(T)。开启选择P34(R)、P32(T)


extern void   UART1Init( );
extern UINT8  CH554UART1RcvByte( );
extern void   CH554UART1SendByte(UINT8 SendDat);