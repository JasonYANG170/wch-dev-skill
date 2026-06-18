/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK.H
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : SCSI协议操作U盘相关头文件
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CH563BAS.H"

#ifndef __UDISK_H__
#define __UDISK_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
#ifdef BIG_ENDIAN
#define USB_BO_CBW_SIG              0x55534243                                  /* 命令块CBW识别标志'USBC' */
#define USB_BO_CSW_SIG              0x55534253                                  /* 命令状态块CSW识别标志'USBS' */
#else
#define USB_BO_CBW_SIG              0x43425355                                  /* 命令块CBW识别标志'USBC' */
#define USB_BO_CSW_SIG              0x53425355                                  /* 命令状态块CSW识别标志'USBS' */
#endif

#define USB_BO_CBW_SIZE             0x1F                                        /* 命令块CBW的总长度 */
#define USB_BO_CSW_SIZE             0x0D                                        /* 命令状态块CSW的总长度 */
#define USB_BO_DATA_IN              0x80
#define USB_BO_DATA_OUT             0x00

/* USB主机方式的操作状态代码 */
#define USB_INT_SUCCESS             0x14                                        /* USB事务或者传输操作成功 */
#define USB_INT_CONNECT             0x15                                        /* 检测到USB设备连接事件 */
#define USB_INT_DISCONNECT          0x16                                        /* 检测到USB设备断开事件 */
#define USB_INT_BUF_OVER           0x17                                         /* USB控制传输的数据太多, 缓冲区溢出 */
#define USB_INT_DISK_ERR           0x1F                                         /* USB存储器操作失败 */

typedef union _BULK_ONLY_CMD 
{
    struct 
    {
        UINT32 mCBW_Sig;
        UINT32 mCBW_Tag;
        UINT32 mCBW_DataLen;                                                    /* 输入: 数据传输长度 */
        UINT8  mCBW_Flag;                                                       /* 输入: 传输方向等标志 */
        UINT8  mCBW_LUN;
        UINT8  mCBW_CB_Len;                                                     /* 输入: 命令块的长度,有效值是1到16 */
        UINT8  mCBW_CB_Buf[16];                                                 /* 输入: 命令块,该缓冲区最多为16个字节 */
    } mCBW;                                                                     /* BulkOnly协议的命令块, 输入CBW结构 */
    struct 
    {
        UINT32 mCSW_Sig;
        UINT32 mCSW_Tag;
        UINT32 mCSW_Residue;                                                    /* 返回: 剩余数据长度 */
        UINT8  mCSW_Status;                                                     /* 返回: 命令执行结果状态 */
    } mCSW;                                                                     /* BulkOnly协议的命令状态块, 输出CSW结构 */
} BULK_ONLY_CMD;

typedef struct _DiskInfo 
{ 
    ATTACH_DEVICE_INFO  Device;                                                    
    qHD_Structure *qHD_Bulk_In;                                                 /* 批量上传端点EHCI相关结构体指针定义 */
    qHD_Structure *qHD_Bulk_Out;                                                /* 批量下传端点EHCI相关结构体指针定义 */
    UINT8  MaxLun;                                                              /* 磁盘最大逻辑单元号 */
    UINT8  CurLun;                                                              /* 磁盘当前操作逻辑单元号 */
    UINT32 Capability;                                                          /* 磁盘总容量 */
    UINT32 PerSecSize;                                                          /* 磁盘扇区大小 */         
    UINT8  BulkInEp;                                                            /* USB大容量存储设备的IN端点地址 */
    UINT8  BulkOutEp;                                                           /* USB大容量存储设备的OUT端点地址 */
    UINT16 BulkInEpSize;                                                        /* USB大容量存储设备的IN端点最大包大小 */    
    UINT16 BulkOutEpSize;                                                       /* USB大容量存储设备的OUT端点最大包大小 */    
    UINT8  InterfNumber;                                                        /* USB大容量存储设备的接口号 */   
}DISK_INFO;                                                                     /* 磁盘信息结构体,存储当前U盘必要参数 */

/******************************************************************************/
/* 变量外扩 */
extern BULK_ONLY_CMD    mBOC;                                                    /* BulkOnly传输结构 */                                                        

/******************************************************************************/
/* 函数外扩 */
extern UINT8 MS_CofDescrAnalyse( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* 分析配置描述符 */                         
extern UINT8 MS_Bluk_QH_Allocate( DISK_INFO *Diskinfo );                        /* USB主机批量传输相关QH、TD分配 */
extern UINT8 MS_GetMaxLun_EX( DISK_INFO *Diskinfo );                            /* USB主机获取最大逻辑单元号 */
extern UINT8 MS_ResetErrorBOC_EX( DISK_INFO *Diskinfo );                        /* USB主机复位U盘 */    
extern UINT8 MS_ScsiCmd_Process_EX( DISK_INFO *Diskinfo, UINT8 *DataBuf );      /* 执行基于BulkOnly协议的命令处理 */
extern UINT8 MS_RequestSense_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* USB主机检查磁盘错误状态 */
extern UINT8 MS_DiskInquiry_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );             /* USB主机获取磁盘特性 */ 
extern UINT8 MS_DiskCapacity_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );            /* USB主机获取磁盘容量 */
extern UINT8 MS_DiskTestReady_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );           /* USB主机测试磁盘是否就绪 */
extern UINT8 MS_Init_EX( DISK_INFO *Diskinfo, UINT8 *pbuf );                    /* USB大容量存储设备初始化 */    
extern UINT8 MS_ReadSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf );/* 以扇区为单位从磁盘读取数据 */
extern UINT8 MS_WriteSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );/* 以扇区为单位将数据写入磁盘 */

#ifdef __cplusplus
}
#endif

#endif
