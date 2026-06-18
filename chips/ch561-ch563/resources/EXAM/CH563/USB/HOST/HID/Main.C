/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Examples
*                      (1)、CH563 USB Examples by KEIL；    
*                      (2)、串口0输出监控信息,115200bps；
*                      (3)、USB HOST以查询方式处理；
*                      (4)、本程序功能:操作HID类键盘鼠标设备； 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "absacc.h"
#include "SYSFREQ.H"                                                            /* 系统定义相关头文件 */
#include "CH563SFR.H"
#include "EHCI_HCD.H"                                                           /* 主机控制器低层操作相关头文件 */
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */
#include "HID.H"                                                                /* HID定义相关头文件 */
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */

/******************************************************************************/
/* 常、变量定义 */
UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                               /* 内存分配缓冲区(由于QH、QTD等结构提需要32字节对齐, 仅用来分配QH、QTD相关内存使用) */
__align( 4096 ) UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                       /* 内存数据缓冲区(需要4K对齐) */                                
__align( 4096 ) UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];          /* 周期性帧列表缓冲区(需要4K对齐) */                                
/* 定义HID类相关结构体 */
HID_INFO        Hid;                                                            /* HID相关结构体定义 */    
Periodic_Frame_List_Structure  *pHID_FramList;                                  /* 周期性帧列表结构体 */

/******************************************************************************/
/* 函数声明 */
extern UINT8 Hid_Test( HID_INFO *Hidinfo );                                     /* HID测试主函数 */        

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : 为printf和getkey输入输出初始化串口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void mInitSTDIO( void )
{
    UINT32    x, x2;

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        /* 115200bps */
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        /* 四舍五入 */
    R8_UART0_LCR = 0x80;                                                        /* DLAB位置1 */
    R8_UART0_DIV = 1;                                                           /* 预分频 */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : 通过串口输出监控信息
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
    UINT8  status;

    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */

#ifdef  MY_DEBUG_PRINTF    
    printf( "CH563_HUB_Example_Text\n" );
#endif

    USBHOST_ModeSet( );                                                         /* 设置USB主机模式 */
    USBHOST_EHCI_Init( );                                                       /* USB主机EHCI复位及初始化 */
    USBHOST_Structure_DeInit( );                                                /* USB主机数据结构初始化 */
    USBHOST_DevicePara_Init( &Hid.Device );                                     /* USB主机相关参数初始化 */

    while( 1 )
    {                
        /***********************************************************************/
        /* 查询等待设备插入 */
#ifdef  MY_DEBUG_PRINTF 
        printf("Wait USB Device In:\n" );
#endif
HOST_WAIT_DEVICE_IN:
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_CONNECT ) /* 检测设备连接 */
            {
                break;
            }
               Delay_ms( 50 );
        }
        Delay_ms( 200 );                                                        /* 延时 */
        if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_DISCONNECT )  /* 检测设备连接 */
        {
            goto HOST_WAIT_DEVICE_IN;                                           /* 设备断开跳转到前面继续等待 */
        }

#ifdef  MY_DEBUG_PRINTF            
        printf("USB Device In:\n" ); 
        printf("Device Speed is: %02x\n",(UINT16)Hid.Device.bSpeed );           /* 0x01:低速设备, 0x00:全速设备, 0x02:高速设备, 0xFF:未知 */            
#endif

        /**********************************************************************/
        /* 对USB设备进行枚举 */    
        status = USBHOST_EHCI_EP0_Init( &Hid.Device );                          /* 初始化端点0 */
        if( status != USB_OPERATE_SUCCESS )
        {
#ifdef  MY_DEBUG_PRINTF            
            printf("USBHOST_EP0_Init Error\n");
#endif                
            return( status );
        }

        status = USBHOST_EHCI_Enumerate( &Hid.Device, EHCI_Data_Buf );          /* USB主机对设备进行枚举 */
        if( status == USB_OPERATE_SUCCESS )
        {
            if( gDeviceClassType == USB_DEV_CLASS_HUMAN_IF )                    /* 当前设备为HID类设备 */
            {
#ifdef  MY_DEBUG_PRINTF    
                printf("Current USB Device is HID Device\n");                
#endif
                   status = Hid_Test( &Hid );                                   /* HID键盘鼠标测试主函数 */    
            }    
        }

        /**********************************************************************/
        /* 测试结束,查询等待设备拔除 */
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &Hid.Device ) == USB_INT_DISCONNECT )
            {
                printf("USB Device Out\n");
                USBHOST_EHCI_Init( );
                USBHOST_DevicePara_Init( &Hid.Device );                         /* USB主机相关参数初始化 */
                USBHOST_Structure_DeInit( );                                    /* USB主机数据结构初始化 */
#ifdef  MY_DEBUG_PRINTF    
                printf("USB Device Out\n");
#endif
                break;
            }
        }    
    }
}

/*******************************************************************************
* Function Name  : Hid_Test
* Description    : HID测试主函数
* Input          : *Hidinfo----当前操作的HID类设备相关结构体
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 Hid_Test( HID_INFO *Hidinfo )
{
    UINT8  status;
    UINT16 i, len;
    UINT8  buf1[ 8 ];
    UINT8  buf2[ 8 ];
        
#ifdef  MY_DEBUG_PRINTF            
    printf("Hid_Test......\n");        
#endif
    
    /* 对U盘描述符进行分析 */
    HID_CofDescrAnalyse( Hidinfo, EHCI_Data_Buf );                              /* 对U盘的配置描述符进行分析,提取有用信息 */

    /* 对HID类设备进行类请求操作 */
    status = HID_ClassRequest_Deal( Hidinfo, EHCI_Data_Buf );
    if( status )
    {
        return( status );
    }

    /* 禁止周期性传输 */    
    USBHOST_Periodic_Setting( USBHOST_OP_DISABLE );

    /* 使能周期性帧链表全部无效 */
       pHID_FramList = ( Periodic_Frame_List_Structure *)EHCI_PERIODIC_LIST_Buf;
    for( i = 0; i < EHCI_P_FRAME_LIST_MAX; i = i + 1 ) 
    {
        pHID_FramList->sCell[ i ].bTerminal = 0x01;
    }
    
    /* 将中断端点1相关QHD链接到周期性帧链表中，并且执行周期位8mS */
    for( i = 0; i < EHCI_P_FRAME_LIST_MAX; i = i + 8 ) 
    {
        pHID_FramList->sCell[ i ].bLinkPointer = ((UINT32)( Hidinfo->qHD_INT_In1 ) ) >> 5;  
        pHID_FramList->sCell[ i ].bTerminal = 0;
        pHID_FramList->sCell[ i ].bType = EHCI_HD_TYPE_QH;      
    }

    /* 将中断端点2相关QHD链接到周期性帧链表中，并且执行周期位8mS */
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
        for( i = 1; i < EHCI_P_FRAME_LIST_MAX; i = i + 8 ) 
        {
            pHID_FramList->sCell[ i ].bLinkPointer = ((UINT32)( Hidinfo->qHD_INT_In2 ) ) >> 5;  
            pHID_FramList->sCell[ i ].bTerminal = 0;
            pHID_FramList->sCell[ i ].bType = EHCI_HD_TYPE_QH;      
        }
    }

    /* 使能周期性传输 */    
    USBHOST_Periodic_Setting( USBHOST_OP_ENABLE );

    /* 获取键盘鼠标数据 */
#ifdef  MY_DEBUG_PRINTF    
    printf("Start Get HID Device Data\n ");
#endif

    /* 获取第一个中断端点数据包 */        
    HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In1, buf1, Hidinfo->IntInEp1Size );

    /* 获取第二个中断端点数据包 */    
    if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
    {
           HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In2, buf2, Hidinfo->IntInEp2Size );
    }

    while( 1 )
    {
        /* 查询第一个中断端点是否有数据包 */
        len = Hidinfo->IntInEp1Size;
        status = HID_Check_Data( Hidinfo, Hidinfo->qHD_INT_In1, &len );
        if( status == USB_OPERATE_SUCCESS )
        {
            /* 处理获取数据包 */
            for( i = 0x00; i < len; i++ )
            {
                printf("%02x ",(UINT16)buf1[ i ]);
            }
            printf("\n");
        
            /* 重新获取第一个中断端点数据包 */
               HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In1, buf1, Hidinfo->IntInEp1Size );
        }

        /* 查询第二个中断端点是否有数据包 */
        if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
        {                
            len = Hidinfo->IntInEp2Size;
            status = HID_Check_Data( Hidinfo, Hidinfo->qHD_INT_In2, &len );
            if( status == USB_OPERATE_SUCCESS )
            {
                /* 处理获取数据包 */
                for( i = 0x00; i < len; i++ )
                {
                    printf("%02x ",(UINT16)buf2[ i ]);
                }
                printf("\n");
            
                /* 重新获取第二个中断端点数据包 */
                   HID_Issue_GetData( Hidinfo, Hidinfo->qHD_INT_In2, buf2, Hidinfo->IntInEp2Size );
            }
        }

        /* 查询设备连接 */
        if( USBHOST_EHCI_Detect_Connect( &Hidinfo->Device ) == USB_INT_DISCONNECT )
        {
            /* 发起强制撤销该端点获取数据请求 */
            HID_Stop_GetData( Hidinfo, Hidinfo->qHD_INT_In1 );        
            if( ( Hidinfo->IntfacNum > 1 ) && ( Hidinfo->IntInEp2 != 0x00 ) )
            {
                HID_Stop_GetData( Hidinfo, Hidinfo->qHD_INT_In2 );    
            }
            return( USB_INT_DISCONNECT );
        }                    
    }
}

/*********************************** endfile **********************************/

