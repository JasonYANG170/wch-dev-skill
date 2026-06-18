/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : CH555串口1主从机收发演示，连接UART从机进行数据收发                    
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "DEBUG.H"
#include "UART.H"

/*硬件接口定义*/
/******************************************************************************
使用CH555 硬件UART接口
         CH555
         P1.6——RXD1_
         P1.7——TXD1_
*******************************************************************************/

/* Global Variable */ 
UINT8 dat;

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	CfgFsys();
	mDelaymS(20);
	mInitSTDIO();                                                             /* 为了让计算机通过串口监控演示过程 */
	CH555UART0Alter();                                                         //串口0引脚映射
	printf("ID=%02X\n", (UINT16)CHIP_ID);
	printf("UART demo start ...\n");

	CH555UART1Init();                                                          //串口1初始化
	CH555UART1Alter();                                                         //串口1引脚映射   

	dat = 1;
#if	UART1_HOST
	CH555UART1SendByte(dat);				                                				   //主机端发送数据
#endif
	
	while(1){
	};
}