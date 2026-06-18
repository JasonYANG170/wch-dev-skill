/********************************** (C) COPYRIGHT *******************************
* File Name          : HOST.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563 PARA_HOST DEMO
*                      (1)、CH563 Examples by KEIL3_V3.53;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、主动并口演示程序,主要实现对外部RAM的操作, 对主动并口的HOLD位以及数据建立进行修改调整 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
 
/******************************************************************************/
/* 变量定义 */
UINT16 my_buffer[ 0x2000 ];
PUINT8V pXbusPt;

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
* Function Name  : XbusTest
* Description    : 并口操作ram
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void XbusTest( void )
{
    UINT8   mFstDat, mTstDat;
    UINT8   mHoldClk;                                                           /* hold clk */
    UINT32    i, j;

    PRINT( "Xbus Test\n" );
    R32_PA_DIR = (1<<21)|( 1<<20 )|0x7FFF;                                      /* 置地址、WR、RD信号输出 */ 
    mFstDat = 0x55;
    PRINT( "setup clk=1:\n" );
    for( mHoldClk = 0; mHoldClk <= RB_XBUS_HOLD; mHoldClk ++ ){
        R8_SAFE_ACCESS_SIG = 0x57;                                              /* unlock step 1 */
        R8_SAFE_ACCESS_SIG = 0xA8;                                              /* unlock step 2 */ 

        R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE;                      /* 外部总线使能 */ 
        R8_XBUS_SETUP_HOLD = mHoldClk;                                          /* 1 setup clocks */ 

        R8_SAFE_ACCESS_SIG = 0x00;                                              /* lock, to prevent unexpected writing */

        j = 1024 * 32;                                                          /* 外部32K RAM */ 
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* 外部总线基址 */ 
        mTstDat = mFstDat;

        PRINT( "    mHoldClk=0x%x, FstDat=0x%x\n", ( UINT16 )mHoldClk, ( UINT16 )mFstDat );
        PRINT( "        write: " );
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x ",( UINT16 )mTstDat );                      /* 显示前16个数据 */ 
            *pXbusPt = mTstDat;
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n" );
        PRINT( "        read:  " );
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* 外部总线基址 */ 
        mTstDat = mFstDat;                                                      /* 测试首数据 */
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x  ", ( UINT16 )*pXbusPt );
            if( *pXbusPt != mTstDat ){
                PRINT( "        err: i=0x%lX, mTstDat=%x\n", ( UINT32 )i, ( UINT16 )mTstDat );
                break;
            }
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n        over\n" );
        mFstDat ^= 0xFF;
    }
    PRINT( "setup clk=2:\n" );
    for( mHoldClk = 0; mHoldClk <= RB_XBUS_HOLD; mHoldClk ++ ){
        R8_SAFE_ACCESS_SIG = 0x57;                                              /* unlock step 1 */ 
        R8_SAFE_ACCESS_SIG = 0xA8;                                              /* unlock step 2 */

        R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE;                      /* 外部总线使能 */
        R8_XBUS_SETUP_HOLD = mHoldClk;                                          /* 1 setup clocks */ 

        R8_SAFE_ACCESS_SIG = 0x00;                                              /* lock, to prevent unexpected writing */

        j = 1024 * 32;                                                          /* 外部32K RAM */
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* 外部总线基址 */ 
        mTstDat = mFstDat;                                                      /* 测试首数据 */ 
        PRINT( "    mHoldClk=0x%x, FstDat=0x%x\n", ( UINT16 )mHoldClk, ( UINT16 )mFstDat );
        PRINT( "        write: " );
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x ",( UINT16 )mTstDat );                      /* 显示前16个数据 */ 
            *pXbusPt = mTstDat;
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n" );
        PRINT( "        read:  " );
        pXbusPt = ( PUINT8V )0x00C00000;                                        /* 外部总线基址 */ 
        mTstDat = mFstDat;                                                      /* 测试首数据 */ 
        for( i = 0; i < j; i ++ ){
            if( i < 16 ) PRINT( "%x  ", ( UINT16 )*pXbusPt );
            if( *pXbusPt != mTstDat ){
                PRINT( "        err: i=0x%lX, mTstDat=%x\n", ( UINT32 )i, ( UINT16 )mTstDat );
                break;
            }
            pXbusPt ++;
            mTstDat ^= 0xFF;
        }
        PRINT( "\n        over\n" );
        mFstDat ^= 0xFF;
    }
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
    PRINT( "Start Para Host Test\n" );
    XbusTest( );                                                                /* 主动并口演示程序 */
    while( 1 );
}

/*********************************** endfile **********************************/
