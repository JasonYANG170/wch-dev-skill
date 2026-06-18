/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_PROT.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef	__PD_PROT_H__
#define __PD_PROT_H__

/*******************************************************************************/
/* 头文件包含 */
#include "CH543.H"

/*******************************************************************************/
/* 全局变量声明 */
typedef union 
{				
    /* Header */
	struct _Fixed_Struct {
			
		UINT8	CurrentL8:8;		// 0:UFP  1:DFP
		
		UINT8	CurrentH2:2;		// 0:UFP  1:DFP
		UINT8	VoltageL6:6;			// 01:Rev2.
		
		UINT8	VoltageH4:4;			// 01:Rev2.0
		UINT8	PeakCurrent:2;
		UINT8	Reserved:1;		// 0:UFP  1:DFP
		UINT8	EPRMode:1;
		
		UINT8	UnchunkedExtMsg:1;
		UINT8	DualRoleData:1;		// 0:UFP  1:DFP
		UINT8	USBComm:1;
		UINT8	UnconstrainedPower:1;		// 0:Sink  1:Source
		UINT8	USBSuspend:1;
		UINT8	DualRolePwr:1;
		UINT8	Type:2;
		
	} Struct;
	UINT32		Data;
} _Union_Fixed;

extern _Union_Fixed	*Fixed;

/*******************************************************************************/
/* 函数声明 */
extern void PD_PROT_HANDLE ( void );

#endif





