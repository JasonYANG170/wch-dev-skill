/********************************** (C) COPYRIGHT *******************************
* File Name          : EEPROM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X EEPROM DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序实现EEPROM读取擦除以及编程过程，CH56X芯片有28K的EEPROM
*                      (4)、写入数据可以通过单片机读取数据也可以使用"WCHISPTool_Setup.exe"软件读取
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

/******************************************************************************/
/* 变量定义 */
UINT8 my_buffer[ 0x2000 ];

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
* Function Name  : CH56X_EEPROM
* Description    : EEPROM操作子程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void CH56X_EEPROM( void ) 
{
    UINT32 i;
    
    PRINT("Eeprom Read:\n");
    EEPROM_READ( 0x0000,my_buffer,64 );                                         /* EEPROM读取地址0x0000数据，读取64字节 */
    for( i=0;i!=64;i++ ){
        if(i%8 == 0) PRINT("\n");
        PRINT("%02x ",(UINT16)my_buffer[i]);
    }
    PRINT("\n");

    PRINT("Eeprom Erase:\n");
    i = EEPROM_ERASE(0x2000,0x1000);                                            /* 擦除0X2000地址开始的4K数据 */
    if( i ){
        PRINT("    ***Error***\n");
    }

    for( i=0;i!=64;i++ )my_buffer[i] = i;
    PRINT("Write_Eeprom:\n");
    i = EEPROM_WRITE( 0x2000,my_buffer,64 );                                    /* 往地址0X2000写64字节的数据 */
    if( i ){
        PRINT("    ***Error***\n");
    }

    PRINT("Eeprom Read:\n");
    EEPROM_READ( 0x2000,my_buffer,64 );                                         /* EEPROM读取地址0x0000数据，读取64字节 */
    for( i=0;i!=64;i++ ){
        if(i%8 == 0) PRINT("\n");
        PRINT("%02x ",(UINT16)my_buffer[i]);
    }
    PRINT("\n");
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
    PRINT( "Start Eeprom Tset\xd\xa" );
    CH56X_EEPROM( );
    while(1);
}

/*********************************** endfile **********************************/
