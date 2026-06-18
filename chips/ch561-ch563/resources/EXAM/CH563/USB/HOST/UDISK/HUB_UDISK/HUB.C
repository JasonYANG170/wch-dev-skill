/********************************** (C) COPYRIGHT *******************************
* File Name          : HUB.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller HUB Operate
*                      注: HUB端口号最小值为1 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  "CH563BAS.H"
#include  "EHCI_HCD.H"                                                          /* 主机控制器低层操作相关头文件 */
#include  "CH563SFR.H"                                                          /* CH563寄存器定义相关头文件 */
#include  "CH563USBSFR.H"                                                       /* 芯片USB寄存器相关头文件 */    
#include  "CH563USB.H"                                                          /* USB常用定义相关头文件 */    
#include  "HUB.H"                                                              
#include  "PRINTF.H"                                                            /* 串口打印输出控制头文件 */

/*******************************************************************************
* Function Name  : HUB_CofDescrAnalyse
* Description    : 分析HUB的配置描述符
* Input          : *Hubinfo----当前HUB相关结构体
*                  *pbuf-------当前要分析的数据缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 HUB_CofDescrAnalyse( HUB_INFO *Hubinfo, UINT8 *pbuf )
{
    UINT16 i;

    /* 分析配置描述符,获取端点数据(各端点地址/各端点大小等) */
    if ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != USB_DEV_CLASS_HUB ) 
    {
        return( USB_OPERATE_ERROR );                                            /* 不是HUB设备,直接返回  */
    }    

    Hubinfo->InterfNumber = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceNumber;  /* 保存接口号 */
    Hubinfo->IntInEp = 0x00;
    for( i = 0; i < 1; i ++ )                                                   /* 只分析1个端点 */
    {
        if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bLength == 0x07
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05  
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes == 0x03 ) 
        {      
            if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
            {    
                /* 中断IN端点 */
                Hubinfo->IntInEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN端点 */
                Hubinfo->IntInEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Hubinfo->IntInEpSize = Hubinfo->IntInEpSize << 8;
                Hubinfo->IntInEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
        }
    }
    if( ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != USB_DEV_CLASS_HUB )
      || ( Hubinfo->IntInEp == 0 ) )
    {                                                                           /* 不是USB存储类设备,不支持 */
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }

#ifdef  MY_DEBUG_PRINTF 
    printf("Hubinfo->IntInEp:%02x\n ",Hubinfo->IntInEp);
    printf("Hubinfo->IntInEpSize:%02x\n ",Hubinfo->IntInEpSize);
    printf("Hubinfo->InterfNumber:%02x\n ",Hubinfo->InterfNumber);
#endif
    return( TRUE );
}

/*******************************************************************************
* Function Name  : HUB_GetClassDevDescr
* Description    : 获取HUB类描述符
* Input          : *Hubinfo---当前HUB相关结构体
                   *buf-------数据缓冲区
                   *len-------数据长度
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_GetClassDevDescr( HUB_INFO *Hubinfo, UINT8 *buf, UINT16 *len ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_GET_HUB_DESCR;
    Ctl_Setup->bRequest = USB_REQ_GET_DESCR;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x29;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x02;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 先获取HUB类描述符的前2个字节 */    
    l = Ctl_Setup->wLengthL;
    status = USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &l );     /* 执行控制传输 */    
    if( status == USB_OPERATE_SUCCESS )
    {        
        /* 再获取全部HUB类描述符 */
        Ctl_Setup->wLengthL = buf[ 0 ];
        l = Ctl_Setup->wLengthL;
        status = USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &l ); /* 执行控制传输 */    
        if( status == USB_OPERATE_SUCCESS )
        {
            *len = l;
        }        
        else
        {
            *len = 0x00;
        }
        return( status );
    }
    return( status );
}

/*******************************************************************************
* Function Name  : HUB_ClearHubFeature
* Description    : 清除HUB特性
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_ClearHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_CLEAR_HUB_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_CLR_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */        
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_ClearPortFeature
* Description    : 清除HUB端口特性
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
*                  port---端口号
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_ClearPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 selector ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_CLEAR_PORT_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_CLR_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* 最小端口号为1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */        
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_GetHubStatus
* Description    : 查询HUB状态
* Input          : *Hubinfo---当前HUB相关结构体
*                   *buf-------数据缓冲区(返回4个字节: wHubStatus、wHubChange)
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_GetHubStatus( HUB_INFO *Hubinfo, UINT8 *buf ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_GET_PORT_STATUS;
    Ctl_Setup->bRequest = USB_REQ_GET_STATUS;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x04;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_GetPortStatus
* Description    : 查询HUB端口状态
* Input          : *Hubinfo---当前HUB相关结构体(返回4个字节: wPortStatus、wPortChange)
*                   *buf-------数据缓冲区
*                  port-------端口号
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_GetPortStatus( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_GET_PORT_STATUS;
    Ctl_Setup->bRequest = USB_REQ_GET_STATUS;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* 最小端口号为1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x04;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_SetHubFeature
* Description    : 设置HUB端口特性
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_SetHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_SET_HUB_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_SET_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;                                            
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_SetPortFeature
* Description    : 设置HUB端口特性
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
*                  port---端口号
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HUB_SetPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 selector ) 
{
    UINT16 len;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = USB_SET_PORT_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_SET_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* 最小端口号为1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 执行控制传输 */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : HUB_CheckPortConnect
* Description    : 检查HUB下游端口连接状态
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
*                  port---端口号(范围为0---3)
* Output         : None
* Return         : 返回当前命令执行状态或端口设备状态
*                    0x00-----------------无USB设备
*                  0x02-----------------有USB设备
*                  USB_HUBDEVICE_CONNECT------检测到HUB下的USB设备连接事件
*                  USB_HUBDEVICE_DISCONNECT---检测到HUB下的USB设备断开事件
*******************************************************************************/
UINT8 HUB_CheckPortConnect( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port ) 
{
    UINT8  status;

    /* 查询端口状态 */
    status = HUB_GetPortStatus( Hubinfo, pbuf, port );                                                                    
    if( status )
    {
        return( status );                                                       /* 操作失败直接返回 */
    }
    
    /* 判断当前端口连接状态 */
    if( pbuf[ 2 ] & 0x01 )                                                      /* 该端口连接状态发生改变 */
    {
        if( pbuf[ 0 ] & 0x01 )
        {
            return( USB_HUBDEVICE_CONNECT );                                    /* 该端口：检测到设备连接 */    
        }
        else
        {
            return( USB_HUBDEVICE_DISCONNECT );                                 /* 该端口：检测到设备断开 */    
        }                    
    }
    else                                                                        /* 该端口连接状态未发生改变 */
    {
        if( pbuf[ 0 ] & 0x01 )
        {
            return( 0x02 );                                                     /* 该端口：有设备 */
        }
        else
        {
            return( 0x00 );                                                     /* 该端口：无设备 */
        }                            
    }    
}

/*******************************************************************************
* Function Name  : HUB_CheckPortSpeed
* Description    : 检查HUB下游端口连接设备的速度
* Input          : *Hubinfo---当前HUB相关结构体
*                  *buf---数据缓冲区
*                  port---端口号(范围为0---3)
* Output         : None
* Return         : 返回当前端口下连接设备的速度
*                  0x00----全速设备
*                  0x01----低速设备
*                  0x02----高速设备
*******************************************************************************/
UINT8 HUB_CheckPortSpeed( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port ) 
{
    UINT8  status;

    /* 查询端口状态 */
    status = HUB_GetPortStatus( Hubinfo, pbuf, port );                                                                    
    if( status )
    {
        return( status );                                                       /* 操作失败直接返回 */
    }
    
    /* 速度由返回的第一个双字节的第9位和第10位决定 */
    if( pbuf[ 1 ] & 0x02 )
    {
        return( EHCI_DEVICE_SPEED_LS );                                         /* 当前为低速设备 */
    }    
    else
    {
        if( pbuf[ 1 ] & 0x04 )
        {
            return( EHCI_DEVICE_SPEED_HS );                                     /* 当前为高速设备 */
        }
        else
        {
            return( EHCI_DEVICE_SPEED_FS );                                     /* 当前为全速设备 */            
        }        
    }        
}     

/*********************************** endfile **********************************/
