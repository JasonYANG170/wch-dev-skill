/********************************** (C) COPYRIGHT *******************************
* File Name          : SPIFlash.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 SPI Flash操作接口函数(W25QXX)
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

/*******************************************************************************/
/* ͷ�ļ����� */
#include ".\SPI\SPI.H"															/* ����SPI������������ */

/*******************************************************************************/
/* ��غ궨�� */
#define  CMD_STATUS1         	0x05
#define  CMD_WR_ENABLE       	0x06
#define  CMD_ERASE_4KBYTE    	0x20
#define  CMD_ERASE_32KBYTE   	0x52
#define  CMD_READ_DATA       	0x03
#define  CMD_PAGE_PROG       	0x02
#define  CMD_FAST_READ       	0x0B
#define  CMD_DEVICE_ID       	0x9F

/*******************************************************************************/
/* �������� */

/* �������� */
extern void EraseExternal4KFlash_SPI( UINT32 Dst_Addr );									/* ����4K */
extern void EraseExternal32KFlash_SPI( UINT32 Dst_Addr );                                   /* ����32K */

/* д���� */
extern void BlukWriteExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 SendBuffer );	/* дSPI Flash����,������ */

/* ������ */
extern void ReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer );			/* ��SPI Flash����,������ */
extern void BlukReadExternalFlash_SPI( UINT32 StarAddr, UINT16 Len, PUINT8 RcvBuffer );		/* ��ȡ���ȱ���Ϊ16������,�ô��Ƕ�ȡ�ٶȸ��� */

/* ��ʼ����� */
extern UINT16 SPIFlash_ReadID( void );
extern void   SPIFlash_Init( void );

#endif
