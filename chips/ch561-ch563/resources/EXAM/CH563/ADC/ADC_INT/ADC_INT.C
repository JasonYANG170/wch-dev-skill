/********************************** (C) COPYRIGHT ******************************
* File Name          : ADC_INT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X ADC_INT Demo   
*                      (1)、串口0输出监控信息,115200bps;
*                      (2)、实现ADC采样数据.AD过程采用DMA传输,默认选择通道2进行ADC转换.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
UINT16 ADC_buf[1024]; 

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
    UINT16 i;
    UINT32 SUM=0;

    if( R8_INT_FLAG_1 & RB_IF_ADC ){                                            /* ADC中断 */
        if( R8_ADC_INT_FLAG & RB_ADC_IF_DMA_END ){                              /* DMA结束 */
            R8_ADC_CTRL_DMA &= ~(RB_ADC_DMA_ENABLE|RB_ADC_DMA_BURST | RB_ADC_MAN_SAMPLE );
                                                                                /* 关闭ADC的DMA */
            R8_ADC_CTRL_DMA &= ~RB_ADC_CHAN_OE;                                 /* 如果禁止ADC通道控制ADCS输出，则清除该位 */
            R16_ADC_DMA_BEG = (UINT32)ADC_buf;
			R8_ADC_INT_FLAG = 0xff;                                             /* 清中断 */ 
            for( i=0;i!=1024;i++ ){                                             /* 显示ADC采集数据,ADC采集数据位10位 */
                SUM +=ADC_buf[i];
                if(i%16 == 0) PRINT("\n");
                PRINT("%04x ",(UINT16)ADC_buf[i]);
            } 
            PRINT("\r\naverage=%06x\n",(UINT32)(SUM/1024));
            
#if 0
            /* 如果需要循环采样,则开启下面程序 */
            i = R8_ADC_FIFO_COUNT;                                              /* 人为清除FIFO内的数据 */
            while(i){
                R16_ADC_FIFO;
                i--;
            }
            R16_ADC_DMA_BEG = (UINT32)&ADC_buf[0];
            R16_ADC_DMA_END = (UINT32)&ADC_buf[1024];
            R8_ADC_CTRL_DMA |= (RB_ADC_DMA_ENABLE|RB_ADC_DMA_BURST | RB_ADC_MAN_SAMPLE );
                                                                                /* 启动DMA */
            R8_ADC_CTRL_DMA &= ~RB_ADC_CHAN_OE;                                 /* 如果禁止ADC通道控制ADCS输出，则清除该位 */
            R8_ADC_INT_FLAG = 0xff;                                             /* 清中断 */    
#endif
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
* Function Name  : ADC_INIT
* Description    : ADC初始化函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/* 函数主要配置ADC基本设置、ADC中断配置以及DMA配置,如果不设置中断则中断配置可以*/
/* 去掉,如果不采用DMA传输则可以读取R16_ADC_DATA寄存器,ADC采集结果在此寄存器    */

void ADC_INIT( void )
{
    UINT16 i;

    R8_ADC_CTRL_MOD = 0;
    R8_ADC_CTRL_MOD |= RB_ADC_POWER_ON;                                         /* 开启ADC使能 */
    R8_ADC_CLOCK_DIV = 0x40;                                                    /* ADC采样时钟信号 */
    R8_ADC_CTRL_MOD |= (RB_ADC_SAMPLE_WID | RB_ADC_CYCLE_CLK );                 /* 采样脉宽以及设置自动循环采样(RB_ADC_CYCLE_CLK必须大于11， */
                                                                                /* 如果为0则为手动采样） */
    R8_ADC_CTRL_MOD |= 0x20;                                                    /* 选择通道2 */    
    Delay_us(100);  

    /*以下为中断配置*/
    R8_ADC_INT_FLAG = 0xff;                                                     /* 清中断标志 */
    R8_ADC_INTER_EN |= RB_ADC_IE_DMA_END;                                       /* 开启DMA结束中断，如果不使用DMA方式则开启ADC完成中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ADC;                                           /* 开启ADC中断 */
    
    R8_ADC_INT_FLAG = 0xff;                                                     /* 清中断标志 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启全局中断 */
    i = R8_ADC_FIFO_COUNT;                                                      /* 人为清除FIFO内的数据 */
    while(i){
        R16_ADC_FIFO;
        i--;
    }
    /* 如果是手动采样，则设置RB_ADC_MAN_SAMPLE位为1，延时150ns以上，再置0以产生手动采样脉冲 */
    /* 以下为DMA配置 */    
    R16_ADC_DMA_BEG = (UINT32)&ADC_buf[0];
    R16_ADC_DMA_END = (UINT32)&ADC_buf[1024];
    R8_ADC_CTRL_DMA |=(RB_ADC_DMA_ENABLE|RB_ADC_DMA_BURST | RB_ADC_MAN_SAMPLE );/* 启动DMA */
    R8_ADC_CTRL_DMA &= ~RB_ADC_CHAN_OE;                                         /* 如果禁止ADC通道控制ADCS输出，则清除该位 */
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
    LED_OUT_ACT( );
    LED_OUT_INACT( );
    PRINT( "ADC Demo....." );
    ADC_INIT(  );
    while(1);                                                                   /* 等待ADC的DMA产生 */
}

/*********************************** endfile **********************************/
