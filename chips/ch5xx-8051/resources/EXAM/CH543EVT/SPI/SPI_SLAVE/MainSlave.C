/********************************** (C) COPYRIGHT *******************************
* File Name          : MainSlave.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 SPI设备例子演示，连接SPI主机进行数据收发，从机获取主机的数据取反
                       然后发送给主机
                       注意包含DEBUG.C/SPI.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/******************************** 硬件接口定义 **********************************
使用CH543 硬件SPI接口
         CH543
         P1.4        =       SCS
         P1.5        =       MOSI
         P1.6        =       MISO
         P1.7        =       SCK
********************************************************************************/

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI.H"

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void )
{
    UINT8 ret,i = 0;
	
    CfgFsys( );																	/* CH543时钟选择配置 */
    mDelaymS( 5 );                                                              /* 配置时钟后,建议延时稳定时钟 */
    mInitSTDIO( );                                                             	/* 串口0初始化 */
    
	printf("SPI Slave start ...\n");
    SPISlvModeSet( );                                                          	/* SPI从机模式设置 */
    
	while( 1 )
    {
#ifndef SPI_INTERRUPT
        ret = CH543SPISlvRead( );                                               /* 主机保持CS=0 */
        CH543SPISlvWrite( ret ^ 0xFF );                                         /* SPI等待主机把数据取走,SPI 主机每次读之前先将CS=0,读完后CS=1 */
        printf("Read#%02x\n",(UINT16)ret);
#endif
    }
}