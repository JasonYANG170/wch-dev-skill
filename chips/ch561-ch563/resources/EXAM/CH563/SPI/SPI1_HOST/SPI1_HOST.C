/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI1_HOST.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_HOST DEMO
*                      (1)、CH563 Examples by KEIL3_V3.53;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、SPI0主机例子程序,查询方式发送和接收数据.
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
* Function Name  : SPI1_MasterInit
* Description    : SPI主机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI1_MasterInit ( void )
{
    R8_SPI1_CTRL_MOD = RB_SPI1_SDO_OE | RB_SPI_SCK_OE;
    R8_SPI1_CLOCK_DIV = 0x0a;                                                   /* 10分频。100/10=10M */
    R32_PB_DIR |= (SDO | SCK1 | ADCS );                                         /* SDO, SCK1，ADCS为输出 */
    R32_PB_PU  |=  ADCS ;
    R8_SPI1_CTRL_DMA = 0x00;                                                    /* 不启动DMA方式 */
}

/*******************************************************************************
* Function Name  : SPI1_MasterTrans
* Description    : 发送一字节数据
* Input          : data -待发送数据 
* Output         : None
* Return         : None
*******************************************************************************/

void SPI1_MasterTrans( UINT8 data )
{
    R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /* 设置方向为输出 */
    R32_SPI1_FIFO = data;
    R16_SPI1_TOTAL_CNT = 1;
    while( R8_SPI1_FIFO_COUNT != 0 );                                           /* 等待数据发送完成 */
}

/*******************************************************************************
* Function Name  : SPI1_MasterRecv
* Description    : 接收一字节数据
* Input          : None
* Output         : None
* Return         : data -接收到数据
*******************************************************************************/

UINT8 SPI1_MasterRecv( void )
{
    R8_SPI1_CTRL_MOD |= RB_SPI_FIFO_DIR;                                        /* 设置方向为输入 */
    R16_SPI1_TOTAL_CNT = 1;
    while( R8_SPI1_FIFO_COUNT == 0 );                                           /* 等待数据回来 */
    return R8_SPI1_FIFO;
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
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 串口0初始化 */
    PRINT("SPI1 HOST DEMO......\n");  
    SPI1_MasterInit( );                                                         /* 主机模式初始化 */
    while(1){
        R32_PB_CLR |= ADCS;                                                     /* 片选使能 */
        SPI1_MasterTrans(0xaa);                                                 /* 发送数据 */
        R32_PB_OUT |= ADCS;             
        Delay_us( 10 )    ;
    }
}

/*********************************** endfile **********************************/
