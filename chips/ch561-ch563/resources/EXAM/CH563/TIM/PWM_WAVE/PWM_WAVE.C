/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM_WAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X ADC_QUERY DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序演示定时器0输出PWM功能.播放一段音频数据。
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "WAVE.H"

/******************************************************************************/
/* 变量定义 */
extern UINT8 sound[];                                                           /* 原型在wave.c文件中声明 */
extern UINT8 *pdata_addr;                                                       /* 数组中数据开始的位置的地址，外部变量，原型在wave.c文件中声明 */
extern UINT32 datasize;                                                         /* 数组中实际数据从长度，外部变量，原型在wave.c文件中声明 */
extern UINT32 CountEnd;
UINT8 PWM_Flag = 0;
UINT8 PlayTimes = 0;
UINT8 *pdata_now;
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

    if(R8_INT_FLAG_0 & RB_IF_TMR0){                                             /* 判断是不是定时器0相关的中断 */
        i= R8_TMR0_INT_FLAG;
        if(i&RB_TMR_IF_CYC_END){
            PlayTimes++;                                                        /* 播放次数自增 */
            if(PlayTimes==4){                                                   /* 同一个采样数据播放次数，增大pwm频率，使得音效更好 */
                R32_TMR0_FIFO = (UINT32)(CountEnd*((*pdata_now)/256.0));        /* 根据采样值算出应该向计数器中放入的终值值 */
                pdata_now++;                                                    /* 指针自加 */
                if((pdata_now-pdata_addr)>datasize){                            /* 如果现在播放到了文件的结尾了，那么重新开始播放 */
                    R8_INT_EN_IRQ_0 &= ~RB_IE_IRQ_TMR0;                         /* 失能定时器0相关中断产生 */
                    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                      /* 关闭IRQ全局中断 */
                    PRINT("Play End\n");                                        /* 打印信息 */
                    goto IRQ_End;                                               /* 跳转到中断函数结束处 */
                }
                PlayTimes = 0;
            }
IRQ_End:
            R8_TMR0_INT_FLAG=RB_TMR_IF_CYC_END;                                 /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* 清除相应中断标志 */
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
    R8_TMR0_INT_FLAG=0xff;                                                      /* 清除所有的中断标志 */
    R8_TMR0_INTER_EN=RB_TMR3_FORCE_EN|RB_TMR_IE_CYC_END|RB_TMR_IE_DATA_ACT;;    /* 定时器时钟周期结束中断使能，PWM 结束中断使能 */
    R8_INT_EN_IRQ_0=RB_IE_IRQ_TMR0;                                             /* 允许定时器0相关中断产生 */
    R8_INT_EN_IRQ_GLOB= RB_IE_IRQ_GLOB;                                         /* 只开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : Timer0_Init
* Description    : 定时器0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer0_Init( UINT32 end )
{
    R8_TMR0_CTRL_MOD=0x00;                                                      /* RB_TMR_MODE_COUNT位清0 */
    R32_PB_DIR|=PWM0;                                                           /* PB0为定时器0输出引脚，设置为输出 */
    R32_TMR0_CNT_END=end;                                                       /* 调节PWM周期 */
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
* Description    : I主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void )
{
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
    PRINT("System Init OK\n");
    IRQ_Init( );                                                                /* 中断初始化 */
    filehead_load(sound);                                                       /* 根据数组中的声音数据，提取出其中采样等相关的信息 */
    Timer0_Init( CountEnd );                                                    /* 将根据波形采样率算出的计数终值写入寄存器 */
    pdata_now = pdata_addr;
    R32_TMR0_FIFO=(UINT32)(CountEnd*((*pdata_now)/256.0));                      /* 将数据中的第一个数据放入 */
    R8_TMR0_CTRL_MOD|=(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);                          /* 允许输出，启动播放 */
    while(1);
}

/*********************************** endfile **********************************/
