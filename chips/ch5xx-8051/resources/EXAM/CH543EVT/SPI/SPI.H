/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 SPI主、从模式接口函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



#ifndef __SPI_H__
#define __SPI_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************/
/* 相关宏定义 */
//#define  SPI_INTERRUPT                                                        	/* 中断使能(仅Slave模式) */
#define  SPI_CK_SET( n ) 		( SPI0_CK_SE = n )								/* SPI时钟设置函数 */

/*******************************************************************************/
/* 函数外扩 */

/* Master相关函数 */
extern void  SPIMasterModeSet( UINT8 mode );									/* SPI主机模式初始化 */
extern void  CH543SPIMasterWrite( UINT8 dat );                                  /* CH543硬件SPI写数据,主机模式 */
extern UINT8 CH543SPIMasterRead( );                                            	/* CH543硬件SPI0读数据,主机模式 */

/* Slave相关函数 */
extern void  SPISlvModeSet( );                                                	/* SPI从机模式初始化 */
extern void  CH543SPISlvWrite( UINT8 dat );                                     /* CH543硬件SPI写数据,从机模式 */
extern UINT8 CH543SPISlvRead( );                                               	/* CH543硬件SPI0读数据,从机模式 */

#endif
