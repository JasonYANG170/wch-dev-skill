/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 GPIO相关函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __GPIO_H__
#define __GPIO_H__

/******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/******************************************************************************/
/* 相关宏定义 */

/****************** GPIO_Init 参数定义 ******************/
/* 端口定义 */
#define  PORT1      0x01
#define  PORT3      0x03

/* 引脚定义（可以组合） */
#define  PIN0       0x01
#define  PIN1       0x02
#define  PIN2       0x04
#define  PIN3       0x08
#define  PIN4       0x10
#define  PIN5       0x20
#define  PIN6       0x40
#define  PIN7       0x80

/* 模式定义 */
#define  MODE0      0x00                                                        /* 高阻输入模式，引脚没有上拉电阻 */
#define  MODE1      0x01                                                        /* 推挽输出模式，具有对称驱动能力，可以输出或者吸收较大电流 */
#define  MODE2      0x02                                                        /* 开漏输出，支持高阻输入，引脚没有上拉电阻 */
#define  MODE3      0x03                                                        /* 准双向模式(标准 8051)，开漏输出，支持输入，引脚有上拉电阻(默认模式) */

/****************** GPIO_INT_Init 参数定义 ********************/
/* IntSrc 外部中断(可组合) */
#define  INT_RXD0_L  0x0001
#define  INT_P31_L   0x0002							                            /* 注意串口引脚映射 */
#define  INT_RST_H   0x0004                                                     /* 唯一一个高电平（上升沿）有效中断 */
#define  INT_P13_L   0x0008
#define  INT_P14_L   0x0010
#define  INT_P15_L   0x0020
#define  INT_P17_L   0x0040
#define  INT_INT0_L  0x0080
#define  INT_INT1_L  0x0100

/* 中断触发模式 */
#define  INT_LEVEL   0x00                                                        /* 电平模式 */
#define  INT_EDGE    0x01                                                        /* 边沿模式 */

/* 中断状态 */
#define  Disable     0x00                                                        /* 中断无效 */
#define  Enable      0x01                                                        /* 中断使能 */

/******************************************************************************/
/* 函数外扩 */
extern void GPIO_Init( UINT8 PORTx, UINT8 PINx, UINT8 MODEx );                   /* GPIO端口初始化 */
extern void GPIO_INT_Init( UINT16 IntSrc, UINT8 Mode, UINT8 NewState );          /* 外部中断初始化 */

#endif

/*********************************END OF FILE**********************************/
