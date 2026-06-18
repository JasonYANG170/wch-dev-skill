/********************************** (C) COPYRIGHT *******************************
* File Name          : SPIFlash.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/29
* Description        : CH549 SPI Flash操作接口函数(W25QXX)
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__
#include ".\SPI\SPI.H"                                                 /* °üº¬SPI²Ù×÷»ù±¾º¯Êý */
#define  CMD_STATUS1         0x05
#define  CMD_WR_ENABLE       0x06
#define  CMD_ERASE_4KBYTE    0x20
#define  CMD_ERASE_32KBYTE   0x52
#define  CMD_READ_DATA       0x03
#define  CMD_PAGE_PROG       0x02
#define  CMD_FAST_READ       0x0B
#define  CMD_DEVICE_ID       0x90
/* ²Á³ýº¯Êý */
extern void EraseExternal4KFlash_SPI( UINT32 Dst_Addr );                                       //²Á³ý4K
extern void EraseExternal32KFlash_SPI( UINT32 Dst_Addr );                                      //²Á³ý32K
/* Ð´º¯Êý */
extern void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer);          //Ð´SPI Flashº¯Êý,ÎÞÏÞÖÆ
/* ¶Áº¯Êý */
extern void ReadExternalFlash_SPI( UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer );              //¶ÁSPI Flashº¯Êý,ÎÞÏÞÖÆ
extern void BlukReadExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer);            //¶ÁÈ¡³¤¶È±ØÐëÎª16ÕûÊý±¶£¬ºÃ´¦ÊÇ¶ÁÈ¡ËÙ¶È¸ü¿ì
/* ³õÊ¼»¯Ïà¹Ø */
extern UINT16 SPIFlash_ReadID(void);
extern void   SPIFlash_Init(void);
#endif
