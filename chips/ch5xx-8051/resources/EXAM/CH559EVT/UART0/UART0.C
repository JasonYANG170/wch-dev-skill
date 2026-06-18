
/********************************** (C) COPYRIGHT *******************************
* File Name          : UART0.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 串口0自发自收演示
                     (1)、串口0收发数据，波特率可调;              				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS
                                        
 UINT8 DAT,FLAG;

/*******************************************************************************
* Function Name  : CH559UART0Interrupt()
* Description    : CH559UART0中断处理函数
*******************************************************************************/
void CH559UART0Interrupt( )  interrupt INT_NO_UART0 using 1                    //看门狗中断服务程序,使用寄存器组1
{
    if(TI)
    {
        TI = 0;                                                                //清空发送中断                        
    }
    if(RI)
    {
        FLAG = 1;
        RI = 0;                                                                //清空接收中断
        DAT = SBUF;
    }
}

main( ) 
{
    UINT8 i;
//  CfgFsys( );                                                                  //CH559时钟选择配置    
    mDelaymS(5);                                                                 //等待外部晶振稳定		  
//  CH559UART0Alter();    
    FLAG = 0;                                                                    //标志位清空
    mInitSTDIO( );                                                               //串口0初始化函数
    ES = 1;                                                                      //开启UART0中断
    EA = 1;                                                                      //总中断开启
    while(1)
	  {
		    if(FLAG == 1)
		    {
			     SBUF = DAT;
			     FLAG = 0;
		    }
	  }
}


