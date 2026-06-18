/********************************** (C) COPYRIGHT *******************************
* File Name          : EHCI_HCD.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller EHCI test pattern file
*******************************************************************************/


#ifndef __EHCI_HCD_H__
#define __EHCI_HCD_H__

/******************************************************************************/
/* 头文件包含 */
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  "CH563BAS.H"
#include  "CH563USB.H"                                                          /* USB常用定义相关头文件 */    

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* EHCI内存分配简单说明: 
   EHCI底层使用所需内存空间需要在外部定义缓冲区EHCI_Allocate_Buf,然后由程序自行
   分配使用,该缓冲区大小由用户根据需要进行定义,可通过修改下面各数据结构体对应的
   分配个数来修改.
*/

/* EHCI内存分配相关宏定义 */
#define EHCI_QHD_SIZE                        0x40                               /* EHCI数据结构QHD大小 */
#define EHCI_QHD_MAX                         14                                 /* EHCI数据结构QHD分配个数 */    

#define EHCI_QTD_SIZE                        0x20                               /* EHCI数据结构QTD大小 */
#define EHCI_QTD_MAX                         14                                 /* EHCI数据结构QTD分配个数 */

#define EHCI_ITD_SIZE                        0x40                               /* EHCI数据结构ITD大小 */
#define EHCI_ITD_MAX                         10                                 /* EHCI数据结构ITD分配个数 */

#define EHCI_P_FRAME_SIZE                    0x04                               /* EHCI周期帧列表大小 */
#define EHCI_P_FRAME_LIST_MAX                1024                               /* EHCI周期帧列表分配个数 */    

#define EHCI_DATAPAGE_SIZE                   4096                               /* EHCI数据缓冲区页大小 */
#define EHCI_DATAPAGE_MAX                    5                                  /* EHCI数据缓冲区页分配个数 */

/* 定义EHCI内存分配缓冲区大小 */
#define EHCI_ALLOCATE_BUF_LEN                 ( ( EHCI_QHD_SIZE * EHCI_QHD_MAX ) + \
                                               ( EHCI_QTD_SIZE * EHCI_QTD_MAX ) + \
                                               ( EHCI_ITD_SIZE * EHCI_ITD_MAX ) + \
                                               ( EHCI_P_FRAME_SIZE * EHCI_P_FRAME_LIST_MAX ) + 64 + 8 )

/* 定义EHCI数据分配缓冲区大小 */
#define EHCI_DATA_BUF_LEN                      ( EHCI_DATAPAGE_SIZE * EHCI_DATAPAGE_MAX )

/* 定义EHCI周期性帧列表缓冲区大小 */
#define EHCI_PER_LISE_BUF_LEN                 ( EHCI_P_FRAME_SIZE * EHCI_P_FRAME_LIST_MAX )

/*******************************************************************************/                                                                                       
/* EHCI内存分配相关宏定义 */
#define EHCI_MEM_TYPE_QHD                    0x00
#define EHCI_MEM_TYPE_QTD                    0x01
#define EHCI_MEM_TYPE_ITD                    0x02
#define EHCI_MEM_TYPE_SITD                     0x03
#define EHCI_MEM_TYPE_4K_BUFFER              0x04

#define EHCI_MEM_FREE                           0x01
#define EHCI_MEM_USED                           0x02

/*******************************************************************************/
/* EHCI相关使能、禁止定义 */
#define USBHOST_OP_ENABLE                      0x01
#define USBHOST_OP_DISABLE                     0x00

/*******************************************************************************/
/* Queue Head Structure Definition */
typedef struct _qHD 
{
    /* <1>.Next_qHD_Pointer Word */
    UINT32 bTerminate: 1;                 
    UINT32 bType: 2;                                
    UINT32 bReserve_1: 2;                   
    UINT32 bNextQHDPointer: 27;              
                
    /* <2>.qHD_2 Word */
    UINT32 bDeviceAddress: 7;             
    UINT32 bInactiveOnNextTransaction: 1;       
    UINT32 bEdNumber: 4;                   
    UINT32 bEdSpeed: 2;                    
    UINT32 bDataToggleControl: 1;         
    UINT32 bHeadOfReclamationListFlag: 1; 
    UINT32 bMaxPacketSize: 11;            
    UINT32 bControlEdFlag: 1;             
    UINT32 bNakCounter: 4;                

    /* <3>.qHD_3 Word */    
    UINT32 bInterruptScheduleMask: 8;     
    UINT32 bSplitTransactionMask: 8;      
    UINT32 bHubAddr: 7;                   
    UINT32 bPortNumber: 7;                
    UINT32 bHighBandwidth: 2;             

    /* <4>.Overlay_CurrentqTD */    
    UINT32 bOverlay_CurrentqTD;       

    /* <5>.Overlay_NextqTD */       
    UINT32 bOverlay_NextTerminate: 1;     
    UINT32 bOverlay_Reserve2: 4;               
    UINT32 bOverlay_NextqTD: 27;          

    /* <6>.Overlay_AlternateNextqTD */       
    UINT32 bOverlay_AlternateNextTerminate: 1; 
    UINT32 bOverlay_NanCnt: 4;                  
    UINT32 bOverlay_AlternateqTD: 27;          

    /* <7>.Overlay_TotalBytes */       
    UINT32 bOverlay_Status: 8;              
    UINT32 bOverlay_PID: 2;                   
    UINT32 bOverlay_ErrorCounter: 2;        
    UINT32 bOverlay_C_Page: 3;              
    UINT32 bOverlay_InterruptOnComplete: 1; 
    UINT32 bOverlay_TotalBytes: 15;         
    UINT32 bOverlay_Direction: 1;           

    /* <8>.Overlay_BufferPointer0 */       
    UINT32 bOverlay_CurrentOffset: 12;     
    UINT32 bOverlay_BufferPointer_0: 20;   

    /* <9>.Overlay_BufferPointer1 */       
    UINT32 bOverlay_C_Prog_Mask: 8;         
    UINT32 bOverlay_Reserve3: 4;            
    UINT32 bOverlay_BufferPointer_1: 20;    
      
    /* <10>.Overlay_BufferPointer2 */       
    UINT32 bOverlay_FrameTag: 5;            
    UINT32 bOverlay_S_Bytes: 7;             
    UINT32 bOverlay_BufferPointer_2: 20;    
      
    /* <11>.Overlay_BufferPointer3 */       
    UINT32 bOverlay_Reserve4: 12;           
    UINT32 bOverlay_BufferPointer_3: 20;    
      
    /* <12>.Overlay_BufferPointer4 */       
    UINT32 bOverlay_Reserve5: 12;           
    UINT32 bOverlay_BufferPointer_4: 20;              
}qHD_Structure;

/*******************************************************************************/
/* qTD Structure Definition */ 
typedef struct _qTD 
{ 
    /* <1>.Next_qTD_Pointer Word */
    UINT32 bTerminate: 1;        
    UINT32 bReserve_1: 4;        
    UINT32 bNextQTDPointer: 27;   
                
    /* <2>.Alternate Next qTD Word */
    UINT32 bAlternateTerminate: 1;    
    UINT32 bReserve_2: 4;             
    UINT32 bAlternateQTDPointer: 27;  
     
    /* <3>.Status Word */     
    UINT32 bStatus_PingState: 1;         
    UINT32 bStatus_SplitState: 1;        
    UINT32 bStatus_MissMicroFrame: 1;    
    UINT32 bStatus_Transaction_Err: 1;   
    UINT32 bStatus_Babble: 1;            
    UINT32 bStatus_Buffer_Err: 1;        
    UINT32 bStatus_Halted: 1;            
    UINT32 bStatus_Active: 1;            
       
    UINT32 bPID: 2;                  
    UINT32 bErrorCounter: 2;         
    UINT32 CurrentPage: 3;           
    UINT32 bInterruptOnComplete: 1;  
    UINT32 bTotalBytes: 15;          
    UINT32 bDataToggle: 1;           

    /* <4>.Buffer Pointer Word Array */    
    UINT32 ArrayBufferPointer_Word[ 5 ];       
}qTD_Structure;

#define EHCI_QTD_PID_OUT                     0x00
#define EHCI_QTD_PID_IN                      0x01
#define EHCI_QTD_PID_SETUP                   0x02
    
#define EHCI_QTD_STATUS_ACTIVE                  0x80
#define EHCI_QTD_STATUS_HALTED               0x40
#define EHCI_QTD_STATUS_BUF_ERROR            0x20
#define EHCI_QTD_STATUS_BABBLE               0x10
#define EHCI_QTD_STATUS_TRANS_ERROR          0x08
#define EHCI_QTD_STATUS_MISS_FRAME           0x04
#define EHCI_QTD_STATUS_SPLIT                0x02
#define EHCI_QTD_STATUS_PING                 0x01

/*******************************************************************************/
/* iTD Structure Definition */ 
typedef struct _iTD_Status 
{     
    UINT32 bOffset: 12;             
    UINT32 bPageSelect: 3;             
    UINT32 bInterruptOnComplete: 1; 
    UINT32 bLength: 12;             
    UINT32 bStatus: 4;                             
}iTD_Status_Structure;

typedef struct _iTD_BufferPointer 
{
    UINT32 bMultiFunction: 12;     
    UINT32 bBufferPointer: 20;           
}iTD_BufferPointer_Structure;
 
typedef struct _iTD 
{
    /* <1>.Next_Link_Pointer Word */
    UINT32 bTerminate: 1;      
    UINT32 bType: 2;           
    UINT32 bReserve_1: 2;      
    UINT32 bNextLinkPointer: 27; 
                
    /* <2>.Status Word */
    iTD_Status_Structure ArrayStatus_Word[ 8 ];    
     
    /* <3>.Buffer_Pointer Word */     
    iTD_BufferPointer_Structure ArrayBufferPointer_Word[ 7 ];                
}iTD_Structure;

#define HOST20_iTD_Status_Active             0x08
#define HOST20_iTD_Status_DataBufferError    0x04
#define HOST20_iTD_Status_BabbleDetect       0x02
#define HOST20_iTD_Status_TransctionError    0x01

/*******************************************************************************/
/* siTD Structure Definition */
typedef struct _siTD 
{     
    /* <1>.1 Word */ 
    UINT32 bTerminate: 1;           
    UINT32 bType: 2;                     
    UINT32 bReserve_1: 2;           
    UINT32 bNextQHDPointer: 27;     
                
    /* <2>.2 Word */ 
    UINT32 bDeviceAddress: 7;       
    UINT32 bReserve_21: 1;              
    UINT32 bEdNumber: 4;            
    UINT32 bReserve_22: 4;          
    UINT32 bHubAddr: 7;             
    UINT32 bReserve_23: 1;               
    UINT32 bPortNumber: 7;          
    UINT32 bInOut: 1;               

    /* <3>.3 Word */      
    UINT32 bS_Mask: 8;                    
    UINT32 bC_Mask: 8;                   
    UINT32 bReserve_31: 16;               

    /* <4>.4 Word */      
    UINT32 bStatus: 8;     
    UINT32 bC_Prog_Mask: 8;          
    UINT32 bTotalBytesToTransfer: 10;          
    UINT32 bReserve_41: 4;    
    UINT32 bPageSelect: 1;         
    UINT32 bInterruptOnComplete: 1;         

    /* <5>.5 Word */      
    UINT32 bCurrentOffset: 12;   
    UINT32 bBufferPointer_Page0: 20;          

    /* <6>.6 Word */      
    UINT32 bT_Count: 3;   
    UINT32 bTransactionPosition: 2;
    UINT32 bReserve_61: 7;         
    UINT32 bBufferPointer_Page1: 20; 
      
    /* <7>.7 Word */     
    UINT32 bBP_Terminate: 1;   
    UINT32 bReserve_71: 4;  
    UINT32 bBackPointer: 27;            
}siTD_Structure;

typedef struct
{
    /* <1>.Next_Link_Pointer Word */
    UINT32   bTerminal: 1;                     
    UINT32   bType: 2;                             
    UINT32   bReserved: 2;                    
    UINT32   bLinkPointer: 27;                     
}Periodic_Frame_List_Cell_Structure;

typedef struct  
{     
    Periodic_Frame_List_Cell_Structure   sCell[ EHCI_P_FRAME_LIST_MAX ]; 

}Periodic_Frame_List_Structure;

#define HOST20_siTD_Status_Active            0x80

#define HOST20_siTD_TP_All                   0x00
#define HOST20_siTD_TP_Begin                 0x01
#define HOST20_siTD_TP_Mid                   0x02
#define HOST20_siTD_TP_End                   0x03

#define EHCI_HD_TYPE_ITD                     0x00
#define EHCI_HD_TYPE_QH                      0x01
#define EHCI_HD_TYPE_SITD                    0x02
#define EHCI_HD_TYPE_FSTN                    0x03


/* 连接设备速度定义 */
#define EHCI_DEVICE_SPEED_LS                 0x01                               /* 当前为低速设备 */
#define EHCI_DEVICE_SPEED_FS                 0x00                               /* 当前为全速设备 */
#define EHCI_DEVICE_SPEED_HS                 0x02                               /* 当前为高速设备 */

/* 端点是否为控制端点定义 */
#define EHCI_EP_TYPE_CONTROL                 0x01
#define EHCI_EP_TYPE_NO_CONTROL              0x00


#define EHCI_PORT_INIT                       0
#define DEFAULT_USB_EP0_SIZE                 64                                 /* 默认USB设备端点0最大包大小 */                                   
#define DEFAULT_USB_OP_ADDR                  2                                  /* 默认操作USB设备的地址 */
#define DEFAULT_MAX_OPERATE_SIZE             20480                              /* 默认当前操作最大包大小 */                                   

#define DEF_USB_TIMEOUT                      50000                              /* USB中断超时时间，单位uS */
    
/* 以下状态代码1XH用于USB主机方式的操作状态代码,可以自行定义任意值,下面的定义仅为了兼容CH37x */
#define    ERR_USB_UNKNOWN                   0xFA                               /* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */
#define    ERR_USB_TIMEOUT                   0xFB                               /* 中断超时/NAK重试超时 */

#ifndef    USB_INT_SUCCESS
#define    USB_OPERATE_SUCCESS               0x00                               /* USB操作成功 */
#define    USB_OPERATE_ERROR                 0x01                               /* USB操作失败 */
#define    USB_HC_ERROR                      0x02                               /* USB主机控制器存在错误 */
#define    USB_PARAMETER_ERROR               0x03                               /* 参数错误, 范围溢出 */
#define    USB_MEM_ALLOCATE_ERROR            0x04                               /* 内存分配失败 */
#define    USB_INT_SUCCESS                   0x14                               /* USB事务或者传输操作成功 */
#define    USB_INT_CONNECT                   0x15                               /* 检测到USB设备连接事件 */
#define    USB_INT_DISCONNECT                0x16                               /* 检测到USB设备断开事件 */
#define    USB_INT_BUF_OVER                  0x17                               /* USB控制传输的数据太多, 缓冲区溢出 */
#define    USB_HUBDEVICE_CONNECT             0x18                               /* 检测到HUB下的USB设备连接事件 */
#define    USB_HUBDEVICE_DISCONNECT          0x19                               /* 检测到HUB下的USB设备断开事件 */
#define    USB_INT_ERROR                     0x1F                               /* USB设备操作失败 */
#endif
#ifndef    USB_INT_CONNECT_LS
#define    USB_INT_CONNECT_LS                0x13                               /* 检测到低速USB设备连接事件 */
#endif

/* 连接的USB设备相关信息 */
typedef struct _ATTACH_DEVICE_INFO
{
    UINT8  bDeviceOnHub;                                                        /* 当前USB设备是否连接在HUB下: 0---否, 1---是 */
    UINT8  bOnHubPortNumber;                                                    /* 当前USB设备在HUB上的端口号 */
    UINT8  bAddress;                                                            /* 当前USB设备地址：默认为0 */
    UINT8  bHubAddress;                                                         /* 当前USB挂载的HUB地址：默认为0 */    
    UINT8  bConnectStatus;                                                      /* 当前USB设备连接状态：0---未连接,1---连接未枚举，2---枚举成功 */
    UINT8  bPortEnableDisableStatus;
    UINT8  bSpeed;                                                              /* 当前USB设备速度：0xFF---未知，0x01---低速，0x00---全速，0x02---高速 */
    UINT8  bEp0MaxPks;                                                          /* 当前USB设备端点0最大包大小 */    
    UINT8  bPortReset;  
    UINT8  bSuspend;  
    UINT8  bRemoteWakeUpDetected;  
    UINT8  bSendOK;
    UINT8  bSendStatusError;
    qHD_Structure *qHD_Control;                                                 /* 控制端点qHD定义 */
    qTD_Structure *psSendLastqTD;    
    UINT8  *bDataBuffer;
}ATTACH_DEVICE_INFO, *pATTACH_DEVICE_INFO;

/*******************************************************************************/
/* 常、变量外扩 */
/* EHCI寄存器相关结构体指针定义 */
extern EHCI_CAPABILITY *EHCI_Caps;
extern EHCI_REGISTERS  *EHCI_Regs;

/* USB主机控制传输SETUP包结构体指针定义 */
extern USB_SETUP_REQ   *Ctl_Setup;
extern UINT8  *Ctl_Data;

/* 变量及缓冲区定义 */
extern UINT8   gDeviceClassType;                                                /* 当前设备类型 */                                            
extern UINT8V  gEHCILastStatus;                                                 /* EHCI最后一次操作状态 */
extern UINT32  gEHCIFreeRamAddr;                                                /* EHCI内存分配空闲起始地址 */    

extern UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                        /* 内存分配缓冲区(仅用来分配QH、QTD相关内存使用) */
extern UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                                /* 内存数据缓冲区(由于需要4K对齐,强制分配指定内存空间) */                                
extern UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];                   /* 周期性帧列表缓冲区(需要4K对齐) */

/*******************************************************************************/
/* 函数外扩 */
extern void Delay_us( int n );                                                  /* 微秒级延时,不精确 */
extern void Delay_ms( int n );                                                  /* 毫秒级延时,不精确 */
extern void USBHOST_ModeSet( void );                                            /* 设置USB主机模式 */ 
extern void USBHOST_INT_Enable( void );                                         /* USB主机相关USB中断使能 */
extern void USBHOST_INT_Disable( void );                                        /* USB主机相关USB中断禁止 */
extern UINT8 USBHOST_HC_StopRun_Setting( UINT8 option );                        /* USB主机控制器停止、启动设置 */
extern UINT8 USBHOST_Asynchronous_Setting( UINT8 option );                      /* USB主机启用、停止Asynchronous传输 */
extern UINT8 USBHOST_Periodic_Setting( UINT8 option );                          /* USB主机启用、停止Periodic传输 */
extern UINT8 USBHOST_EHCI_Init( void );                                         /* USB主机EHCI初始化 */
extern UINT8 USBHOST_Structure_DeInit( void );                                  /* USB主机控制器所有数据结构所在内存分配及初始化 */
extern UINT32 USBHOST_GetStructure( UINT8 Type );                               /* 分配数据结构所需内存 */
extern UINT8 USBHOST_ReleaseStructure( UINT8 Type, UINT32 pwAddress );          /* 回收数据结构所分配的内存 */
extern UINT8 USBHOST_Allocate_QHD( ATTACH_DEVICE_INFO *Device, qHD_Structure *psQHTemp, UINT8 bQHType, UINT8 bType, UINT8 bHead, UINT8 bEndPt, UINT32 wMaxPacketSize ); /* 初始化分配的QH */
extern UINT8 USBHOST_Resume( ATTACH_DEVICE_INFO *Device );
extern UINT8 USBHOST_Suspend( ATTACH_DEVICE_INFO *Device );
extern void DEBUG_Print_Memory( UINT32 startaddr, UINT16 size );                /* 打印指定内存空间数据 */
extern void USBHOST_DevicePara_Init( ATTACH_DEVICE_INFO *Device );              /* 初始化设备相关参数 */
extern UINT8 USBHOST_EHCI_BusReset( ATTACH_DEVICE_INFO *Device );               /* USB主机进行总线复位 */
extern UINT8 USBHOST_EHCI_Detect_Connect( ATTACH_DEVICE_INFO *Device );         /* USB主机检测设备插拔 */
extern UINT8 USBHOST_EHCI_EP0_Init( ATTACH_DEVICE_INFO *Device );               /* USB主机控制端点0初始化 */
extern UINT8 USBHOST_Send_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen, UINT32 wTimeOutSec ); /* USB主机发送一个QTD */
extern UINT8 USBHOST_Issue_Control( ATTACH_DEVICE_INFO *Device, USB_SETUP_REQ *pCmdPack, UINT8 *pDatBuf, UINT16 *pRetLen );/* USB主机发起一个控制传输 */
extern UINT8 USBHOST_Issue_Bulk( ATTACH_DEVICE_INFO *Device, qHD_Structure *pBlukqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type ); /* USB主机执行批量传输 */
extern UINT8 USBHOST_Issue_Interrupt( ATTACH_DEVICE_INFO *Device, qHD_Structure *pIntqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type );    /* USB主机执行中断传输 */
extern UINT8 USBHOST_EHCI_GetDeviceDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len );    /* USB主机获取设备描述符 */
extern UINT8 USBHOST_EHCI_GetConfigDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len );    /* USB主机获取配置描述符 */ 
extern UINT8 USBHOST_EHCI_SetAddress( ATTACH_DEVICE_INFO *Device, UINT8 addr ); /* USB主机设置地址 */
extern UINT8 USBHOST_EHCI_SetConfig( ATTACH_DEVICE_INFO *Device, UINT8 cfg );   /* USB主机设置配置值 */
extern UINT8 USBHOST_EHCI_ClearEndpStall( ATTACH_DEVICE_INFO *Device, UINT8 endp );    /* USB主机清除端点 */
extern UINT8 USBHOST_CofDescrAnalyse( UINT8 *pbuf );                            /* 分析配置描述符 */
extern UINT8 USBHOST_EHCI_Enumerate( ATTACH_DEVICE_INFO *Device, UINT8 *pbuf ); /* USB主机对设备进行枚举 */
extern UINT8 USBHOST_qHD_Link_Insert( qHD_Structure *CurrentqHD );              /* 将当前qHD链接到链表中 */    
extern UINT8 USBHOST_QH_Link_BackOut( qHD_Structure *CurrentQH );               /* 将当前QH从链表中摘除 */
extern UINT8 USBHOST_Release_QHD( qHD_Structure *CurrentQH );                   /* 释放当前QHD(包括从链表摘除、回收QH下挂载的TD、以及回收QH) */
extern UINT32 USBHOST_Issue_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD );
extern UINT8 USBHOST_Check_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen );
extern UINT8 USBHOST_Remove_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD );

#ifdef __cplusplus
}
#endif

#endif

/* ********************************************************************************************************************* */


