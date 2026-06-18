/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_PORT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/04/28
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/*******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "CH543.H"
#include "DEBUG.H"
#include ".\PD_PROT\PD_PHY.H"
#include ".\PD_PROT\PD_PROT.H"

/*******************************************************************************/
/* 全局变量定义 */
_Union_Fixed	*Fixed;

/********************************************************************************
* Function Name  : PD_PROT_HANDLE
* Description    : PD 协议层消息处理，此时已经生成了GoodCRC消息但未发送，
					所以此处生成的消息需要设定发送延时
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void PD_PROT_HANDLE( void )
{
	PD_PHY_Stat.MsgCnt = 0;
	Header = PD_RX_BUF;
	
	if( Header->Struct.Extended == 1 ) 
    {
        return;		                                                            // Extended Message
    }    
	if( Header->Struct.NDO ) 
    {						
        /* Data Message */
		switch( Header->Struct.MsgType ) 
        {
			case SRCCap:
				
				Header = PD_TX_BUF;			                                    //生成Request消息
				Fixed = &PD_TX_BUF[ 2 ];
			
				HeaderAutoFill( );
				Header->Struct.NDO = 1;
				Header->Struct.MsgType = Request;
				Fixed->Data = 0x2CB10411;
			
				PD_PHY_Stat.MsgCnt = 4;		                                    //单位ms
				PD_PHY_Stat.MsgLen = 6;
			
				break;
		}
	}
	else 
    {											
        /* Control Message */
		
	}
}
