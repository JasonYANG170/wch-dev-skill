/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0_HOST.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SPI0_HOST DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、SPI0主机例子程序,查询方式发送和接收数据.
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
UINT8 SPI_BUF[1024];

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
* Function Name  : SPI_MASTER_INIT
* Description    : SPI主机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SPI_MASTER_INIT ( void )
{
    R8_SPI0_CTRL_MOD &= ~ RB_SPI_MODE_SLAVE;                                    /* 模式设置寄存器,主机模式 */
    R8_SPI0_CTRL_MOD |= RB_SPI_MOSI_OE|RB_SPI_SCK_OE;                           /* MOSI,SCK output enable */
    R8_SPI0_CTRL_MOD |= ( RB_SPI_FIFO_DIR | RB_SPI_ALL_CLEAR );                 /* 清除FIFO,FIFO方向默认设置为输入*/
    R8_SPI0_CTRL_MOD &= ~RB_SPI_ALL_CLEAR;                                      /* 清除FIFO,FIFO方向默认设置为输入*/
    R8_SPI0_CLOCK_DIV = 0x0a;                                                   /* 10分频。100/10=10M */
    R32_PB_DIR |= (MOSI | SCK0 | SCS );                                         /* MOSI，SCK0, SCS为输出 */
    R32_PB_PU  |=  SCS ;
    R8_SPI0_CTRL_DMA = 0x00;                                                    /* 不启动DMA方式 */
}

/*******************************************************************************
* Function Name  : SPI0_MasterTransByte
* Description    : 发送数据
* Input          : data -待发送数据 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0_MasterTransByte( UINT8 data )
{
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /* 设置方向为输出 */
    R16_SPI0_TOTAL_CNT = 1;                                                     /* 长度置为1 */
    R32_SPI0_FIFO = data;                                                       /* 往FIFO里写入数据，当FIFO有数据且长度不为0时则会启动传输 */
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* 等待数据发送完成 */
    R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;                                        /* 设置默认方向为输入 */
}

// 使用SPI数据缓冲区寄存器发送数据
void SPI0_MasterTrans1( UINT8 data )
{
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                        /* 方向位置0 */
    R8_SPI0_BUFFER = data;                                                       /* 写数据并启动传输 */
    while( R8_SPI0_INT_FLAG&RB_SPI_FREE ==0 );                                   /* 等待数据传输完成 */
    R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;                                         /* 设置默认方向为输入 */
}

// 使用FIFO发送多字节
void SPI0_MasterTrans( UINT16 len, UINT8 *pbuf )
{
    UINT16 sendlen;
    
    sendlen = len;
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /* 设置数据方向为输出 */
    R16_SPI0_TOTAL_CNT = len;                                                   /* 设置要发送的数据长度 */
    while( sendlen ){                                                           /* 设置要发送的数据长度 */
        if( R8_SPI0_FIFO_COUNT < SPI0_FIFO_SIZE ){                              /* FIFO中未发送数据个数小于FIFO大小则可以继续往FIFO写数据 */
            R32_SPI0_FIFO = *pbuf;
            pbuf++;
            sendlen--;
        }
    }
    while( R8_SPI0_FIFO_COUNT != 0 );                                           /* 等待FIFO中的数据全部发送完成 */
    R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;                                        /* 设置默认方向为输入 */
}

/*******************************************************************************
* Function Name  : SPI0_MASTER_Recv
* Description    : 接收数据
* Input          : None
* Output         : None
* Return         : data -接收到数据
*******************************************************************************/
UINT8 SPI0_MasterRecvByte( void )
{
    R16_SPI0_TOTAL_CNT = 1;                                                     /* 长度置为1 */
    while( R8_SPI0_FIFO_COUNT == 0 );                                           /* 等待数据回来 */
    return R8_SPI0_FIFO;                                                        /* 读FIFO中的数据 */
}
// 使用SPI数据缓冲区寄存器读取数据
void SPI0_MasterRecv1( UINT8 len, UINT8 *pbuf )
{
    R8_SPI0_BUFFER = 0xff;                                                      /* 启动传输*/
    while(len>1){
//        Delay_us( 10 );
        *pbuf = R8_SPI0_BUFFER;                                                 /* 读取数据，并启动传输 */
        pbuf++;
        len--;
    }
//    Delay_us( 10 );
    R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;                                       /* 方向位置输出 */
    *pbuf = R8_SPI0_BUFFER;                                                     /* 读取最后一字节 */
    R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;                                        /* 设置默认方向为输入 */
}

// 使用FIFO读取多字节数据
void SPI0_MasterRecv( UINT16 len, UINT8 *pbuf )
{
    UINT16 readlen;
    
    readlen = len;
    R16_SPI0_TOTAL_CNT = len;                                                   /* 设置需要接收的数据长度，FIFO方向为输入长度不为0则会启动传输 */
    while( readlen ){                                                           /* 剩余数据长度，从FIFO里读数据，每读一字节长度会自动减1 */
        if( R8_SPI0_FIFO_COUNT ){                                               /* 查询FIFO里是否有数据 */
            *pbuf = R8_SPI0_FIFO;                                               /* 读数据 */
            pbuf++;
            readlen--;
        }
    }
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
    UINT8 spi_buf[1024];

    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 串口0初始化 */
    PRINT("START SPI0 HOST:\n");  
    SPI_MASTER_INIT ( );                                                        /* 主机模式初始化 */
    R32_PB_CLR |= SCS;                                                          /* 片选使能 */
    while(1){
        SPI0_MasterTransByte(0xaa);                                             /* 发送数据 */
        Delay_ms( 10);
        SPI0_MasterRecv( 100, spi_buf);
        PRINT("\r\n");
        for(i=0;i<10;i++) PRINT("%2lX ", spi_buf[i]);                           /* 通过串口0打印出来 */
        PRINT("\r\n");
        Delay_ms( 5000 );
    }
}

/*********************************** endfile **********************************/
