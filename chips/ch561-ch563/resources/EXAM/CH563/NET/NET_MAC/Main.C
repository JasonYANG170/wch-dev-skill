/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 NET Demo
*                      (1)、串口0输出监控信息,115200bps;    
*                      (2)、演示CH563底层收发数据;
*                      (3)、主动发送ARP数据包，以及应答ARP数据包。
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "NETMAC.H"
#include "ISPXT56X.H"


/******************************************************************************/
/* 变量定义 */
__align(16) UINT8  TxDesBuf[TX_QUEUE_NUM*16];
__align(16) UINT8  RxDesBuf[RX_QUEUE_NUM*16];
__align(4)  UINT8  MACRxBuf[RX_QUEUE_NUM][RX_BUF_SIZE];                         /* MAC接收缓冲区，4字节对齐 */
__align(4)  UINT8  MACTxBuf[TX_QUEUE_NUM][TX_BUF_SIZE];                         /* MAC发送缓冲区，4字节对齐 */
UINT8   RecvBuf[RX_BUF_SIZE];
UINT8   CH563MACAddr[MACADDR_LEN]= {0x02,0x23,0x34,0x45,0x56,0x67};             /* MAC地址 */
UINT8   CH563IPAddr[IPADDR_LEN]  = {192,168,1,10};
UINT8   IPDestAddr[IPADDR_LEN]   = {192,168,1,1};
PUINT8  MACSendbuf;

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000){                                                  /* 以太网中断 */
        MAC_Isr( );                                                             /* 处理中断事务 */
    }
    if(R32_INT_FLAG & RB_IF_TMR0){                                              /* 定时器中断 */
         NET_TimeIsr( 1000 );                                                   /* 定时器中断服务函数 */
         R8_TMR0_INT_FLAG |= 0xff;                                              /* 清除定时器中断标志 */
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
* Function Name  : SysTimeInit
* Description    : 系统定时器初始化，CH563@100MHZ TIME0 10ms，根据CH563NETTIMEPERIOD
*                ：来初始化定时器。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTimeNetInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * 1000;                                          /* 设置为10000MS定时 */
    R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                          /* 开启TIM0中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;                                           /* 开启ETH中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启IRQ全局中断 */
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
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为28，清发送和接收FIFO，FIFO使能 */
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
* Function Name  : NET_TimeIsr
* Description    : 发送ARP请求
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  NET_TimeIsr( UINT16 time )
{
    ARP_Output(IPDestAddr,ARP_CODE_QUEST);
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
    UINT16  len;
     
    Delay_ms( 100 );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
#if  CH563_DEBUG
    printf("***********************************\r\n");
    printf("start net  0025:\r\n");
    printf("***********************************\r\n");
#endif
    CH56X_GetMac(CH563MACAddr);
    NET_INIT( );
    SysTimeNetInit( );
    while(1){
        if( MAC_QueryInput( ) ){
            MAC_Input(RecvBuf,&len);  
            if( ETHER_TYPE_ARP == GLOB_nTohs(   ETH.Head.PROTOCOL ) ){
#if  CH563_DEBUG
    printf("arp request*****\r\n");
#endif
                ARP_Input(RecvBuf,len );
            }
            else{
#if  CH563_DEBUG
                printf("pro_type=%-8x",GLOB_nTohs( ETH.Head.PROTOCOL ) );
#endif
            }
        }
    }
}

/*********************************** endfile **********************************/
