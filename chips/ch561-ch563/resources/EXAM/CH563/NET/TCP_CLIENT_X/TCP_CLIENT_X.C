/********************************** (C) COPYRIGHT ******************************
* File Name          : TCP_CLIENT_X.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563NET库演示文件
*                      (1)、CH563 Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、本程序用本程序用于演示tcp　client在发送非复制方式下通讯，单片机收到数据后，回传给远端，ARM966,Thumb,小端
*******************************************************************************/


/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define CH563NET_MAX_SOCKET_NUM              9                                  /* Socket的个数，用户可以配置，默认为4个Socket */
#define CH563NET_NUM_TCP                     8                                  /* TCP连接的个数 */ 
#define MISCE_CFG0_TCP_SEND_COPY             0                                  /* TCP发送缓冲区复制 */
#define SEND_BUF_COUNT                       16
#define CH563NET_NUM_PBUF                    12                                 /* PBUF结构的个数 */

#include "CH563NET.H"
#define CH563NET_DBG                          1

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

/******************************************************************************/
/* CH563相关定义 */
UINT8 MACAddr[6] =  {0x02,0x03,0x04,0x05,0x06,0x17};                            /* CH563MAC地址 */
const UINT8 IPAddr[4] =   {192,168,1,2};                                        /* CH563IP地址 */
const UINT8 GWIPAddr[4] = {192,168,1,1};                                        /* CH563网关 */
const UINT8 IPMask[4] =   {255,255,255,0};                                      /* CH563子网掩码 */
const UINT8 DESIP[4] =    {192,168,1,100};   

#define RECE_BUF_LEN                  (CH563NET_TCP_MSS)             

UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */
__align(4) UINT8 SocketRecvBuf[CH563NET_NUM_TCP+1][RECE_BUF_LEN];               /* socket接收缓冲区 */
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* 定义一个临时缓冲区 */
UINT8 SendBuf[SEND_BUF_COUNT][RECE_BUF_LEN];
UINT8 connect = 0;
UINT8 socketnum =0;
/******************************************************************************/
/******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                        /* 以太网中断 */
    {                                                                 /* 以太网中断中断服务函数 */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                     /* 定时器中断 */
    {
         CH563NET_TimeIsr(CH563NETTIMEPERIOD);                       /* 定时器中断服务函数 */
         R8_TMR0_INT_FLAG |= 0xff;                                   /* 清除定时器中断标志 */
    }
}

/******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{

}


__packed struct _SEND_CTRL
{
   UINT32 BufAddr;
   UINT8  Status;                                                               /*0为空闲，1为忙*/
   UINT8  SocketID;                                                             
};
__packed struct _SEND_CTRL  SendCtrl[SEND_BUF_COUNT];


UINT32 AddrList[32];
void TCPSendOKCallBack(struct _SCOK_INF *socinf,UINT32 recv1,UINT16 recv2,UINT8 *buf,UINT32 len)
{
    UINT8  socketid=0xff;
    UINT8  i,count;
    UINT8  liscount;

    for(i=0;i<CH563NET_MAX_SOCKET_NUM;i++){
        if(socinf == &SocketInf[i]){
            socketid = i;
            break;
        }
    }
    liscount = CH563NET_QueryUnack(socinf,AddrList,sizeof(AddrList));
    for(count = 0; count < SEND_BUF_COUNT; count++){                            /* AddrList保存了当前还没发送成功的缓冲区，所以先将各缓冲区发送状态置为成功 */
        if( SendCtrl[count].SocketID == socketid ){
            SendCtrl[count].Status = 0;                                         /* 缓冲区状态为0，表示缓冲区空闲，可以继续使用 */
            if( len&&liscount ){
                for(i = 0; i < liscount; i++){                                  /* 循环读出未发送成功的缓冲区地址 */
                    if(SendCtrl[count].BufAddr == AddrList[i]){                 /* 查看此地址是否在socket发送缓冲区地址 */
                        SendCtrl[count].Status = 1;                             /* 此缓冲区不可以用，还没发送完毕 */
                    }
                }
            }
        }
    }    
}

/******************************************************************************
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
    R32_TMR0_COUNT   = 0x00000000; 
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
    R8_INT_EN_FIQ_GLOB |= RB_IE_FIQ_GLOB;
}

/******************************************************************************
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
    cfg.TCPMss   = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
    return (i);                      
}


/*******************************************************************************
* Function Name  : CH563NET_CreatTcpSocket
* Description    : 创建TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_CreatTcpClient( UINT16 dport,UINT16 sport,UINT8 count)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   printf("Creat Socekt Tcp Client%-2d\n",count);
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort = dport;                                                /* 设置目的端口 */
   TmpSocketInf.SourPort = sport;                                               /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[count];                  /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   TmpSocketInf.AppCallBack = TCPSendOKCallBack;                                /* 发送回调函数 */
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   i = CH563NET_SocketConnect(SocketId);                                        /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */

}

/******************************************************************************
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
    UINT8  i=0;
    UINT32 totallen=0;
    UINT8 *p;

    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
//        printf("SINT_STAT_RECV \n");
        len = CH563NET_SocketRecvLen(sockeid,NULL);                    
        for(i = 0; i < SEND_BUF_COUNT; i++){
            if(SendCtrl[i].Status == 0){
                if( len>RECE_BUF_LEN ) len = RECE_BUF_LEN;
                CH563NET_SocketRecv(sockeid,(UINT8 *)SendCtrl[i].BufAddr,&len);    
                p = (UINT8 *)SendCtrl[i].BufAddr;
                SendCtrl[i].Status = 1;
                SendCtrl[i].SocketID = sockeid;
                totallen = len;
                while(totallen){
                   len = totallen;
                   i = CH563NET_SocketSend(sockeid,p,&len);                     /* 将MyBuf中的数据发送 */
                   totallen -= len;                                             /* 将总长度减去已经发送完毕的长度 */
                   p += len;                                                    /* 将缓冲区指针偏移*/
                }
                break;
            }
        }

    }
    if( initstat & SINT_STAT_CONNECT )                                          /* TCP连接中断 */
    {
        printf("SINT_STAT_CONNECT \n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {
        printf("SINT_STAT_DISCONNECT\n");                                   
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {
        printf("SINT_STAT_TIM_OUT\n");                           
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGlobalInt
* Description    : 全局中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
        printf("UnreachCode ：%d\n",CH563Inf.UnreachCode);                      /* 查看不可达代码 */
        printf("UnreachProto ：%d\n",CH563Inf.UnreachProto);                    /* 查看不可达协议类型 */
        printf("UnreachPort ：%d\n",CH563Inf.UnreachPort);                      /* 查询不可达端口 */
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
   
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY改变中断 */
   {
       i = CH563NET_GetPHYStatus();                                             /* 获取PHY状态 */
       printf("GINT_STAT_PHY_CHANGE %02x\n",i);

   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket中断 */
   {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* 读socket中断并清零 */
           if(socketinit)CH563NET_HandleSockInt(i,socketinit);                  /* 如果有中断则清零 */
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
    R32_PB_PD  &= ~ RXD0;                                                       /* disable pulldown for RXD0, keep pullup */
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
int    main(void) 
{

    UINT8 i = 0;
    Delay_ms(100);
    mInitSTDIO( );
    i = CH56X_GetMac(MACAddr);
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
    printf("CH563IPLibInit Success %8lx\n",(UINT32)CH563MACRxBuf);

    for(i = 0; i < SEND_BUF_COUNT;i++)
    {
        SendCtrl[i].BufAddr = (UINT32)&SendBuf[i][0];
         SendCtrl[i].Status = 0;
    }
    SysTimeInit( );                                                             /* 系统定时器初始化 */
    InitSysHal( );                                                              /* 初始化中断 */

    for(socketnum=0;socketnum<CH563NET_NUM_TCP;socketnum++){
        CH563NET_CreatTcpClient(2000,6000+socketnum,socketnum);
    }
      printf("mian task\n"); 
    while(1)
    {
        CH563NET_MainTask();                                                    /* CH563NET库主任务函数，需要在主循环中不断调用 */
        if(CH563NET_QueryGlobalInt())CH563NET_HandleGlobalInt();                /* 查询中断，如果有中断，则调用全局中断处理函数 */
    }
}


/****************************endfile@tech9*************************************/
