/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH 
* Version            : V1.0
* Date               : 2015/03/02
* Description        : CH563 TCP/IP 协议族接口
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/


/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

#include "upnp.h"
#include "ISPXT56X.H"

#define CH563NET_DBG                          1
#define  CH563NET_TCP_MSS                     536
#include "CH563NET.H"
/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8    CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                   /* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8    CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                /* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];                         /* Socket信息表，4字节对齐 */
const UINT16 MemNum[8] = {CH563NET_NUM_IPRAW,
                         CH563NET_NUM_UDP,
                         CH563NET_NUM_TCP,
                         CH563NET_NUM_TCP_LISTEN,
                         CH563NET_NUM_TCP_SEG,
                         CH563NET_NUM_IP_REASSDATA,
                         CH563NET_NUM_PBUF,
                         CH563NET_NUM_POOL_BUF
                         };
const UINT16 MemSize[8] = {CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IPRAW_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_UDP_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB_LISTEN),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_SEG),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IP_REASSDATA),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(0),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_POOL_BUF)
                         };
__align(4)UINT8 Memp_Memory[CH563NET_MEMP_SIZE];
__align(4)UINT8 Mem_Heap_Memory[CH563NET_RAM_HEAP_SIZE];
__align(4)UINT8 Mem_ArpTable[CH563NET_RAM_ARP_TABLE_SIZE];

#define CH563NET_TIMEPERIOD                   10                                /* 定时器定时周期，单位mS*/

/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          CH563NET_TCP_MSS                  /* 接收缓冲区的大小 */
#define READ_RECV_BUF_MODE                    0                                 /* socket接收缓冲区读取模式，1：复制，0：不复制 */

UINT8 SocketRecvBuf[8192];                                                      /* socket接收缓冲区 */
/* CH563相关定义 */                                                                   
UINT8 MACAddr[6]  = {0x02,0x03,0x04,0x05,0x06,0x07};                            /* CH563MAC地址 */ 
const UINT8 IPAddr[4]   = {192,168,1,101};                                      /* CH563IP地址 */ 
const UINT8 GWIPAddr[4] = {192,168,1,1};                                        /* CH563网关 */ 
const UINT8 IPMask[4]   = {255,255,255,0};                                      /* CH563子网掩码 */ 

UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */
UINT8 gPort; 
UINT8 Time = 0;                                                                 /* 用于改变端口值 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                                   /* 以太网中断 */
    {                                                                           /* 以太网中断中断服务函数 */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                               /* 定时器中断 */
    {
         CH563NET_TimeIsr(CH563NET_TIMEPERIOD);                                 /* CH563NET_ 定时器中断服务函数 */
         Time++;
         if(Time > 25)
         {
             Time = 0;
             CH563NET_UPNPTimeIsr();
         }
         R8_TMR0_INT_FLAG |= 0xff;                                              /* 清除定时器中断标志 */
   }
}

__irq void FIQ_Handler( void )
{
    while(1);
}

/*******************************************************************************
* Function Name  : SysTimeInit
* Description    : 系统定时器初始化，CH563@100MHZ TIME0 10ms，根据CH563NET_TIMEPERIOD
*                ：来初始化定时器。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTimeInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * CH563NET_TIMEPERIOD;                           /* 设置为10MS定时 */
    R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}

/*******************************************************************************
* Function Name  : InitSysHal
* Description    : 硬件初始化操作，开启TIM0，ETH中断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSysHal(void)
{
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                          /* 开启TIM0中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;                                           /* 开启ETH中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 调试使用，显示错误代码
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
    if (iError == CH563NET_ERR_SUCCESS) return;                                 /* 操作成功 */
#if CH563NET_DBG
    printf("Error: %02X\n", (UINT16)iError);                                    /* 显示错误 */
#endif    
}

/*******************************************************************************
* Function Name  : CH563NET_LibInit
* Description    : 库初始化操作
* Input          : ip      ip地址指针
*                ：gwip    网关ip地址指针
*                : mask    掩码指针
*                : macaddr MAC地址F指针 
* Output         : None
* Return         : 执行状态
*******************************************************************************/
UINT8 CH563NET_LibInit(const UINT8 *ip,const UINT8 *gwip,const UINT8 *mask,const UINT8 *macaddr)
{
    UINT8 i;
    struct _CH563_CFG cfg;
    if(CH563NET_GetVer() != CH563NET_LIB_VER)return 0xfc;                       /* 获取库的版本号，检查是否和头文件一致 */
    CH563NETConfig = LIB_CFG_VALUE;                                             /* 将配置信息传递给库的配置变量 */
    cfg.RxBufSize = RX_BUF_SIZE; 
    cfg.TCPMss   = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
#if CH563NET_DBG                                                         
    printf("CH563NET_Config: %x\n",CH563NETConfig);                           
#endif        
    return (i);                                                                 /* 库初始化 */
}

/*******************************************************************************
* Function Name  : CH563NET_HandleSokcetInt
* Description    : socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSokcetInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT8   upnpflag = 0;
#if  CH563NET_DBG
     //   printf("sockeid：%02x  %02x\n",(UINT16)sockeid,initstat);
#endif

    upnpflag = CH563NET_UPNPCheckSocketAvail(sockeid);
    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
        if(upnpflag)CH563NET_UPNPReceiveDataHandle(sockeid);
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP连接中断 */
    {
#if CH563NET_DBG                                                                /* 产生此中断表示TCP已经连接，可以进行收发数据 */
        printf("TCP Connect Success\n");                           
#endif
        if(upnpflag)CH563NET_UPNPTCPConnectHandle(sockeid);
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {
#if CH563NET_DBG                                                                /* 产生此中断，CH563NET_库内部会将此socket清除，置为关闭*/
        printf("TCP Disconnect\n");                                             /* 应用曾需可以重新创建连接 */
#endif
        if(upnpflag)CH563NET_UPNPTCPDisConnectHandle(sockeid);
        
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {
        if(upnpflag)CH563NET_UPNPTCPDisConnectHandle(sockeid);
#if CH563NET_DBG                                                                /* 产生此中断，CH563NET_库内部会将此socket清除，置为关闭*/
        printf("TCP Timout\n");                                                 /* 应用曾需可以重新创建连接 */
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGlobalInt
* Description    : 全局中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UpnpTest(u8 n);
u8 tmp = 2;
void CH563NET_HandleGlobalInt( void )
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
#if CH563NET_DBG
        printf("UnreachCode ：%d\n",CH563Inf.UnreachCode);                      /* 查看不可达代码 */
        printf("UnreachProto ：%d\n",CH563Inf.UnreachProto);                    /* 查看不可达协议类型 */
        printf("UnreachPort ：%d\n",CH563Inf.UnreachPort);                      /* 查询不可达端口 */
#endif       
    }
    if(initstat & GINT_STAT_IP_CONFLI)                                          /* IP冲突中断 */
    {
#if CH563NET_DBG
       printf("IP interrupt\n");
#endif   
    }
    if(initstat & GINT_STAT_PHY_CHANGE)                                         /* PHY改变中断 */
    {
       i = CH563NET_GetPHYStatus();
       UpnpTest(tmp);                                                           /* 获取PHY状态 */
       tmp++;
#if CH563NET_DBG
       printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif   
    }
    if(initstat & GINT_STAT_SOCKET)                                             /* Socket中断 */
    {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* 读socket中断并清零 */
           if(socketinit)CH563NET_HandleSokcetInt(i,socketinit);                /* 如果有中断则清零 */
       }    
    }
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

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        // default 9600
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        // 四舍五入
    R8_UART0_LCR = 0x80;                                                        // DLAB位置1
    R8_UART0_DIV = 1;                                                           // 预分频
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = 0x03;
    R8_UART0_FCR = 0xC7;
    R8_UART0_IER = RB_IER_TXD_EN;                                               // TXD enable

    R32_PB_SMT |= RXD0|TXD0;                                                    // RXD0 schmitt input, TXD0 slow rate
    R32_PB_PD &= ~ RXD0;                                                        // disable pulldown for RXD0, keep pullup
    R32_PB_DIR |= TXD0;                                                         // TXD0 output enable

//    R8_UART1_IER |= RB_IER_RECV_RDY;                                          /* 使能UART1接收中断 */
    R8_UART0_MCR |= RB_MCR_OUT2;                                                /* 允许串口中断请求输出 */
    
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_UART0;                                         /* 使能IRQ中的UART1中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 全局中断使能 */}

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
* Function Name  : UpnpTest
* Description    : 
* Input          : n 
* Output         : None
* Return         : None
*******************************************************************************/
u8 buf[2];
void UpnpTest(u8 n)
{
   UPNP_MAP upnpmap;

    upnpmap.InternelPort = 9999 + n;
    upnpmap.ExternalPort = 9999 + n;
    upnpmap.UpnpProType = MAP_TYPE_TCP;
    buf[0] = n + 0x30;
    buf[1] = 0;
    upnpmap.MapPortDes = buf;
    upnpmap.IPAddr[0] = 192;
    upnpmap.IPAddr[1] = 168;
    upnpmap.IPAddr[2] = 1;
    upnpmap.IPAddr[3] = 122;
    CH563NET_UPNPStart(&upnpmap);
    if(n > 10)while(1);
}

void UpnpCallBack(u8 errcode)
{
   UpnpTest(tmp);
   tmp++;
   printf("UpnpCallBack = %02x\n",errcode);
   printf("UpnpCallBack = %02x\n",errcode);

}
/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define CH563MAC_BASE_ADDR        0x00406000
#define R32_ETHE_MACCR           (*((PUINT32)(0x88 + CH563MAC_BASE_ADDR)))
#define RB_RCV_ALL                 (1 << 12)
int main( void )
{
    UINT8 i;
    UPNP_CFG upnpsfg;

    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
    i = CH56X_GetMac(MACAddr);    
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError( i );                                                          /* 检查错误 */
#if CH563NET_DBG
    printf("CH563NET_LibInit Success\n");
#endif    
    R32_ETHE_MACCR |= RB_RCV_ALL;
    SysTimeInit( );                                                             /* 系统定时器初始化 */
    InitSysHal( );
    upnpsfg.TcpMss = CH563NET_TCP_MSS;
    upnpsfg.MemoryLen = sizeof(SocketRecvBuf);
    upnpsfg.UpnpMemory = SocketRecvBuf;
    upnpsfg.GateIPAddr[0] = GWIPAddr[0];
    upnpsfg.GateIPAddr[1] = GWIPAddr[1];
    upnpsfg.GateIPAddr[2] = GWIPAddr[2];
    upnpsfg.GateIPAddr[3] = GWIPAddr[3];
    upnpsfg.AppUpnpCallBack = UpnpCallBack;
    CH563NET_UPNPInit(&upnpsfg);
    while(1)
    {
        CH563NET_MainTask( );                                                   /* CH563IP库主任务函数，需要在主循环中不断调用 */
        if(CH563NET_QueryGlobalInt( ))CH563NET_HandleGlobalInt();               /* 查询中断，如果有中断，则调用全局中断处理函数 */
        CH563NET_UPNPMainTask();
    }
}

/*********************************** endfile **********************************/
