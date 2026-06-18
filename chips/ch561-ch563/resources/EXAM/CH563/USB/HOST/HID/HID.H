/********************************** (C) COPYRIGHT *******************************
* File Name          : HID.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : USB主机操作HID类键盘、鼠标相关头文件
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CH563BAS.H"

#ifndef __HID_H__
#define __HID_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* HID类设备相关宏定义 */
#define HID_TYPE_UNKNOW					0x00								   /* HID类未知设备 */
#define HID_TYPE_KEYBOARD				0x01								   /* HID类键盘设备 */
#define HID_TYPE_MOUSE					0x02								   /* HID鼠标设备 */

/******************************************************************************/
/* HID类设备信息结构体定义 */
typedef struct _HidInfo 
{ 
	ATTACH_DEVICE_INFO  Device;													
	qHD_Structure *qHD_INT_In1; 												/* 中断上传端点1 EHCI相关结构体指针定义 */
	qHD_Structure *qHD_INT_In2; 												/* 中断上传端点2 EHCI相关结构体指针定义 */
	qTD_Structure *IntInEp1qTD;
	qTD_Structure *IntInEp2qTD;
	UINT8  HidType;																/* HID类设备的类型：0:None,1:Keyboard,2:Mouse */
	UINT8  IntfacNum;															/* HID类设备的接口号 */   
	UINT8  IntInEp1;															/* HID类设备的IN端点1地址 */
	UINT8  IntInEp2;															/* HID类设备的IN端点2地址 */
	UINT16 IntInEp1Size;  	    												/* HID类设备的IN端点1的最大包大小 */	
	UINT16 IntInEp2Size;  														/* HID类设备的IN端点2的最大包大小 */	
	UINT16 ReportDes_Len1;														/* HID类报表描述符总长度1 */
	UINT16 ReportDes_Len2;														/* HID类报表描述符总长度2 */
}HID_INFO;																		/* HID类信息结构体,存储当前HID类必要参数 */

/******************************************************************************/
/* 变量外扩 */	
extern Periodic_Frame_List_Structure  *pHID_FramList;
extern UINT32 gEHCI_Preiodic_Frame_List_BaseAddr;  								/* EHCI周期列表数据结构基地址 */

/******************************************************************************/
/* 函数外扩 */
extern UINT8 HID_CofDescrAnalyse( HID_INFO *Hidinfo, UINT8 *pbuf );				/* 分析HID类配置描述符 */
extern UINT8 HID_SetIdle( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf );
extern UINT8 HID_SetReport( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf );
extern UINT8 HID_GetReportDescr( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf, UINT16 len );
extern UINT8 HID_ClassRequest_Deal( HID_INFO *Hidinfo, UINT8 *pbuf );
extern UINT8 HID_Issue_GetData( HID_INFO *Hidinfo, qHD_Structure *spTempqHD, UINT8 *pbuf, UINT16 len );
extern UINT8 HID_Check_Data( HID_INFO *Hidinfo, qHD_Structure *spTempqHD, UINT16 *len );
extern UINT8 HID_Stop_GetData( HID_INFO *Hidinfo, qHD_Structure *spTempqHD );

#ifdef __cplusplus
}
#endif

#endif
