/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 串口初始化相关
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/*******************************************************************************/
/* 头文件包含 */
#include ".\UART\UART.H"
#pragma  NOAREGS

/********************************************************************************
* Function Name  : CH543UART0RcvByte( void )
* Description    : CH543UART0接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
********************************************************************************/
UINT8 CH543UART0RcvByte( void )
{
    while( RI == 0 );    														/* 查询接收 */
	RI = 0;                                   	                      			/* 清除接收中断标志位 */
    return SBUF;
}

/********************************************************************************
* Function Name  : CH543UART0SendByte( UINT8 SendDat )
* Description    : CH543UART0发送一个字节
* Input          : SendDat -> 要发送的数据
* Output         : None
* Return         : None
********************************************************************************/
void CH543UART0SendByte( UINT8 SendDat )
{
	TI = 0;
    SBUF = SendDat;
    while( TI == 0 );															/* 查询等待发送完成 */
    TI = 0;                                                          			/* 清除发送完成中断 */
}
