/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI1Master.C
* Author             : WCH
* Version            : V1.3
* Date               : 2019/07/22
* Description        : CH559 SPI1主机接口操作SPI从机
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
/*硬件接口定义*/
/******************************************************************************
使用CH559 硬件SPI1操作CH376
硬件接口 
         CH559        DIR       CH376
         P2.1        <==>       MOSI
         P2.2        <==>       MISO
         P2.3        <==>       SCK
         P1.4        <==>       SCS
*******************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

UINT8  buffer,flag;   
#define SET_SPI1_CK( d )   { SPI1_CK_SE = d; }                                //d>=2

/*******************************************************************************
* Function Name  : CH559SPI1Init()
* Description    : CH559SPI1初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI1Init( )
{
	P1_DIR |= bSCS;
	P2_DIR |= bMOSI1 | bSCK1;                                                  //设置SPI接口的引脚方向
	SPI1_CTRL |= bS1_SCK_OE | bS1_AUTO_IF ;                                    //MISO输出使能，SCK输出使能
	SPI1_CTRL &= ~(bS1_DATA_DIR | bS1_2_WIRE);                                 //使用3线SPI，读数据不启动传输
	                                                                           //如果使能bS1_DATA_DIR，那么发送数据后自动产生一个字节的时钟，用于快速数据收发
	SPI1_CK_SE = 0x20;                                                         //设置SPI工作时钟，可以自己配置
	SPI1_CTRL &= ~bS1_CLR_ALL;                                                 //清空SPI1的FIFO,默认是1，必须置零才能发送数据
}

/***************************************************************************** **
* Function Name  : CH559SPI1Write(UINT8 dat)
* Description    : SPI1输出数据
* Input          : UINT8 dat 数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI1Write(UINT8 dat)
{
//  SCS = 0;	
    SPI1_DATA = dat;                                                           //输出数据
    while((SPI1_STAT & 0x08) == 0);	                                           //等待传输完成	
//如果bS1_DATA_DIR为1，此处可以直接读取一个字节的数据用于快速读写
//   SCS = 1;																	
}

/*******************************************************************************
* Function Name  : CH559SPI1Read()
* Description    : SPI1读数据
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
UINT8 CH559SPI1Read( )
{
    SPI1_DATA = 0xff;                                                          //启动时钟                                               
    while((SPI1_STAT & 0x08) == 0);                                            //等待传输完成	
    return SPI1_DATA;
}

main( ) 
{
    UINT8 ret,i = 0;
//  CfgFsys( );                                                               //时钟配置    
    mDelaymS(5);                                                              //等待内部晶振稳定,必加 
    
    P4_DIR |= bLED2;
    P3_DIR |= bTXD;
    mInitSTDIO( );                                                            //串口0,可以用于调试
    printf("master_SPI1_start ...\n");  
	
    CH559SPI1Init();   
    mDelaymS(100);	//SPI1的初始化 
    while(1)
    { 
	     SCS = 0;
	     CH559SPI1Write(i);
	     mDelayuS(2);
	     ret = CH559SPI1Read();
		 SCS = 1;
	      if(ret != (i^0xff))
        {
            printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //如果不等于发送数据的取反，打印错误信息
        }
        else
        {
            printf("success %02x\n",(UINT16)i);                               
        }
		 i++;
		 mDelaymS(20);   		
    }
}

