/********************************** (C) COPYRIGHT *******************************
* File Name          : FLASH.H
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH555 Flash
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef	__DATAFLASH_H__
#define __DATAFLASH_H__

#include "Debug.H"
/*********************** Flash空间划分 *****************************
* 0xF400 —— 0xFFFD  Code Flash(BOOT区域3K) 
* 0xF000 —— 0xF3FF  Data Flash（1K）
* 0x0000 —— 0xEFFF  Code Flash（用户代码区60K）
******************************************************************/

/* Check of Flash Operation */ 
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55   

/* Flash Operation Flags */    
extern UINT8 Flash_Op_Check_Byte1;
extern UINT8 Flash_Op_Check_Byte2;

/* Code Flash和Data Flash操作 */
extern UINT8 Flash_Op_Unlock( UINT8 flash_type );                               
extern void  Flash_Op_Lock( UINT8 flash_type ); 
extern UINT8 FlashErasePage(UINT16 Addr);
extern UINT8 FlashProgByte(UINT16 Addr, UINT8 Data);
extern UINT8 FlashProgPage(UINT16 Addr, PUINT8X Buf, UINT8 len);
extern UINT8 FlashReadBuf(UINT16 Addr, PUINT8 buf, UINT16 len);

/* OTP区与ReadOnly区操作 */
extern UINT8  FlashProgOTPbyte(UINT8 Addr, UINT8 Data);
extern UINT32 FlashReadOTPword(UINT8 Addr);

#endif