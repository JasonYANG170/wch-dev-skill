/********************************** (C) COPYRIGHT ********************************
* File Name          : DataFlash.H
* Author             : WCH
* Version            : V1.3
* Date               : 2023/05/31
* Description        : CH543 DataFlash字节读写函数定义
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/***************************** Flash空间划分 ***********************************
*   0x3FFD  ---
*            |                   Code Flash(BOOT区域 1.75K)
*   0x3900  ---
*   0x38FF  ---
*            |                   Data Flash（256）
*   0x3800  ---
*   0x37FF  ---
*            |                   Code Flash（用户代码区14K）
*   0x0000  ---
*******************************************************************************/

/* Check of Flash Operation */ 
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55   

/* Flash Operation Flags */    
extern UINT8 Flash_Op_Check_Byte1;
extern UINT8 Flash_Op_Check_Byte2;

/******************************************************************************/
/* 函数外扩 */
/* Code Flash和Data Flash操作 */
extern UINT8 Flash_Op_Unlock( UINT8 flash_type );                               /* Flash�������� */
extern void Flash_Op_Lock( UINT8 flash_type );                                  /* Flash�������� */
extern UINT8 FlashErasePage( UINT16 Addr );                                     /* 用于页擦除, 每64字节为一页, 将页内所有数据变为0x00 */
extern UINT8 FlashProgByte( UINT16 Addr, UINT8 Data );                          /* Flash字节编程 */
extern UINT8 FlashProgPage( UINT16 Addr, PUINT8X Buf, UINT8 len );              /* 页编程,仅编程当前Addr所在页 */
extern UINT8 FlashReadBuf( UINT16 Addr, PUINT8 buf, UINT16 len );               /* 读Flash(包含data和code) */

/* OTP区与ReadOnly区操作 */
extern UINT8  FlashProgOTPbyte( UINT8 Addr, UINT8 Data );                       /* 单字节写OTP区域，只能0变成1,且不可擦除 */
extern UINT32 FlashReadOTPword( UINT8 Addr );                                   /* 4字节为单位读取ReadOnly区或者OTP区 */

#endif

/*********************************END OF FILE**********************************/
