/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK_FAT.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : USB大容量存储设备调用文件系统库相关文件
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "EHCI_HCD.H"                                                           /* 主机控制器低层操作相关头文件 */
#include "CH563SFR.H"                                                           /* CH563寄存器定义相关头文件 */
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */    
#include "CH563USB.H"                                                           /* USB常用定义相关头文件 */    
#include "Udisk.H"                                                              /* Udisk定义相关头文件 */
#include "Udisk_FAT.H"                                                          /* Udisk封装函数相关头文件 */

/******************************************************************************/
/* 常、变量定义 */
UINT8  gDiskMaxLun;                                                             /* 磁盘最大逻辑单元号 */
UINT8  gDiskCurLun;                                                             /* 磁盘当前操作逻辑单元号 */

/******************************************************************************/ 
/******************************************************************************/
/* 以下为EHCI底层相关封装函数 */
/******************************************************************************/
/******************************************************************************/ 

/*******************************************************************************
* Function Name  : USBHOST_BusReset
* Description    : USB主机进行总线复位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_BusReset( void )
{
    return( USBHOST_EHCI_BusReset( &UDisk.Device ) );
}                             

/*******************************************************************************
* Function Name  : USBHOST_EP0_Init
* Description    : USB主机控制端点0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_EP0_Init( void )
{
       return( USBHOST_EHCI_EP0_Init( &UDisk.Device) );
}

/*******************************************************************************
* Function Name  : USBHOST_CtrlTransfer
* Description    : USB主机发起一个控制传输
* Input          : *pDatBuf----收发数据缓冲区
*                   *pRetLen----收发数据实际长度
* Output         : None   
* Return         : None
*******************************************************************************/
UINT8 USBHOST_CtrlTransfer( UINT8 *DatBuf, UINT16 *RetLen )
{
       return( USBHOST_Issue_Control( &UDisk.Device, Ctl_Setup, DatBuf, RetLen ) );
}

/*******************************************************************************
* Function Name  : USBHOST_GetDeviceDescr
* Description    : USB主机获取设备描述符
* Input          : *buf------数据缓冲区                 
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_GetDeviceDescr( UINT8 *buf )
{
    UINT16  len;    

       return( USBHOST_EHCI_GetDeviceDescr( &UDisk.Device, buf, &len ) );    
}

/*******************************************************************************
* Function Name  : USBHOST_GetConfigDescr
* Description    : USB主机获取配置描述符
* Input          : *buf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_GetConfigDescr( UINT8 *buf )
{
    UINT16  len;

       return( USBHOST_EHCI_GetConfigDescr( &UDisk.Device, buf, &len ) );
}


/*******************************************************************************
* Function Name  : USBHOST_SetAddress
* Description    : USB主机设置地址
* Input          : addr------要设置的地址
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_SetAddress( UINT8 addr )
{
       return( USBHOST_EHCI_SetAddress( &UDisk.Device, addr ) );
}

/*******************************************************************************
* Function Name  : USBHOST_SetConfig
* Description    : USB主机设置配置值
* Input          : cfg-------要设置的配置值
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_SetConfig( UINT8 cfg )
{
       return( USBHOST_EHCI_SetConfig( &UDisk.Device, cfg ) );
}

/*******************************************************************************
* Function Name  : USBHOST_ClearEndpStall
* Description    : USB主机清除端点
* Input          : endp------要清除的端点号
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_ClearEndpStall( UINT8 endp )
{
    return( USBHOST_EHCI_ClearEndpStall( &UDisk.Device, endp ) );
}

/*******************************************************************************
* Function Name  : USBHOST_SetTargetAddr
* Description    : USB主机设置目标地址
* Input          : addr------要设置的地址
* Output         : None
* Return         : None
*******************************************************************************/
void USBHOST_SetTargetAddr( UINT8 addr )
{
}

/*******************************************************************************
* Function Name  : USBHOST_Detect_Connect
* Description    : USB主机检测设备插拔
*                  注：端口使能位在总线复位时由主机控制器自动置1
* Input          : None
* Output         : None
* Return         : 0x00-----------------无USB设备
*                  0x01-----------------有USB设备但尚未使能
*                  0x02-----------------有USB设备且已经使能
*                  USB_INT_CONNECT------检测到USB设备连接事件
*                  USB_INT_DISCONNECT---检测到USB设备断开事件
*******************************************************************************/
UINT8 USBHOST_Detect_Connect( void )
{
    return( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) );
}

/*******************************************************************************
* Function Name  : USBHOST_Parameter_Init
* Description    : 初始化设备相关参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBHOST_Parameter_Init( void )
{                                                                
    USBHOST_DevicePara_Init( &UDisk.Device );
}

/*******************************************************************************
* Function Name  : USBHOST_Enumerate
* Description    : USB主机对设备进行枚举
* Input          : *pbuf-----数据缓冲区
* Output         : None
* Return         : USB_OPERATE_SUCCESS---设备枚举成功;
*                  USB_OPERATE_ERROR-----设备枚举失败;
*******************************************************************************/
UINT8 USBHOST_Enumerate( UINT8 *pbuf )
{
    return( USBHOST_EHCI_Enumerate( &UDisk.Device, pbuf ) );
}

/******************************************************************************/
/******************************************************************************/
/* 以下为U盘底层相关第封装函数 */
/******************************************************************************/
/******************************************************************************/
/*******************************************************************************
* Function Name  : USBHOST_MS_CofDescrAnalyse
* Description    : 分析Mass Storage Device配置描述符
* Input          : *pbuf--------当前需要分析的配置描述符缓冲区
* Output         : None
* Return         : USB_OPERATE_SUCCESS----Mass Storage Device; 
*                  USB_OPERATE_ERROR---其它设备
*******************************************************************************/
UINT8 USBHOST_MS_CofDescrAnalyse( UINT8 *pbuf )
{
    return( MS_CofDescrAnalyse( &UDisk, pbuf ) );    
}

/*******************************************************************************
* Function Name  : MS_GetMaxLun
* Description    : USB主机获取最大逻辑单元号
* Input          : None
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_GetMaxLun( void )
{
    return( MS_GetMaxLun_EX( &UDisk ) );    
}

/*******************************************************************************
* Function Name  : MS_ResetErrorBOC
* Description    : USB主机复位U盘
* Input          : None
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_ResetErrorBOC( void )
{
    return( MS_ResetErrorBOC_EX( &UDisk ) );    
}

/*******************************************************************************
* Function Name  : MS_ScsiCmd_Process
* Description    : 执行基于BulkOnly协议的命令处理
*                  由于一次可以传输最多20K数据,故未进行循环处理
*                  注意: 数据缓冲区最好是4K边界对齐的,如果未对齐,则缓冲区本身大小必须
*                        比实际要读写的大小要大4K。 
* Input          : *DataBuf-----输入、输出数据缓冲区
* Output         : None
* Return         : 返回执行状态
*******************************************************************************/
UINT8 MS_ScsiCmd_Process( UINT8 *DataBuf )
{
    return( MS_ScsiCmd_Process_EX( &UDisk, DataBuf ) );    
}

/*******************************************************************************
* Function Name  : MS_Init
* Description    : USB大容量存储设备初始化 
* Input          : *pbuf------数据缓冲区 
* Output         : None
* Return         : USB_OPERATE_SUCCESS---初始化成功;
*                  USB_OPERATE_ERROR--初始化失败;
*******************************************************************************/
UINT8 MS_Init( UINT8 *pbuf )
{
    return( MS_Init_EX( &UDisk, pbuf ) );
}

/*******************************************************************************
* Function Name  : MS_RequestSense
* Description    : USB主机检查磁盘错误状态
* Input          : *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_RequestSense( UINT8 *pbuf )
{
    return( MS_RequestSense_EX( &UDisk, pbuf ) );
}

/*******************************************************************************
* Function Name  : MS_DiskInquiry
* Description    : USB主机获取磁盘特性
* Input          : *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskInquiry( UINT8 *pbuf )
{
    return( MS_DiskInquiry_EX( &UDisk, pbuf ) );
}

/*******************************************************************************
* Function Name  : MS_DiskCapacity
* Description    : USB主机获取磁盘容量
* Input          : *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskCapacity( UINT8 *pbuf )
{
    return( MS_DiskCapacity_EX( &UDisk, pbuf ) );
}

/*******************************************************************************
* Function Name  : MS_DiskTestReady
* Description    : USB主机测试磁盘是否就绪
* Input          : *pbuf------数据缓冲区 
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskTestReady( UINT8 *pbuf )
{
    return( MS_DiskTestReady_EX( &UDisk, pbuf ) );
}

/*******************************************************************************
* Function Name  : MS_ReadSector
* Description    : 以扇区为单位从磁盘读取数据
* Input          : StartLba----起始扇区号
*                  SectCount---需要读取的扇区数目
*                  DataBuf-----数据缓冲区 
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 MS_ReadSector( UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf )
{
    return( MS_ReadSector_EX( &UDisk, StartLba, SectCount, DataBuf ) );
}

/*******************************************************************************
* Function Name  : MS_WriteSector
* Description    : 以扇区为单位将数据写入磁盘
* Input          : StartLba----起始扇区号
*                  SectCount---需要写入的扇区数目
*                  DataBuf-----数据缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 MS_WriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
    return( MS_WriteSector_EX( &UDisk, StartLba, SectCount, DataBuf ) );
}


