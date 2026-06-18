/********************************** (C) COPYRIGHT *******************************
* File Name          : HUB.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : HUB操作相关头文件
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CH563BAS.H"

#ifndef __HUB_H__
#define __HUB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* USB HUB Class Request Types */
#ifndef USB_CLEAR_HUB_FEATURE
#define USB_CLEAR_HUB_FEATURE           0x20
#define USB_CLEAR_PORT_FEATURE          0x23
#define USB_CLEAR_TT_BUFFER             0x23
#define USB_GET_HUB_DESCR               0xA0
#define USB_GET_HUB_STATUS              0xA0
#define USB_GET_PORT_STATUS             0xA3
#define USB_RESET_TT                    0x23
#define USB_SET_HUB_DESCR               0x20
#define USB_SET_HUB_FEATURE             0x20
#define USB_SET_PORT_FEATURE            0x23
#define USB_GET_TT_STATUS               0xA3
#define USB_STOP_TT                     0x23
#endif

/* Hub Class Feature Selectors */
#define    USB_C_HUB_LOCAL_POWER        0
#define USB_C_HUB_OVER_CURRENT          1
#define USB_PORT_CONNECTION             0
#define USB_PORT_ENABLE                 1
#define USB_PORT_SUSPEND                2
#define USB_PORT_OVER_CURRENT           3
#define USB_PORT_RESET                  4
#define USB_PORT_POWER                  8
#define    USB_PORT_LOW_SPEED           9
#define USB_C_PORT_CONNECTION           16
#define USB_C_PORT_ENABLE               17
#define USB_C_PORT_SUSPEND              18
#define USB_C_PORT_OVER_CURRENT         19
#define USB_C_PORT_RESET                20
#define USB_PORT_TEST                   21
#define USB_PORT_INDICATOR              22

#ifndef ROOT_HUB_DEVICE
typedef struct _ROOT_HUB_DEVICE
{
    UINT8  bHubStatus;                                                          /* 当前HUB状态: 0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功 */
    UINT8  bHubSpeed;                                                           /* 当前HUB速度：0---未知，1---低速，2---全速，3---高速 */
    UINT8  bEp0MaxPks;                                                          /* 当前HUB端点0最大包大小 */    
    UINT8  bHubAddress;                                                         /* 当前HUB被分配的USB地址 */
    UINT8  bHubPortNum;                                                         /* 当前HUB端口数目 */    
}ROOT_HUB_DEVICE, *PROOT_HUB_DEVICE;
#endif

typedef struct _HubInfo 
{ 
    ATTACH_DEVICE_INFO  Device;                                                    
    UINT8  IntInEp;                                                             /* HUB设备的IN端点地址 */
    UINT16 IntInEpSize;                                                         /* HUB设备的IN端点最大包大小 */    
    UINT8  InterfNumber;                                                        /* USB大容量存储设备的接口号 */  
}HUB_INFO;                                                                      /* HUB信息结构体,存储当前U盘必要参数 */

/******************************************************************************/
/* 函数外扩 */
extern UINT8 HUB_CofDescrAnalyse( HUB_INFO *Hubinfo, UINT8 *pbuf );                          /* USB主机分析HUB Device配置描述符 */
extern UINT8 HUB_GetClassDevDescr( HUB_INFO *Hubinfo, UINT8 *buf, UINT16 *len );             /* USB主机获取HUB类描述符 */
extern UINT8 HUB_ClearHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector );           /* 清除HUB特性 */
extern UINT8 HUB_ClearPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 select );/* 清除HUB端口特性 */ 
extern UINT8 HUB_GetHubStatus( HUB_INFO *Hubinfo, UINT8 *buf );                              /* 查询HUB状态 */ 
extern UINT8 HUB_GetPortStatus( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port );                 /* 查询HUB端口状态 */
extern UINT8 HUB_SetHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector );             /* 设置HUB特性 */
extern UINT8 HUB_SetPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 select );  /* 设置HUB端口特性 */ 
extern UINT8 HUB_CheckPortConnect( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port );             /* 检查HUB下游端口连接状态 */
extern UINT8 HUB_CheckPortSpeed( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port );               /* 检查HUB下游端口连接设备的速度 */
                        
#ifdef __cplusplus
}
#endif

#endif
