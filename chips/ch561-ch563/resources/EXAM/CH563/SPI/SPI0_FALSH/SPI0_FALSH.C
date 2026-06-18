/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0_FALSH.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_FALSH DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序主要是使用SPI0在方式0 下读写FLASH.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                     1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }         /* LED 高电平为输出方向 */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                            /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                            /* LED 高电平关闭LED显示 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )   
{
    while(1);
}
 
/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void FIQ_Handler( void )
{
    while(1);
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
* Function Name  : SPI_MASTER_INIT
* Description    : SPI主机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI_MASTER_INIT ( void )
{
  R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE|RB_SPI_SCK_OE;                              /* MOSI,SCK输出使能，主机模式，方式0 */
  R8_SPI0_CLOCK_DIV = 0x0a;                                                     /* 10分频。100/10=10M */
  R32_PB_DIR |= (MOSI | SCK0 | SCS);                                            /* MOSI(PB14),SCK0(PA13),SCS(PB12)为输出 */
  R32_PB_PU  |=  SCS ; 
  R8_SPI0_CTRL_DMA = 0;
}

/*******************************************************************************
* Function Name  : SPI0_Trans
* Description    : 发送一字节数据
* Input          : data -要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_Trans( UINT8 data )
{
    R32_SPI0_FIFO = data;
    R16_SPI0_TOTAL_CNT = 0x01;
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* 等待数据发送完成 */
}

/*******************************************************************************
* Function Name  : SPI0_Trans
* Description    : 接收一字节数据
* Input          : None
* Output         : None
* Return         : data -接收到的数据
*******************************************************************************/

UINT8 SPI0_Recv( void )
{
    UINT8 data;
    R32_SPI0_FIFO = 0xff;
    R16_SPI0_TOTAL_CNT = 0x01;
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* 等待数据回来 */
    data = R8_SPI0_BUFFER;
    return data;
}

/*******************************************************************************
* Function Name  : Read_Status_Register
* Description    : 用来读取状态寄存器,并返回状态寄存器的值
* Input          : None
* Output         : None
* Return         : byte -寄存器状态值
*******************************************************************************/

UINT8 Read_Status_Register( void )   
{   
    UINT8 byte = 0;
    R32_PB_CLR |=  SCS ;                                                        /* 使能设备 */   
    SPI0_Trans(0x05);                                                           /* 发送读状态寄存器的命令 */   
    byte = SPI0_Recv( );                                                        /* 读取状态寄存器 */  
    R32_PB_OUT |=  SCS ;                                                        /* 禁止设备 */   
    return byte;   
}
   
/*******************************************************************************
* Function Name  : Wait_Busy
* Description    : 等待芯片空闲(在执行Byte-Program, Sector-Erase, Block-Erase, Chip-Erase操作后)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Wait_Busy( void )   
{   
    while ((Read_Status_Register())&0x01 == 0x01 )                              /* waste time until not busy */   
          Read_Status_Register( );   
}

/*******************************************************************************
* Function Name  : WRSR
* Description    : 往状态寄存器里写一个字节
* Input          : byte -写入的数据
* Output         : None
* Return         : None
*******************************************************************************/

void WRSR( UINT8 byte )   
{   
    R32_PB_CLR |=  SCS ;                                                        /* 使能设备 */  
    SPI0_Trans(0x01);                                                           /* 发送写状态寄存器 */   
    SPI0_Trans(byte);                                                           /* 改变寄存器里BPx或者BPL (只有2,3,4,5,7位可以改写) */   
    R32_PB_OUT |=  SCS ;                                                        /* 禁止设备 */   
}
 
/*******************************************************************************
* Function Name  : WREN
* Description    : 写使能,同样可以用于使能写状态寄存器 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void WREN( void )   
{   
   R32_PB_CLR |=  SCS ;         
   SPI0_Trans(0x06);                                                            /* 发送WREN命令 */  
   R32_PB_OUT |=  SCS ;             
}

/*******************************************************************************
* Function Name  : WREN_Check
* Description    : 检查擦写操作前WEL位是否为1 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void WREN_Check( void )   
{   
    UINT8 byte;   
    byte = Read_Status_Register( );                                             /* 读取状态register */  
    if ((byte&0x02) != 0x02)                                                    /* 检查WEL位置位 */  
    {   
        WREN( );                                                                /* 如果未置1进行相应处理,如对其进行写使得操作 */
    }   
}
 
/*******************************************************************************
* Function Name  : Read
* Description    : 读取一个地址内一个字节的数据.返回读取的数据 
* Input          : Dst -Destination Address 000000H - 1FFFFFH
* Output         : None
* Return         : byte -读取的数据
*******************************************************************************/

UINT8 Read(UINT32 Dst)    
{   
    UINT8 byte = 0;    
    R32_PB_CLR |=  SCS ;                                                        /* enable device */   
    SPI0_Trans(0x03);                                                           /* read command */
    SPI0_Trans(((Dst & 0xFFFFFF) >> 16));                                       /* send 3 address bytes */ 
    SPI0_Trans(((Dst & 0xFFFF) >> 8));
    SPI0_Trans(Dst & 0xFF);
    byte = SPI0_Recv();   
    R32_PB_OUT |=  SCS ;                                                        /* disable device */   
    return byte;                                                                /* return one byte read */
} 

/*******************************************************************************
* Function Name  : Byte_Program
* Description    : 写数据
* Input          : Dst  -Destination Address 000000H - 1FFFFFH
*                  byte -要写入的数据
* Output         : None
* Return         : None
*******************************************************************************/

void Byte_Program(UINT32 Dst, UINT8 byte)
{
    WREN();
    R32_PB_CLR |=  SCS ;                                                        /* 芯片使能 */
    SPI0_Trans(0x02);                                                           /* 发送写操作指令 */
    SPI0_Trans(((Dst & 0xFFFFFF) >> 16));                                       /* 发送3字节地址 */
    SPI0_Trans(((Dst & 0xFFFF) >> 8));
    SPI0_Trans(Dst & 0xFF);
    SPI0_Trans(byte);                                                           /* 发送要写的数据 */
    R32_PB_OUT |=  SCS ;
    Wait_Busy();
}
 
/*******************************************************************************
* Function Name  : Chip_Erase
* Description    : 擦除
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Chip_Erase( void )   
{                          
    WREN_Check();   
    R32_PB_CLR |=  SCS ;            
    SPI0_Trans(0x60);                                                           /* 发送 Chip Erase命令 (60h or C7h) */    
    R32_PB_OUT |=  SCS ;  
    Wait_Busy();   
}
   
/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int main( void ) 
{ 

    UINT8 buf[1024];
    UINT32 i;

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 串口1初始化 */
    PRINT("START SPI FLASH\n");
    SPI_MASTER_INIT ( );                                                        /* SPI0主机模式初始化 */
    WREN( );                                                                    /* FLASH写使能 */
    WRSR(0x00);                                                                 /* 写寄存器 */
    Chip_Erase( );                                                              /* FLASH整片擦除 */
    PRINT("Chip_Erase over\n");
    for(i=0;i<1024;i++){
        Byte_Program(i,i%256);                                                  /* 在FLASH地址0x00000001写入数据 */
    }
    PRINT("Write over\n");
    for(i=0;i<1024;i++){
        if(i%16 == 0) PRINT("\n");
        buf[i] = Read(i);                                                       /* 从FLASH地址0x00000001读出数据 */
        PRINT("%-8x", buf[i]);
    }
    while(1);
}

/*********************************** endfile **********************************/
