/********************************** (C) COPYRIGHT *******************************
* File Name          : HID.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller HID Operate
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563SFR.H"
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */    
#include "CH563USB.H"                                                           /* USB常用定义相关头文件 */    
#include "EHCI_HCD.H"                                                           /* 主机控制器低层操作相关头文件 */
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */
#include "HID.H"                                                                /* HID类定义相关头文件 */

/*******************************************************************************
* Function Name  : HID_CofDescrAnalyse
* Description    : 分析HID类配置描述符
* Input          : *Hidinfo-----当前操作的HID类设备相关结构体
*                  *pbuf--------当前需要分析的配置描述符缓冲区
* Output         : None
* Return         : USB_OPERATE_SUCCESS----HID类设备; 
*                  USB_OPERATE_ERROR------其它设备
*******************************************************************************/
UINT8 HID_CofDescrAnalyse( HID_INFO *Hidinfo, UINT8 *pbuf )
{
    UINT16 i, j;
    UINT32 temp;

    /* 简单分析配置描述符,并保存有用信息 */    
    /* 对于符合设备,只处理键盘、鼠标的接口,而忽略其它接口 */
    Hidinfo->IntfacNum = pbuf[ 4 ];                                             /* 保存接口号 */
    Hidinfo->ReportDes_Len1 = 0x00;
    Hidinfo->ReportDes_Len2 = 0x00;    
    for( i = 0; i < pbuf[ 2 ]; i++ )
    {
        if( ( pbuf[ i ] == 0x09 ) && ( pbuf[ i + 1 ] == 0x04 ) && ( pbuf[ i + 2 ] == 0x00 ) )
        {
            for( j = 0 ; j < pbuf[ 2 ] - i; j++ ) 
            {
                if( ( pbuf[ i + j ] == 0x09 ) && ( pbuf[ i + j + 1 ] == 0x21 ) && 
                    ( pbuf[ i + j + 6 ] == 0x22 ) )                             /* 判断是否为HID类端点 */    
                {    
                    Hidinfo->ReportDes_Len1 = pbuf[ i + j + 7 ];                /* 保存报告描述符的长度 */    
                    Hidinfo->HidType = pbuf[ i + 7 ];                    
                    break;
                }
            }
            if( Hidinfo->IntfacNum == 0x01 )
            {
                break;
            }                    
        }
        else if( ( pbuf[ i ] == 0x09 ) && ( pbuf[ i + 1 ] == 0x04 ) && ( pbuf[ i + 2 ] == 0x01 ) )
        {
            for( j = 0 ; j < pbuf[ 2 ] - i; j++ ) 
            {
                if( ( pbuf[ i + j ] == 0x09 ) && ( pbuf[ i + j + 1 ] == 0x21 ) && 
                    ( pbuf[ i + j + 6 ] == 0x22 ) )                             /* 判断是否为HID类端点 */    
                {    
                    Hidinfo->ReportDes_Len2 = pbuf[ i + j + 7 ];                /* 保存报告描述符的长度 */    
                    break;
                }
            }
            if( Hidinfo->ReportDes_Len2 != 0x00 )
            {
                break;
            }                    
        }                                
    }

    for( i = 0; i < pbuf[ 2 ]; i++ )
    {
        if( ( pbuf[ i ] == 0x09 ) && ( pbuf[ i + 1 ] == 0x04 ) && 
            ( pbuf[ i + 2 ] == 0x00 ) && ( pbuf[ i + 5 ] == 0x03 ) )            /* 接口描述符为HID的鼠标、键盘 */   
        { 
            for( j = 0 ; j < pbuf[ 2 ] - i; j++ ) 
            {
                if( ( pbuf[ i + j ] == 0x07 ) && ( pbuf[ i + j + 1 ] == 0x05 ) && 
                    ( pbuf[ i + j + 3 ] == 0x03 ) )                             /* 判断是否为中断端点 */    
                {    
                    if( pbuf[ i + j + 2 ] & 0x80 )                              /* IN端点的地址 */
                    {
                        Hidinfo->IntInEp1 = pbuf[ i + j + 2 ];                  /* 保存IN端点1端点号 */
                        Hidinfo->IntInEp1Size = pbuf[ i + j + 4 ];              /* 保存IN端点1端点大小 */
                    }
                    break;                    
                }       
            }
            if( Hidinfo->IntfacNum == 0x01 )
            {
                break;
            }                    
        }
        else if( ( pbuf[ i ] == 0x09 ) && ( pbuf[ i + 1 ] == 0x04 ) && 
               ( pbuf[ i + 2 ] == 0x01 ) && ( pbuf[ i + 5 ] == 0x03 ) )         /* 接口描述符为HID的鼠标、键盘 */   
        { 
            for( j = 0 ; j < pbuf[ 2 ] - i; j++ ) 
            {
                if( ( pbuf[ i + j ] == 0x07 ) && ( pbuf[ i + j + 1 ] == 0x05 ) && 
                    ( pbuf[ i + j + 3 ] == 0x03 ) )                             /* 判断是否为中断端点 */    
                {    
                    if( pbuf[ i + j + 2 ] & 0x80 )                              /* IN端点的地址 */
                    {
                        Hidinfo->IntInEp2 = pbuf[ i + j + 2 ];                  /* 保存IN端点2端点号 */
                        Hidinfo->IntInEp2Size = pbuf[ i + j + 4 ];              /* 保存IN端点2端点大小 */                                        
                    }
                    break;                     
                }       
            }                
        }                
    }
    if( ( Hidinfo->IntInEp1 == 0x00 ) && ( Hidinfo->IntInEp2 == 0x00 ) )
    {
        return( USB_OPERATE_ERROR );                                            /* 当前不是HID类设备,直接返回 */
    }

#ifdef  MY_DEBUG_PRINTF 
    printf(" Hidinfo->HidType:%x\n ",(UINT16)Hidinfo->HidType);            
    printf("Hidinfo->IntfacNum:%x\n",(UINT16)Hidinfo->IntfacNum);        
    printf("Hidinfo->IntInEp1:%x\n",(UINT16)Hidinfo->IntInEp1);
    printf("Hidinfo->IntInEp1Size:%x\n",(UINT16)Hidinfo->IntInEp1Size);
    printf("Hidinfo->ReportDes_Len1:%x\n",(UINT16)Hidinfo->ReportDes_Len1);
    printf("Hidinfo->IntInEp2:%x\n",(UINT16)Hidinfo->IntInEp2);
    printf("Hidinfo->IntInEp2Size:%x\n",(UINT16)Hidinfo->IntInEp2Size);
    printf("Hidinfo->ReportDes_Len2:%x\n",(UINT16)Hidinfo->ReportDes_Len2);
#endif

    /*******************************分配HID类设备相关内存及QH******************/
    if( Hidinfo->IntInEp1 )
    {    
        /* 分配中断传输相关QH(qHD_INT_In1) */
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Hidinfo->qHD_INT_In1 = ( qHD_Structure *)temp;                          /* 分配qHD_Structure */
    
        /* 初始化分配的IN端点相关QHD */
        USBHOST_Allocate_QHD( &Hidinfo->Device, 
                              Hidinfo->qHD_INT_In1, 
                              EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Hidinfo->IntInEp1, 
                              Hidinfo->IntInEp1Size );    
    
           /* 周期性传输相关QHD指针关联 */
        Hidinfo->qHD_INT_In1->bTerminate = 0x01;                                /* 下一个指针无效 */
    }
    
    if( Hidinfo->IntInEp2 )
    {    
        /* 分配中断传输相关QH(qHD_INT_In2) */
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Hidinfo->qHD_INT_In2 = ( qHD_Structure *)temp;                          /* 分配qHD_Structure */
    
        /* 初始化分配的IN端点相关QHD */
        USBHOST_Allocate_QHD( &Hidinfo->Device, 
                              Hidinfo->qHD_INT_In2, 
                              EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Hidinfo->IntInEp2, 
                              Hidinfo->IntInEp2Size );    

           /* 周期性传输相关QHD指针关联 */
        Hidinfo->qHD_INT_In2->bTerminate = 0x01;                                /* 下一个指针无效 */        
    }    
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : HID_SetIdle
* Description    : HID类设备SET IDLE命令
* Input          : *Hidinfo---当前HID相关结构体
*                   intface----当前接口号
                   *buf-------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HID_SetIdle( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x21;
    Ctl_Setup->bRequest = 0x0A;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = intface;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* SET_IDLE */
    l = 0x00;
    status = USBHOST_Issue_Control( &Hidinfo->Device, Ctl_Setup, buf, &l );     /* 执行控制传输 */    
    return( status );
}

/*******************************************************************************
* Function Name  : HID_GetReportDescr
* Description    : 获取HID类报表描述符
* Input          : *Hidinfo---当前HID相关结构体
*                   intface----当前接口号
                   *buf-------数据缓冲区
                   *len-------数据长度
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HID_GetReportDescr( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf, UINT16 len ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x81;
    Ctl_Setup->bRequest = 0x06;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x22;
    Ctl_Setup->wIndexL  = intface;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = ( UINT8 )len;
    Ctl_Setup->wLengthH = ( UINT8)( len >> 8 );
    
    /* 获取HID类报表描述符 */
    l = len;
    status = USBHOST_Issue_Control( &Hidinfo->Device, Ctl_Setup, buf, &l );     /* 执行控制传输 */    
    return( status );
}

/*******************************************************************************
* Function Name  : HID_SetReport
* Description    : HID类设备SET REPORT命令
* Input          : *Hidinfo---当前HID相关结构体
*                   intface----当前接口号
                   *buf-------数据缓冲区
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 HID_SetReport( HID_INFO *Hidinfo, UINT8 intface, UINT8 *buf ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x21;
    Ctl_Setup->bRequest = 0x09;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x02;
    Ctl_Setup->wIndexL  = intface;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x01;
    Ctl_Setup->wLengthH = 0x00;
    *buf = 0x00;                                                                /* 熄灭三个LED指示灯 */
    
    /* SET REPORT */
    l = 0x01;
    status = USBHOST_Issue_Control( &Hidinfo->Device, Ctl_Setup, buf, &l );     /* 执行控制传输 */    
    return( status );
}

/*******************************************************************************
* Function Name  : HID_ClassRequest_Deal
* Description    : HID类请求处理
* Input          : *Hidinfo-----当前操作的HID类设备相关结构体
*                  *pbuf--------缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 HID_ClassRequest_Deal( HID_INFO *Hidinfo, UINT8 *pbuf )
{
    UINT8  status;
    UINT16 i;

    /**************************************************************************/    
    /* Set Idle */ 
#ifdef  MY_DEBUG_PRINTF            
    printf("Set Idle\n");        
#endif
    status = HID_SetIdle( Hidinfo, 0x00, pbuf );
    if( status )
    {
#ifdef  MY_DEBUG_PRINTF            
    printf("Set Idle Error\n");        
#endif        
    }
    
    /* 如果有第二个HID类接口,继续操作该接口 */
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
#ifdef  MY_DEBUG_PRINTF            
        printf("Set Idle2\n");        
#endif
        status = HID_SetIdle( Hidinfo, 0x01, pbuf );
        if( status )
        {
#ifdef  MY_DEBUG_PRINTF            
        printf("Set Idle2 Error\n");        
#endif        
        }        
    }

    /***********************************************************************************/      
    /* Get Report Desc */ 
#ifdef  MY_DEBUG_PRINTF            
    printf("Get Report Desc\n");        
#endif
    status = HID_GetReportDescr( Hidinfo, 0x00, pbuf, Hidinfo->ReportDes_Len1 + 0x40 );
    if( status )
    {
#ifdef  MY_DEBUG_PRINTF            
        printf("Get Report Desc Error\n");    
#endif        
    }
    else
    {
#ifdef  MY_DEBUG_PRINTF
        for( i = 0; i < Hidinfo->ReportDes_Len1; i++ )
        {
            printf("%02x ",pbuf[ i ]);        
        } 
        printf("\n"); 
#endif
    }

    /* 如果有第二个HID类接口,继续操作该接口 */
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
#ifdef  MY_DEBUG_PRINTF            
        printf("Get Report Desc2\n");        
#endif
        status = HID_GetReportDescr( Hidinfo, 0x01, pbuf, Hidinfo->ReportDes_Len2 + 0x40 );
        if( status )
        {
#ifdef  MY_DEBUG_PRINTF            
        printf("Get Report Desc2 Error\n");    
#endif        
        }
        else
        {
#ifdef  MY_DEBUG_PRINTF    
            for( i = 0; i < Hidinfo->ReportDes_Len2; i++ )
            {
                printf("%02x ",pbuf[ i ]);        
            } 
            printf("\n"); 
#endif
        }        
    }

    /***********************************************************************************/      
    /* SET REPORT */ 
    /* 该命令只发生给键盘设备 */
    if( Hidinfo->HidType == HID_TYPE_KEYBOARD )
    {
#ifdef  MY_DEBUG_PRINTF            
        printf("Set Report\n");        
#endif
        status = HID_SetReport( Hidinfo, 0x00, pbuf );
        if( status )
        {
#ifdef  MY_DEBUG_PRINTF            
            printf("Set Report Error\n");    
#endif        
        }
    }
    return( status );        
}

/*******************************************************************************
* Function Name  : HID_Issue_GetData
* Description    : 发起获取端点数据请求
* Input          : *Hidinfo-----当前操作的HID类设备相关结构体
*                  *spTempqHD---当前要执行的QHD
*                  *pbuf--------当前数据缓冲区
*                  len----------当前获取长度 
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 HID_Issue_GetData( HID_INFO *Hidinfo, qHD_Structure *spTempqHD, UINT8 *pbuf, UINT16 len )
{
    UINT32 temp;
    qTD_Structure *spTempqTD;

    /* 初始化中断传输的分配qTD */    
    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD ); 
    spTempqTD->bTotalBytes = len;      
//    spTempqTD->bDataToggle = 0;  
    spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pbuf;
    temp = ( ( ( UINT32 )( pbuf ) >> 12 ) + 1 ) << 12;
    spTempqTD->ArrayBufferPointer_Word[ 1 ] = (UINT32)temp; 
    spTempqTD->ArrayBufferPointer_Word[ 2 ] = (UINT32)temp + 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 3 ] = (UINT32)temp + 2 * 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 4 ] = (UINT32)temp + 3 * 4096;                       
    spTempqTD->bPID = EHCI_QTD_PID_IN;             

    /* USB主机发起一个QTD */
    temp = USBHOST_Issue_qTD( &Hidinfo->Device, spTempqTD, spTempqHD );

    if( spTempqHD == Hidinfo->qHD_INT_In1 )
    {    
        Hidinfo->IntInEp1qTD = ( qTD_Structure *)temp;                          /* 暂存真正执行的qTD */
    }
    else if( spTempqHD == Hidinfo->qHD_INT_In2 )
    {
        Hidinfo->IntInEp2qTD = ( qTD_Structure *)temp;                          /* 暂存真正执行的qTD */        
    } 
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : HID_Check_Data
* Description    : 查询端点是否获取到数据
* Input          : *Hidinfo-----当前操作的HID类设备相关结构体
*                  *spTempqHD---当前要执行的QHD
*                  len----------当前获取长度 
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 HID_Check_Data( HID_INFO *Hidinfo, qHD_Structure *spTempqHD, UINT16 *len )
{
    UINT8  status = 0xFF;

    if( spTempqHD == Hidinfo->qHD_INT_In1 )
    {    
        status = USBHOST_Check_qTD( &Hidinfo->Device, Hidinfo->IntInEp1qTD, Hidinfo->qHD_INT_In1, len );    
    }
    else if( spTempqHD == Hidinfo->qHD_INT_In2 )
    {
        status = USBHOST_Check_qTD( &Hidinfo->Device, Hidinfo->IntInEp2qTD, Hidinfo->qHD_INT_In2, len );        
    } 
    return( status );
}

/*******************************************************************************
* Function Name  : HID_Stop_GetData
* Description    : 发起强制撤销该端点获取数据请求
* Input          : *Hidinfo-----当前操作的HID类设备相关结构体
*                  *spTempqHD---当前要执行的QHD
*                  len----------当前获取长度 
* Output         : None
* Return         : None
*******************************************************************************/

UINT8 HID_Stop_GetData( HID_INFO *Hidinfo, qHD_Structure *spTempqHD )
{
    UINT8  status = 0xFF;

    if( spTempqHD == Hidinfo->qHD_INT_In1 )
    {    
        status = USBHOST_Remove_qTD( &Hidinfo->Device, Hidinfo->IntInEp1qTD, Hidinfo->qHD_INT_In1 );    
    }
    else if( spTempqHD == Hidinfo->qHD_INT_In2 )
    {
        status = USBHOST_Remove_qTD( &Hidinfo->Device, Hidinfo->IntInEp2qTD, Hidinfo->qHD_INT_In2 );        
    }
    return( status );        
}

/*********************************** endfile **********************************/
