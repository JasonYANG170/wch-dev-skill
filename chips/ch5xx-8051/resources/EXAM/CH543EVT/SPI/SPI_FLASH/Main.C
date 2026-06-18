/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 SPI Flash读写操作演示
                       注意包含DEBUG.C/SPI.C/SPIFlash.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI_FLASH\SPIFlash.H"
#pragma  NOAREGS

/*******************************************************************************/
/* 变量定义 */
UINT8X buf[64];

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void )
{
    UINT16X i;
	
    CfgFsys( );																	/* CH543时钟选择配置 */
    mDelaymS( 20 );																/* 配置时钟后,建议延时稳定时钟 */
    mInitSTDIO( );                                                             	/* 串口0初始化 */
    printf("SPI Flash demo start ...\n");
    
    for( i = 0; i != 64; i++ )
    {
        buf[ i ] = i;
    }
	
    SPIFlash_Init( );
    EraseExternal4KFlash_SPI( 0 );
    BlukWriteExternalFlash_SPI(3, 64, buf );
    BlukReadExternalFlash_SPI( 0, 64, buf );
    
	for( i = 0; i != 64; i++ )
    {
        printf("%02x ",(UINT16)buf[ i ]);
    }
    printf("done\n");
	
    while( 1 )
    {
		/*********************************************************************/
		/* 主循环处理 */
		
    }
}
