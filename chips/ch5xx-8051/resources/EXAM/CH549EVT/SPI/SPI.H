/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI主、从模式接口函数
注：片选有效时，从机会自动加载SPI0_S_PRE的预置值到发送移位缓冲区，所以最好可以在片选
有效前向SPI0_S_PRE寄存器写入预发值，或者在主机端丢弃首个接收字节，发送时注意主机会先
取走SPI0_S_PRE里面的值产生一个S0_IF_BYTE中断。
如果片选从无效到有效，从机首先进行发送的话，最好把输出的首字节放到SPI0_S_PRE寄存器中；
如果已经处于片选有效的话，数据数据使用SPI0_DATA就可以
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
//#define  SPI_INTERRUPT                                                        //ÖĐ¶ÏÊ¹ÄÜ£¨½öSlaveÄ£Ê½£©
#define  SPI_CK_SET( n ) (SPI0_CK_SE = n)                                     //SPIÊ±ÖÓÉèÖĂº¯Êư 
//MasterÏà¹Øº¯Êư
extern void  SPIMasterModeSet(UINT8 mode);                                    //SPIÖ÷»úÄ£Ê½³ơÊ¼»¯
extern void  CH549SPIMasterWrite(UINT8 dat);                                  //CH549Ó²¼₫SPIĐ´Êư¾Ư£¬Ö÷»úÄ£Ê½
extern UINT8 CH549SPIMasterRead();                                            //CH549Ó²¼₫SPI0¶ÁÊư¾Ư£¬Ö÷»úÄ£Ê½
//SlaveÏà¹Øº¯Êư
extern void  SPISlvModeSet( );                                                //SPI´Ó»úÄ£Ê½³ơÊ¼»¯
extern void  CH549SPISlvWrite(UINT8 dat);                                     //CH549Ó²¼₫SPIĐ´Êư¾Ư£¬´Ó»úÄ£Ê½
extern UINT8 CH549SPISlvRead();                                               //CH549Ó²¼₫SPI0¶ÁÊư¾Ư£¬´Ó»úÄ£Ê½
#endif
