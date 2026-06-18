/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/02/27
* Description        : CH563NET库演示
*                    : MDK3.36@ARM966E-S,Thumb
*******************************************************************************/

/*******************************************************************************
CH563 TCP/IP 协议族接口
CH563NET库演示文件，本程序用于演示UDP通讯，单片机收到数据后，回传给远端。
ARM966,Thumb,小端
*******************************************************************************/

/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"


void ResetSys(void);
/****************************************************************************/

/* 库配置 */
/* MAC 收发缓冲区配置 */
/* CH563NET.LIB 内部会分配 (RX_QUEUE_ENTRIES + TX_QUEUE_ENTRIES)*MAX_PKT_SIZE 个字节的缓冲区用于以太网的数据收发 */
/* 用户可以修改RX_QUEUE_ENTRIES和TX_QUEUE_ENTRIES来减少或者增加内部缓冲区, TX_QUEUE_ENTRIES定义为2即可，但是RX_QUEUE_ENTRIES*/
/* 则要根据实际情况进行调整，如果需要大批量的数据进行收发，RX_QUEUE_ENTRIES必须调大，否则可能会丢失数据。*/
#define CH563NET_PING_ENABLE                 TRUE                    /* PING开启，FALSE为关闭，不定义则开启 */
/* 包含库的头文件，务必在上面的配置后包含文件，否则配置无效 */


#define CH563NET_MAX_SOCKET_NUM               4                                 /* Socket的个数，用户可以配置，默认为4个Socket,最大为32 */
#define RECE_BUF_LEN                          (CH563NET_TCP_MSS * 2  )                               /* ???????? */
#define CH563NET_TCP_MSS                      536                               /* tcp MSS的大小*/
#define CH563NET_DBG                          1

#include "CH563NET.H"

/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8  CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                     /* MAC接收描述符缓冲区，16字节对齐 */
__align(4)UINT8   CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                  /* MAC接收缓冲区，4字节对齐 */
__align(4)SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];                          /* Socket信息表，4字节对齐 */
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

/*=============================================================================*/
/* CH563相关定义 */
UINT8 MACAddr[6] = {0x02,0x03,0x04,0x05,0x06,0x07};                             /* CH563MAC地址 */

UINT8 IPAddr[4] = {192,168,1,11};                                               /* CH563IP地址 */
UINT8 GWIPAddr[4] = {192,168,1,1};                                              /* CH563网关 */
UINT8 IPMask[4] = {255,255,255,0};                                              /* CH563子网掩码 */
UINT8 RemoIP[4] = {192,168,1,3};                                                /* CH563子网掩码 */
UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */

UINT8 SocketRecvBuf[CH563NET_MAX_SOCKET_NUM][RECE_BUF_LEN];                     /* socket接收缓冲区 */
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* 定义一个临时缓冲区 */
#define SEND_BUF_COUNT                               5
UINT8 UDPServerFlag = 0 ;

UINT8 DNSHostIP[4] = {218,2,135,1};

UINT16 DNSPort = 53;
UINT8 RemoteIp[4];

UINT8 DNSGetFlag = 0;
/******************************************************************************/
/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                                   /* 以太网中断 */
    {                                                                           /* 以太网中断中断服务函数 */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                               /* 定时器中断 */
    {
         CH563NET_TimeIsr(CH563NETTIMEPERIOD);                                  /* 定时器中断服务函数 */
         R8_TMR0_INT_FLAG |= 0xff;                                              /* 清除定时器中断标志 */
    }
}


__irq void FIQ_Handler( void )
{

}

/*******************************************************************************
* Function Name  : SysTimeInit
* Description    : 系统定时器初始化，CH563@100MHZ TIME0 10ms，根据CH563NETTIMEPERIOD
*                ：来初始化定时器。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTimeInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * CH563NETTIMEPERIOD;                            /* 设置为10MS定时 */
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
    printf("Error: %02X\n", (UINT16)iError);                                    /* 显示错误 */
}

/*******************************************************************************
* Function Name  : CH563NET_LibInit
* Description    : 库初始化操作
* Input          : ip      ip地址指针
*                ：gwip    网关ip地址指针
*                 : mask    掩码指针
*                 : macaddr MAC地址指针 
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
    cfg.TCPMss = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
    return (i);                      /* 库初始化 */
}

/*******************************************************************************
* Function Name  : CH563NET_HandleSockInt
* Description    : Socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT32 totallen;
    UINT8 i,*p = MyBuf;

    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
        len = CH563NET_SocketRecvLen(sockeid,NULL);                             /* 会将当前接收指针传递给precv*/
        CH563NET_SocketRecv(sockeid,MyBuf,&len);                                /* 将接收缓冲区的数据读到MyBuf中*/
        totallen = len;
        while(1)
        {
           len = totallen;
           i = CH563NET_SocketSend(sockeid,p,&len);                             /* 将MyBuf中的数据发送 */
           totallen -= len;                                                     /* 将总长度减去以及发送完毕的长度 */
           p += len;                                                            /* 将缓冲区指针偏移*/
           if(totallen)continue;                                                /* 如果数据未发送完毕，则继续发送*/
           break;                                                               /* 发送完毕，退出 */
        }
    
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP连接中断 */
    {
#if CH563NET_DBG
        printf("TCP CONNECT :SockID:%d \n",(UINT16)sockeid);
#endif
        CH563NET_ModifyRecvBuf(sockeid,(UINT32)SocketRecvBuf[sockeid],RECE_BUF_LEN);  

    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {
#if CH563NET_DBG
        printf("SINT_STAT_DISCONNECT\n");                           
#endif
        CH563NET_SocketClose(sockeid,0);
        
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {
#if CH563NET_DBG
        printf("SINT_STAT_TIM_OUT\n");                           
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGloableInt
* Description    : 全局中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleGloableInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
      
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
   
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY改变中断 */
   {
        i = CH563NET_GetPHYStatus();                                             /* 获取PHY状态 */
#if CH563NET_DBG
        printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif  
        if(i&PHY_DISCONN)
        {
          
        }
        else
        {
             DNSGetFlag = 1;
        }
        
   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket中断 */
   {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* 读socket中断并清零 */
           if(socketinit)CH563NET_HandleSockInt(i,socketinit);                  /* 如果有中断则清零 */
       }    
   }
}



/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : 串口初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mInitSTDIO(void)
{
    UINT32    x, x2;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;  // 115200bps
//    x = 10 * 62500000 * 2 / 16 / 115200;  // 默认是62.5MHz
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;  // 四舍五入
    R8_UART0_LCR = 0x80;  // DLAB位置1
    R8_UART0_DIV = 1;  // 预分频
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;
    R8_UART0_LCR = 0x03;
    R8_UART0_FCR = 0xC7;
    R8_UART0_IER = RB_IER_TXD_EN;  // TXD enable
    R32_PB_SMT |= RXD0|TXD0;  // RXD0 schmitt input, TXD0 slow rate
    R32_PB_PD &= ~ RXD0;  // disable pulldown for RXD0, keep pullup
    R32_PB_DIR |= TXD0;  // TXD0 output enable
}

/* 通过串口输出监控信息 */
int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                            // 发送数据
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );       // 等待数据发送
    return( c );
}

/*******************************************************************************
* Function Name  : CH563NET_DNSCallBack
* Description    : DNS回调函数
* Input          : None
* Output         : None
* Return         : 执行状态
*******************************************************************************/
void  CH563NET_DNSCallBack(const  char  *name,  UINT8  *ipaddr,  void  *callback_arg) 
{ 
    if(ipaddr == NULL) 
    { 
        printf("DNS Fail\n"); 
        return; 
    } 
#if CH563NET_DBG
    printf("Host Name = %s\n",name); 
    printf("IP= %d.%d.%d.%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]); 
    if(callback_arg != NULL) 
    { 
        printf("callback_arg = %02x\n",(*(UINT8 *)callback_arg)); 
    } 
#endif
} 

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void) 
{
    UINT8 i = 0;

    mInitSTDIO();
    CH563NET_InitDNS(DNSHostIP,DNSPort); 
    i = CH56X_GetMac(MACAddr);
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
    SysTimeInit();                                                              /* 系统定时器初始化 */
    InitSysHal();                                                               /* 初始化中断 */
    while(1)
    {
        CH563NET_MainTask();                                                    /* CH563NET库主任务函数，需要在主循环中不断调用 */
        if(CH563NET_QueryGlobalInt())CH563NET_HandleGloableInt();               /* 查询中断，如果有中断，则调用全局中断处理函数 */
        if(DNSGetFlag)
        {
            DNSGetFlag = 0;
            i= CH563NET_GetHostName("www.wch.cn",RemoteIp,CH563NET_DNSCallBack,NULL);
            if(i == 0) 
            { 
    #if CH563NET_DBG
                printf("563NET_GetHostName Success\n"); 
                printf("IP= %d.%d.%d.%d\n",RemoteIp[0],RemoteIp[1],RemoteIp[2],RemoteIp[3]); 
    #endif
                while(1);
            } 
        }
    }
}


/****************************endfile@tech9*************************************/
