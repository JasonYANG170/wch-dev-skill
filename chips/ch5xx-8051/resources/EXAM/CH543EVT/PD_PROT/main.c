/********************************** (C) COPYRIGHT *******************************
* File Name          : main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/04/28
* Description        : CH543 PD Sink功能, 固定请求5V3A档位
*                      Notice that this example also needs to add TIMER.C file.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/*******************************************************************************/
/* 相关宏定义 */
#define PD_PROT_EXAM 1

/*******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "CH543.H"
#include "DEBUG.H"
#include ".\PD_PROT\PD_PHY.H"
#include ".\PD_PROT\PD_PROT.H"

/********************************************************************************
* Function Name  : main
* Description    : 主循环
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void main( void ) 
{
	CfgFsys( );							                                        /* 24MHz */
	mInitSTDIO( );						                                        /* UART初始化 */
	

	mDelaymS( 10 );
	printf("CH543D\r\n");
	
	
	PD_PHY_Init( );
	
	
	while( 1 ) 
    {					
        /* 检测设备接入 */
		if( ( CC1_CTRL & bCC_CMPO ) && ( !( CC2_CTRL & bCC_CMPO ) ) ) 
        {
			UPD_CTRL &= ~bPD_CC_SEL;
			break;
		}
		if( ( CC2_CTRL & bCC_CMPO ) && ( !( CC1_CTRL & bCC_CMPO ) ) ) 
        {
			UPD_CTRL |= bPD_CC_SEL;
			break;
		}
		mDelaymS( 5 );
	}
	
	UPD_CTRL |= bPD_DMA_EN;		//启动PD DMA
	PD_PHY_RX_INIT( );			//PD进入接收状态
	IE_USBPD = 1;				//开启中断
	EA = 1;
	
	while( 1 ) 
    {
		mDelaymS( 10 );
	}
}


