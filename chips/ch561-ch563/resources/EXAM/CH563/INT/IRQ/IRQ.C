/********************************** (C) COPYRIGHT *******************************
* File Name          : IRQ.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X IRQ DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序实现IRQ中断功能.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h> 
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
__packed union  _BUF {
  UINT8  buf[4096];
  UINT32 my_buffer[ 1024 ];
} pbuf;

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
    UINT16 i;
    /*捕获中断*/
    if( R8_INT_FLAG_0 & RB_IF_TMR0 ){                                           /* 定时器中断 */
        PRINT("CAT_IRQ\n");
        if( R8_TMR0_INT_FLAG & RB_TMR_IF_DMA_END ){
            R8_INT_EN_IRQ_0 &= ~RB_IE_IRQ_TMR0;                                 /* 关闭定时器中断 */      
            R16_TMR0_DMA_BEG = (UINT32)&pbuf.my_buffer[0];                      /* 如果不使用RB_TMR_DMA_LOOP功能则需要重新装载DMA开始和结束缓冲区 */
            R16_TMR0_DMA_END = (UINT32)&pbuf.my_buffer[64];
            /*下面为通过串口监测捕获数据*/
            PRINT("CAT_DATA:    \n");
            for( i=0;i!=64*4;i++ ){
                if(!(i%8)) PRINT("\n"); 
                PRINT("%8x",(UINT16)pbuf.buf[i]);
            }
            PRINT("\n\n");
            R8_TMR0_CTRL_DMA = ( RB_TMR_DMA_ENABLE | RB_TMR_DMA_BURST  );    
            /*开启定时器以及PWM功能*/ 
            R8_TMR0_CTRL_MOD |= ( RB_TMR_COUNT_EN );
            R8_TMR0_INT_FLAG = 0x7f;                                            /* 清中断状态 */
            R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                  /* 开启定时器中断 */
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
* Function Name  : TIM0_INIT
* Description    : 捕获模式定时器0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIM0_INIT( void ) 
{
    R32_PB_DIR &= ~CAT0;                                                        /* PB1使能输入 */
    R8_TMR0_CTRL_MOD =  RB_TMR_MODE_IN ;                                        /* 定时器输出模式设置 */
    R8_TMR0_CTRL_MOD |= 1<<6;                                                   /* 捕获  00=disable, 01=edge change, 10=fall to fall, 11-rise to rise */
    R8_TMR0_CTRL_DMA &= 0x00;                                                   /* 禁止DMA */
    R32_TMR0_CNT_END = 0xfffffff;                                               /* 总时间  捕获总时间 */
    /*以下为中断配置*/    
    R8_TMR0_INTER_EN |= RB_TMR_IE_DMA_END;                                      /* 开启捕获DMA结束中断 */     
    R8_INT_EN_IRQ_0  |= RB_IE_IRQ_TMR0;                                         /* 开启定时器 */
    /*以下为DMA配置*/     
    R16_TMR0_DMA_BEG = (UINT32)&pbuf.my_buffer[0];                              /* DMA开始缓冲区地址 */
    R16_TMR0_DMA_END = (UINT32)&pbuf.my_buffer[64];                             /* DMA结束缓冲区地址 */

    PRINT("DMA_BEG =%02x\n",(UINT16)R16_TMR0_DMA_BEG);                          /* 监控下DMA开始和结束地址。如果地址异常需要重新设置 */
    PRINT("DMA_END =%02x\n",(UINT16)R16_TMR0_DMA_END);                          /* 地址必须为4的整数倍 */

    R8_TMR0_CTRL_DMA = ( RB_TMR_DMA_ENABLE | RB_TMR_DMA_BURST );                /* 开始DMA */    
    /*开启定时器功能*/ 
    R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN ;                                       /* 开启定时器计数功能 */
    R8_TMR0_INT_FLAG = 0x7f;                                                    /* 清中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启全局中断 */
    R8_TMR0_INT_FLAG = 0x7f;                                                    /* 清中断 */
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
    PRINT( "Start\n" );
    TIM0_INIT( );                                                               /* 捕获DMA初始化函数 */
    while(1);                                                                   /* 等到捕获DMA中断 */
}

/*********************************** endfile **********************************/
