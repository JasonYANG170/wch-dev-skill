/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X PWM DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、ARM9-CH56X单片机C语言的定时器定时示例程序
*                           本程序演示定时器0输出PWM功能，定时器1，2，3类似，相同之处则不再重述，不同之处则在程序注释中指出;
*                           可以调节PWM周期和占空比，此程序输出周期为40ms,高低电平的占空比为1：3,PMM输出是PB0引脚.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

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
        if(i&RB_TMR_IF_DATA_ACT){
            R8_TMR0_INT_FLAG = RB_TMR_IF_DATA_ACT | RB_TMR_IF_FIFO_HF |
                               RB_TMR_IF_FIFO_OV;                               /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* 清除相应中断标志 */
        }
        if(i&RB_TMR_IF_CYC_END){
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
    R8_TMR0_INTER_EN=RB_TMR3_FORCE_EN|RB_TMR_IE_CYC_END|RB_TMR_IE_DATA_ACT;     /* 定时器时钟周期结束中断使能，PWM 结束中断使能 */
    R8_INT_EN_IRQ_0=RB_IE_IRQ_TMR0;                                             /* 允许定时器0相关中断产生 */
    R8_INT_EN_IRQ_GLOB= RB_IE_IRQ_GLOB;                                         /* 只开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : Timer0_Init
* Description    : 定时器0初始化
* Input          : end  -定时器0定时周期，定时周期Time = Pclk*end, Pclk为系统时钟
*                  fifo -调节PWM占空比
* Output         : None
* Return         : None
*******************************************************************************/

void Timer0_Init( UINT32 end ,UINT32 fifo )  
{
    R8_TMR0_CTRL_MOD=0x00;                                  
    /* 如果是TIM0或者TIM1或TIM2,则需要将RB_TMR_MODE_NRZI位清0,如果是TIM3,则RB_TMR_MODE_COUNT位清0 */
    /* 默认情况下R8_TMR0_CTRL_MOD的位RB_TMR_ALL_CLEAR为1，开始时必须把此位清0，否则R32_TMR0_FIFO即*/
    /* 使赋值成功也不能工作，而如果R32_TMR0_CNT_END赋值成功不对其产生影响                          */
    R32_PB_DIR|=PWM0;                                                           /* PB0为定时器0输出引脚，设置为输出 */
    R32_TMR0_CNT_END=end;                                                       /* 调节PWM周期 */
    R32_TMR0_FIFO=fifo;                                                         /* 调节占空比 */
    R8_TMR0_CTRL_MOD|=(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);                          /* 允许输出，启动定时器 */
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
    PRINT( "Start PWM demo\xd\xa");
    IRQ_Init( );                                                                /* 中断初始化 */
    Timer0_Init( 4000000,1000000 );                                             /* PWM周期为40ms(主频默认100M),高低电平的占空比为1：3 */
    while(1){                                                 
        Delay_ms( 10 );
        PRINT("TMR0_COUNT=%ld\n",(UINT32)R32_TMR0_COUNT);                       /* 计数器状态值显示 */
    }
}

/*********************************** endfile **********************************/
