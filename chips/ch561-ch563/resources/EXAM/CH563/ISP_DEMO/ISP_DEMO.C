/********************************** (C) COPYRIGHT *******************************
* File Name          : ISP_DEMO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2016/12/15
* Description        : CH56X ISP DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序演示进入ISP网络下载，写入成功后将芯片重新上电即可通过ISP网络下载
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>

#ifndef    DEBUG
#define    DEBUG  1
#endif

#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"
                              

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED_OUT_INIT( LED )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }       /* LED 高电平为输出方向 */
#define LED_OUT_ACT( LED )      { R32_PB_CLR |= LED; }                          /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT( LED )    { R32_PB_OUT |= LED; }                          /* LED 高电平关闭LED显示 */

__align(4) UINT8  TestBuf[1024*4];                                              //该缓存区是在调用ISP设置的相关函数时给的一个临时缓存区

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

    x = 10 * FREQ_SYS/ 8 / 115200;                                              /* 115200bps */
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
    UINT8 i,my_buffer[8];

    R32_PB_DIR &=  ~(1<<7);                                                     // PB7输入上拉                                
    R32_PB_PU  |=  (1<<7);                   
    mInitSTDIO();                                                               
    PRINT("Edit Date and Time is: "__DATE__" , " __TIME__"\n");
    GET_UNIQUE_ID( my_buffer );
    PRINT("chip id: ");
    for(i=0;i<8;i++) PRINT("%-2x ",my_buffer[i]);
    PRINT("\n");

    while(1){
        if( (R32_PB_PIN & (1<<7) )== 0  ){                                      // 本程序中设置的更新条件
            Delay_ms(50);
            if( (R32_PB_PIN & (1<<7) )== 0  ){
                while( (R32_PB_PIN & (1<<7) )== 0  );
                PRINT("ISP TEST\n");
                i = CH56X_EnterIspSet( TestBuf );
                if(i == 0)
                {
                    printf("entry isp success\n");
                                                                                //现在可以将芯片掉电，再重新上电及进入ISP
                }
            }
        }
    }
}

/*********************************** endfile **********************************/
