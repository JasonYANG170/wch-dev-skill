/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK_FAT.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : USB大容量存储设备调用文件系统库相关头文件
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CH563BAS.H"

#ifndef __UDISK_FAT_H__
#define __UDISK_FAT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* 变量外扩 */
extern UINT8  gDiskMaxLun;				    									/* 磁盘最大逻辑单元号 */
extern UINT8  gDiskCurLun;	    												/* 磁盘当前操作逻辑单元号 */
extern DISK_INFO    	UDisk;													/* U盘相关结构体定义 */	

/*******************************************************************************/
/* 函数外扩 */
/* 以下为EHCI底层相关封装函数 */
/*******************************************************************************/
extern UINT8 USBHOST_BusReset( void );
extern UINT8 USBHOST_EP0_Init( void );
extern UINT8 USBHOST_CtrlTransfer( UINT8 *DatBuf, UINT16 *RetLen );
extern UINT8 USBHOST_GetDeviceDescr( UINT8 *buf );
extern UINT8 USBHOST_GetConfigDescr( UINT8 *buf );
extern UINT8 USBHOST_SetAddress( UINT8 addr );
extern UINT8 USBHOST_SetConfig( UINT8 cfg );
extern UINT8 USBHOST_ClearEndpStall( UINT8 endp );
extern void USBHOST_SetTargetAddr( UINT8 addr );
extern UINT8 USBHOST_Detect_Connect( void );
extern void USBHOST_Parameter_Init( void );
extern UINT8 USBHOST_Enumerate( UINT8 *pbuf );

/******************************************************************************/
/* 以下为U盘底层相关第封装函数 */
extern UINT8 USBHOST_MS_CofDescrAnalyse( UINT8 *pbuf );
extern UINT8 MS_GetMaxLun( void );
extern UINT8 MS_ResetErrorBOC( void );
extern UINT8 MS_ScsiCmd_Process( UINT8 *DataBuf );
extern UINT8 MS_Init( UINT8 *pbuf );
extern UINT8 MS_RequestSense( UINT8 *pbuf );
extern UINT8 MS_DiskInquiry( UINT8 *pbuf );
extern UINT8 MS_DiskCapacity( UINT8 *pbuf );
extern UINT8 MS_DiskTestReady( UINT8 *pbuf );
extern UINT8 MS_ReadSector( UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf );
extern UINT8 MS_WriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );

#ifdef __cplusplus
}
#endif

#endif
