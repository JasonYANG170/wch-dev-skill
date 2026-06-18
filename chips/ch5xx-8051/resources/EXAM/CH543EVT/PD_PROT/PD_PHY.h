/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_PHY.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef	__PD_PHY_H__
#define __PD_PHY_H__

/*******************************************************************************/
/* ͷ�ļ����� */
#include "CH543.H"

/*******************************************************************************/
/* ��غ궨�� */
/* Message Type : Control Message */
#define GoodCRC		0x01		// 5B:00001
#define Accept		0x03		// 5B:00011
#define Reject		0x04		// 5B:00100
#define PS_RDY		0x06		// 5B:00110
#define GetSrcCap	0x07		// 5B:00111
#define DR_Swap		0x09		// 5B:01001
#define PR_Swap		0x0A		// 5B:01010
#define SoftRst		0x0D		// 5B:01101
#define NotSup		0x10		// 5B:10000

/* Message Type : Data Message */
#define SRCCap		0x01		// 5B:00001
#define Request		0x02		// 5B:00010
#define VDef		0x0F

/*******************************************************************************/
/* ȫ�ֱ������� */
extern xdata UINT8 PD_RX_BUF[30];
extern xdata UINT8 PD_TX_BUF[30];
extern xdata UINT8 PD_GCRC_BUF[2];

typedef struct  
{
	UINT8 	tmp;
	UINT8	SendMsgID;
	UINT8	WaitingGCRC;		// 0-δ׼������GoodCRC    x-�ƻ�����GoodCRC
	UINT8	QueueSendGCRC;		// 0-δ׼������GoodCRC    x-�ƻ�����GoodCRC
	UINT8	SOPType;			// Ŀǰ���͵�SOP״̬
		
	UINT8	RetryCnt;
		
	UINT8	RecvMsgID;
		
	UINT16	MsgCnt;				//��Ϣ�����͵���ʱ
	UINT8	MsgLen;				//��Ϣ����
		
	UINT8	PortPwrRole;
	UINT8	PortDataRole;
		
	UINT8	ExpectPwrRole;
	UINT8	ExpectDataRole;
	
} _PD_PHY_STAT_st;

extern _PD_PHY_STAT_st  PD_PHY_Stat;

typedef union {				/* Header */
	struct _Header_Struct {
			
		UINT8	MsgType:5;
		UINT8	DataRole:1;		// 0:UFP  1:DFP
		UINT8	SpecRev:2;			// 01:Rev2.0
		
		UINT8	PwrRole:1;		// 0:Sink  1:Source
		UINT8	MsgID:3;
		UINT8	NDO:3;
		UINT8	Extended:1;
		
	} Struct;
	UINT16		Data;
} _Union_Header;

extern _Union_Header	*Header;

/*******************************************************************************/
/* �������� */
extern void Set_TX_GoodCRC( void );
extern void Check_If_GoodCRC( void );
extern void HeaderAutoFill( void );
extern void PD_PHY_Init( void );
extern void PD_PHY_RX_INIT( void );
extern void PD_PHY_TX_INIT( PUINT8 Addr, UINT8 Len, UINT8 sop );

#endif

