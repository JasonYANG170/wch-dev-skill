/********************************** (C) COPYRIGHT ********************************
* File Name          : DataFlash.H
* Author             : WCH
* Version            : V1.7
* Date               : 2023/05/31
* Description        : CH554 DataFlash
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

/*******************************************************************************/
/* 头文件 */
#include "..\Public\CH554.H"                                                         
#include "..\Public\Debug.H"

/* Check of Flash Operation */ 
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55   

/* Flash Operation Flags */    
extern UINT8 Flash_Op_Check_Byte1;
extern UINT8 Flash_Op_Check_Byte2;

/******************************************************************************/
/* Flash操作 */
extern UINT8 Flash_Op_Unlock( UINT8 flash_type );                              
extern void  Flash_Op_Lock( UINT8 flash_type );                             
extern UINT8 WriteDataFlash(UINT8 Addr,PUINT8 buf,UINT8 len);
extern UINT8 ReadDataFlash(UINT8 Addr,UINT8 len,PUINT8 buf);

#endif