/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Examples
*                      (1)、CH563 USB Examples by KEIL；    
*                      (2)、串口0输出监控信息,115200bps；
*                      (3)、USB HOST以查询方式处理；
*                      (4)、本程序功能:操作U盘,进行物理扇区读写； 
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
#include "Udisk.H"                                                              /* Udisk定义相关头文件 */
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */

/******************************************************************************/
/* 常、变量定义 */
UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                               /* 内存分配缓冲区(由于QH、QTD等结构提需要32字节对齐, 仅用来分配QH、QTD相关内存使用) */
__align( 4096 ) UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                       /* 内存数据缓冲区(需要4K对齐) */                                
__align( 4096 ) UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];          /* 周期性帧列表缓冲区(需要4K对齐) */                                
/* 定义U盘相关结构体 */
BULK_ONLY_CMD    mBOC;                                                          /* BulkOnly传输结构定义 */
DISK_INFO        UDisk;                                                         /* U盘相关结构体定义 */                                                        
DISK_INFO        HUB_UDisk[ 4 ];                                                /* HUB下U盘相关结构体定义 */
/******************************************************************************/
/* 函数声明 */
extern UINT8 Disk_Test( DISK_INFO *Diskinfo );                                  /* U盘测试主函数 */

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
* Function Name  : Disk_Test
* Description    : U盘测试主函数
* Input          : *Diskinfo----当前操作的U盘相关结构体
* Output         : None
* Return         : None
*******************************************************************************/

UINT8 Disk_Test( DISK_INFO *Diskinfo )
{
    UINT8  status;
    UINT32 sector;
    
#ifdef  MY_DEBUG_PRINTF            
    printf("Disk_Test......\n");        
#endif
    /* 对U盘描述符进行分析 */
    MS_CofDescrAnalyse( Diskinfo, EHCI_Data_Buf );                              /* 对U盘的配置描述符进行分析,提取有用信息 */

    /* 对U盘进行初始化操作 */
    status = MS_Init_EX( Diskinfo, EHCI_Data_Buf );                             /* 大容量存储设备初始化 */
    if( status == USB_OPERATE_SUCCESS )
    {        
        sector = 0x00;
        while( 1 )  
        {
            /* 测试一次读取20K数据 */
            status = MS_ReadSector_EX( Diskinfo, sector, EHCI_DATA_BUF_LEN / 
                Diskinfo->PerSecSize, EHCI_Data_Buf );                          /* 读取N个扇区数据 */                        
            if( status )
            {
                return( status );
            }
            /* 判断是否读完全部扇区,是则从头再读 */
            if( sector <= ( Diskinfo->Capability - 40 ) )
            {

                sector = sector + EHCI_DATA_BUF_LEN / Diskinfo->PerSecSize;                        
            }
            else
            {
                sector = 0x00;    
            }
        }
    }
    return( status );
}    

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int    main( void ) 
{
    UINT8  status;

//    SysFreq( );                                                               /* 根据需要设置PLL获得相应的时钟频率 */
    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */

#ifdef  MY_DEBUG_PRINTF    
    printf( "CH563_HUB_Example_Test\xd\xa" );
#endif

    USBHOST_ModeSet( );                                                         /* 设置USB主机模式 */
    USBHOST_EHCI_Init( );                                                       /* USB主机EHCI复位及初始化 */
    USBHOST_Structure_DeInit( );                                                /* USB主机数据结构初始化 */
    USBHOST_DevicePara_Init( &UDisk.Device );                                   /* USB主机相关参数初始化 */

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
            if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_CONNECT ) /* 检测设备连接 */
            {
                break;
            }
               Delay_ms( 50 );
        }
        Delay_ms( 200 );                                                        /* 延时 */
        if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_DISCONNECT )/* 检测设备连接 */
        {
            goto HOST_WAIT_DEVICE_IN;                                           /* 设备断开跳转到前面继续等待 */
        }

#ifdef  MY_DEBUG_PRINTF            
        printf("USB Device In:\n" ); 
        printf("Device Speed is: %02x\n",(UINT16)UDisk.Device.bSpeed );         /* 0x01:低速设备, 0x00:全速设备, 0x02:高速设备, 0xFF:未知 */            
#endif

        /***********************************************************************/
        /* 对USB设备进行枚举 */    
        status = USBHOST_EHCI_EP0_Init( &UDisk.Device );                        /* 初始化端点0 */
        if( status != USB_OPERATE_SUCCESS )
        {
#ifdef  MY_DEBUG_PRINTF            
            printf("USBHOST_EP0_Init Error\n");
#endif                
            return( status );
        }

        status = USBHOST_EHCI_Enumerate( &UDisk.Device, EHCI_Data_Buf );        /* USB主机对设备进行枚举 */
        if( status == USB_OPERATE_SUCCESS )
        {
            if( gDeviceClassType == USB_DEV_CLASS_STORAGE )                     /* 当前设备为大容量存储设备 */
            {
#ifdef  MY_DEBUG_PRINTF            
                printf("Current USB Device is Mass Storage Device\n");
#endif
                   status = Disk_Test( &UDisk );                                /* U盘测试主函数 */    
            }
        }
        /***********************************************************************/
        /* 测试结束,查询等待设备拔除 */
        while( 1 )
        {
            if( USBHOST_EHCI_Detect_Connect( &UDisk.Device ) == USB_INT_DISCONNECT )
            {
                printf("USB Device Out\n");
                USBHOST_EHCI_Init( );
                USBHOST_DevicePara_Init( &UDisk.Device );                       /* USB主机相关参数初始化 */
                USBHOST_Structure_DeInit( );                                    /* USB主机数据结构初始化 */
#ifdef  MY_DEBUG_PRINTF    
                printf("USB Device Out\n");
#endif
                break;
            }
        }    
    }
}

/*********************************** endfile **********************************/
