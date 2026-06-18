/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 串口0 查询收发演示,实现数据回环
                      注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\UART\UART.H"
#pragma  NOAREGS

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void )
{
    UINT8 dat;
	
    CfgFsys( );                                                                	/* CH543时钟选择配置 */
    mDelaymS( 20 );																/* 配置时钟后,建议延时稳定时钟 */
    mInitSTDIO( );                                                             	/* 串口0初始化 */
	printf("UART demo start ...\n");
    
	while( 1 )
    {
        dat = CH543UART0RcvByte( );
        CH543UART0SendByte( dat );
    }
}
