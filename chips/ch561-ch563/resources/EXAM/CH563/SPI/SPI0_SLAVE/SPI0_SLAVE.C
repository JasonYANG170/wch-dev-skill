/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0_SLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_SLAVE DEMO
*                      (1)、CH563 Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序主要是SPI做设备接收数据，包含发送和接收子程序.
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
* Function Name  : SPI0_SlaveInit
* Description    : SPI设备模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_SlaveInit( void )
{
    R8_SPI0_CTRL_MOD |= ( RB_SPI_MISO_OE | RB_SPI_MODE_SLAVE );                 /* 模式设置寄存器,设备模式 */
    R8_SPI0_CTRL_MOD |= ( RB_SPI_FIFO_DIR | RB_SPI_ALL_CLEAR );                 /* 清除FIFO,FIFO方向默认设置为输入*/
    R8_SPI0_CTRL_MOD &= ~RB_SPI_ALL_CLEAR ;                                    
    R8_SPI1_CTRL_DMA =  0x00;                                                   /* 不启动DMA方式 */
//    R32_PB_DIR |= MISO;                                                       /* MISO output enable,如果有多个从机设备连接在一起，此引脚不能置为输出 */
}

/*******************************************************************************
* Function Name  : SPI0_SlaveRecv
* Description    : 接收一字节数据  
* Input          : None
* Output         : None
* Return         : R8_SPI0_FIFO -接收到数据
*******************************************************************************/

UINT8 SPI0_SlaveRecv( void )
{
    while( R8_SPI0_FIFO_COUNT == 0 );                                           /* FIFO数据长度寄存器，FIFO缓冲区为32个字节,不为0说明有数据 */
    return R8_SPI0_FIFO;                                                       
}

/*******************************************************************************
* Function Name  : SPI0_SlaveTrans
* Description    : 发送一字节数据  
* Input          : data -待发送数据 
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_SlaveTrans( UINT8 data )
{
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /* FIFO方向设置为输出，准备发送数据 */
    R16_SPI0_TOTAL_CNT = 0x01;                                                  /* 发送数据长度 */
    R8_SPI0_FIFO = data;                    
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* 等待发送完成 */
    R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;                                        /* FIFO方向设置为输入，准备接收数据 */
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
    UINT8    a;

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */ 
    PRINT("Start SPI0 Slave:\n");
    SPI0_SlaveInit( );                                                          /* SPI设备模式初始化 */
    while(1){
        a = SPI0_SlaveRecv();
        SPI0_SlaveTrans( a );
    }      
}

/*********************************** endfile **********************************/
