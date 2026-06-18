/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 串口初始化相关
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\UART\UART.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : CH549UART1Init
* Description    : UART1初始化函数,串口默认引脚 P26 P27
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1Init()
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
* Function Name  : CH549UART1Alter()
* Description    : CH549串口1引脚映射,串口映射到 P16 P17
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1Alter()
{
    P1_MOD_OC |= (3<<6);                                                   //准双向模式
    P1_DIR_PU |= (3<<6);
    PIN_FUNC |= bUART1_PIN_X;                                              //开启引脚复用功能
}
/*******************************************************************************
* Function Name  : CH549UART1RcvByte()
* Description    : CH549UART1接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8  CH549UART1RcvByte( )
{
    while((SIF1&bU1RI) == 0)
    {
        ;    //查询接收
    }
    SIF1 = bU1RI;                                                          //清除接收中断
    return SBUF1;
}
/*******************************************************************************
* Function Name  : CH549UART1SendByte(UINT8 SendDat)
* Description    : CH549UART1发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1SendByte(UINT8 SendDat)
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
* Function Name  : UART1Interrupt(void)
* Description    : UART1 中断服务程序
*******************************************************************************/
void UART1Interrupt( void ) interrupt INT_NO_UART1 using 1                //串口1中断服务程序,使用寄存器组1
{
    UINT8 dat;
    if(SIF1&bU1RI)
    {
        SIF1 = bU1RI;                                                     //清除接收完中断
        dat = SBUF1;
        CH549UART1SendByte(dat);
    }
}
#endif
/*******************************************************************************
* Function Name  : CH549UART2Init
* Description    : UART2初始化函数,串口默认引脚 P04 P05
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART2Init()
{
    SCON2 &= ~bU2SM0;                            //选择8位数据通讯
    SCON2 |= bU2SMOD;                            //快速模式
    SCON2 |= bU2REN;                             //使能接收
    SBAUD2 = 0 - FREQ_SYS/16/UART2_BUAD;         //波特率配置
    SIF2 = bU2TI;                                //清空发送完成标志
#ifdef UART2_INTERRUPT                           //开启中断使能
    SCON2 |= bU2IE;                              //开启UART2中断，关闭ADC中断
    IE_UART2 = 1;
    EA = 1;
#endif
}
/*******************************************************************************
* Function Name  : CH549UART2RcvByte()
* Description    : CH549UART2接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8 CH549UART2RcvByte( )
{
    while((SIF2&bU2RI) == 0)
    {
        ;    //查询接收
    }
    SIF2 = bU2RI;                                                          //清除接收中断
    return SBUF2;
}
/*******************************************************************************
* Function Name  : CH549UART2SendByte(UINT8 SendDat)
* Description    : CH549UART2发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART2SendByte(UINT8 SendDat)
{
    SBUF2 = SendDat;
    while((SIF2&bU2TI) == 0)
    {
        ;    //查询等待发送完成
    }
    SIF2 = bU2TI;                                                          //清除发送完成中断
}
#ifdef UART2_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 中断服务程序
*******************************************************************************/
void UART2Interrupt( void ) interrupt INT_NO_UART2 using 1                //串口2中断服务程序,使用寄存器组1
{
    UINT8 dat;
    if(SIF2&bU2RI)
    {
        SIF2 = bU2RI;                                                     //清除接收完中断
        dat = SBUF2;
        CH549UART2SendByte(dat);
    }
}
#endif
/*******************************************************************************
* Function Name  : CH549UART3Init
* Description    : UART3初始化函数,串口默认引脚 P06 P07
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART3Init()
{
    SCON3 &= ~bU3SM0;                            //选择8位数据通讯
    SCON3 |= bU3SMOD;                            //快速模式
    SCON3 |= bU3REN;                             //使能接收
    SBAUD3 = 0 - FREQ_SYS/16/UART3_BUAD;         //波特率配置
    SIF3 = bU3TI;                                //清空发送完成标志
#ifdef UART3_INTERRUPT                           //开启中断使能
    SCON3 |= bU3IE;                              //开启UART3中断，关闭PWM中断
    IE_UART3 = 1;
    EA = 1;
#endif
}
/*******************************************************************************
* Function Name  : CH549UART3RcvByte()
* Description    : CH549UART3接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8 CH549UART3RcvByte( )
{
    while((SIF3&bU3RI) == 0)
    {
        ;    //查询接收
    }
    SIF3 = bU3RI;                                                          //清除接收中断
    return SBUF3;
}
/*******************************************************************************
* Function Name  : CH549UART3SendByte(UINT8 SendDat)
* Description    : CH549UART3发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART3SendByte(UINT8 SendDat)
{
    SBUF3 = SendDat;
    while((SIF3&bU3TI) == 0)
    {
        ;    //查询等待发送完成
    }
    SIF3 = bU3TI;                                                          //清除发送完成中断
}
#ifdef UART3_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 中断服务程序
*******************************************************************************/
void UART3Interrupt( void ) interrupt INT_NO_UART3 using 1                //串口3中断服务程序,使用寄存器组1
{
    UINT8 dat;
    if(SIF3&bU3RI)
    {
        SIF3 = bU3RI;                                                     //清除接收完中断
        dat = SBUF3;
        CH549UART3SendByte(dat);
    }
}
#endif