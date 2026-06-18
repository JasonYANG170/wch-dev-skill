/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 TFT DEMO
*                      驱动芯片6804，支持横竖屏操作。英文字符，图形，汉字，图片的显示
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "..\PARA\PARA_TFT\LED\LED.H"
#include "..\PARA\PARA_TFT\LCD\LCD.H"
#include "..\PARA\PARA_TFT\KEY\KEY.H"
#include "..\PARA\PARA_TFT\CHAR\CHAR.H"
#include "..\PARA\PARA_TFT\CHINESE\CHINESE.H"
#include "..\PARA\PARA_TFT\PICTURE\PICTURE.H"
#include "..\PARA\PARA_TFT\EXTEST\EXTEST.H"
#include "..\PARA\PARA_TFT\TP\TP.H"

/******************************************************************************/
extern void show_pic(const unsigned short *p);

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED_OUT_INIT( LED )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }       /* LED 高电平为输出方向 */
#define LED_OUT_ACT( LED )      { R32_PB_CLR |= LED; }                          /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT( LED )    { R32_PB_OUT |= LED; }                          /* LED 高电平关闭LED显示 */

#define LED                     1<<3

/* 选择通讯接口 */    
#define  IC_OP_INTERFACE_MODE                 2                                 /* 定义芯片操作接口模式(1:硬件并口;  2:软件模拟并口; */ 
#if    ( IC_OP_INTERFACE_MODE == 1 )
#include "PARA_HW.C"                                                            /* 硬件总线并口连接方式 */
#elif  ( IC_OP_INTERFACE_MODE == 2 )
#include "PARA_SW.C"                                                            /* 软件模拟并口连接方式 */
#endif


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
    LED_OUT_INIT( LED );
    LED_OUT_ACT( LED );
    Delay_ms(300);
    LED_OUT_INACT( LED );
    mInitSTDIO( );
    PRINT("TFT DEMO...\r\n");
    CH563_PORT_INIT( );
    LED_Init( );
    LCD_Init( );
    KEY_INIT( );
    LCD_Clear(BLUE);                                                            /* 清屏 */
    Delay_ms(800);
    while(1){
#if 1
        LCD_Clear(WHITE);                                                       /* 清屏 */    
        TEST_CHAR( );                                                           /* 字符串显示示例 */ 
        Delay_ms(3000);
        LCD_Clear(WHITE);                                                       /* 清屏 */     
        EX_TEST( );                                                             /* 扩展测试,显示图形 */
        Delay_ms(3000);
        LCD_Clear(WHITE);                                                       /* 清屏 */
        TEST_FONT();                                                            /* 中文显示示例 */ 
        Delay_ms(3000);
        LCD_Clear(WHITE);                                                       /* 清屏 */
        show_pic(PIC_DATA);                                                     /* 图片显示示例 */
        Delay_ms(5000);
#endif
#if 0
        LCD_Clear(BLACK);                                                       /* 清屏 */
        TP_DrawLine_TEST();
#endif
    }
}

/*********************************** endfile **********************************/
