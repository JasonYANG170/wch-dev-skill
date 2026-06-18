/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH545串口0~1 查询与中断收发演示，实现数据回环                     
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "DEBUG.H"
#include "UART.H"

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
	CH545UART0Alter();                                                         //串口0引脚映射
	printf("ID=%02X\n", (UINT16)CHIP_ID);
	printf("UART demo start ...\n");

	CH545UART1Init();                                                          //串口1初始化
	CH545UART1Alter();                                                         //串口1引脚映射   

	dat = 0;
	
	while(1){
#ifndef UART1_INTERRUPT
		dat = CH545UART1RcvByte();
		CH545UART1SendByte(dat);
		
#endif
	}
}