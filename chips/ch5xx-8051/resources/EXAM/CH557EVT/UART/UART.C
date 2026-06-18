/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 UART初始化相关
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "UART.H"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : CH557UART1Init
* Description    : UART1初始化函数,串口默认引脚 P26 P27
* Input          : None
* Return         : None
*******************************************************************************/
void CH557UART1Init(void)
{
	SCON1 &= ~bU1SM0;                            //选择8位数据通讯
	SCON1 |= bU1SMOD;                            //快速模式
	SCON1 |= bU1REN;                             //使能接收
	SBAUD1 = 0 - FREQ_SYS/16/UART1_BUAD;         //波特率配置
	SIF1 = bU1TI;                                //清空发送完成标志
	
#ifdef UART1_INTERRUPT                           //开启中断使能
	IE_UART1 = 1;
	EA = 1;
	
#endif
}

/*******************************************************************************
* Function Name  : CH557UART1Alter
* Description    : CH557串口1引脚映射,串口映射到 P16 P17
* Input          : None
* Return         : None
*******************************************************************************/
void CH557UART1Alter(void)
{
	P1_MOD_OC |= (3<<6);                                                   //准双向模式
	P1_DIR_PU |= (3<<6);
	PIN_FUNC |= bUART1_PIN_X;                                              //开启引脚复用功能
}

/*******************************************************************************
* Function Name  : CH557UART1RcvByte
* Description    : CH557UART1接收一个字节
* Input          : None
* Return         : SBUF
*******************************************************************************/
UINT8 CH557UART1RcvByte(void)
{
	while((SIF1&bU1RI) == 0)
	{
			;    //查询接收
	}
	SIF1 = bU1RI;                                                          //清除接收中断
	return SBUF1;
}

/*******************************************************************************
* Function Name  : CH557UART1SendByte
* Description    : CH557UART1发送一个字节
* Input          : SendDat；要发送的数据
* Return         : None
*******************************************************************************/
void CH557UART1SendByte(UINT8 SendDat)
{
	SBUF1 = SendDat;
	while((SIF1&bU1TI) == 0)
	{
			;    //查询等待发送完成
	}
	SIF1 = bU1TI;                                                          //清除发送完成中断
}

#ifdef UART1_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt
* Description    : UART1 中断服务程序
* Input          : None
* Return         : None
*******************************************************************************/
void UART1Interrupt(void) interrupt INT_NO_UART1 using 1                //串口1中断服务程序,使用寄存器组1
{
	UINT8 dat;
	
	if(SIF1&bU1RI)
	{
		SIF1 = bU1RI;                                                     //清除接收完中断
		dat = SBUF1;
		CH557UART1SendByte(dat);
	}
}

#endif