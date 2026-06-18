/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_PHY.C
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
#include "Timer.H"
#include ".\PD_PROT\PD_PHY.H"
#include ".\PD_PROT\PD_PROT.H"

/*******************************************************************************/
/* 全局变量定义 */
xdata UINT8 PD_RX_BUF[ 30 ];		//接收Buffer
xdata UINT8 PD_TX_BUF[ 30 ];		//发送Buffer
xdata UINT8 PD_GCRC_BUF[ 2 ];		//发送GoodCRC使用单独的Buffer

_PD_PHY_STAT_st  PD_PHY_Stat;		//PD所用参数变量结构体
_Union_Header	*Header;			//Header消息解析指针

/********************************************************************************
* Function Name  : PD_PHY_ISR
* Description    : PD中断处理函数
					接收消息成功：回复GoodCRC，并交协议层处理
					发送消息成功：若需要收取GoodCRC则启动定时器等待
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void PD_PHY_ISR( void ) interrupt INT_NO_USBPD using 1
{
	
	/* 接收到复位信息中断 */
	if( PIF_RX_RST ) 
    {
		printf("RST");
		
		/* 此处可进行PD HRST相关处理 */
		
		
		
		
		PD_PHY_RX_INIT( );
	}
	
	/* 数据包发送完成中断 */
	if( PIF_TX_END ) 
    {		
		if( !( UPD_CTRL & bPD_CC_SEL ) ) 
        {
            CC1_CTRL &= ~bCC_LVO;	                                            /* 发送完成关闭低压 */
        }
		else 
        {
            CC2_CTRL &= ~bCC_LVO;
        }    
        
		if( PD_PHY_Stat.WaitingGCRC ) 
        {
			mTimer_x_SetData( 0, 4000 );		                                // 1ms @ 48MHz 收取GoodCRC的倒计时
		}
		PD_PHY_RX_INIT( );
	}
	
	
	/* 数据包接受完成中断 */
	if( PIF_RX_ACT ) 
    {
		if( ( UPD_INT_FG & MASK_PD_STAT ) == PD_RX_SOP0 ) 
        {	
            /* 收到SOP数据 */
			
			if( PD_PHY_Stat.WaitingGCRC ) 
            {			
                /* 正在等待GoodCRC消息 */
				Check_If_GoodCRC( );
			}
			else 
            {
				Set_TX_GoodCRC( );		                                        //收到消息，回复GoodCRC
				PD_PROT_HANDLE( );
			}
		}
		PD_PHY_RX_INIT( );
	}
}

/********************************************************************************
* Function Name  : Timer0Interrupt
* Description    : 定时器函数，提供1ms定时，可供用户增加以1ms为时基的任务
					当协议通讯时，定时器会被短暂地修改，此时并不会触发用户定时任务，
					但确实会导致用户的定时任务时间略微偏移。
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void Timer0Interrupt( void ) interrupt INT_NO_TMR0                      
{  
	if( PD_PHY_Stat.QueueSendGCRC ) 
    {			
        /* 回复GoodCRC任务 */
		PD_PHY_TX_INIT( PD_GCRC_BUF , 2 , PD_PHY_Stat.SOPType );
		PD_PHY_Stat.QueueSendGCRC = 0;
	}
	else if( PD_PHY_Stat.WaitingGCRC ) 
    {		
        /* 收取GoodCRC计时 */
		
		PD_PHY_Stat.RetryCnt --;
		if( PD_PHY_Stat.RetryCnt ) 
        {
			PD_PHY_TX_INIT( PD_TX_BUF , PD_PHY_Stat.MsgLen , PD_PHY_Stat.SOPType );
		}
		else 
        {
			PD_PHY_Stat.WaitingGCRC = 0;
			/* 复位流程 */
		}
	}
	else 
    {										
        /* 协议计划发送任务 */
		if( PD_PHY_Stat.MsgCnt ) 
        {
			PD_PHY_Stat.MsgCnt --;
			if( !PD_PHY_Stat.MsgCnt ) 
            {
				PD_PHY_Stat.RetryCnt = 3;
				PD_PHY_TX_INIT( PD_TX_BUF , PD_PHY_Stat.MsgLen , PD_PHY_Stat.SOPType );
				PD_PHY_Stat.WaitingGCRC = 1;
			}
		}
		
		/*  此处添加通用定时任务 */
		
		
		
		
		/*  此处添加通用定时任务 */
		
	}
	mTimer_x_SetData(0,4000);	               // 1ms @ 48MHz
}

/********************************************************************************
* Function Name  : Set_TX_GoodCRC
* Description    : 检查数据合法性，重置定时器为40us，生成GoodCRC数据
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void Set_TX_GoodCRC( void )
{
	if( UPD_T_SOP ) 
    {
        return;
    }
    
	mTimer_x_SetData( 0, 200 );		                                            // 50us @ 48MHz
	
	Header = PD_RX_BUF;
	PD_PHY_Stat.RecvMsgID = Header->Struct.MsgID;		                        //更新MsgID
	PD_PHY_Stat.QueueSendGCRC = 1;				                                //置发送GoodCRC标志位
	PD_PHY_Stat.SOPType = UPD_SOP0;
	
	Header = PD_GCRC_BUF;			                                            //生成GoodCRC消息
	HeaderAutoFill( );
	Header->Struct.MsgID = PD_PHY_Stat.RecvMsgID;
}

/********************************************************************************
* Function Name  : Check_If_GoodCRC
* Description    : 检查收到的消息是否为合法GoodCRC
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void Check_If_GoodCRC( void )
{
	Header = PD_RX_BUF;
	
	if( !( Header->Struct.MsgID ^ PD_PHY_Stat.SendMsgID ) && !( Header->Struct.MsgType ^ GoodCRC ) ) 
    {
		PD_PHY_Stat.SendMsgID++;
		PD_PHY_Stat.WaitingGCRC = 0;
	}
	
}

/********************************************************************************
* Function Name  : HeaderAutoFill
* Description    : 按照统一的模板填充消息Header
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void HeaderAutoFill( void )
{
	Header->Struct.Extended = 0;
	Header->Struct.NDO = 0;
	Header->Struct.MsgID = PD_PHY_Stat.SendMsgID;
	Header->Struct.SpecRev = 1;
	Header->Struct.MsgType = GoodCRC;
	Header->Struct.DataRole = PD_PHY_Stat.PortDataRole;
	Header->Struct.PwrRole = PD_PHY_Stat.PortPwrRole;
	
}

/********************************************************************************
* Function Name  : PD_PHY_Init
* Description    : 初始化PD PHY和定时器0
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void PD_PHY_Init( void )
{
	P1_MOD_OC &= ~( bCC1 | bCC2 );			//初始化CC状态
	P1_DIR_PU &= ~( bCC1 | bCC2 );
	CC1_CTRL = CC2_CTRL = bCC_CE | bCC_PD;
	
	PD_PHY_Stat.WaitingGCRC=0;				//初始化PD所用变量
	PD_PHY_Stat.QueueSendGCRC=0;	
	PD_PHY_Stat.RetryCnt=0;
	PD_PHY_Stat.SendMsgID=0;
	PD_PHY_Stat.RecvMsgID=0;
	PD_PHY_Stat.PortPwrRole=0;
	PD_PHY_Stat.PortDataRole=0;
	PD_PHY_Stat.ExpectPwrRole=1;
    PD_PHY_Stat.ExpectDataRole=1;
	PD_PHY_Stat.MsgCnt = 0;
	PD_PHY_Stat.tmp = 0x5A;
	
	mTimer0Clk12DivFsys( );					//TMR0 12分频
	mTimer_x_ModInit( 0, 1 );		      	//TMR0 16位
	mTimer_x_SetData( 0, 4000 );	      	// 1ms @ 48MHz
	ET0 = 1;                              	//启用定时器中断
	TR0 = 1;
}

/********************************************************************************
* Function Name  : PD_PHY_RX_INIT
* Description    : 初始化PD PHY以接收一包消息
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void PD_PHY_RX_INIT( void )
{
	UPD_INT_EN = bPIE_RX_ACT | bPIE_RX_RST;
	
	UPD_CTRL |= ( bPD_CLR_ALL );
	UPD_CTRL &= ~( bPD_CLR_ALL | bPD_TX_EN );
	
	UPD_DMA_L = PD_RX_BUF;
	UPD_TIMER = FREQ_SYS/400000 - 1;
	PD_START = 1;
}

/********************************************************************************
* Function Name  : PD_PHY_TX_INIT
* Description    : 初始化PD PHY以发送一包消息
* Input          : PUINT8 Addr -> 发送消息DMA地址
*                  UINT8 Len -> 发送消息长度（Byte）
*                  UINT8 sop -> 消息的SOP类型
* Output         : None
* Return         : None
********************************************************************************/
void PD_PHY_TX_INIT( PUINT8 Addr, UINT8 Len, UINT8 sop )
{
	PD_START = 0;
	UPD_INT_EN = bPIE_TX_END;
	
	UPD_T_SOP = sop;
	UPD_T_LEN = Len;
	
	UPD_CTRL |= ( bPD_CLR_ALL | bPD_TX_EN );
	UPD_CTRL &= ~( bPD_CLR_ALL );
	
	UPD_DMA_L = Addr;
	UPD_TIMER = FREQ_SYS/600000 - 1;
	
	if( !( UPD_CTRL & bPD_CC_SEL ) ) 
    {
        CC1_CTRL |= bCC_LVO;	                   //发送时置低压
    }    
	else 
    {
        CC2_CTRL |= bCC_LVO;
    }
	PD_START = 1;
}