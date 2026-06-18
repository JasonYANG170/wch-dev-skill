/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0_DMA.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_DMA DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序主要为DMA传输，包含DMA发送子程序.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
UINT8  upper[1024];

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
    if (R32_INT_FLAG & RB_IF_SPI0 ){                                           /* 查询是否为SPI0中断 */
        if( R8_SPI0_INT_FLAG & RB_SPI_IF_DMA_END ){
            PRINT("R32_INT_FLAG    = %8lX \n", R32_INT_FLAG);   
            PRINT("R8_SPI0_INT_FLAG= %8lX \n", R8_SPI0_INT_FLAG);
            PRINT("DMA_BEG= %8lX \n", R16_SPI0_DMA_BEG);
            PRINT("DMA_END= %8lX \n", R16_SPI0_DMA_END);
            PRINT("DMA_NOW= %8lX \n", R16_SPI0_DMA_NOW);
            PRINT("DMA END \n"); 
            R16_SPI0_DMA_BEG = (UINT32)upper;                                   /* DMA设置，数据开始地址 */
        }
        R8_SPI0_INT_FLAG = 0xff;                                                /* 清中断标志 */
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
* Function Name  : IRQInit
* Description    : 中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void IRQInit( void )
{
    R8_SPI0_INT_FLAG = 0xff;                                                    /* 中断标志，写1清零 */ 
    R8_SPI0_INTER_EN |= RB_SPI_IE_DMA_END;                                      /* DMA结束中断 */
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_SPI0;                                          /* 开SPI0中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开全局中断 */
}

/*******************************************************************************
* Function Name  : SPI0_MasterInit
* Description    : SPI主机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_MasterInit ( void )
{
    R8_SPI0_CTRL_MOD &= ~ RB_SPI_MODE_SLAVE;
    R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE|RB_SPI_SCK_OE;                            /* MOSI，SCK0输出使能 */
    R8_SPI0_CLOCK_DIV = 0x0a;                                                   /* 10分频。100/10=10M */
    R32_PB_DIR |= (MOSI | SCK0 | SCS );                                         /* MOSI(PB14),SCK0(PA13),SCS(PB12)为输出 */
    R32_PB_PU  |=  SCS ;                         
    R32_PB_OUT |=  SCS ; 
}

/*******************************************************************************
* Function Name  : SPI0_DMASend
* Description    : DMA传输   
* Input          : *BEG -数据开始地址
*                  *END -数据结束地址
*                  LEN  -数据长度
* Output         : None
* Return         : None
*******************************************************************************/

void SPI0_DMASend (UINT8 *BEG,UINT8 *END,UINT16 LEN)
{
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /*  设置方向为输出 */
    R16_SPI0_DMA_BEG = (UINT32)BEG;                                             /* DMA设置，数据开始地址 */
    R16_SPI0_DMA_END = (UINT32)END;                                             /* 数据结束地址 */
    R16_SPI0_TOTAL_CNT = LEN;                                                   /* 数据长度 */
    PRINT("R16_SPI0_DMA_BEG= %8lX \n", R16_SPI0_DMA_BEG);                       /* 输出地址信息 */
    PRINT("R16_SPI0_DMA_END= %8lX \n", R16_SPI0_DMA_END);                       /* 结束地址 */
    PRINT("R16_SPI0_DMA_NOW= %8lX \n", R16_SPI0_DMA_NOW);                       /* 当前地址 */
    R8_SPI0_CTRL_DMA |= RB_SPI_DMA_ENABLE;                                      /* DMA使能 */
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
    UINT16 i;

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 串口0初始化 */
    PRINT("start:\n");
    for(i=0;i<1024;i++) upper[i] = i;
    SPI0_MasterInit( );                                                         /* SPI主机模式初始化 */
    IRQInit( );
    R32_PB_CLR |= SCS;                                                          /* 片选使能 */
    SPI0_DMASend(upper,&upper[1024],1024);                                      /* DMA传输 */
    while(1);
}

/*********************************** endfile **********************************/
