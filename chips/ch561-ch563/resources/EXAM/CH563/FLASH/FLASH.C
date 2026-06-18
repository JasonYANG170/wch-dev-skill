/********************************** (C) COPYRIGHT *******************************
* File Name          : FLASH.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X EEPROM DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序实现EEPROM读取擦除以及编程过程，CH56X芯片有28K的EEPROM
*                      (4)、写入数据可以通过单片机读取数据也可以使用"WCHISPTool_Setup.exe"软件读取
*                      (5)、本程序演示FLASH擦除写校验功能
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
#define LED_OUT_INIT( LED )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }          /* LED 高电平为输出方向 */
#define LED_OUT_ACT( LED )      { R32_PB_CLR |= LED; }                             /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT( LED )    { R32_PB_OUT |= LED; }                             /* LED 高电平关闭LED显示 */

__align(4)    UINT8 my_buffer[1024];

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
* Function Name  : CH56X_EEPROM
* Description    : EEPROM测试
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void CH56X_EEPROM( void ) 
{
    UINT32 i;
    
    PRINT("Eeprom Erase:\n");
    i = EEPROM_ERASE(0x2000,0x1000);                                            /* 擦除0X2000地址开始的4K数据 */
    if( i ){
        PRINT("    ***Error***\n");
    }
    for( i=0;i!=64;i++ )my_buffer[i] = i;
    PRINT("Eeprom Write:\n");
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
* Function Name  : CH56X_EEPROM
* Description    : flash 测试
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void CH56X_FLASH_CODE( void ) 
{
    UINT32 i;
    
    FLASH_ROM_LOCK( 0x00 );                                                          //先将FLASH解锁
    PRINT("flash Erase:\n");
    i = FLASH_ROM_ERASE( 0X20000,1024*4);                                            /* 擦除0X20000地址开始的4K数据 */
    if( i ){
        PRINT("    ***Erase Error***\n");
    }
    else{
        PRINT("    *** erase ok***\n");
    }
    for( i=0;i!=64;i++ )my_buffer[i] = i;
    PRINT("flash Write:\n");
    i = FLASH_ROM_WRITE( 0X20000,my_buffer,64 );                                    /* 往地址0X20000写64字节的数据 */
    if( i ){
        PRINT("    *** Write Error***\n");
    }
    else{
        PRINT("    *** write ok***\n");
    }
    PRINT("flash  verify:\n");
    i = FLASH_ROM_VERIFY( 0X20000,my_buffer,64 );                                    /*  校验是否正确 */
    if( i ){
        PRINT("    *** verify Error***\n");
    }
    else{
        PRINT("    *** verify ok***\n");
    }
    FLASH_ROM_LOCK(FLASH_ROM_LOCK_AUTO);                                             //全部LOCK
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
    UINT8 i;

    LED_OUT_INIT( (1<<3|1<<4|1<<5) );
    LED_OUT_ACT( (1<<3|1<<4|1<<5) );
    Delay_ms( 100 );
    LED_OUT_INACT( (1<<3|1<<4|1<<5) ); 
    mInitSTDIO( );                                                               
    PRINT("Edit Date and Time is: "__DATE__" , " __TIME__"\n");
    GET_UNIQUE_ID( my_buffer );
    PRINT("chip id: ");
    for(i=0;i<8;i++) PRINT("%-2x ",my_buffer[i]);
    PRINT("\n");
    CH56X_EEPROM( );
    CH56X_FLASH_CODE( );
    while(1);
}

/*********************************** endfile **********************************/
