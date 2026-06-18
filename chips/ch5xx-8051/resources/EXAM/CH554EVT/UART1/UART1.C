
/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/07/25
* Description        : CH554 串口1收发  
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\Public\CH554.H"                                                          
#include "..\Public\Debug.H"
#include "UART1.H"
#include "stdio.h"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : UART1Setup()
* Description    : CH554串口1初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1Init( )
{
	U1SM0 = 0;                                                                   //UART1选择8位数据位
	U1SMOD = 1;                                                                  //快速模式
	U1REN = 1;                                                                   //使能接收
	SBAUD1 = 0 - FREQ_SYS/16/UART1_BUAD;
	U1TI = 0;
#if UART1_PINMAP	
    PIN_FUNC |= bUART1_PIN_X;                                                   //映射到P34(R)、P32(T)
#endif

#if UART1_INTERRUPT                                                            //开启中断使能
	IE_UART1 = 1;	
	EA = 1;
#endif	
}
/*******************************************************************************
* Function Name  : CH554UART1RcvByte()
* Description    : CH554UART1接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8  CH554UART1RcvByte( )
{
    while(U1RI == 0);                                                           //查询接收，中断方式可不用
    U1RI = 0;
    return SBUF1;
}

/*******************************************************************************
* Function Name  : CH554UART1SendByte(UINT8 SendDat)
* Description    : CH554UART1发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH554UART1SendByte(UINT8 SendDat)
{
	SBUF1 = SendDat;                                                             //查询发送，中断方式可不用下面2条语句,但发送前需TI=0
	while(U1TI ==0);
	U1TI = 0;
}


#if UART1_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 中断服务程序
*******************************************************************************/
void UART1Interrupt( void ) interrupt INT_NO_UART1 using 1                       //串口1中断服务程序,使用寄存器组1
{
	UINT8 dat;
	if(U1RI)
	{
		dat = SBUF1;
		U1RI = 0;
		
		CH554UART1SendByte(dat);
	}
}
#endif

