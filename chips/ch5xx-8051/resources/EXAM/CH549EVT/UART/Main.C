/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 串口1~3 查询与中断收发演示，实现数据回环
                      注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\UART\UART.H"
#pragma  NOAREGS
void main( )
{
    UINT8 dat;
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0初始化
    printf("UART demo start ...\n");
    CH549UART1Init();                                                          //串口1初始化
    CH549UART1Alter();                                                         //串口1引脚映射
    CH549UART2Init();                                                          //串口2初始化
    CH549UART3Init();                                                          //串口3初始化
    while(1)
    {
#ifndef UART1_INTERRUPT
        dat = CH549UART1RcvByte();
        CH549UART1SendByte(dat);
#endif
#ifndef UART2_INTERRUPT
        dat = CH549UART2RcvByte();
        CH549UART2SendByte(dat);
#endif
#ifndef UART3_INTERRUPT
        dat = CH549UART3RcvByte();
        CH549UART3SendByte(dat);
#endif
    }
}
