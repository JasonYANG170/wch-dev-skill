/********************************** (C) COPYRIGHT *******************************
* File Name          : SLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 PARA_SLAVE DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、被动并口演示例子程序.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
UINT16    my_buffer[ 0x2000 ];

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
* Function Name  : ParaSlvQryTest
* Description    : 被动并口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/* 
    硬件连接如下:
    D0-D7    PD0-PD7
    INT        PA9        
    A0        PA10
    CS        PA11
    RD        PA20
    WR        PA21
    */

void ParaSlvQryTest( void )
{
    UINT8 dat;
    
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */
    R8_SLV_CONFIG = (RB_SLV_ENABLE | RB_SLV_IE_CMD);                            /* 使能从模式并口，禁止命令，读写中断 */ 
    R8_SAFE_ACCESS_SIG = 0x00 ;                                                 /* lock, to prevent unexpected writing */

    R32_PA_DIR &= ~SLVA ;                                                       /* A0输入 */
    R32_PA_DIR &= ~SLVCS ;                                                      /* CS输入 */
    R32_PA_DIR &= ~PRD ;                                                        /* RD输入 */
    R32_PA_DIR &= ~PWR ;                                                        /* WR输入 */

    R32_PA_DIR |= SLVI;                                                         /* INT输出 */
    R32_PA_OUT |= SLVI;                                                         /* INT默认为高 */

    while( (R8_INT_FLAG_SLV& RB_IF_SLV_CMD) == 0 );                             /* 等待命令 */
    R8_INT_FLAG_SLV = 0xff;
    PRINT("$$");
    dat = R8_INT_SLV_DIN;
    PRINT("%02x\n",(UINT16)dat);
    if( dat == 0xaa ){
        R8_SLV_STATUS =  0x01;                                                  /* 中断状态 */
        R32_PA_CLR |= SLVI;                                                     /* 把INT脚拉低 */

        while( ( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) ==0 );
        R32_PA_OUT |= SLVI;                                                     /* 把INT脚拉高 */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;
        R8_SLV_DOUT = 0xaa;
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* 等待读中断 */ 
        PRINT("R8_INT_FLAG_SLV=%02x\n",(UINT16)R8_INT_FLAG_SLV);
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */
        PRINT("!!");
    }
    if( dat == 0x55 ){
        R8_SLV_STATUS = 0x80 | 0x02;                                            /* 中断状态 */
        R32_PA_CLR |= SLVI;                                                     /* 把INT脚拉低 */
        R8_INT_FLAG_SLV = 0xff;                                                 /* 清中断 */
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );
        PRINT("@@");
        R8_SLV_DOUT = 0xaa;
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* 等待读中断 */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */ 
    }
#if 0
    while( !( R8_INT_FLAG_SLV & RB_IF_SLV_CMD ) );                              /* 等待命令中断 */
    PRINT( "IF CMD: 0x%x\n", ( UINT16 )R8_INT_SLV_DIN );
    if( R8_INT_SLV_DIN == 0x55 ){
        R8_SLV_STATUS = 0x80 | 0x01;                                            /* 中断状态 */
        PRINT( "    Out Int Sts: 0x%x\n", ( UINT16 )R8_SLV_STATUS );
        PRINT( "    Wait Master Get Int Sts\n" ); 
        R32_PA_OUT &= ~( 1<<9 );                                                /* 产生中断信号 */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* 等待中断状态被取走 */
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */ 
        R32_PA_OUT |= 1<<9;                                                     /* 撤销中断信号 */ 

        R8_SLV_DOUT = 0xAA;                                                     /* 输出数据：0xAA */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* 等待读中断 */ 
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */ 
        PRINT( "    Ok\n" );
    }
    while( !( R8_INT_FLAG_SLV & RB_IF_SLV_CMD ) );                              /* 等待命令中断 */ 
    PRINT( "IF CMD: 0x%x\n", ( UINT16 )R8_INT_SLV_DIN );
    if( R8_INT_SLV_DIN == 0xAA ){
        R8_SLV_STATUS = 0x80 | 0x02;                                            /* 中断状态 */
        PRINT( "    Out Int Sts: 0x%x\n", ( UINT16 )R8_SLV_STATUS );
        PRINT( "    Wait Master Get Int Sts\n" );

        R32_PA_OUT &= ~( 1<<9 );                                                /* 产生中断信号 */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_RD ) );                           /* 等待中断状态被取走 */ 
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */ 
        R32_PA_OUT |= 1<<9;                                                     /* 撤销中断信号 */ 
        while( !( R8_INT_FLAG_SLV & RB_IF_SLV_WR ) );                           /* 等待写中断 */ 
        dat = R8_INT_SLV_DIN;
        R8_INT_FLAG_SLV |= RB_IF_SLV_RD;                                        /* 清中断标志 */ 
        PRINT( "    Ok, dat=0x%x\n", ( UINT16 )dat );
    }
#endif
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
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
    PRINT( "Start Para Slave Test\xd\xa" );
    ParaSlvQryTest( );                                                          /* 被动并口测试 */
    while( 1 );
}

/*********************************** endfile **********************************/
