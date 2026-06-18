/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller Mass Storage Device Operate
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
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */

/*******************************************************************************
* Function Name  : MS_CofDescrAnalyse
* Description    : 分析Mass Storage Device配置描述符
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf--------当前需要分析的配置描述符缓冲区
* Output         : None
* Return         : USB_OPERATE_SUCCESS----Mass Storage Device; 
*                  USB_OPERATE_ERROR---其它设备
*******************************************************************************/
UINT8 MS_CofDescrAnalyse( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    UINT16 i;
    UINT8  status;
    
    /* 分析配置描述符,获取端点数据(各端点地址/各端点大小等) */
    if ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != 0x08 ) 
    {
        return( USB_OPERATE_ERROR );                                            /* 不是USB存储类设备,直接返回  */
    }    
    Diskinfo->InterfNumber = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceNumber;  /* 保存接口号 */
    Diskinfo->BulkInEp  = 0;
    Diskinfo->BulkOutEp = 0;
    for( i = 0; i < 2; i ++ )                                                   /* 只分析前两个端点 */
    {
        if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bLength == 0x07
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05  
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes == 2 ) 
        {      
            if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
            {    
                /* 批量IN端点 */
                Diskinfo->BulkInEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN端点 */
                Diskinfo->BulkInEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Diskinfo->BulkInEpSize = Diskinfo->BulkInEpSize << 8;
                Diskinfo->BulkInEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
            else 
            {
                Diskinfo->BulkOutEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT端点 */
                Diskinfo->BulkOutEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Diskinfo->BulkOutEpSize = Diskinfo->BulkOutEpSize << 8;
                Diskinfo->BulkOutEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
        }
    }
    if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != 0x08 || Diskinfo->BulkInEp == 0 || Diskinfo->BulkOutEp == 0 )
    {                                                                           /* 不是USB存储类设备,不支持 */
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }

#ifdef  MY_DEBUG_PRINTF 
    printf("Diskinfo->BulkInEp:%02x\n ",Diskinfo->BulkInEp);
    printf("Diskinfo->BulkOutEp:%02x\n ",Diskinfo->BulkOutEp);
    printf("Diskinfo->BulkInEpSize:%02x\n ",Diskinfo->BulkInEpSize);
    printf("Diskinfo->BulkOutEpSize:%02x\n ",Diskinfo->BulkOutEpSize);
#endif

    /*******************************分配大容量存储设备相关内存************************/
    status = MS_Bluk_QH_Allocate( Diskinfo );                                   /* USB主机批量传输相关QH、TD分配 */
    if ( status != USB_OPERATE_SUCCESS ) 
    {
        return( status );                                                       /* 终止操作,直接返回 */    
    }    
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : MS_Bluk_QH_Allocate
* Description    : 分配批量传输相关QH
* Input          : *Diskinfo----当前操作的U盘相关结构体
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 MS_Bluk_QH_Allocate( DISK_INFO *Diskinfo )
{
    UINT32 temp;

    /* 分配批量传输相关QH(qHD_Bulk_In) */
    if( Diskinfo->BulkInEp )
    {
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Diskinfo->qHD_Bulk_In = ( qHD_Structure *)temp;                         /* 分配qHD_Structure */
    
        /* 初始化分配的IN端点 */
        USBHOST_Allocate_QHD( &Diskinfo->Device, 
                              Diskinfo->qHD_Bulk_In, 
                              EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Diskinfo->BulkInEp, 
                              Diskinfo->BulkInEpSize );    
    
        /* 指针关联 */    
        USBHOST_qHD_Link_Insert( Diskinfo->qHD_Bulk_In );                       /* 将qHD_Bulk_In链接到QH链表中 */
    }

    /* 分配批量传输相关QH(qHD_Bulk_Out) */
    if( Diskinfo->BulkOutEp )
    {
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Diskinfo->qHD_Bulk_Out = ( qHD_Structure *)temp;                        /* 分配qHD_Structure */
    
        /* 初始化分配的OUT端点 */
        USBHOST_Allocate_QHD( &Diskinfo->Device, 
                              Diskinfo->qHD_Bulk_Out, 
                               EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Diskinfo->BulkOutEp, 
                              Diskinfo->BulkOutEpSize );
                
        /* 指针关联 */    
        USBHOST_qHD_Link_Insert( Diskinfo->qHD_Bulk_Out );                      /* 将qHD_Bulk_Out链接到QH链表中 */
    }
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : MS_GetMaxLun_EX
* Description    : USB主机获取最大逻辑单元号
* Input          : *Diskinfo----当前操作的U盘相关结构体
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_GetMaxLun_EX( DISK_INFO *Diskinfo ) 
{
    UINT8  status;
    UINT8  buf[ 1 ];  

    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0xA1;
    Ctl_Setup->bRequest = 0xFE;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x01;
    Ctl_Setup->wLengthH = 0x00;

    /* 获取当前U盘最大逻辑单元号 */        
    Diskinfo->MaxLun = 0x00;
    status = USBHOST_Issue_Control( &Diskinfo->Device, Ctl_Setup, buf, NULL );  /* 执行控制传输 */
    if( status == USB_OPERATE_SUCCESS )
    {
        Diskinfo->MaxLun = buf[ 0 ];                                            /* 保存当前U盘的最大逻辑单元号 */
    }
    else
    {
        if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                          /* 返回STALL */     
        {    
            status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x00 );    /* 清除端点0 */    
        }                        
    }            
    return( status );
}

/*******************************************************************************
* Function Name  : MS_ResetErrorBOC_EX
* Description    : USB主机复位U盘
* Input          : *Diskinfo----当前操作的U盘相关结构体
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_ResetErrorBOC_EX( DISK_INFO *Diskinfo )
{
    UINT8  status;

    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x21;
    Ctl_Setup->bRequest = 0xFF;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = Diskinfo->InterfNumber;                               /* 接口号 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;

    /* USB主机复位U盘 */        
    status = USBHOST_Issue_Control( &Diskinfo->Device, Ctl_Setup, NULL, NULL  );/* 执行控制传输 */
    if( status == USB_OPERATE_SUCCESS )    
    {
        /* 清除批量上、下传端点 */
        status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp ); /* 清除批量上传端点 */
        if( status != USB_OPERATE_SUCCESS )    
        {
            return( status );
        }
        status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, Diskinfo->BulkOutEp );    /* 清除批量下传端点 */
    }        
    return( status );                                                      
}

/*******************************************************************************
* Function Name  : MS_ScsiCmd_Process_EX
* Description    : 执行基于BulkOnly协议的命令处理
*                  由于一次可以传输最多20K数据,故未进行循环处理
*                  注意: 数据缓冲区最好是4K边界对齐的,如果未对齐,则缓冲区本身大小必须
*                        比实际要读写的大小要大4K。 
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *DataBuf-----输入、输出数据缓冲区
* Output         : None
* Return         : 返回执行状态
*******************************************************************************/
UINT8 MS_ScsiCmd_Process_EX( DISK_INFO *Diskinfo, UINT8 *DataBuf )
{
    UINT8  status, s;
    UINT8  *p;
    UINT16 len;
    
#ifdef  MY_DEBUG_PRINTF     
//    printf("MS_ScsiCmd_Process...\n");
#endif

    /* 1、填充SCSI命令包信息 */
    p = DataBuf;
    mBOC.mCBW.mCBW_Sig = USB_BO_CBW_SIG;
    mBOC.mCBW.mCBW_Tag = 0x05630563;
    mBOC.mCBW.mCBW_LUN = Diskinfo->CurLun;                                      /* 操作当前逻辑单元号 */

    /* 2、发送CSW包 */
#ifdef  MY_DEBUG_PRINTF     
//    printf("CBW\n");
#endif
    len = USB_BO_CBW_SIZE;
    status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, (UINT8 *)&mBOC.mCBW, &len, EHCI_QTD_PID_OUT );
    if( status == USB_INT_DISCONNECT )                                          /* 如果设备断开,直接返回 */
    {
        return( status );
    }
    if( status != USB_OPERATE_SUCCESS )
    {
        status = MS_ResetErrorBOC_EX( Diskinfo );
        if( status == USB_INT_DISCONNECT )                                      /* 如果设备断开,直接返回 */
        {
            return( status );
        }
                        
        /* 再次发送CBW包 */
        len = USB_BO_CBW_SIZE;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, (UINT8 *)&mBOC.mCBW, &len, EHCI_QTD_PID_OUT );            
        if( status == USB_INT_DISCONNECT )                                      /* 如果设备断开,直接返回 */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {
            return( MS_ResetErrorBOC_EX( Diskinfo ) );
        }    
    }

    /* 3、接收数据包或发送数据包 */    
    if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_IN ) )
    {        
#ifdef  MY_DEBUG_PRINTF     
//        printf("In\n");
#endif        
        /* 如果有数据需要上传,则发送IN令牌进行数据读取 */    
        if( mBOC.mCBW.mCBW_DataLen > DEFAULT_MAX_OPERATE_SIZE )
        {
            return( USB_PARAMETER_ERROR );                                      /* 参数错误 */
        }    
        
        /* 发送批量上传IN */
        len = mBOC.mCBW.mCBW_DataLen;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_In, p, &len, EHCI_QTD_PID_IN );                
        if( status == USB_INT_DISCONNECT )                                      /* 如果设备断开,直接返回 */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {    
            if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                      /* 返回STALL错误 */
            {
                s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp );/* 清除上传端点 */
                if( s == USB_INT_DISCONNECT )                                   /* 如果设备断开,直接返回 */
                {
                    return( s );
                }                                    
            }    
        }    
    }
    else if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_OUT ) )
    {
#ifdef  MY_DEBUG_PRINTF     
//        printf("Out\n");
#endif                
        /* 如果有数据需要下传,则发送OUT令牌进行数据读取 */    
        if( mBOC.mCBW.mCBW_DataLen > DEFAULT_MAX_OPERATE_SIZE )
        {
            return( USB_PARAMETER_ERROR );                                      /* 参数错误 */
        }    

        /* 发送批量下传OUT */
        len = mBOC.mCBW.mCBW_DataLen;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, p, &len, EHCI_QTD_PID_OUT );        
        if( status == USB_INT_DISCONNECT )                                      /* 如果设备断开,直接返回 */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {    
            if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                      /* 返回STALL错误 */
            {
                s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, Diskinfo->BulkOutEp );/* 清除下传端点 */
                if( s == USB_INT_DISCONNECT )                                   /* 如果设备断开,直接返回 */
                {
                    return( s );
                }
            }
        }
    }    

#ifdef  MY_DEBUG_PRINTF     
//    printf("CSW\n");
#endif
    /* 4、发送CSW包 */
    len = USB_BO_CSW_SIZE;
    status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_In, (UINT8 *)&mBOC.mCSW, &len, EHCI_QTD_PID_IN );    
    if( status == USB_OPERATE_SUCCESS )
    {
        if( len != 0x0D )                                                       /* 判断长度是否为13个字节 */
//        if( ( len != 0x00 ) || ( mBOC.mCSW.mCSW_Sig != USB_BO_CSW_SIG ) )     /* 有些U盘可能不按这个标志处理 */ 
        {    
            return( USB_INT_DISK_ERR );
        }
        if( mBOC.mCSW.mCSW_Status == 0 ) 
        {
            return( USB_OPERATE_SUCCESS );
        }
        else if( mBOC.mCSW.mCSW_Status >= 2 ) 
        {    
            return( MS_ResetErrorBOC_EX( Diskinfo ) );
        }
        else 
        {
            return( USB_INT_DISK_ERR );                                         /* 磁盘操作错误 */
        }            
    }    
    else if( status == USB_INT_DISCONNECT )                                     /* 如果设备断开,直接返回 */
    {
        return( status );
    }
    else
    {
        /* 判断哪一步出的错误 */
        /* 位6:Halted; 位5:Data Buffer Error; 位4: Babble Detectd; 位3: Transaction Error; 位2：Missed Micri-fRAME */
        if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                          /* 返回STALL错误 */
        {
            s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp );    
            if( s == USB_INT_DISCONNECT )                                       /* 如果设备断开,直接返回 */
            {
                return( s );
            }
        }        
    }    
    return( USB_INT_DISK_ERR ); 
}

/*******************************************************************************
* Function Name  : MS_RequestSense_EX
* Description    : USB主机检查磁盘错误状态
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_RequestSense_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    UINT8  status;
    
#ifdef  MY_DEBUG_PRINTF
    printf( "MS_RequestSense:...\n" );
#endif
    
    Delay_ms( 10 );                                                             /* 延时10毫秒 */    
    status = USBHOST_EHCI_Detect_Connect( &Diskinfo->Device );    
    if( ( status == USB_INT_CONNECT ) || ( status == USB_INT_DISCONNECT ) || ( status == 0 ) )
    {
        return( status );
    }    
    mBOC.mCBW.mCBW_DataLen        = 0x00000012;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len        = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x03;                                          /* 命令码 */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x12;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* 执行基于BulkOnly协议的命令 */
}

/*******************************************************************************
* Function Name  : MS_DiskInquiry_EX
* Description    : USB主机获取磁盘特性
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskInquiry_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen        = 0x00000024;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len        = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x12;                                          /* 命令码 */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x24;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* 执行基于BulkOnly协议的命令 */
}

/*******************************************************************************
* Function Name  : MS_DiskCapacity_EX
* Description    : USB主机获取磁盘容量
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskCapacity_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen     = 0x00000008;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len      = 10;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x25;                                          /* 命令码 */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 8 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* 执行基于BulkOnly协议的命令 */
}

/*******************************************************************************
* Function Name  : MS_DiskTestReady_EX
* Description    : USB主机测试磁盘是否就绪
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf------数据缓冲区 
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 MS_DiskTestReady_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen        = 0x00;
    mBOC.mCBW.mCBW_Flag        = 0x00;
    mBOC.mCBW.mCBW_CB_Len      = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x00;                                          /* 命令码 */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* 执行基于BulkOnly协议的命令 */
}

/*******************************************************************************
* Function Name  : MS_Init_EX
* Description    : USB大容量存储设备初始化 
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  *pbuf------数据缓冲区 
* Output         : None
* Return         : USB_OPERATE_SUCCESS---初始化成功;
*                  USB_OPERATE_ERROR--初始化失败;
*******************************************************************************/
UINT8 MS_Init_EX( DISK_INFO *Diskinfo, UINT8 *pbuf ) 
{
    UINT8  count, status;        
    UINT16 i;
    
    /*******************************获取最大逻辑单元号*************************/
#ifdef  MY_DEBUG_PRINTF
    printf( "MS_GetMaxLun...\n" );
#endif
    status = MS_GetMaxLun_EX( Diskinfo );
    if ( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        Diskinfo->MaxLun = 0x00;                                                /* 有些U盘可能不支持,出错之后,取默认值 */
    }
    Delay_ms( 3 );

    /*******************************获取U盘信息(INQUIRY)***********************/
    /* 判断当前逻辑单元是否是CD-ROM */
    for( count = 0; count < Diskinfo->MaxLun + 1; count++ )
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Inquiry:...\n" );
#endif
        status = MS_DiskInquiry_EX( Diskinfo, pbuf );                           /* 获取磁盘特性 */
        if ( status != USB_OPERATE_SUCCESS ) 
        {
#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02X\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* 如果设备断开,直接返回 */
            {
                return( status );
            }
            return( USB_OPERATE_ERROR );                                        /* 终止操作,直接返回 */
        }
    
#ifdef  MY_DEBUG_PRINTF
        for ( i = 8; i < 36; i ++ ) 
        {
            printf( "%c", *( pbuf + i ) );
        }
        printf( "\n" );
#endif
        if( ( Diskinfo->MaxLun == 0x00 ) && ( *( pbuf + 0 ) == 0x05 ) )        
        {
            /* 当前U盘只有1个逻辑盘,且是CD-ROM ,则终止操作,直接返回 */
            return( USB_OPERATE_ERROR );                                                      
        }
        if( *( pbuf + 0 ) == 0x05 ) 
        {
            Diskinfo->CurLun++;
            Delay_ms( 3 );
            continue;                                                           /* 继续 */
        }
        else
        {
            break;
        }        
    }

    /*******************************获取U盘容量********************************/
    for( count = 0; count < 5; count++ )
    {
        /* 注意：对于有些U盘,第一次出错之后,第二次操作必须等待足够长时间之后再操作,否则会一直返回NAK */
        Delay_ms( 200 );
        Delay_ms( 100 * count );                                               
#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Capacity:...\n" );
#endif
        status = MS_DiskCapacity_EX( Diskinfo, pbuf );                          /* 获取磁盘容量 */
        if ( status != USB_OPERATE_SUCCESS ) 
        {
#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02x\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* 如果设备断开,直接返回 */
            {
                return( status );
            }
            MS_RequestSense_EX( Diskinfo, pbuf );                               /* 如果出错,则进行低层出错处理 */
        }
        else 
        {            
            /* 保存当前扇区大小(分开计算防止编译器出错) */            
            Diskinfo->PerSecSize = ( ( (UINT32)( *( pbuf + 4 ) ) ) << 24 );
            Diskinfo->PerSecSize |= ( ( (UINT32)( *( pbuf + 5 ) ) ) << 16 );
            Diskinfo->PerSecSize |= ( ( (UINT32)( *( pbuf + 6 ) ) ) << 8 ) + ( *( pbuf + 7 ) );

            /* 保存当前扇区数目(分开计算防止编译器出错) */            
            Diskinfo->Capability = ( ( (UINT32)( *( pbuf + 0 ) ) ) << 24 );
            Diskinfo->Capability |= ( ( (UINT32)( *( pbuf + 1 ) ) ) << 16 );
            Diskinfo->Capability |= ( ( (UINT32)( *( pbuf + 2 ) ) ) << 8 ) + ( *( pbuf + 3 ) );

#ifdef  MY_DEBUG_PRINTF
            printf("Diskinfo->PerSecSize: %04x\n",(UINT32)Diskinfo->PerSecSize);
            printf("Diskinfo->Capability: %08lx\n",Diskinfo->Capability);
#endif            
            break;
        }
    }

    /***************************测试U盘是否准备好******************************/
    for( count = 0; count < 5; count ++ )
    {
        Delay_ms( 50 );

#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Ready:...\n" );
#endif
        status = MS_DiskTestReady_EX( Diskinfo, pbuf );                         /* 测试磁盘是否就绪 */
        if ( status != USB_OPERATE_SUCCESS ) 
        {

#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02X\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* 如果设备断开,直接返回 */
            {
                return( status );
            }
            MS_RequestSense_EX( Diskinfo, pbuf );                               /* 如果出错,则进行低层出错处理 */
        }
        else 
        {
            break;            
        }
    }
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : MS_ReadSector_EX
* Description    : 以扇区为单位从磁盘读取数据
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  StartLba----起始扇区号
*                  SectCount---需要读取的扇区数目
*                  DataBuf-----数据缓冲区 
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 MS_ReadSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf )
{
    UINT8  err, s;
    UINT32 len;

#ifdef  MY_DEBUG_PRINTF
//    printf( "MS_ReadSector:...\n" );
#endif    
    len = SectCount * Diskinfo->PerSecSize;                                     /* 计算读取总长度 */ 
    if( len > DEFAULT_MAX_OPERATE_SIZE )
    {
        return( USB_PARAMETER_ERROR );                                          /* 参数错误 */        
    }    
    for( err = 0; err < 3; err ++ )                                             /* 错误重试 */
    {      
        mBOC.mCBW.mCBW_DataLen = len;    
        mBOC.mCBW.mCBW_Flag = 0x80;
        mBOC.mCBW.mCBW_CB_Len = 10;
        mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x28;                                      /* 命令码 */
        mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
        mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
        mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
        mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
        mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
        mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
        s = MS_ScsiCmd_Process_EX( Diskinfo, DataBuf );                          /* 执行基于BulkOnly协议的命令 */        
        if( s == USB_OPERATE_SUCCESS ) 
        {
            return( USB_OPERATE_SUCCESS );                                      /* 操作成功 */
        }
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                        /* 检测到USB设备断开事件,磁盘已经断开或者刚刚重新插上 */
        }
        s = MS_RequestSense_EX( Diskinfo, DataBuf );
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                          
        }
    }
    return( s );                                                                /* 返回错误 */
}

/*******************************************************************************
* Function Name  : MS_WriteSector_EX
* Description    : 以扇区为单位将数据写入磁盘
* Input          : *Diskinfo----当前操作的U盘相关结构体
*                  StartLba----起始扇区号
*                  SectCount---需要写入的扇区数目
*                  DataBuf-----数据缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 MS_WriteSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
    UINT8  err, s;
    UINT32 len;

#ifdef  MY_DEBUG_PRINTF
//    printf( "MS_WriteSector:...\n" );
#endif        
    len = SectCount * Diskinfo->PerSecSize;                                     /* 计算读取总长度 */
    if( len > DEFAULT_MAX_OPERATE_SIZE )
    {
        return( USB_PARAMETER_ERROR );                                          /* 参数错误 */        
    }    
    for( err = 0; err < 3; err++ )                                              /* 错误重试 */
    {      
        mBOC.mCBW.mCBW_DataLen = len;
        mBOC.mCBW.mCBW_Flag = 0x00;
        mBOC.mCBW.mCBW_CB_Len = 10;
        mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x2A;                                      /* 命令码 */
        mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
        mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
        mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
        mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
        mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
        mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
        s = MS_ScsiCmd_Process_EX( Diskinfo, DataBuf );                         /* 执行基于BulkOnly协议的命令 */
        if( s == USB_OPERATE_SUCCESS ) 
        {
            return( USB_OPERATE_SUCCESS );                                      /* 操作成功 */
        }
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT ) 
        {
            return( s );                                                        /* 检测到USB设备断开事件,磁盘已经断开或者刚刚重新插上 */
        }
        s = MS_RequestSense_EX( Diskinfo, DataBuf );
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                          
        }        
    }
    return( s );                                                                /* 返回错误 */
}
