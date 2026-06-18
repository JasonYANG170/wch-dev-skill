/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_udisk.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/30
* Description        : This file contains all the functions prototypes for the 
*                      ch32h417_udisk.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_UDSIK_H_
#define __CH32H417_UDSIK_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
//#ifdef BIG_ENDIAN
//#define USB_BO_CBW_SIG			  0x55534243									/* 命令块CBW识别标志'USBC' */
//#define USB_BO_CSW_SIG			  0x55534253									/* 命令状态块CSW识别标志'USBS' */
//#else
#define USB_BO_CBW_SIG			  0x43425355									/* 命令块CBW识别标志'USBC' */
#define USB_BO_CSW_SIG			  0x53425355									/* 命令状态块CSW识别标志'USBS' */
//#endif

#define USB_BO_CBW_SIZE			  0x1F											/* 命令块CBW的总长度 */
#define USB_BO_CSW_SIZE			  0x0D											/* 命令状态块CSW的总长度 */
#define USB_BO_DATA_IN			  0x80
#define USB_BO_DATA_OUT			  0x00


typedef union _BULK_ONLY_CMD
{
	struct
	{
		UINT32 mCBW_Sig;
		UINT32 mCBW_Tag;
		UINT32 mCBW_DataLen;													/* 输入: 数据传输长度 */
		UINT8  mCBW_Flag;														/* 输入: 传输方向等标志 */
		UINT8  mCBW_LUN;
		UINT8  mCBW_CB_Len;														/* 输入: 命令块的长度,有效值是1到16 */
		UINT8  mCBW_CB_Buf[16];													/* 输入: 命令块,该缓冲区最多为16个字节 */
	} mCBW;																		/* BulkOnly协议的命令块, 输入CBW结构 */
	struct
	{
		UINT32 mCSW_Sig;
		UINT32 mCSW_Tag;
		UINT32 mCSW_Residue;													/* 返回: 剩余数据长度 */
		UINT8  mCSW_Status;														/* 返回: 命令执行结果状态 */
	} mCSW;																		/* BulkOnly协议的命令状态块, 输出CSW结构 */
} BULK_ONLY_CMD;

/******************************************************************************/
/* 变量外扩 */
extern UINT8  gDiskMaxLun;				    									/* 磁盘最大逻辑单元号 */
extern UINT8  gDiskCurLun;	    												/* 磁盘当前操作逻辑单元号 */
extern UINT32 gDiskCapability;		    										/* 磁盘总容量 */
extern UINT32 gDiskPerSecSize;	    											/* 磁盘扇区大小 */
extern UINT8  gDiskBulkInEp;													/* USB大容量存储设备的IN端点地址 */
extern UINT8  gDiskBulkOutEp;													/* USB大容量存储设备的OUT端点地址 */
extern UINT16 gDiskBulkInEpSize;  	    										/* USB大容量存储设备的IN端点最大包大小 */
extern UINT16 gDiskBulkOutEpSize;  												/* USB大容量存储设备的OUT端点最大包大小 */
extern UINT8  gDiskInterfNumber;												/* USB大容量存储设备的接口号 */
extern BULK_ONLY_CMD	mBOC;													/* BulkOnly传输结构 */


#define	DEFAULT_MAX_OPERATE_SIZE      	8192									/* 默认当前操作最大包大小 */
#define		MAX_DATA_ADDR	0x20030000
extern UINT8 U30HOST_MS_CofDescrAnalyse( UINT8 *pbuf );
extern UINT8 MS_Init(  UINT8 *pbuf );
extern UINT8 MS_ReadSector( UINT32 StartLba, UINT16 SectCount, PUINT8 DataBuf );
extern UINT8 MS_WriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );

extern UINT8 CHRV3BulkOnlyCmd( UINT8 *DataBuf );
extern UINT8 U20HOST_Issue_BulkOut( UINT8 *pDatBuf, UINT32 *pSize );
extern UINT8 U20HOST_Issue_BulkIn( UINT8 *pDatBuf, UINT32 *pSize );
extern UINT8 U20HOST_MS_CofDescrAnalyse( UINT8 *pbuf );
extern UINT8V U30_TIME_OUT;
extern UINT8 tx_lmp_port;
extern UINT8 Hot_Reset( UINT8 *pdata );
extern UINT8V Hot_ret_flag;
extern UINT8 *pNTFS_BUF;
extern UINT8 gUdisk_flag;
extern UINT8 gUdisk_flag1;
extern UINT16 gUdisk_delay;
extern uint8_t U30HOST_Issue_Bulk( uint8_t EndpNum, uint8_t *SeqNum, uint8_t PacketNum, uint8_t *pDatBuf, UINT32 *pSize, uint8_t Type, uint8_t BrustNum );

#ifdef __cplusplus
}
#endif

#endif

