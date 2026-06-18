/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 AUDIO Demo Test
*                      (1)、CH563 USB Device Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、该程序以中断方式处理;
*                      (4)、模拟USB声卡.配合CH563评估板可以播放音乐
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "CH563USBSFR.H"                                                        /* 芯片USB寄存器相关头文件 */
#include "USB_DEVICE.H"                                                         /* USB设备定义相关头文件 */
#include "SYSFREQ.H"                                                            /* 系统配置相关头文件 */    
#include "PRINTF.H"                                                             /* 串口打印输出控制头文件 */
#include "AUDIO.H"

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

__irq void IRQ_Handler( void )
{
    UINT8 i;

    if( R8_INT_FLAG_1 & RB_IF_USB )                                             /* USB中断 */
    {
        if( USB_GL_INT_STATUS & RB_DEV_INT )
        {
            USBDev_IRQHandler( );                                               /* USB设备中断处理函数 */
        }
    }
    if(R8_INT_FLAG_0 & RB_IF_TMR0)                                              /* 判断是不是定时器0相关的中断 */
    {
        i = R8_TMR0_INT_FLAG;
           if( i&RB_TMR_IF_DMA_END ){
            R8_TMR0_CTRL_MOD &= ~(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);               /* 关闭定时器 */
            R16_TMR0_DMA_BEG = (UINT32)&pwm.DatBuf[0];                          /* 数组的首地址给DMA开始缓冲区地址 */
            R8_TMR0_INT_FLAG = RB_TMR_IF_DMA_END;                               /* 清除相应中断标志 */
            R8_INT_FLAG_0 |= RB_IF_TMR0;                                        /* 清除相应中断标志 */
        }
    }
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断处理函数
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
* Function Name  : Tim0_IRQ_Init
* Description    : PWM初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Tim0_IRQ_Init( )
{
    pwm.EndCount = 100000000/8000/8;
    printf("end count=%8d\n",pwm.EndCount);
    R8_TMR0_INT_FLAG=0xff;                                                      /* 清除所有的中断标志 */                                           
    R8_INT_EN_IRQ_0=RB_IE_IRQ_TMR0;                                             /* 只允许定时器0相关中断产生 */
    R8_TMR0_INTER_EN |= RB_TMR_IE_DMA_END;                                      /* 开启DMA结束中断 */
    R8_INT_EN_IRQ_GLOB= RB_IE_IRQ_GLOB;                                         /* 只开启IRQ全局中断 */
    R8_TMR0_CTRL_DMA= (RB_TMR_DMA_ENABLE)|(RB_TMR_DMA_LOOP&0);                  /* 开启DMA，禁止DMA循环 */
    R32_TMR0_CNT_END = pwm.EndCount;                                            /* 调节PWM周期 */
    R8_TMR0_CTRL_MOD = 0x00;                                                    /* 清0 */
/* 16 */
//    R8_TMR0_CTRL_MOD |= (1<<7)|(1<<6);                                        /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
/* 8 */
    R8_TMR0_CTRL_MOD |= (1<<7);                                                 /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
/* 4 */
//    R8_TMR0_CTRL_MOD |= (1<<6);                                               /* timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16 */
    R32_PB_DIR |= PWM0;                                                         /* PB0为定时器0输出引脚，设置为输出 */
}
void Star_PWM_DMA( )
{
    R16_TMR0_DMA_BEG = (UINT32)&pwm.DatBuf[0];                                  /* 数组的首地址给DMA开始缓冲区地址 */
    R16_TMR0_DMA_END = (UINT32)&pwm.DatBuf[pwm.DatLen+2];                       /* 数组的结束地址给DMA结束缓冲区地址 */
    R8_TMR0_CTRL_MOD|=(RB_TMR_COUNT_EN|RB_TMR_OUT_EN);                          /* 启动定时器 */
}
/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int main( void ) 
{
    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
#if  MY_DEBUG_PRINTF 
    printf("CH563 USB Device Test\n");
#endif
    Tim0_IRQ_Init( );
    USBDev_ModeSet( );                                                          /* USB设置为设备模式 */
    USBDev_Init( );                                                             /* USB设备初始化 */
    USBDev_UsbInt_Enable( );                                                    /* USB设备各相关中断使能 */
    while( 1 )
    {
        /* 所有操作均在中断中处理 */
    }
}

/*********************************** endfile **********************************/
