/********************************** (C) COPYRIGHT ******************************
* File Name          : MAIL_SMTP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 SMTP DEMO
*                      (1)、CH563 Examples by KEIL;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、CH563NET库演示文件，本程序用于演示发送邮件功能，ARM966,Thumb,小端
*******************************************************************************/

/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
/* 库配置 */
#include "CH563NET.H"
#include "ISPXT56X.H"

#define CH563NET_DBG                          0

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

#include "SMTP.H"
#define  CH563NET_TIMEPERIOD                 50
/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          536*3                             /* 接收缓冲区的大小 */
/* CH563IP库TCP的MSS长度为536字节，即一个TCP包里的数据部分最长为536字节 */
/* TCP协议栈采用滑动窗口进行流控，窗口最大值为socket的接收缓冲区长度。在设定 */
/* RX_QUEUE_ENTRIES时要考虑MSS和窗口之间的关系，例如窗口值为4*MSS，则远端一次会发送 */
/* 4个TCP包，如果RX_QUEUE_ENTRIES小于4，则必然会导致数据包丢失，从而导致通讯效率降低 */
/* 建议RX_QUEUE_ENTRIES要大于( 窗口/MSS ),如果多个socket同时进行大批量发送数据，则 */
/* 建议RX_QUEUE_ENTRIES要大于(( 窗口/MSS )*socket个数) 在多个socket同时进行大批数据收发时 */
/* 为了节约RAM，请将接收缓冲区的长度设置为MSS */
/******************************************************************************/
/* CH563相关定义 */                                                                   
UINT8 MACAddr[6] = {0x02,0x03,0x04,0x05,0x06,0x07};                             /* CH563MAC地址 */ 
const UINT8 IPAddr[4] =  {192,168,1,10};                                        /* CH563IP地址 */ 
const UINT8 GWIPAddr[4] ={192,168,1,1};                                         /* CH563网关 */ 
const UINT8 IPMask[4] =  {255,255,255,0};                                       /* CH563子网掩码 */

/* socketsmtp 相关定义,SMTP发送邮件    */
const UINT16 SmtpSourPrt = 3510;
const UINT8  SocketsmtpIP[4] = {113,108,16,44};                                 /* Socket smtp目的IP地址 */ 
//    {123,58,178,201}    "smtp.126.com"
//    {123,58,178,204}    "smtp.163.com"
//    {113,108,16,44}     "smtp.qq.com"
//    {74,125,129,108}    "smtp.gmail.com"

/* smtp发送邮件相关参数 */
const char  *m_Server =   "smtp.qq.com";                                        /* 服务器名称 */
const char    *m_UserName = "XXX";                                              /* 用户名 */
const char    *m_PassWord = "******";                                           /* 密码 */
const char    *m_SendFrom = "XXX@qq.com";                                       /* 发件人地址 */
const char    *m_SendName = "one";                                              /* 发送人名字 */
const char    *m_SendTo =   "XXX@qq.com";                                       /* 收件人地址 */
const char    *m_Subject =  "test";                                             /* 主题 */
const char  *m_FileName = "m_file.txt";                                         /* 附件名字(如果不发送附件,则置为"\0") */

/* 变量相关定义 */
UINT8     ReceDatFlag = 0;
UINT8     CheckType;
UINT8     OrderType;
UINT16     ReceLen;
extern SMTP    *p_smtp;
UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */
UINT8 SocketRecvBuf[CH563NET_MAX_SOCKET_NUM][RECE_BUF_LEN];                     /* socket接收缓冲区 */

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
#if DEBUG
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
* Function Name  : CH563IPHandleSokcetInt
* Description    : socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSokcetInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT8 i;

    if(initstat & SINT_STAT_RECV)                                               /* 接收中断 */
    {
        ReceDatFlag = 1;
        len = CH563NET_SocketRecvLen(sockeid,NULL);                             /* 查询长度 */
        ReceLen = len;
        CH563NET_SocketRecv(sockeid,SocketRecvBuf[sockeid],&len);               /* 将接收缓冲区的数据读到MyBuf中*/
#if  DEBUG
        printf("ReceLen= %04x\n",ReceLen);
        printf("**RecvData:\r\n%s\n",(char *)SocketRecvBuf[sockeid]);
#endif
        if( CheckType != uncheck ){
            i = CH563NET_CheckResponse((char *)SocketRecvBuf[sockeid],CheckType);
            if(i != CHECK_SUCCESS){
                if(sockeid == p_smtp->Socket) OrderType = SMTP_ERR_CHECK;
#if  DEBUG
                printf("ERROR: %02x\n",(UINT16)i);
#endif
            }
        }
        memset((void *)SocketRecvBuf[sockeid],'\0',sizeof(SocketRecvBuf[sockeid]));              
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP连接中断 */
    {
#if DEBUG                                                                       /* 产生此中断表示TCP已经连接，可以进行收发数据 */
        printf("TCP Connect Success\n");                           
#endif
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP断开中断 */
    {
#if DEBUG                                                                       /* 产生此中断，CH563IP库内部会将此socket清除，置为关闭 */
        printf("TCP Disconnect\n");                                             /* 应用曾需可以重新创建连接 */
#endif
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP超时中断 */
    {
#if DEBUG                                                                       /* 产生此中断，CH563IP库内部会将此socket清除，置为关闭 */
        printf("TCP Timout\n");                                                 /* 应用曾需可以重新创建连接 */
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
void CH563NET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;

    initstat = CH563NET_GetGlobalInt();                                         /* 读全局中断状态并清除 */
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
#if DEBUG
        printf("UnreachCode ：%d\n",CH563Inf.UnreachCode);                      /* 查看不可达代码 */
        printf("UnreachProto ：%d\n",CH563Inf.UnreachProto);                    /* 查看不可达协议类型 */
        printf("UnreachPort ：%d\n",CH563Inf.UnreachPort);                      /* 查询不可达端口 */
#endif       
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
   
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY改变中断 */
   {
       i = CH563NET_GetPHYStatus();                                             /* 获取PHY状态 */
#if DEBUG
   printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif   
   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket中断 */
   {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* 读socket中断并清零 */
           if(socketinit)CH563NET_HandleSokcetInt(i,socketinit);                /* 如果有中断则清零 */
       }    
   }
}

/*******************************************************************************
* Function Name  : CH563IP_CreatTcpSocket
* Description    : 创建TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_CreatTcpSmtp( )
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,SocketsmtpIP,4);                          /* 设置目的IP地址 */
   TmpSocketInf.DesPort = 25;                                                   /* 设置目的端口 */
   TmpSocketInf.SourPort = SmtpSourPrt;                                         /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socekt类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[0];                      /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
#if DEBUG
    printf("SocketId TCP smtp = %02x\n",(UINT16)SocketId);
#endif    
   mStopIfError(i);                                                             /* 检查错误 */
   p_smtp->Socket = SocketId;    
   CheckType = SMTP_CHECK_CNNT;
   i = CH563NET_SocketConnect(SocketId);                                        /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
   memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));              
}

/*******************************************************************************
* Function Name  : CH563NET_SendData
* Description    : 发送数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SendData( char *PSend, UINT32 Len,UINT8 type,UINT8 index  )
{
    UINT32 length;
    UINT8 i;
    UINT8 *p;

#if  DEBUG
    printf("**send data:\r\n%s\n",PSend);
#endif
    p = (UINT8 *)PSend;
    length = Len;
    CheckType = type;
    while(1)
    {
        Len = length;
        i = CH563NET_SocketSend(index,p,&Len);                                  /* 将MyBuf中的数据发送 */
        mStopIfError(i);                                                        /* 检查错误 */
        length -= Len;                                                          /* 将总长度减去以及发送完毕的长度 */
        p += Len;                                                               /* 将缓冲区指针偏移*/
        if(length)continue;                                                     /* 如果数据未发送完毕，则继续发送*/
        break;                                                                  /* 发送完毕，退出 */
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
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD1 output enable */
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
* Function Name  : CH563NET_MAILInit
* Description    : 初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_MAILInit( void ) 
{

    UINT8 i = 0;

    i = CH56X_GetMac(MACAddr);
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
#if DEBUG
    printf("CH563IPLibInit Success\n");
#endif    
    SysTimeInit();                                                              /* 系统定时器初始化 */
    InitSysHal();                                                               /* 初始化中断 */
    CH563NET_SMTPInit( );                                                   
    CH563NET_CreatTcpSmtp( );
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
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
    CH563NET_MAILInit( );
    while(1)
    {
        CH563NET_MainTask( );                                                   /* CH563NET库主任务函数，需要在主循环中不断调用 */  
        if( CH563NET_QueryGlobalInt() )CH563NET_HandleGlobalInt( );             /* 查询中断，如果有中断，则调用全局中断处理函数 */ 
        if(ReceDatFlag){
            ReceDatFlag = 0;
            CH563NET_MailCmd( OrderType );                                          
        }
    }
}

/*********************************** endfile **********************************/
