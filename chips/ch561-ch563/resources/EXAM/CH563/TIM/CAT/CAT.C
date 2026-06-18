/********************************** (C) COPYRIGHT *******************************
* File Name          : CAT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X CAT DEMO
*                      (1)、CH56X Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、ARM9-CH56X单片机C语言的捕获示例程序,
*                           本程序演示定时器0捕获功能，定时器1，2，3类似，相同之处则不再重述，不同之处则在程序注释中指出;
*                           本程序用来捕获PB1引脚的一个高电平或者低电平；
*                           高低电平最短时间是8个或者16个时钟周期（可以选择），最长时间是Pclk*(2^27),Pclk为系统时钟.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
UINT32 Capture;   /* 保存捕获的值 */
UINT8 FLAG=0;     /* FLAG为1时表示有边沿触发 */

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
    UINT8 i;
    if(R8_INT_FLAG_0&RB_IF_TMR0){
        i= R8_TMR0_INT_FLAG;
        if(i&RB_TMR_IF_DATA_ACT){                                               /* 电平变化中断 */
            R8_TMR0_INT_FLAG = RB_TMR_IF_DATA_ACT | RB_TMR_IF_FIFO_HF |
                               RB_TMR_IF_FIFO_OV;                               /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* 清除相应中断标志 */
            Capture=R32_TMR0_FIFO;                                              /* 捕获到的时间值保存下来 */
            FLAG = 1;   
        }                
        if(i&RB_TMR_IF_CYC_END){                                                /* 捕获超时中断 */
            R8_TMR0_INT_FLAG=0xff;                                              /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* 清除相应中断标志 */
            R32_TMR0_FIFO;
            PRINT("overtime\n") ;
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
    /* 清除所有的中断标志 */    
    R8_TMR0_INT_FLAG=RB_TMR_IF_DATA_ACT|RB_TMR_IE_CYC_END|RB_TMR_IF_FIFO_HF|RB_TMR_IF_DMA_END|RB_TMR_IF_FIFO_OV|RB_TMR_IF_DMA_ERR;                                                                   
    R8_TMR0_INTER_EN=RB_TMR_IE_DATA_ACT|RB_TMR_IE_CYC_END;                      /* 捕获模式下有电平变化中断使能，捕获模式下捕获超时中断使能 */
    R8_INT_EN_IRQ_0=RB_IE_IRQ_TMR0;                                             /* 允许定时器0相关中断产生 */
    R8_INT_EN_IRQ_GLOB= RB_IE_IRQ_GLOB;                                         /* 只开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : Timer0_Init
* Description    : 定时器0初始化
* Input          : max_time--  为捕获的最大超时时间设置，超时时间time = Pclk*max_time, Pclk为系统时钟
*                  min_time--  最小捕获脉宽:0:16个时钟周期,1:8个时钟周期
*                  catch_edge--捕获模式下边沿触发方式:    00:禁用触发,01:捕获任何边沿触发的时间,10:捕获下降沿到下降沿之间时间,
*                              11:捕获上升沿到上升沿之间时间
* Output         : None
* Return         : None
*******************************************************************************/

void Timer0_Init( UINT32 max_time ,UINT8 min_time,UINT8 capture_edge  ) 
{
    R32_PB_PU|=0x00000002;                                                      /* 输入上拉    */
    R32_PB_DIR&=0xfffffffd;                                                     /* PB1为输入 */
    R8_TMR0_CTRL_MOD=(RB_TMR_CATCH_EDGE&(capture_edge<<6))|RB_TMR_MODE_IN|(RB_TMR_CAT_WIDTH&(min_time<<3));
                                                                                /* 边沿触发方式,捕获模式,8时钟周期采样 */
    R32_TMR0_CNT_END=max_time;                                                  /* 捕获的最大超时时间 */
    R8_TMR0_CTRL_MOD|=(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);                          /* 启动定时器 */
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
* Function Name  : UART1_SendByte
* Description    : 通过串口发送一字节数据
* Input          : dat--要发送的一字节数据
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendByte( UINT8 dat )   
{        
    R8_UART1_THR  = dat;
    while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                         /* 等待数据发送 */         
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
    PRINT( "Start Capture demo\xd\xa" );
    IRQ_Init( );                                                                /* 中断初始化 */
    Timer0_Init( 0x0fffffff,1,1);                                               /* 最大超时时间，最小捕获脉冲为8个时钟，任意边沿触发 */
    while ( 1 ){
        if(FLAG){
            FLAG = 0;
            if(Capture&(1<<27))                                                 /* 寄存器R32_TMR_FIFO值最高位为1表示捕获电平为高电平,反之为低电平 */
                PRINT("Hig=%ld\n",(Capture&0x07ffffff));
            else
                PRINT("Low=%ld\n",Capture);
        }   
    }
}

/*********************************** endfile **********************************/
