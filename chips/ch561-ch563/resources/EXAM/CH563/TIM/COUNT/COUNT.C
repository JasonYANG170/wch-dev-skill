/********************************** (C) COPYRIGHT *******************************
* File Name          : COUNT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X COUNT DEMO
*                      (1)、CH56X Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、ARM9-CH56X单片机C语言的计数示例程序,
*                           本程序演示定时器3计数功能；
*                           捕获PB6引脚的信号计数，捕获的最短时间是8个或者16个时钟周期.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                     1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }          /* LED 高电平为输出方向 */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                             /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                             /* LED 高电平关闭LED显示 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )
{
    UINT8 i;

   PRINT( "irq#  " );
   if(R8_INT_FLAG_0&RB_IF_TMR3){
       i= R8_TMR3_INT_FLAG;
       if(i&RB_TMR_IF_CYC_END){                                                /* 捕获超时中断 */
            R8_TMR3_INT_FLAG=0xff;                                             /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR3;                                       /* 清除相应中断标志 */
            R32_TMR3_FIFO;
        }
    }
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
* Function Name  : IRQ_Init
* Description    : IRQ中断初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void IRQ_Init( void )
{    
    R8_TMR0_INT_FLAG   = RB_TMR_IF_DATA_ACT|RB_TMR_IE_CYC_END|RB_TMR_IF_FIFO_HF|
                         RB_TMR_IF_DMA_END|RB_TMR_IF_FIFO_OV|RB_TMR_IF_DMA_ERR; /* 清除所有的中断标志 */    
    R8_TMR3_INTER_EN   = RB_TMR_IE_CYC_END;                                     /* 计数到达终值中断使能 */
    R8_INT_EN_IRQ_0    = RB_IE_IRQ_TMR3;                                        /* 允许定时器3相关中断产生 */
    R8_INT_EN_IRQ_GLOB = RB_IE_IRQ_GLOB;                                        /* 只开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : Timer3_Init
* Description    : 定时器3初始化
* Input          : count_end-- 计数终值
*                  min_time--  最小捕获脉宽:0:16个时钟周期,1:8个时钟周期
*                  catch_edge--捕获模式设置  
* Output         : None
* Return         : None
*******************************************************************************/

void Timer3_Init( UINT32 count_end ,UINT8 min_time,UINT8 capture_edge  ) 
{
    R32_PB_PU |= CAT3;                                                          /* 输入上拉    */
    R32_PB_DIR &= ~CAT3;                                                    
    R8_TMR3_CTRL_MOD = RB_TMR_CATCH_EDGE&(capture_edge<<6) | 
                       (RB_TMR_CAT_WIDTH&(min_time<<3)) | RB_TMR3_MODE_COUNT;    
    R32_TMR3_CNT_END = count_end;                                               /* 捕获的最大超时时间 */
    R8_TMR3_CTRL_MOD |= RB_TMR_COUNT_EN ;                                       /* 允许计数，启动定时器 */
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
    PRINT( "TIM3 count demo\xd\xa" );
    IRQ_Init( );                                                                /* 中断初始化 */
    Timer3_Init( 10,1,1);                                               
    while ( 1 ){
        PRINT( "COUNT =%-8x",R32_TMR3_COUNT );
    }
}

/*********************************** endfile **********************************/
