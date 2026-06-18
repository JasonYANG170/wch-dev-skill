/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1_INT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH56X UART1_INT DEMO
*                      (1)、CH56X Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、串口1中断的使用及中断的处理，以及字节收发子程序，FIFO收发子程序.
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

/******************************************************************************/
/* 变量定义 */
void UART1_SendStr( UINT8 *str );  
void  Seril1Send( UINT8 *Data, UINT8 Num );
UINT8  Seril1Rcv( UINT8 *buf );
void UART1_SendByte( UINT8 dat ) ;

UINT8 SEND_STRING[ ] = { "l am uart1! \n" };
UINT8 SEND_STRING1[ ] = { "Modem Change!\n" };
UINT8  MyBuf[ 5000 ]; 

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
      
    UINT8 RcvNum = 0;
    UINT8 ReadDat = 0;

     
    if (R8_INT_FLAG_0&RB_IF_UART1){
        switch( R8_UART1_IIR & RB_IIR_INT_MASK ){
            case UART_II_MODEM_CHG :                                            /* Modem 信号变化 */
                UART1_SendStr(SEND_STRING1);
                if ( R8_UART1_MSR == 0x30 ){                                    /* Modem 信号发送数据变化 */ 
                    UART1_SendStr( SEND_STRING1 );
                }
                else if ( R8_UART1_MSR == 0x11 ){                               /* Modem 信号接收数据变化 */ 
                    R8_UART1_THR= 0xAA;
                }
                else if ( R8_UART1_MSR == 0x22 ){                               /* Modem 信号接收数据变化 */ 
                    RcvNum = Seril1Rcv( MyBuf );
                    Seril1Send( MyBuf, RcvNum );
                }
                break;
            case UART_II_NO_INTER :                                             /* 没有中断 */ 
                break;
            case UART_II_THR_EMPTY:                                             /* 发送保持寄存器空中断 */
                break;
            case UART_II_RECV_RDY:                                              /* 串口接收可用数据中断 */
                RcvNum = Seril1Rcv( MyBuf );
                Seril1Send( MyBuf, RcvNum );
                break;
            case UART_II_LINE_STAT:                                             /* 接收线路状态中断 */
                ReadDat = R8_UART1_LSR;
                PRINT("ReadDat = %x\n",ReadDat);
                break;
            case UART_II_RECV_TOUT:                                             /* 接收数据超时中断 */
                RcvNum = Seril1Rcv( MyBuf );
                Seril1Send( MyBuf, RcvNum );
                break;
            default:                                                            /* 不可能发生的中断 */ 
                break;
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
* Function Name  : Uart1_Init
* Description    : 串口1初始化
* Input          : baud-串口波特率，最高为主频1/8
* Output         : None
* Return         : None
*******************************************************************************/

void Uart1_Init( UINT32 baud )
{
    UINT32 x;

    x = 10 * FREQ_SYS/ 8 / baud;                                                /* 115200bps */
    x += 5;                                                                     /* 四舍五入 */
    x /= 10;
    R8_UART1_LCR = RB_LCR_DLAB;                                                 /* DLAB位置1 */
    R8_UART1_DIV = 1;                                                           /* 预分频 */
    R8_UART1_DLM = x>>8;
    R8_UART1_DLL = x&0xff;

    R8_UART1_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8    */
    R8_UART1_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为28，清发送和接收FIFO，FIFO使能 */
    R8_UART1_IER = RB_IER_TXD_EN | RB_IER_LINE_STAT |RB_IER_THR_EMPTY | 
                   RB_IER_RECV_RDY  ;                                           /* TXD enable */
    R8_UART1_MCR = RB_MCR_OUT2;                                                
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_UART1;                                         /* 串口中断输出使能 */
    R32_PB_SMT |= RXD1|TXD1;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PD  &= ~ RXD1;                                                       /* disable pulldown for RXD1, keep pullup */
    R32_PB_DIR |= TXD1;                                                         /* TXD1 output enable */
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
* Input          : c  -writes the character specified by c 
*                  *f -the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int    fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/*******************************************************************************
* Function Name  : UART1_SendByte
* Description    : 串口1发送一字节子程序
* Input          : dat -要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendByte( UINT8 dat )   
{        
    R8_UART1_THR  = dat;
    while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                         /* 等待数据发送 */       
}

/*******************************************************************************
* Function Name  : UART1_SendStr
* Description    : 串口1发送字符串子程序
* Input          : *str -要发送字符串的指针
* Output         : None
* Return         : None
*******************************************************************************/

void UART1_SendStr( UINT8 *str )   
{
    while( 1 ){
        if( *str == '\0' ) break;
        UART1_SendByte( *str++ );
    }
}

/*******************************************************************************
* Function Name  : UART1_RcvByte
* Description    : 串口1接收一字节子程序
* Input          : None
* Output         : None
* Return         : Rcvdat -接收到的数据
*******************************************************************************/

UINT8 UART1_RcvByte( void )    
{
    UINT8 Rcvdat = 0;
    
    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ){
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* 等待数据准备好 */ 
        Rcvdat = R8_UART1_RBR;                                                  /* 从接收缓冲寄存器读出数据 */ 
    }
    else{
        R8_UART1_RBR;                                                           /* 有错误清除 */
    }
    return( Rcvdat );
}

/*******************************************************************************
* Function Name  : Seril1Send
* Description    : 串口1发送多字节子程序
* Input          : *Data -待发送数据区指针
*                  Num   -发送数据长度
* Output         : None
* Return         : None
*******************************************************************************/

void  Seril1Send( UINT8 *Data, UINT8 Num )                        
{
    do{
        while( ( R8_UART1_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                    /* 等待数据发送完毕 */ 
        R8_UART1_THR  = *Data++;  
    }while( --Num );
}

/*******************************************************************************
* Function Name  : Seril1Rcv
* Description    : 禁用FIFO,串口1接收多字节子程序
* Input          : *pbuf -接收缓冲区指针
* Output         : None
* Return         : RcvNum -接收到数据的数据长度
*******************************************************************************/

UINT8  Seril1Rcv( UINT8 *pbuf )    
{
    UINT8 RcvNum = 0;

    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ){
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* 等待数据准备好 */ 
        do{
            *pbuf++ = R8_UART1_RBR;                                             /* 从接收缓冲寄存器读出数据 */ 
            RcvNum++;
        }while( ( R8_UART1_LSR & RB_LSR_DATA_RDY   ) == 0x01 );
    }
    else{
        R8_UART1_RBR;
    }
    return( RcvNum );
}

/*******************************************************************************
* Function Name  : UART1Send_FIFO
* Description    : 启用FIFO,一次最多32字节，CH432串口1发送多字节子程序
* Input          : *Data -待发送数据区指针
*                  Num   -发送数据长度
* Output         : None
* Return         : None
*******************************************************************************/

void UART1Send_FIFO( UINT8 *Data, UINT8 Num ) 
{
    int i;

    while( 1 ){
        while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                     /* 等待数据发送完毕，THR,TSR全空 */
        if( Num <= 32){                                                         /* FIFO长度为32，数据长度不满32字节，一次发送完成 */
            do{
                R8_UART1_THR=*Data++;
            }while(--Num) ;
            break;
        }
        else{                                                                   /* FIFO长度为32，数据长度超过32字节，分多次发送，一次32字节 */
            for(i=0;i<32;i++){
                R8_UART1_THR=*Data++;
            }
            Num -= 32;
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
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */ 
    Uart1_Init( 115200 );
    memset(MyBuf,0,sizeof(MyBuf));
    PRINT("start:\n");
    R8_INT_EN_IRQ_GLOB  = RB_IE_IRQ_GLOB;                                       /* 全局中断使能 */
    while ( 1 ) ;
}

/*********************************** endfile **********************************/
