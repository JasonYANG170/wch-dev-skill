/********************************** (C) COPYRIGHT *******************************
* File Name          : WDOG.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/08/25
* Description        : CH563 EEPROM DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序演示看门狗功能
*******************************************************************************/


#define           DEBUG            1
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

UINT32  TIM_COUNT;

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & RB_IF_TMR0){                                              /* 定时器中断 */
        R8_TMR0_INT_FLAG |= 0xff;                                               /* 清除定时器中断标志 */
        TIM_COUNT ++;
        if( TIM_COUNT >= 1000 ){                  // time=1000*10 ms=10s
            R8_SAFE_ACCESS_SIG = 0x57 ;                                         /* unlock step 1 */
            R8_SAFE_ACCESS_SIG = 0xA8 ;                                         /* unlock step 2 */                     
            R8_GLOB_RST_CFG |= (0x40 | RB_GLOB_FORCE_RST);                      /* bit7:6 must write 0:1 */
            R8_SAFE_ACCESS_SIG = 0;                                             /* 保护，禁止读写 */
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
* Function Name  : TIM_Time0Init
* Description    : 定时器0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_Time0Init( void )
{
    R8_TMR0_CTRL_MOD  = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT    = 0x00000000; 
    R32_TMR0_CNT_END  = 0x186a0 * 10;                                           /* 设置为10MS定时 */
    R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
    R8_TMR0_CTRL_MOD  = RB_TMR_COUNT_EN;
    R8_INT_EN_IRQ_0  |= RB_IE_IRQ_TMR0;                                         /* 开启TIM0中断 */
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
    R32_PB_DIR &=  ~(1<<7);                                                     /* PB7输入上拉 */                                
    R32_PB_PU  |=  (1<<7);                   
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */ 
    PRINT( "Start:\r\n" );
    TIM_Time0Init( );
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启IRQ全局中断 */
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */                     
    R8_GLOB_RST_CFG |= (0x40 | RB_GLOB_WDOG_EN);                                /* bit7:6 must write 0:1 开启看门狗，时间=2^23=8388608个主时钟周期*/
    R8_SAFE_ACCESS_SIG = 0;                                                     /* 保护，禁止读写 */
    TIM_COUNT = 0;                                                            
    while(1){
        R8_WDOG_CLEAR = 0;
        if( (R32_PB_PIN & (1<<7) )== 0 ){                                       /* 演示看门狗功能 */
            while(1);
        }
//        TIM_COUNT = 0;                                                        /* 不清除该变量每60s复位一次 */
    }
}

/*********************************** endfile **********************************/
