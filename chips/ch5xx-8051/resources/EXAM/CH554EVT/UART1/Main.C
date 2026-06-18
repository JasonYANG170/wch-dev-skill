
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/07/25
* Description        : CH544 串口1 收发                   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\Public\CH554.H"                                                  
#include "..\Public\Debug.H"
#include "UART1.H"
#include "stdio.h"
#include <string.h>

#pragma  NOAREGS

void main( ) 
{
#if ( UART1_INTERRUPT == 0) 
	UINT8 dat;
#endif
    CfgFsys( );                                                                //CH554时钟选择配置   
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0调试端口初始化
    printf("start ...\n"); 

    UART1Init( );                                                              //串口1初始化
	while(1)
	{
#if ( UART1_INTERRUPT == 0)                                                    //查询方式，收到一个字节，并转发出去
		dat = CH554UART1RcvByte( );                                            //程序死等，直到收到一个字节
		CH554UART1SendByte(dat);                                               //并通过串口1发回去
#endif		
	}

}