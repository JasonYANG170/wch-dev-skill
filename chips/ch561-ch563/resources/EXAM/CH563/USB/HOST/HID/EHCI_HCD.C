/********************************** (C) COPYRIGHT *******************************
* File Name          : EHCI_HCD.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller EHCI test pattern file
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CH563BAS.H"                                                           /* CH563基本定义相关头文件 */
#include "CH563SFR.H"                                                           /* CH563寄存器定义相关头文件 */    
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */    
#include "CH563USB.H"                                                           /* USB常用定义相关头文件 */    
#include "EHCI_HCD.H"                                                           /* 主机控制器低层操作相关头文件 */
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */                                

/******************************************************************************/
/* 常、变量定义 */
/* USB主机控制传输SETUP包结构体指针定义 */
UINT8  gDeviceClassType;                                                        /* 当前设备类型 */
UINT8V gEHCILastStatus;                                                         /* EHCI最后一次操作状态 */

/* EHCI寄存器相关结构体指针定义 */
EHCI_CAPABILITY *EHCI_Caps;
EHCI_REGISTERS    *EHCI_Regs;
USB_SETUP_REQ   *Ctl_Setup;                                                     /* 控制传输数据包 */

/* 变量及缓冲区定义 */
UINT32 gEHCIFreeRamAddr;                                                        /* EHCI内存分配空闲起始地址 */
UINT32 gEHCI_RAM_BaseAddr;                                                      /* EHCI数据结构基地址 */
UINT32 gEHCI_QHD_BaseAddr;                                                      /* EHCI数据结构QHD基地址 */
UINT32 gEHCI_QTD_BaseAddr;                                                      /* EHCI数据结构QTD基地址 */
UINT32 gEHCI_ITD_BaseAddr;                                                      /* EHCI数据结构ITD基地址 */
UINT32 gEHCI_Preiodic_Frame_List_BaseAddr;                                      /* EHCI周期列表数据结构基地址 */
UINT32 gEHCI_DataPage_BaseAddr;                                                 /* EHCI数据缓冲区基地址 */
UINT8  EHCI_qHD_Manage[ EHCI_QHD_MAX ];                                         /* 1=>Free; 2=>used */
UINT8  EHCI_qTD_Manage[ EHCI_QTD_MAX ];                                         /* 1=>Free; 2=>used */
UINT8  EHCI_iTD_Manage[ EHCI_ITD_MAX ];                                         /* 1=>Free; 2=>used */  
UINT8  EHCI_DataPage_Manage[ EHCI_DATAPAGE_MAX ];                               /* 1=>Free; 2=>used */ 

/*******************************************************************************
* Function Name  : USBHOST_ModeSet
* Description    : 设置USB主机模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBHOST_ModeSet( void )
{ 
    /* 配置芯片为主机模式 */
    R8_MISC_CTRL_USB =  R8_MISC_CTRL_USB & ( ~RB_MISC_USB_ID_EN );              /* USB OTG ID pin enable */                                    
    R8_MISC_CTRL_USB = R8_MISC_CTRL_USB & ( ~RB_MISC_USB_ID_ST );               /* 设置成USB主机 */    
    OTG_CONTROL_STATUS = 0x000E0190;                                            /* 配置USB为主机模式 */    
}

/*******************************************************************************
* Function Name  : USBHOST_HC_StopRun_Setting
* Description    : USB主机控制器停止、启动设置
* Input          : option---USBHOST_OP_DISABLE: 停止USB主机控制器
*                           USBHOST_OP_ENABLE:  启动USB主机控制器
* Output         : None
* Return         : 返回当前执行状态
*******************************************************************************/
UINT8 USBHOST_HC_StopRun_Setting( UINT8 option )
{
    UINT16 timeout;

    if( option == USBHOST_OP_ENABLE ) 
    {
        /* 判断是否已启用,若已启用则直接返回 */
        if( ( EHCI_Regs->hc_usbcmd & RB_RS ) && ( EHCI_Regs->hc_usbsts & RB_HCHALTED ) == 0 ) 
        {
            return( USB_OPERATE_SUCCESS );                                                                
        }

        /* 启动USB主机控制器,并等待启动成功 */        
        EHCI_Regs->hc_usbcmd |= RB_RS;
        Delay_ms( 1 );
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( ( EHCI_Regs->hc_usbcmd & RB_RS ) && ( EHCI_Regs->hc_usbsts & RB_HCHALTED ) == 0 ) 
            {
                break;
            }
        }
        if ( timeout == 0 ) 
        {
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
        }
        return( USB_OPERATE_SUCCESS );        
    }
    else if( option == USBHOST_OP_DISABLE )
    {    
        /* 判断是否已停止,若已停止则直接返回 */
        if( ( EHCI_Regs->hc_usbcmd & RB_RS ) == 0 && ( EHCI_Regs->hc_usbsts & RB_HCHALTED ) ) 
        {    
            return( USB_OPERATE_SUCCESS );                                                                
        }
        
        /* 停止Asynchronous和Periodic传输 */
         USBHOST_Asynchronous_Setting( USBHOST_OP_DISABLE );
         USBHOST_Periodic_Setting( USBHOST_OP_DISABLE );
    
        /* 停止USB主机控制器,并等待停止成功 */        
        EHCI_Regs->hc_usbcmd &= ~RB_RS;
        Delay_ms( 1 );
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( ( EHCI_Regs->hc_usbcmd & RB_RS ) == 0 && ( EHCI_Regs->hc_usbsts & RB_HCHALTED ) ) 
            {    
                break;
            }
        }
        if( timeout == 0 ) 
        {    
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
        }
         return( USB_OPERATE_SUCCESS );        
    }
    return( USB_PARAMETER_ERROR );
}    
    
/*******************************************************************************
* Function Name  : USBHOST_Asynchronous_Setting
* Description    : USB主机启用、停止Asynchronous传输
* Input          : option---USBHOST_OP_DISABLE: USB主机停止Asynchronous传输
*                           USBHOST_OP_ENABLE:  USB主机启用Asynchronous传输
* Output         : None
* Return         : 返回当前执行状态
*******************************************************************************/
UINT8 USBHOST_Asynchronous_Setting( UINT8 option )
{            
    UINT16 timeout;

    if( option == USBHOST_OP_ENABLE ) 
    {    
        /* 判断是否已启用,若已启用则直接返回 */
        if( EHCI_Regs->hc_usbsts & RB_ASCH_STS ) 
        {
            return( USB_OPERATE_SUCCESS );    
        }
                                
        /* 启动Asynchronous传输,并等待启动成功 */    
        EHCI_Regs->hc_usbcmd |= RB_ASCH_EN;
        Delay_ms( 1 );    
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( EHCI_Regs->hc_usbsts & RB_ASCH_STS ) 
            {
                break;
            }
        }
        if( timeout == 0 ) 
        {
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
        }
        return( USB_OPERATE_SUCCESS );
    }
    else if( option == USBHOST_OP_DISABLE ) 
    {
        /* 判断是否已停止,若已停止则直接返回 */
        if( ( EHCI_Regs->hc_usbcmd & RB_ASCH_EN ) == 0x00 && ( EHCI_Regs->hc_usbsts & RB_ASCH_STS ) == 0x00 ) 
        {
            return( USB_OPERATE_SUCCESS );    
        }
        
        /* 停止Asynchronous传输,并等待停止结束 */                
        EHCI_Regs->hc_usbcmd &= ~RB_ASCH_EN;
        Delay_ms( 1 );    
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( ( EHCI_Regs->hc_usbsts & RB_ASCH_STS ) == 0 ) 
            {
                break;
            }    
        }
        if( timeout == 0 ) 
        {
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
         }
         return( USB_OPERATE_SUCCESS );
    }                
    return( USB_PARAMETER_ERROR );
}

/*******************************************************************************
* Function Name  : USBHOST_Periodic_Setting
* Description    : USB主机启用、停止Periodic传输
* Input          : option---USBHOST_OP_DISABLE: USB主机停止Periodic传输
*                           USBHOST_OP_ENABLE:  USB主机启用Periodic传输
* Output         : None
* Return         : 返回当前执行状态
*******************************************************************************/
UINT8 USBHOST_Periodic_Setting( UINT8 option )
{
    UINT16 timeout;

    if( option == USBHOST_OP_ENABLE ) 
    {    
        /* 判断是否已启用,若已启用则直接返回 */
        if( EHCI_Regs->hc_usbsts & RB_PSCH_STS ) 
        {
            return( USB_OPERATE_SUCCESS );    
        }
                
        /* 启动Periodic传输,并等待启动成功 */    
        EHCI_Regs->hc_usbcmd |= RB_PSCH_EN;    
        Delay_ms( 1 );                                    
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( EHCI_Regs->hc_usbsts & RB_PSCH_STS ) 
            {
                break;
            }
        }
        if( timeout == 0 ) 
        {
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
        }
         return( USB_OPERATE_SUCCESS );
    }
    else if( option == USBHOST_OP_DISABLE ) 
    {
        /* 判断是否已停止,若已停止则直接返回 */
        if( ( EHCI_Regs->hc_usbcmd & RB_PSCH_EN ) == 0x00 && ( EHCI_Regs->hc_usbsts & RB_PSCH_STS ) == 0x00 ) 
        {
            return( USB_OPERATE_SUCCESS );    
        }
                
        /* 停止Periodic传输,并等待停止结束 */                
        EHCI_Regs->hc_usbcmd &= ~RB_PSCH_EN;
        Delay_ms( 1 );    
        for( timeout = 1000; timeout != 0; timeout -- ) 
        {
            Delay_us( 10 );        
            if( ( EHCI_Regs->hc_usbsts & RB_PSCH_STS ) == 0 ) 
            {
                break;
            }
        }
        if( timeout == 0 ) 
        {
            return( USB_OPERATE_ERROR );                                        /* 超时退出 */
        }
         return( USB_OPERATE_SUCCESS );
    }                
    return( USB_PARAMETER_ERROR );
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_Init
* Description    : USB主机EHCI初始化
* Input          : None
* Output         : None
* Return         : USB_OPERATE_SUCCESS---EHCI主机初始化成功
*                  USB_HC_ERROR----------EHCI主机初始化失败
*******************************************************************************/
UINT8 USBHOST_EHCI_Init( void )
{
    UINT8  status;
    UINT16 count;
    
#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_EHCI_Init:...\n " );
#endif
    /* Set EHCI Memory Register */
    EHCI_Caps = ( EHCI_CAPABILITY *)( USB_BASE_ADDR + 0x000 );
    EHCI_Regs = ( EHCI_REGISTERS *)( USB_BASE_ADDR + 0x010 );      

    USBHOST_HC_StopRun_Setting( USBHOST_OP_DISABLE );                           /* 关闭USB主机控制器 */
    Delay_ms( 3 );

    /* Reset EHCI Host Control */
    EHCI_Regs->hc_usbcmd |= RB_HC_RESET;                                        /* 开始HC复位 */
    for( count = 1000; count != 0; count -- ) 
    {
        Delay_us( 10 );
        if( ( EHCI_Regs->hc_usbcmd & RB_HC_RESET ) == 0 ) 
        {
            break;                                                              /* 等待HC复位完成,硬件自动清0 */
        }
    }
    if( count == 0 ) 
    {
        return( USB_HC_ERROR );                                                 /* 超时未复位完成,则报告错误 */    
    }    
    Delay_ms( 3 );    

    /* 检查部分寄存器默认值是否正确 */
    if( RB_HC_LENGTH( EHCI_Caps->hc_caplength ) != 0x0010 )
    {
        return( USB_HC_ERROR );
    }    
    if( RB_HC_VERSION( EHCI_Caps->hc_caplength ) != 0x0100 )
    {
        return( USB_HC_ERROR );
    }    
    if( EHCI_Caps->hc_hcsparams != 0x0001 )
    {
        return( USB_HC_ERROR );
    }    
    if( EHCI_Caps->hc_hccparams != 0x0006 )
    {
        return( USB_HC_ERROR );
    }    

    /* 初始化EHCI部分寄存器 */
//    EHCI_Regs->hc_frindex = 0x00000000;                                       /* 清USB数据索引寄存器 */
    EHCI_Regs->hc_periodiclistbase = 0x00000000;                                /* 周期性数据帧列表基址清0 */
    EHCI_Regs->hc_asynchronous = 0x00000000;                                    /* 异步列表地址清0 */                                     
    EHCI_Regs->hc_portsc = EHCI_PORT_INIT;                                      /* 清端口状态、控制寄存器 */    
    EHCI_Regs->hc_usbint_en = RB_INT_OAA_EN | RB_H_SYSERR_EN | RB_PO_CHG_INT_EN |
                              RB_USBERR_INT_EN | RB_USB_INT_EN;                 /* 设置USB中断启用寄存器 */
    EHCI_Regs->hc_usbcmd = 0x00080100 | 0x08;                                   /* 清USB命令寄存器 */        
    EHCI_Regs->hc_usbsts = EHCI_Regs->hc_usbsts;                                /* 清USB状态寄存器 */ 
    status = USBHOST_Structure_DeInit( );                                       /* 初始化USB HOST为所有结构体分配的内存 */
    if( status )
    {
        return( status );
    }    
    EHCI_Regs->hc_asynchronous = gEHCI_QHD_BaseAddr;                            /* 设置异步列表地址 */ 
    EHCI_Regs->hc_periodiclistbase = gEHCI_Preiodic_Frame_List_BaseAddr;        /* 设置周期性数据帧列表基址 */
    Delay_ms( 1 );                                                              /* 延时 */
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : USBHOST_Structure_DeInit
* Description    : USB主机控制器所有数据结构所在内存分配及初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Structure_DeInit( void )
{
    UINT8  i;
    UINT8  *pData;
    UINT32 address;
    char   *host_memory_ptr;

    /* 确保传输全部关闭 */
    USBHOST_Periodic_Setting( USBHOST_OP_DISABLE );
    USBHOST_Asynchronous_Setting( USBHOST_OP_DISABLE );
    Delay_ms( 3 );

    /* 1、进行EHCI相关内存分配 */
    gEHCIFreeRamAddr = (UINT32)EHCI_Allocate_Buf;                                
    host_memory_ptr = (char *)( ( gEHCIFreeRamAddr + 0x1F ) & ~0x1F );
    gEHCI_RAM_BaseAddr = (UINT32)host_memory_ptr;                               /* EHCI数据结构基地址 */
    gEHCIFreeRamAddr = (UINT32)host_memory_ptr;                                 /* EHCI内存分配空闲起始地址 */
                    
    /* 1.1、分配QHD所需内存空间 */
    address = (UINT32)host_memory_ptr;
    if( ( address + ( EHCI_QHD_SIZE * EHCI_QHD_MAX ) ) > ( (UINT32)EHCI_Allocate_Buf + EHCI_ALLOCATE_BUF_LEN ) )
    {
        return( USB_MEM_ALLOCATE_ERROR );                                       /* 内存分配失败 */    
    }                                            
    gEHCI_QHD_BaseAddr = address;                                               /* EHCI数据结构QHD基地址 */        
    gEHCIFreeRamAddr = address + ( EHCI_QHD_SIZE * EHCI_QHD_MAX );              /* 更新可用起始地址 */    

    /* 1.2、分配QTD所需内存空间 */
    address = gEHCIFreeRamAddr + ( EHCI_QTD_SIZE * EHCI_QTD_MAX );
    if( address > ( (UINT32)EHCI_Allocate_Buf + EHCI_ALLOCATE_BUF_LEN ) )
    {
        return( USB_MEM_ALLOCATE_ERROR );                                       /* 内存分配失败 */    
    }                                            
    gEHCI_QTD_BaseAddr = gEHCIFreeRamAddr;                                      /* EHCI数据结构QTD基地址 */
    gEHCIFreeRamAddr = address;                                                 /* 更新可用起始地址 */    

    /* 1.3、分配ITD所需内存空间 */
    address = gEHCIFreeRamAddr + ( EHCI_ITD_SIZE * EHCI_ITD_MAX );
    if( address > ( (UINT32)EHCI_Allocate_Buf + EHCI_ALLOCATE_BUF_LEN ) )
    {
        return( USB_MEM_ALLOCATE_ERROR );                                       /* 内存分配失败 */    
    }                                            
    gEHCI_ITD_BaseAddr = gEHCIFreeRamAddr;                                      /* EHCI数据结构ITD基地址 */
    gEHCIFreeRamAddr = address;                                                 /* 更新可用起始地址 */    

    /* 1.4、分配周期帧列表所需内存空间 */                                        
    gEHCI_Preiodic_Frame_List_BaseAddr = (UINT32)EHCI_PERIODIC_LIST_Buf;        /* EHCI周期列表数据结构基地址 */

    /* 2、初始化已分配的EHCI相关内存 */
    pData = EHCI_Allocate_Buf;
    while( (UINT32)pData < gEHCIFreeRamAddr )
    {
        *pData = 0;
        pData++;
    }
    
    /* 3、清QHD、QTD、ITD、DataPage内存管理单元 */    
    for( i = 0; i < EHCI_QHD_MAX; i++ )
    {
        EHCI_qHD_Manage[ i ] = EHCI_MEM_FREE;
    }
    for( i = 0; i < EHCI_QTD_MAX; i++ )
    {
        EHCI_qTD_Manage[ i ] = EHCI_MEM_FREE;
    }    
    for( i = 0; i < EHCI_ITD_MAX; i++ )
    {
        EHCI_iTD_Manage[ i ] = EHCI_MEM_FREE;
    }    
    for( i = 0; i < EHCI_DATAPAGE_MAX; i++ )
    {
        EHCI_DataPage_Manage[ i ] = EHCI_MEM_FREE;
    } 
    
    /* 4、指定EHCI数据缓冲区地址 */
    gEHCI_DataPage_BaseAddr = (UINT32)EHCI_Data_Buf;                            /* EHCI数据缓冲区基地址 */

    /* 5、分配Setup包所需内存 */
    Ctl_Setup = (USB_SETUP_REQ *)gEHCIFreeRamAddr;
    gEHCIFreeRamAddr = gEHCIFreeRamAddr + 8;                        
    return( USB_OPERATE_SUCCESS );           
}

/*******************************************************************************
* Function Name  : USBHOST_GetStructure
* Description    : 分配数据结构所需内存
* Input          : Type---要分配的数据结构类型
* Output         : None
* Return         : 返回USB_MEM_ALLOCATE_ERROR表示失败; 
*                  返回非USB_MEM_ALLOCATE_ERROR表示成功分配的实际内存地址
*******************************************************************************/
UINT32 USBHOST_GetStructure( UINT8 Type )
{
    UINT32 i;
    qHD_Structure *spTempqHD;    
    qTD_Structure *spTempqTD;
    iTD_Structure *spTempiTD;

    switch( Type ) 
    {
        case EHCI_MEM_TYPE_QHD:                                                 /* 数据结构: QHD */
            for( i = 0; i < EHCI_QHD_MAX; i++ )
            {
                /* 寻找未用内存 */
                if( EHCI_qHD_Manage[ i ] == EHCI_MEM_FREE ) 
                {
                    EHCI_qHD_Manage[ i ] = EHCI_MEM_USED;                       /* 设置已分配标志 */
                    
                    /* 对qHD数据结构部分进行初始化 */                    
                    spTempqHD = ( qHD_Structure * )( gEHCI_QHD_BaseAddr + ( i * EHCI_QHD_SIZE ) );
                    return( (UINT32)spTempqHD );
                }
            }                 
            break;        
        
        case EHCI_MEM_TYPE_QTD:                                                 /* 数据结构: QTD */
            for( i = 0; i < EHCI_QTD_MAX; i++ )
            {
                /* 寻找未用内存 */
                if( EHCI_qTD_Manage[ i ] == EHCI_MEM_FREE ) 
                {
                    EHCI_qTD_Manage[ i ] = EHCI_MEM_USED;                       /* 设置已分配标志 */

                    /* 对qTD数据结构部分进行初始化 */                    
                    spTempqTD = ( qTD_Structure * )( gEHCI_QTD_BaseAddr + ( i * EHCI_QTD_SIZE ) );                
                    spTempqTD->bTerminate = 0x01;       
                    spTempqTD->bStatus_Active = 0x00;            
                    spTempqTD->bInterruptOnComplete = 0x01;   
                    spTempqTD->bAlternateTerminate = 0x01;    
                    spTempqTD->bErrorCounter = 0x03;                        
                    return( (UINT32)spTempqTD );
                }
            }                 
            break;

        case EHCI_MEM_TYPE_ITD:                                                 /* 数据结构: ITD */ 
            for( i = 0; i < EHCI_ITD_MAX; i++ )
            {
                /* 寻找未用内存 */
                if( EHCI_iTD_Manage[ i ] == EHCI_MEM_FREE )                 
                {
                    EHCI_iTD_Manage[ i ] = EHCI_MEM_USED;                       /* 设置已分配标志 */

                    /* 对iTD数据结构部分进行初始化 */                                        
                    spTempiTD = ( iTD_Structure * )( gEHCI_ITD_BaseAddr + ( i * EHCI_ITD_SIZE ) );
                    spTempiTD->bTerminate = 0x01; 
                    return( (UINT32)spTempiTD );
                }
            }
            break;

        case EHCI_MEM_TYPE_SITD:                                                /* 数据结构: SITD */ 
            /* 不支持SITD */
            break;

        case EHCI_MEM_TYPE_4K_BUFFER:                                           /* 数据缓冲区 */     
            for( i = 0; i < EHCI_DATAPAGE_MAX; i++ )                        
            {
                /* 寻找未用内存 */
                if( EHCI_DataPage_Manage[ i ] == EHCI_MEM_FREE ) 
                {
                    EHCI_DataPage_Manage[ i ] = EHCI_MEM_USED;
                    return( (UINT32)( gEHCI_DataPage_BaseAddr + i * EHCI_DATAPAGE_SIZE ) );    
                }               
            }
            break;
           
        default:
            break;      
    }
    return( USB_MEM_ALLOCATE_ERROR );                                           /* 内存分配出错 */
}

/*******************************************************************************
* Function Name  : USBHOST_ReleaseStructure
* Description    : 回收数据结构所分配的内存
* Input          : Type--------要回收的数据结构类型
*                  pwAddress---要回收的数据结构物理地址
* Output         : None
* Return         : 返回USB_OPERATE_ERROR表示回收失败; 
*                  返回USB_OPERATE_SUCCESS表示回收成功
*******************************************************************************/
UINT8 USBHOST_ReleaseStructure( UINT8 Type, UINT32 pwAddress )
{
    UINT32 i;
    UINT32 wReleaseNum;
    UINT32 *pData;

    pData = ( UINT32 *)pwAddress; 
    switch( Type ) 
    {
        case EHCI_MEM_TYPE_QHD:  
            if( ( pwAddress < gEHCI_QHD_BaseAddr ) || ( ( pwAddress > gEHCI_QTD_BaseAddr ) ) ) 
            {
                return( USB_OPERATE_ERROR );
            }
            wReleaseNum = ( pwAddress - gEHCI_QHD_BaseAddr ) / EHCI_QHD_SIZE;                
            if( wReleaseNum > EHCI_QHD_MAX ) 
            {
                return( USB_OPERATE_ERROR );
            }

            /* 回收数据结构所分配的内存 */                
            EHCI_qHD_Manage[ wReleaseNum ] = EHCI_MEM_FREE;                     /* 清已分配标志 */
            for( i = 0; i < EHCI_QHD_SIZE / 4; i++ )
            {
                *( pData + i ) = 0x00;
            }
            break;        
        
        case EHCI_MEM_TYPE_QTD:  
            if( ( pwAddress < gEHCI_QTD_BaseAddr ) || ( ( pwAddress > gEHCI_ITD_BaseAddr ) ) ) 
            {
                return( USB_OPERATE_ERROR );
            }
            wReleaseNum = ( pwAddress - gEHCI_QTD_BaseAddr ) / EHCI_QTD_SIZE;                
            if( wReleaseNum > EHCI_QTD_MAX ) 
            {
                return( USB_OPERATE_ERROR );
            }

            /* 回收数据结构所分配的内存 */                
            EHCI_qTD_Manage[ wReleaseNum ] = EHCI_MEM_FREE;                     /* 清已分配标志 */
            for( i = 0; i < EHCI_QTD_SIZE / 4; i++ )
            {
                *( pData + i ) = 0x00;
            }
            break;

        case EHCI_MEM_TYPE_ITD:
            if( ( pwAddress < gEHCI_ITD_BaseAddr ) || ( pwAddress > gEHCI_Preiodic_Frame_List_BaseAddr ) ) 
            {
                return( USB_OPERATE_ERROR );
            }
            wReleaseNum = ( pwAddress - gEHCI_ITD_BaseAddr ) / EHCI_ITD_SIZE;                
            if( wReleaseNum > EHCI_ITD_MAX ) 
            {
                return( USB_OPERATE_ERROR );
            }
            
            /* 回收数据结构所分配的内存 */     
            EHCI_iTD_Manage[ wReleaseNum ] = EHCI_MEM_FREE;                     /* 清已分配标志 */             
            for( i = 0; i < EHCI_ITD_SIZE / 4; i++ )
            {
                *( pData + i ) = 0x00;        
            }
            break;

        case EHCI_MEM_TYPE_4K_BUFFER:      
            if( ( pwAddress < gEHCI_DataPage_BaseAddr ) || ( pwAddress > gEHCI_DataPage_BaseAddr + EHCI_DATA_BUF_LEN ) ) 
            {
                return( USB_OPERATE_ERROR );
            }
            wReleaseNum = ( pwAddress - gEHCI_DataPage_BaseAddr ) / EHCI_DATAPAGE_SIZE;
            if( wReleaseNum > EHCI_DATAPAGE_MAX) 
            {
                return( USB_OPERATE_ERROR );
            }  
            
            /* 回收数据缓冲区所分配的内存 */                        
            EHCI_DataPage_Manage[ wReleaseNum ] = EHCI_MEM_FREE;                /* 清已分配标志 */     
            break;    

        default: 
            return( USB_OPERATE_ERROR );
    }
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : USBHOST_Allocate_QH
* Description    : 初始化分配的QH
* Input          : *Device---------当前操作的设备
                   *psQHTemp-------需要进行初始化的QH
*                  bQHType---------当前QH类型
*                  bType-----------当前端点是否为控制传输端点
*                  bHead-----------当前QH是否为队列头
*                  bEndPt----------当前端点号
*                  wMaxPacketSize--当前端点最大包大小
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Allocate_QHD( ATTACH_DEVICE_INFO *Device, 
                            qHD_Structure *psQHTemp, 
                            UINT8 bQHType, 
                            UINT8 bType, 
                            UINT8 bHead, 
                            UINT8 bEndPt, 
                            UINT32 wMaxPacketSize )
{
    qTD_Structure *spTempqTD;

    /* 根据提供的参数,初始化分配的qHD */
    /* <1>.Next_qHD_Pointer Word */
    psQHTemp->bTerminate = 0x00;                  
    psQHTemp->bType = bQHType;                         
                
    /* <2>.qHD_2 Word */
    psQHTemp->bDeviceAddress = Device->bAddress;                                /* Bit6---0 */     
    psQHTemp->bInactiveOnNextTransaction = 0x00;                                /* Bit7 */
    psQHTemp->bEdNumber = bEndPt;                                               /* Bit11---8 */               
    psQHTemp->bEdSpeed = Device->bSpeed;                                        /* Bit13---12 */ 
    if( bType )
    {    
        psQHTemp->bDataToggleControl = 0x01;                                    /* Bit14: 1 for control 0 for Bulk */   
    }
    else
    {
        psQHTemp->bDataToggleControl = 0x00;                                    /* Bit14: 1 for control 0 for Bulk */   
    }    
    psQHTemp->bHeadOfReclamationListFlag = bHead;                               /* Bit15 */
    psQHTemp->bMaxPacketSize = wMaxPacketSize;                                  /* Bit26---16 */     
    if( ( Device->bSpeed != EHCI_DEVICE_SPEED_HS ) && ( bType ) )
    {    
        psQHTemp->bControlEdFlag = 0x01;                                        /* Bit27 */ 
    }
    psQHTemp->bNakCounter = 0x00;                                               /* Bit31---28 */ 

    /* <3>.qHD_3 Word */    
    psQHTemp->bInterruptScheduleMask = 0x00;                                    /* Bit7---0 */
    psQHTemp->bSplitTransactionMask = 0x00;                                     /* Bit15---8 */
    psQHTemp->bHubAddr = Device->bHubAddress;                                   /* Bit22---16 */  
    psQHTemp->bPortNumber = Device->bOnHubPortNumber;                           /* Bit29---23 */  
    psQHTemp->bHighBandwidth = 0x01;                                            /* Bit31---30 */  

    /* <4>.Overlay_CurrentqTD */  

    /* <5>.Overlay_NextqTD */       
    psQHTemp->bOverlay_NextTerminate = 0x01;                                    /* Bit0 */

    /* <6>.Overlay_AlternateNextqTD */       
    psQHTemp->bOverlay_AlternateNextTerminate = 0x01;                           /* Bit0 */

    /* <7>.Overlay_TotalBytes */       
    psQHTemp->bOverlay_Status = 0;

    /* <8>.Overlay_BufferPointer0 */ 
   
    /* 分配1个qTD,并挂载到QH上 */
    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD ); 
    psQHTemp->bOverlay_NextqTD = ( ( (UINT32)spTempqTD ) >> 0x05 );
    psQHTemp->bOverlay_AlternateqTD = ( ( (UINT32)spTempqTD ) >> 0x05 );

    /* <4>.Overlay_CurrentqTD */  
    psQHTemp->bOverlay_CurrentqTD = (UINT32)spTempqTD;                          /* 不必要,用于记忆当前操作副本的原始TD */
    psQHTemp->bOverlay_NextTerminate = 0x00;
    psQHTemp->bOverlay_AlternateNextTerminate = 0x00;
    
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : USBHOST_Resume
* Description    : 
* Input          : *Device----------当前操作的设备
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Resume( ATTACH_DEVICE_INFO *Device )
{
//    if( ( EHCI_Regs->hc_portsc & RB_PO_SUSP ) == 0 )  return( USB_PARAMETER_ERROR );

    EHCI_Regs->hc_portsc |= RB_F_PO_RESM;    
    Delay_ms( 30 );      
    
    /* Write PORTSC->ForcePortResume = 0 (Host should not issue the PortChange interrupt) */
    EHCI_Regs->hc_portsc &= ~RB_F_PO_RESM;    
    Delay_us( 100 );

    if( EHCI_Regs->hc_portsc & RB_PO_SUSP )  
    {
        return( USB_PARAMETER_ERROR );
    }
    Device->bSuspend = 0x00;                                                    /* 清设备挂起标志 */
    return ( USB_OPERATE_SUCCESS );  
}

/*******************************************************************************
* Function Name  : USBHOST_Suspend
* Description    : 
* Input          : *Device----------当前操作的设备
* Output         : None
* Return         : 返回0x00表示成功; 返回0x01表示失败
*******************************************************************************/
UINT8 USBHOST_Suspend( ATTACH_DEVICE_INFO *Device )
{
    if( ( EHCI_Regs->hc_portsc & RB_PO_EN ) == 0 )    
    {
        return( USB_PARAMETER_ERROR );
    }
       
    /* Write PORTSC->Suspend=1 */    
    EHCI_Regs->hc_portsc |= RB_PO_SUSP; 
    Device->bSuspend = 1;
    Delay_us( 100 );
    if ( ( EHCI_Regs->hc_portsc & RB_PO_SUSP ) == 0x00 ) EHCI_Regs->hc_portsc |= RB_PO_SUSP;

    /* Waiting for the PORTSC->Suspend=1 */
//    while( EHCI_Regs->hc_portsc & RB_PO_SUSP == 0x00 );

    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : DEBUG_Printf_Memary
* Description    : 打印指定内存空间数据
* Input          : startaddr---内存空间起始地址
*                  size--------打印双字节数据数
* Output         : None
* Return         : None
*******************************************************************************/
void DEBUG_Print_Memory( UINT32 startaddr, UINT16 size )
{
    UINT32 *p;
    UINT16 i;
    
    p = (UINT32 *)startaddr;    
    printf( "Memory Start Address: %08lx\n",startaddr);
    for( i = 0; i < size; i++ )
    {
        printf( "%08lx\n",*p++ );
    }        
}

/*******************************************************************************
* Function Name  : USBHOST_DevicePara_Init
* Description    : 初始化设备相关参数
* Input          : *Device---当前操作的设备
* Output         : None
* Return         : None
*******************************************************************************/
void USBHOST_DevicePara_Init( ATTACH_DEVICE_INFO *Device )
{                                                                
    gEHCILastStatus = 0x00;                                                     /* 清EHCI最后一次操作状态 */

    Device->bDeviceOnHub = 0x00;                                                /* 默认当前USB设备未连接在HUB下 */
    Device->bOnHubPortNumber = 0x00;                                            /* 默认当前USB设备在HUB上的端口号为0 */
    Device->bAddress = 0x00;                                                    /* 默认当前USB设备地址为0 */
    Device->bHubAddress = 0x00;                                                 /* 默认当前USB挂载的HUB地址为0 */    
    Device->bConnectStatus = 0x00;                                              /* 默认当前USB设备未连接 */
    Device->bPortEnableDisableStatus = 0x00;                                    /* 默认端口未使能 */    
    Device->bSpeed = 0xFF;                                                      /* 默认当前USB设备速度未知 */    
    Device->bEp0MaxPks = DEFAULT_USB_EP0_SIZE;                                  /* 默认当前USB设备端点0最大包大小为8 */    
    Device->bPortReset = 0x00;                                                  /* 默认当前USB设备未复位 */
    Device->bSuspend = 0x00;                                                    /* 默认当前USB设备未挂起 */
    Device->bRemoteWakeUpDetected = 0x00;                                    
    Device->bSendOK = 0x00;                                                     /* 默认当前USB设备发送成功 */
    Device->bSendStatusError = 0x00;                                            /* 默认当前USB设备发送状态为0 */        
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_BusReset
* Description    : USB主机进行总线复位
* Input          : *Device---当前操作的设备
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_EHCI_BusReset( ATTACH_DEVICE_INFO *Device )  
{
    UINT16 timeout;

#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_BusReset:...\n " );
#endif    

    if( Device->bDeviceOnHub )                                                  /* 当前设备连接在HUB下 */    
    {
        return( USB_OPERATE_SUCCESS );
    }

    /* 设置设备地址及端点0最大包大小 */
    Device->bEp0MaxPks = DEFAULT_USB_EP0_SIZE;                                  /* 默认当前USB设备端点0最大包大小为64 */    
    Device->bAddress = 0x00;                                                    /* 保存当前操作的地址值 */
    
    /* 进行USB总线复位 */
    EHCI_Regs->hc_portsc = EHCI_PORT_INIT | RB_PO_RESET;                        /* 设置PORTSC位8进行USB总线复位 */
    Delay_ms( 60 );        
    EHCI_Regs->hc_portsc = EHCI_PORT_INIT;                                      /* 清PORTSC位8结束USB总线复位 */
    Delay_ms( 2 );        
        
    /* 等待复位结束 */    
    for( timeout = 2000; timeout != 0; timeout -- ) 
    {
        Delay_us( 10 );                
        if( ( EHCI_Regs->hc_portsc & RB_PO_RESET ) == 0 ) 
        {
            break;                                                              /* 等待复位结束 */
        }
    }
    if( timeout == 0 ) 
    {
        return( USB_OPERATE_ERROR );                                            /* 超时未复位完成,则报告错误 */    
    }
    Delay_ms( 5 );                                                              /* EHCI必须等一会才能工作 */
    EHCI_Regs->hc_usbsts = RB_PO_CHG_DET;
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_Detect_Connect
* Description    : USB主机检测设备插拔
*                  注：端口使能位在总线复位时由主机控制器自动置1
* Input          : *Device---当前操作的设备
* Output         : None
* Return         : 0x00-----------------无USB设备
*                  0x01-----------------有USB设备但尚未使能
*                  0x02-----------------有USB设备且已经使能
*                  USB_INT_CONNECT------检测到USB设备连接事件
*                  USB_INT_DISCONNECT---检测到USB设备断开事件
*******************************************************************************/
UINT8 USBHOST_EHCI_Detect_Connect( ATTACH_DEVICE_INFO *Device )
{            
    if( EHCI_Regs->hc_portsc & RB_CONN_CHG )                                    /* Connect status change */
    {    
        EHCI_Regs->hc_usbsts = RB_PO_CHG_DET;                                   /* Clear Port change detect */
        EHCI_Regs->hc_portsc = RB_CONN_CHG;                                     /* 清变化位 */
        Delay_ms( 3 );
                    
        /* 判断当前改变是连接还是断开 */
        if( EHCI_Regs->hc_portsc & RB_CONN_STS )
        {
            /* start controller operations */                
            USBHOST_HC_StopRun_Setting( USBHOST_OP_ENABLE );                    /* 启动USB主机控制器 */                
            Delay_ms( 110 );
            USBHOST_EHCI_BusReset( Device );                                    /* 进行USB总线复位 */
            Delay_ms( 5 );
            EHCI_Regs->hc_usbsts = RB_PO_CHG_DET;
            Device->bConnectStatus = 0x01;                

            /* 进行当前USB设备速度判断 */
            Device->bSpeed = ( OTG_CONTROL_STATUS >> 22 ) & 0x03;               /* 取OTG Control Status Register(0x080)的位23-22 */        
            return( USB_INT_CONNECT );                                          /* 检测到USB设备连接事件 */                 
        }    
        else
        {
            USBHOST_DevicePara_Init( Device );                                  /* 清相关参数 */    
            return( USB_INT_DISCONNECT );                                       /* 检测到USB设备断开事件 */
        }            
    }    
    else
    {            
        if( ( EHCI_Regs->hc_portsc & RB_CONN_STS ) == 0x00 )
        {
            return( USB_INT_DISCONNECT );                                       /* 无USB设备 */
        }        
        else
        {
            if( EHCI_Regs->hc_portsc & RB_PO_EN )
            {
                return( 0x02 );                                                 /* 有USB设备且已经使能 */    
            }    
            else
            {
                /* start controller operations */                
                USBHOST_HC_StopRun_Setting( USBHOST_OP_ENABLE );                /* 启动USB主机控制器 */                
                Delay_ms( 110 );
                USBHOST_EHCI_BusReset( Device );                                /* 进行USB总线复位 */
                Delay_ms( 5 );
                EHCI_Regs->hc_usbsts = RB_PO_CHG_DET;
                Device->bConnectStatus = 0x01;                
    
                /* 进行当前USB设备速度判断 */
                Device->bSpeed = ( OTG_CONTROL_STATUS >> 22 ) & 0x03;           /* 取OTG Control Status Register(0x080)的位23-22 */        

                return( 0x01 );                                                 /* 有USB设备但尚未使能 */    
            }    
        }                    
    }            
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_EP0_Init
* Description    : USB主机控制端点0初始化
* Input          : *Device---当前操作的设备
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_EHCI_EP0_Init( ATTACH_DEVICE_INFO *Device )
{
    UINT32 temp;
    
#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_EP0_Init:...\n" );
#endif
    
    /* 分配当前控制传输所需要的QH */
    temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
    if( temp == USB_MEM_ALLOCATE_ERROR )
    {
        return( USB_MEM_ALLOCATE_ERROR );
    }    
    Device->qHD_Control = ( qHD_Structure *)temp;                               /* 分配qHD_Structure */

    /*  初始化分配的QHD */
    USBHOST_Allocate_QHD( Device, 
                          Device->qHD_Control, 
                          EHCI_HD_TYPE_QH, 
                          EHCI_EP_TYPE_CONTROL, 
                          1, 
                          0, 
                          DEFAULT_USB_EP0_SIZE );

    /* 将分配的QHD链接到QHD链表中 */
    USBHOST_qHD_Link_Insert( Device->qHD_Control );    
    
    EHCI_Regs->hc_asynchronous = (UINT32)Device->qHD_Control;
    USBHOST_Asynchronous_Setting( USBHOST_OP_ENABLE );                          /* Enable USB asynchronous Schedule */ 
    Delay_ms( 3 );
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : USBHOST_Error_Int_Deal
* Description    : USB主机EHCI相关错误中断处理
*                  只关心以下中断: 位0----RB_USB_INT
*                                  位1----RB_USBERR_INT
*                                  位2----RB_PO_CHG_DET
*                                  位4----RB_H_STSERR 
* Input          : None
* Output         : None
* Return         : 返回当前中断状态
*******************************************************************************/
UINT8 USBHOST_Error_Int_Deal( ATTACH_DEVICE_INFO *Device )
{
    UINT32 ints;
        
    ints = EHCI_Regs->hc_usbsts;                                                /* 读取EHCI中USBSTS寄存器 */
    if( ints & RB_USB_INT )                                                     /* USBSTS位0: USB Interrupt */    
    {
        EHCI_Regs->hc_usbsts = RB_USB_INT;                                      /* 清USB事务完成中断 */
    }
    else if( ints & RB_USBERR_INT )                                             /* USBSTS位1: USB Error Interrupt */
    {    
        EHCI_Regs->hc_usbsts = RB_USBERR_INT;                                   /* 清USB错误中断 */
    }
    else if( ints & RB_PO_CHG_DET )                                             /* USBSTS位2: Port Change Detect */
    {        
        EHCI_Regs->hc_usbsts = RB_PO_CHG_DET;
        return( USB_INT_DISCONNECT );
    }    
    else if( ints & RB_H_STSERR )                                               /* USBSTS位4: HOST系统错误中断 */
    {    
        EHCI_Regs->hc_usbsts = RB_H_STSERR;                                     /* 清HOST系统错误中断 */
    }            
    else
    {
        EHCI_Regs->hc_usbsts = EHCI_Regs->hc_usbsts;                            /* 意外中断 */    
    }                
    return( USB_INT_ERROR );
}

/*******************************************************************************
* Function Name  : USBHOST_Send_qTD
* Description    : USB主机发送一个QTD
* Input          : *Device-------当前操作的设备
                   *spHeadqTD----当前要执行的qTD
*                  *spTempqHD----当前挂载的qHD
*                  *pRetLen------当前qTD执行完毕后返回长度
*                  wTimeOutSec---超时时间(单位为毫秒)
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Send_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen, UINT32 wTimeOutSec )
{
    qTD_Structure *spOldDumyqTD; 
    qTD_Structure *spReleaseqTD;    
    UINT32 timeout;
    UINT8  flag;     

    spOldDumyqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 已有的TD,当前TD */
    memcpy( spOldDumyqTD, spHeadqTD, EHCI_QTD_SIZE );                           /* 复制到当前TD中供操作 */
        
    spOldDumyqTD->bNextQTDPointer = ( (UINT32)spHeadqTD ) >> 5;                 /* 将新TD链接上去作为备用,在处理完后从该指针离开 */
    spOldDumyqTD->bTerminate = 0;   
    spOldDumyqTD->bStatus_Active = 1;                                           /* 启动对当前TD的操作,新TD不会被操作,当前TD的数据被复制到QH中成为操作副本再处理 */
    spOldDumyqTD->bAlternateQTDPointer = ( (UINT32)spHeadqTD ) >> 5;            /* 处理完短包后优先从该指针离开 */
    spOldDumyqTD->bAlternateTerminate = 0;

    Device->psSendLastqTD = spOldDumyqTD;
    Device->bSendStatusError = 0;
  
    flag = 0x00;     
    wTimeOutSec *= 50;
    for( timeout = wTimeOutSec; timeout != 0; timeout -- ) 
    {     
        /* 等待传输结束 */
        Delay_us( 20 );                                                         /* 每间隔20uS左右查询一次 */
        if( spOldDumyqTD->bStatus_Active == 0x00 ) 
        {
            break;                                                              /* 完成,操作完成后自动从QH操作副本中复制回来 */
        }
        if( EHCI_Regs->hc_usbsts & RB_PO_CHG_DET ) 
        {  
            /* 查询设备是否断开 */
            flag = 0x02;
            break;
        }
    }

    if( timeout == 0 )                                                          /* 超时退出处理 */
    {
        USBHOST_Asynchronous_Setting( USBHOST_OP_DISABLE );                     /* 停止异步传输 */
        spOldDumyqTD->bStatus_Active = 0;                                       /* 停止QTD */        
        spOldDumyqTD->bTerminate = 1;                                           /* 断开链表 */
        spOldDumyqTD->bAlternateTerminate = 1;
        spTempqHD->bOverlay_Status &= 0x7F;                                     /* 停止操作副本 */
        Delay_us( 100 );                                                        /* 等待完成 */
    }
    if( pRetLen ) 
    {
        *pRetLen = *pRetLen - spOldDumyqTD->bTotalBytes;                        /* 保存长度 */
    }
    gEHCILastStatus = spTempqHD->bOverlay_Status;                               /* 保存上次清空累计来的执行状态,这是当前操作副本,如果不是超时建议使用spOldDumyqTD的状态 */
    Device->bSendStatusError = gEHCILastStatus;                                 /* 暂存当前qHD执行状态,并判断是否有错误发生 */
    spTempqHD->bOverlay_Status = 0;                                             /* 清空以便再次累计错误 */
    spReleaseqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 超时停在当前TD,完成则停在下一个TD即备用的新TD */
    spReleaseqTD = spReleaseqTD == spOldDumyqTD ? spHeadqTD : spOldDumyqTD;     /* 超时释放新TD,完成则释放当前TD */
    USBHOST_ReleaseStructure( EHCI_MEM_TYPE_QTD, ( UINT32 )spReleaseqTD );      /* 回收qTD */

    if( timeout == 0 ) 
    {
        USBHOST_Asynchronous_Setting( USBHOST_OP_ENABLE );
    }
    
    if( ( gEHCILastStatus & EHCI_QTD_STATUS_HALTED ) || ( gEHCILastStatus & EHCI_QTD_STATUS_BUF_ERROR ) || 
        ( gEHCILastStatus & EHCI_QTD_STATUS_BABBLE ) || ( gEHCILastStatus & EHCI_QTD_STATUS_TRANS_ERROR ) ||
        ( gEHCILastStatus & EHCI_QTD_STATUS_MISS_FRAME ) || ( timeout == 0 ) ) 
    {
        flag = 0x01;
    }
        
#ifdef  MY_DEBUG_PRINTF         
    if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED ) 
    {
        printf("ERR: EHCI_QTD_STATUS_HALTED\n");
    }
    if( gEHCILastStatus & EHCI_QTD_STATUS_BUF_ERROR ) 
    {
        printf("ERR: EHCI_QTD_STATUS_BUF_ERROR\n");
    }
    if( gEHCILastStatus & EHCI_QTD_STATUS_BABBLE ) 
    {
        printf("ERR: EHCI_QTD_STATUS_BABBLE\n");
    }
    if( gEHCILastStatus & EHCI_QTD_STATUS_TRANS_ERROR ) 
    {
        printf("ERR: EHCI_QTD_STATUS_TRANS_ERROR\n");
    }
    if( gEHCILastStatus & EHCI_QTD_STATUS_MISS_FRAME ) 
    {  
        printf("ERR: EHCI_QTD_STATUS_MISS_FRAME\n");
    }
    if( timeout == 0 )
    {
//        printf("EHCI Time Out.........\n");        
    }
#endif   

    /* 返回执行状态 */
    if( flag )  
    {
        if( flag == 0x02 )
        {
            return( USB_INT_DISCONNECT );    
        }
        if( timeout == 0 ) 
        {
            return( ERR_USB_TIMEOUT );
        }
        return( USBHOST_Error_Int_Deal( Device ) );        
    }  
    else
    {         
        return( USB_OPERATE_SUCCESS );
    }
}

/*******************************************************************************
* Function Name  : USBHOST_Issue_Control
* Description    : USB主机发起一个控制传输
* Input          : *Device-----当前操作的设备
*                  *pCmdPack---控制传输命令包
*                  *pDatBuf----收发数据缓冲区
*                   *pRetLen----收发数据实际长度
* Output         : None   
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Issue_Control( ATTACH_DEVICE_INFO *Device, USB_SETUP_REQ *pCmdPack, UINT8 *pDatBuf, UINT16 *pRetLen )
{
    UINT8  status;
    UINT32 total, temp;
    UINT16 retlen;
    UINT8  tog; 
    UINT8  retry;
    UINT8  back[ 8 ];
    UINT8  *pbuf;
    qTD_Structure *spTempqTD;
    
    memcpy( back, pCmdPack, 8 );                                                /* 备份当前命令包 */    
    for( retry = 0; retry < 3; retry++ )                                        /* 重试3次 */
    {
        /* 检测设备是否连接 */
        status = USBHOST_EHCI_Detect_Connect( Device );                         /* 检测设备是否存在 */
        if( ( status == USB_INT_DISCONNECT ) || ( status == 0x00 ) )
        {
             return( USB_INT_DISCONNECT );
        }        
        
        /* 1. SETUP阶段 */
        Delay_us( 500 );        
        pbuf = back;

        /* 1.1 分配qTD并初始化 */
        spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD );  
        spTempqTD->bPID = EHCI_QTD_PID_SETUP;                                   /* Bit9---8 */   
        spTempqTD->bTotalBytes = retlen = 8;                                    /* Bit30---16 */   
        spTempqTD->bDataToggle = 0;                                             /* Bit31 */ 
        spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pbuf; 

        /* 1.2 发送qTD并等待执行完毕  */
        status = USBHOST_Send_qTD( Device, spTempqTD, Device->qHD_Control, &retlen, 1000 );
        Delay_us( 500 );        
        if( status == USB_OPERATE_SUCCESS )
        {    
            /* 2. 数据阶段 */
            /* 由于一次传输最大可以传输20K数据,故没有进行多次循环 */
            tog = TRUE;                                                         /* 默认DATA1,默认无数据故状态阶段为IN */    
            total = ( ( (UINT32)Ctl_Setup->wLengthH ) << 8 ) + Ctl_Setup->wLengthL;
            if( total && pDatBuf )                                              /* 需要进行数据收发 */
            {
                pbuf = pDatBuf;
                if( Ctl_Setup->bReqType & 0x80 )                                /* DATA阶段: 数据接收 */
                {    
                    /* 2.1 分配IN包相关qTD并初始化 */
                    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD );
                    spTempqTD->bPID = EHCI_QTD_PID_IN;                          /* Bit9---8 */   
                    spTempqTD->bTotalBytes = retlen = total;                    /* Bit30---16 */   
                    spTempqTD->bDataToggle = 1;                                 /* Bit31 */ 
                    spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pbuf;
                    temp = ( ( UINT32 )( pbuf ) >> 12  + 1 ) << 12;
                    spTempqTD->ArrayBufferPointer_Word[ 1 ] = (UINT32)temp; 
                    spTempqTD->ArrayBufferPointer_Word[ 2 ] = (UINT32)temp + 4096;                    
                    spTempqTD->ArrayBufferPointer_Word[ 3 ] = (UINT32)temp + 2 * 4096;                    
                    spTempqTD->ArrayBufferPointer_Word[ 4 ] = (UINT32)temp + 3 * 4096;                    

                    /* 2.2 启动传输并等待执行完毕  */
                    status = USBHOST_Send_qTD( Device, spTempqTD, Device->qHD_Control,  &retlen, 1000 );                    
                    if( status == USB_INT_DISCONNECT )                          /* 如果设备断开,直接返回 */
                    {
                        return( status );
                    }
                    if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )              /* 返回STALL错误 */
                    {
                        USBHOST_EHCI_ClearEndpStall( Device, 0x00 );            /* 清除端点 */
                    }
                    tog = FALSE;                                                /* 状态阶段为OUT */
                }           
                else                                                            /* DATA阶段: 数据发送 */
                {
                    /* 2.1 分配OUT包相关qTD并初始化 */
                    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD );  
                    spTempqTD->bPID = EHCI_QTD_PID_OUT;                         /* Bit9---8 */   
                    spTempqTD->bTotalBytes = retlen = total;                    /* Bit30---16 */   
                    spTempqTD->bDataToggle = 1;                                 /* Bit31 */ 
                    spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pbuf;
                    temp = ( ( UINT32 )( pbuf ) >> 12  + 1 ) << 12;                    
                    spTempqTD->ArrayBufferPointer_Word[ 1 ] = (UINT32)temp; 
                    spTempqTD->ArrayBufferPointer_Word[ 2 ] = (UINT32)temp + 4096;                    
                    spTempqTD->ArrayBufferPointer_Word[ 3 ] = (UINT32)temp + 2 * 4096;                    
                    spTempqTD->ArrayBufferPointer_Word[ 4 ] = (UINT32)temp + 3 * 4096;                    

                    /* 启动传输并等待执行完毕  */
                    status = USBHOST_Send_qTD( Device, spTempqTD, Device->qHD_Control,  &retlen, 1000 );
                    if( status == USB_INT_DISCONNECT )                          /* 如果设备断开,直接返回 */
                    {
                        return( status );
                    }
                    if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )              /* 返回STALL错误 */
                    {
                        USBHOST_EHCI_ClearEndpStall( Device, 0x00 );            /* 清除端点 */
                    }
                    tog = TRUE;                                                 /* 状态阶段为IN */
                }
                
                /* 保存实际传输长度 */        
                if( pRetLen )  
                {
                    *pRetLen = retlen;                                          /* 实际成功收发的总长度 */
                }                
            }            
            if( status == USB_OPERATE_SUCCESS )
            {    
                /* 3. 状态阶段 */                
                Delay_us( 500 );        
                /* 3.1 分配IN或OUT包相关qTD并初始化 */
                spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD );
                if( tog == FALSE )    
                {    
                    spTempqTD->bPID = EHCI_QTD_PID_OUT;                         /* Bit9---8 */   
                }
                else
                {
                    spTempqTD->bPID = EHCI_QTD_PID_IN;                          /* Bit9---8 */                       
                }    
                spTempqTD->bDataToggle = 1;                                     /* Bit31 */ 
                spTempqTD->bTotalBytes = retlen = 0x00;                         /* Bit30---16 */   
                pbuf = back;
                spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pbuf;

                /* 3.2 启动传输并等待执行完毕  */
                status = USBHOST_Send_qTD( Device, spTempqTD, Device->qHD_Control,  &retlen, 1000 );
                if( status == USB_OPERATE_SUCCESS )    
                {    
                    Delay_us( 200 );        
                    return( USB_OPERATE_SUCCESS );            
                }
                else if( status == USB_INT_DISCONNECT )                         /* 如果设备断开,直接返回 */
                {
                    return( status );
                }                
                else
                {
                    /* 缓冲区错、Timeout、CRC、Bad PID等错误则重试 */    
                }    
            }
            else if( status == USB_INT_DISCONNECT )                             /* 如果设备断开,直接返回 */
            {
                return( status );                
            }     
        }    
        Delay_us( 1000 );        
    }
    return( USB_OPERATE_ERROR );    
}

/*******************************************************************************
* Function Name  : USBHOST_Issue_Bulk
* Description    : USB主机执行批量传输
* Input          : *Device-----当前操作的设备
*                  *pBlukqHD---批量端点QH
*                  *pDatBuf---数据缓冲区
*                  *pSize-----发送、接收数据大小
*                  Type-------传输类型(IN/OUT)
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_Issue_Bulk( ATTACH_DEVICE_INFO *Device, qHD_Structure *pBlukqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type )
{
    qTD_Structure *spTempqTD;
    UINT32 temp;

    /* 为当前批量传输分配1个qTD */    
    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD ); 
    spTempqTD->bTotalBytes = *pSize;      
    spTempqTD->bDataToggle = 0;  
    spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pDatBuf;
    temp = ( ( ( UINT32 )( pDatBuf ) >> 12 ) + 1 ) << 12;
    spTempqTD->ArrayBufferPointer_Word[ 1 ] = (UINT32)temp; 
    spTempqTD->ArrayBufferPointer_Word[ 2 ] = (UINT32)temp + 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 3 ] = (UINT32)temp + 2 * 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 4 ] = (UINT32)temp + 3 * 4096;                       
    spTempqTD->bPID = Type;             

    /* 执行传输并等待返回结果 */
    return( USBHOST_Send_qTD( Device, spTempqTD, pBlukqHD, pSize, 20000 ) );    
}

/*******************************************************************************
* Function Name  : USBHOST_Issue_Interrupt
* Description    : USB主机执行中断传输
* Input          : *Device-----当前操作的设备
*                  *pIntqHD---批量端点QH
*                  *pDatBuf---数据缓冲区
*                  *pSize-----发送、接收数据大小
*                  Type-------传输类型(IN/OUT)
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_Issue_Interrupt( ATTACH_DEVICE_INFO *Device, qHD_Structure *pIntqHD, UINT8 *pDatBuf, UINT16 *pSize, UINT8 Type )
{
    qTD_Structure *spTempqTD;
    UINT32 temp;

    /* 为当前中断传输分配1个qTD */    
    spTempqTD = ( qTD_Structure *)USBHOST_GetStructure( EHCI_MEM_TYPE_QTD ); 
    spTempqTD->bTotalBytes = *pSize;      
//    spTempqTD->bDataToggle = 0;  
    spTempqTD->ArrayBufferPointer_Word[ 0 ] = (UINT32)pDatBuf;
    temp = ( ( ( UINT32 )( pDatBuf ) >> 12 ) + 1 ) << 12;
    spTempqTD->ArrayBufferPointer_Word[ 1 ] = (UINT32)temp; 
    spTempqTD->ArrayBufferPointer_Word[ 2 ] = (UINT32)temp + 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 3 ] = (UINT32)temp + 2 * 4096;                    
    spTempqTD->ArrayBufferPointer_Word[ 4 ] = (UINT32)temp + 3 * 4096;                       
    spTempqTD->bPID = Type;             

    /* 执行传输并等待返回结果 */
    return( USBHOST_Send_qTD( Device, spTempqTD, pIntqHD, pSize, 1 ) );    
}  

/*******************************************************************************
* Function Name  : USBHOST_Issue_qTD
* Description    : USB主机发起一个QTD
*                  该函数仅仅发起qTD,不等待执行完毕,也不撤销
* Input          : *Device-------当前操作的设备
                   *spHeadqTD----当前要执行的qTD
*                  *spTempqHD----当前挂载的qHD
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 USBHOST_Issue_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD )
{
    qTD_Structure *spOldDumyqTD;   

    spOldDumyqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 已有的TD,当前TD */    
    memcpy( spOldDumyqTD, spHeadqTD, EHCI_QTD_SIZE );                           /* 复制到当前TD中供操作 */   
    spOldDumyqTD->bNextQTDPointer = ( (UINT32)spHeadqTD ) >> 5;                 /* 将新TD链接上去作为备用,在处理完后从该指针离开 */
    spOldDumyqTD->bTerminate = 0;   
    spOldDumyqTD->bStatus_Active = 1;                                           /* 启动对当前TD的操作,新TD不会被操作,当前TD的数据被复制到QH中成为操作副本再处理 */
    spOldDumyqTD->bAlternateQTDPointer = ( (UINT32)spHeadqTD ) >> 5;            /* 处理完短包后优先从该指针离开 */
    spOldDumyqTD->bAlternateTerminate = 0;
    Device->psSendLastqTD = spOldDumyqTD;
    Device->bSendStatusError = 0; 
    return( (UINT32)spOldDumyqTD );
}

/*******************************************************************************
* Function Name  : USBHOST_Check_qTD
* Description    : USB主机查询QTD是否执行完毕
* Input          : *Device-------当前操作的设备
*                  *spHeadqTD----当前要查询的qTD
*                  *spTempqHD----当前挂载的qHD
*                  *pRetLen------输入期望获取长度,成功返回实际获取长度
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Check_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD, UINT16 *pRetLen )
{
    qTD_Structure *spOldDumyqTD; 
    qTD_Structure *spReleaseqTD; 

       spOldDumyqTD = ( qTD_Structure *)spHeadqTD;
    if( spOldDumyqTD->bStatus_Active == 0x00 ) 
    {
        if( pRetLen ) 
        {
            *pRetLen = *pRetLen - spOldDumyqTD->bTotalBytes;                    /* 保存长度 */
        } 

        /* 执行成功后的处理 */
        gEHCILastStatus = spTempqHD->bOverlay_Status;                           /* 保存上次清空累计来的执行状态,这是当前操作副本,如果不是超时建议使用spOldDumyqTD的状态 */
        Device->bSendStatusError = gEHCILastStatus;                             /* 暂存当前qHD执行状态,并判断是否有错误发生 */
        spTempqHD->bOverlay_Status = 0;                                         /* 清空以便再次累计错误 */
        spReleaseqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 超时停在当前TD,完成则停在下一个TD即备用的新TD */
        spReleaseqTD = spReleaseqTD == spOldDumyqTD ? spHeadqTD : spOldDumyqTD; /* 超时释放新TD,完成则释放当前TD */
        USBHOST_ReleaseStructure( EHCI_MEM_TYPE_QTD, ( UINT32 )spReleaseqTD );  /* 回收qTD */
        return( USB_OPERATE_SUCCESS );
    }
    return( USB_OPERATE_ERROR );
}

/*******************************************************************************
* Function Name  : USBHOST_Remove_qTD
* Description    : USB主机强制撤销当前QTD
* Input          : *Device-------当前操作的设备
                   *spHeadqTD----当前要查询的qTD
*                  *spTempqHD----当前挂载的qHD
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Remove_qTD( ATTACH_DEVICE_INFO *Device, qTD_Structure *spHeadqTD, qHD_Structure *spTempqHD )
{
    qTD_Structure *spOldDumyqTD; 
    qTD_Structure *spReleaseqTD;   

    spOldDumyqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 已有的TD,当前TD */
    USBHOST_Asynchronous_Setting( USBHOST_OP_DISABLE );                         /* 停止异步传输 */
    spOldDumyqTD->bStatus_Active = 0;                                           /* 停止QTD */        
    spOldDumyqTD->bTerminate = 1;                                               /* 断开链表 */
    spOldDumyqTD->bAlternateTerminate = 1;
    spTempqHD->bOverlay_Status &= 0x7F;                                         /* 停止操作副本 */
    Delay_us( 100 );                                                            /* 等待完成 */
    gEHCILastStatus = spTempqHD->bOverlay_Status;                               /* 保存上次清空累计来的执行状态,这是当前操作副本,如果不是超时建议使用spOldDumyqTD的状态 */
    Device->bSendStatusError = gEHCILastStatus;                                 /* 暂存当前qHD执行状态,并判断是否有错误发生 */
    spTempqHD->bOverlay_Status = 0;                                             /* 清空以便再次累计错误 */
    spReleaseqTD = ( qTD_Structure *)( ( (UINT32)( spTempqHD->bOverlay_NextqTD ) ) << 5 );  /* 超时停在当前TD,完成则停在下一个TD即备用的新TD */
    spReleaseqTD = spReleaseqTD == spOldDumyqTD ? spHeadqTD : spOldDumyqTD;     /* 超时释放新TD,完成则释放当前TD */
    USBHOST_ReleaseStructure( EHCI_MEM_TYPE_QTD, ( UINT32 )spReleaseqTD );      /* 回收qTD */

    USBHOST_Asynchronous_Setting( USBHOST_OP_ENABLE );
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_GetDeviceDescr
* Description    : USB主机获取设备描述符
* Input          : *Device---当前操作的设备
                   *buf------数据缓冲区
                   *len------数据长度                   
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_EHCI_GetDeviceDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x80;
    Ctl_Setup->bRequest = 0x06;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x01;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = *len;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 先获取设备描述符的前8个字节 */    
    l = *len;
    status = USBHOST_Issue_Control( Device, Ctl_Setup, buf, &l  );              /* 执行控制传输 */    
    if( status == USB_OPERATE_SUCCESS )
    {
        Device->bEp0MaxPks = ( (PUSB_DEV_DESCR)buf ) -> bMaxPacketSize0;        /* 保存端点0的最大包大小 */
           
        /* 重新分配端点0的最大包大小 */
        Device->qHD_Control->bMaxPacketSize = Device->bEp0MaxPks;               /* Bit26---16 */ 
        *len = l;                                                               /* 实际获取长度 */    
    }
    return( status );
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_GetConfigDescr
* Description    : USB主机获取配置描述符
* Input          : *Device---当前操作的设备
                   *buf------数据缓冲区
                   *len------数据长度
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_EHCI_GetConfigDescr( ATTACH_DEVICE_INFO *Device, UINT8 *buf, UINT16 *len ) 
{
    UINT16 l;
    UINT8  status;
    
    /* 填充SETUP命令包 */
    Ctl_Setup->bReqType = 0x80;
    Ctl_Setup->bRequest = 0x06;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x02;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x04;
    Ctl_Setup->wLengthH = 0x00;

    /* 先获取配置描述符的前4个字节,以便确定实际总长度 */        
    l = Ctl_Setup->wLengthL;
    status = USBHOST_Issue_Control( Device, Ctl_Setup, buf, &l  );              /* 执行控制传输 */    
    if( status == USB_OPERATE_SUCCESS )
    {
        /* 再以实际总长度获取全部配置描述符 */        
        Ctl_Setup->wLengthL = ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL;
        Ctl_Setup->wLengthH = ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthH;
        l = ( ( ( UINT16 )Ctl_Setup->wLengthH << 8 ) ) + Ctl_Setup->wLengthL;
        status = USBHOST_Issue_Control( Device, Ctl_Setup, buf, &l  );          /* 执行控制传输 */
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
* Function Name  : USBHOST_EHCI_SetAddress
* Description    : USB主机设置地址
* Input          : *Device---当前操作的设备
                   addr------要设置的地址
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_EHCI_SetAddress( ATTACH_DEVICE_INFO *Device, UINT8 addr ) 
{
    UINT8  status;
            
    /* 填充SETUP命令包 */        
    Ctl_Setup->bReqType = 0x00;
    Ctl_Setup->bRequest = 0x05;
    Ctl_Setup->wValueL  = Device->bDeviceOnHub ? Device->bOnHubPortNumber + addr + 1 : addr;                                
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;

    /* 设置USB设备地址 */
    status = USBHOST_Issue_Control( Device, Ctl_Setup, NULL, NULL  );           /* 执行控制传输 */        
    if( status == USB_OPERATE_SUCCESS )
    {
        /* 设置自己地址 */       
        Device->bAddress = Device-> bDeviceOnHub ? Device->bOnHubPortNumber + addr + 1 : addr;    /* 保存当前操作的地址值 */
        Device->qHD_Control->bDeviceAddress = Device->bAddress;                 /* 修改端点操作的地址 */
    }    
    Delay_ms( 5 );                                                              /* 等待USB设备完成操作 */    
    return( status );
}
                                   
/*******************************************************************************
* Function Name  : USBHOST_EHCI_SetConfig
* Description    : USB主机设置配置值
* Input          : *Device---当前操作的设备
                   cfg------要设置的配置值
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_EHCI_SetConfig( ATTACH_DEVICE_INFO *Device, UINT8 cfg ) 
{
    UINT8  status;

    /* 填充SETUP命令包 */                
    Ctl_Setup->bReqType = 0x00;
    Ctl_Setup->bRequest = 0x09;
    Ctl_Setup->wValueL  = cfg;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 设置USB设备配置 */    
    status = USBHOST_Issue_Control( Device, Ctl_Setup, NULL, NULL  );           /* 执行控制传输 */
    return( status );
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_ClearEndpStall
* Description    : USB主机清除端点
* Input          : *Device---当前操作的设备
                   endp------要清除的端点号
* Output         : None
* Return         : 返回当前命令执行状态
*******************************************************************************/
UINT8 USBHOST_EHCI_ClearEndpStall( ATTACH_DEVICE_INFO *Device, UINT8 endp )
{
    UINT8  status;

    /* 填充SETUP命令包 */                
    Ctl_Setup->bReqType = 0x02;
    Ctl_Setup->bRequest = 0x01;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = endp;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* 设置USB设备配置 */    
    status = USBHOST_Issue_Control( Device, Ctl_Setup, NULL, NULL  );           /* 执行控制传输 */    
    return( status );
}

/*******************************************************************************
* Function Name  : USBHOST_CofDescrAnalyse
* Description    : USB主机分析配置描述符
*                  此处只是简单判断设备类型,具体需要根据设备类型进行细一步分析
* Input          : *pbuf---描述符缓冲区 
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_CofDescrAnalyse( UINT8 *pbuf )
{
    gDeviceClassType = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass;
    if( ( gDeviceClassType <= 0x09 ) || ( gDeviceClassType == 0xFF ) )    
    {
        return( USB_OPERATE_SUCCESS ); 
    }
    else
    {
        gDeviceClassType = USB_DEV_CLASS_RESERVED;                              /* 当前设备类型默认未知 */     
        return( ERR_USB_UNKNOWN );                                              /* 不支持,返回  */
    }            
}

/*******************************************************************************
* Function Name  : USBHOST_EHCI_Enumerate
* Description    : USB主机对设备进行枚举
*                  枚举流程:
                   (1)、以最大包获取设备描述符;
                   (2)、复位USB设备;
                   (3)、设置USB地址;
                   (4)、以实际长度获取设备描述符;
                   (5)、获取4个字节的配置描述符;
                   (6)、以实际长度获取配置描述符;
                   (7)、分析描述符;
                   (8)、设置配置值
* Input          : *Device---当前操作的设备
                   *pbuf-----数据缓冲区
* Output         : None
* Return         : USB_OPERATE_SUCCESS---设备枚举成功;
*                  USB_OPERATE_ERROR-----设备枚举失败;
*******************************************************************************/
UINT8 USBHOST_EHCI_Enumerate( ATTACH_DEVICE_INFO *Device, UINT8 *pbuf )
{
    UINT8  status;
    UINT16 i, len;    

#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_Enumerate:..\n" );
#endif
    
    /* (1)、以最大包获取设备描述符 */
#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_GetDeviceDescr:...\n" );
#endif
    len = DEFAULT_USB_EP0_SIZE;
    status = USBHOST_EHCI_GetDeviceDescr( Device, pbuf, &len  );                /* 获取设备描述符 */
    if( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF 
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }

#ifdef  MY_DEBUG_PRINTF
    for( i = 0; i < len; i ++ ) 
    {
        printf( "%02X ", (UINT16)*( pbuf + i ) );
    }
    printf( "\n" );
#endif
    Delay_ms( 3 );

    /* (2)、复位USB设备*/
    USBHOST_EHCI_BusReset( Device );

    /* 检测等待设备重新连接 */
    for( i = 0; i < 10; i ++ )
    {
        Delay_ms( 50 );
        if( USBHOST_EHCI_Detect_Connect( Device ) == USB_INT_CONNECT )          /* 检测设备连接 */
        {
            break;
        }
    }
    
    /* (3)、设置USB地址 */
#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_SetAddress:...\n " );
#endif
    status = USBHOST_EHCI_SetAddress( Device, DEFAULT_USB_OP_ADDR );            /* 设置USB设备地址 */
    if( status != USB_OPERATE_SUCCESS )
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }
    Delay_ms( 3 );

    /* (4)、以实际长度获取设备描述符 */
#ifdef  MY_DEBUG_PRINTF 
    printf( "USBHOST_GetDeviceDescr:...\n" );
#endif
    len = 0x12;
    status = USBHOST_EHCI_GetDeviceDescr( Device, pbuf, &len  );                /* 获取设备描述符 */
    if( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF 
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }
#ifdef  MY_DEBUG_PRINTF
    for( i = 0; i < len; i ++ ) 
    {
        printf( "%02X ", (UINT16)*( pbuf + i ) );
    }
    printf( "\n" );
#endif
    Delay_ms( 3 );    
    
    /* (5)、获取4个字节的配置描述符 */
    /* (6)、以实际长度获取配置描述符 */
#ifdef  MY_DEBUG_PRINTF
    printf( "USBHOST_GetConfigDescr:...\n " );
#endif
    status = USBHOST_EHCI_GetConfigDescr( Device, pbuf, &len );                 /* 获取配置描述符 */
    if( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }
#ifdef  MY_DEBUG_PRINTF
    for ( i = 0; i < ( (PUSB_CFG_DESCR)pbuf ) -> wTotalLengthL + ( (UINT16)( (PUSB_CFG_DESCR)pbuf ) -> wTotalLengthH << 8 ); i ++ ) 
    {   
        printf( "%02X ", (UINT16)*( pbuf + i ) );
    }
    printf( "\n" );
#endif
    Delay_ms( 3 );
    
    /* (7)、分析描述符 */    
#ifdef  MY_DEBUG_PRINTF
    printf( "USBHOST_CofDescrAnalyse:...\n " );
#endif    
    status = USBHOST_CofDescrAnalyse( pbuf );                                   /* 分析配置描述符 */
    if( status != USB_OPERATE_SUCCESS )    
    {    
#ifdef  MY_DEBUG_PRINTF
        printf( "Current Device is Not USB Mass Storage Device\n" );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }
            
    /* (8)、设置配置值 */    
#ifdef  MY_DEBUG_PRINTF
    printf( "USBHOST_SetConfig:...\n " );
#endif
    status = USBHOST_EHCI_SetConfig( Device, ( (PUSB_CFG_DESCR)pbuf ) -> bConfigurationValue ); /* 设置USB设备配置 */
    if ( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        return( USB_OPERATE_ERROR );                                            /* 终止操作,直接返回 */
    }
    Delay_ms( 3 );    
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : USBHOST_QH_Link_Insert
* Description    : 将当前qHD链接到链表中
* Input          : CurrentQH---当前要链接到链表中去的QH
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_qHD_Link_Insert( qHD_Structure *CurrentqHD )
{
    UINT8  i, flag;
    UINT32 temp;
    qHD_Structure *FirstqHD;
    qHD_Structure *LastqHD;

    FirstqHD = (qHD_Structure*)gEHCI_QHD_BaseAddr;                              /* 指向第一个qHD表 */

    /* 通过查询链表结构找到最后一个qHD */    
    flag = 0x00;     
    LastqHD = (qHD_Structure*)gEHCI_QHD_BaseAddr;    
    for( i = 0; i < EHCI_QHD_MAX; i++ )                                            
    {
        temp = ( UINT32 )( LastqHD->bNextQHDPointer ) << 5;                     /* 链表中当前qHD指向的下一个qHD起始地址 */
        if( ( temp == gEHCI_QHD_BaseAddr ) || ( temp == 0 ) )                    
        {    
            flag = 0x01;
            break;
        }
        else
        {
            LastqHD = (qHD_Structure*)temp;
        }                
    }

    if( flag )
    {
        CurrentqHD->bNextQHDPointer = ( ( ( UINT32)FirstqHD ) >> 5 );           /* 将当前qHD链向第一个qHD */        
        LastqHD->bNextQHDPointer = ( ( (UINT32)CurrentqHD ) >> 5 );             /* 将最后一个qHD链向当前qHD */
    }
    else
    {
        return( USB_OPERATE_ERROR );            
    }
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : USBHOST_QH_Link_BackOut
* Description    : 将当前QH从链表中摘除
* Input          : CurrentQH---当前要链接到链表中去的QH
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_QH_Link_BackOut( qHD_Structure *CurrentQH )
{
    UINT8  i;
    UINT32 temp;
    UINT32 address;
    qHD_Structure *LastqHD;

    USBHOST_Asynchronous_Setting( USBHOST_OP_DISABLE );                         /* 挂起异步传输队列 */
    Delay_ms( 3 );

    /* 通过查询链表结构找到指向该qHD的上一个qHD */        
    LastqHD = (qHD_Structure*)gEHCI_QHD_BaseAddr;    
    address = ( UINT32 )CurrentQH;
    for( i = 0; i < EHCI_QHD_MAX; i++ )                                            
    {
        temp = ( UINT32 )( LastqHD->bNextQHDPointer ) << 5;                     /* 链表中当前qHD指向的下一个qHD起始地址 */
        if( temp == address )                    
        {        
            LastqHD->bNextQHDPointer = ( UINT32 )( CurrentQH->bNextQHDPointer );
            break;
        }
        else
        {
            LastqHD = (qHD_Structure*)temp;
        }                
    }
    USBHOST_Asynchronous_Setting( USBHOST_OP_ENABLE );                          /* 重新使能异步传输队列 */
    Delay_ms( 3 );
    return( USB_OPERATE_SUCCESS );
}
      
/*******************************************************************************
* Function Name  : USBHOST_Release_QHD
* Description    : 释放当前QHD(包括从链表摘除、回收QH下挂载的TD、以及回收QH)
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBHOST_Release_QHD( qHD_Structure *CurrentQH )
{
    UINT8  status;

    /* 将当前QH从QH链表中摘除 */
    status = USBHOST_QH_Link_BackOut( CurrentQH );
    if( status )
    {
        return( status );
    }

    /* 回收当前QH下挂载的TD相关内存 */
    status = USBHOST_ReleaseStructure( EHCI_MEM_TYPE_QTD, ( UINT32 )( CurrentQH->bOverlay_NextqTD << 5 ) );
    if( status )
    {
        return( status );
    }

    /* 回收当前QH相关内存 */
    status = USBHOST_ReleaseStructure( EHCI_MEM_TYPE_QHD, ( UINT32 )CurrentQH );
    return( status );
}
