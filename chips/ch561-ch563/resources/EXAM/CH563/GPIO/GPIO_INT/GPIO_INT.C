/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO_INT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X GPIO_INT DEMO
*                      (1)、CH56X Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序主要是GPIO作为外部中断的设置，并在中断服务子程序中读取中断状态和清中断标志.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                  1<<3

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
    PRINT("R32_INT_FLAG=%8lx\n",R32_INT_FLAG);
    if((R32_INT_FLAG>>8) & RB_IF_PB){                                           /* PB组中断 */
        PRINT("%8lX \n", R32_INT_STATUS_PB&0x000000ff );                        /* 查看中断标志 */
        R32_INT_STATUS_PB = 0xfffff;                                            /* 中断标志写1清零 */
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
* Function Name  : IRQ_InitPB
* Description    : 中断配置初始化 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void IRQ_InitPB( void )
{
    R32_PB_PU  |= 0x000000ff;                                                   /* GPIO B上拉设置，置1表示上拉 */ 
    R32_PB_DIR &= ~0x000000ff;                                                  /* GPIO B方向设置为输入 , direction: 0=in, 1=out */

    R32_INT_ENABLE_PB |= 0x000000ff;                                            /* GPIO B组各位中断使能 ： 1-使能，0-禁止 */
    R32_INT_MODE_PB   |= 0x000000ff;                                            /* GPIO B组中断方式 ：1-边沿中断,0-电平中断 */      
    R32_INT_POLAR_PB  &= ~0x000000ff;                                           /* GPIO B组中断极性 ：1-上升沿中断/高电平，0-下降沿中断/低电平 */      
    
    R32_INT_STATUS_PB  = 0xfffff;                                               /* 中断标志写1清零 */
    R8_INT_EN_IRQ_1   |= RB_IE_IRQ_PB;                                          /* GPIO B组中断使能 */ 
    R8_INT_EN_IRQ_GLOB|= RB_IE_IRQ_GLOB;                                        /* 全局中断使能 */     
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
    PRINT("GPIO INT DEMO.....\n");
    IRQ_InitPB( );                                                              /* 中断配置初始化 */
    while(1);
}

/*********************************** endfile **********************************/
