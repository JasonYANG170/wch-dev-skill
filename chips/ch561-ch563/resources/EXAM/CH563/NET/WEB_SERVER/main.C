/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 WEB Server DEMO
*                      (1)、CH563 Examples by KEIL3_V3.53;
*                      (2)、串口1输出监控信息,115200bps;
*                      (3)本程序用于演示WEB服务器配置563功能，563内置web服务器 ，
                          通过网页可以实现563的网络参数配置，以及密码管理
*******************************************************************************/
  

/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include   "HTTPS.H"
#include  "ISPXT56X.H"
#define CH563NET_DBG                          1

#define CH563NET_MAX_SOCKET_NUM              6                                 /* Socket的个数，用户可以配置，默认为4个Socket,最大为32 */
#define RX_QUEUE_ENTRIES                     4                                /* CH563 MAC接收描述符队列，此值不得小于2,可配置默认为7，最大为32 */
#define CH563NET_TCP_MSS                     1024                             /* tcp MSS的大小*/

#include "CH563NET.H"
/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8    CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                             /* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8    CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                          /* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];                                   /* Socket信息表，4字节对齐 */
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
 
#define CH563NETTIMEPERIOD                   10                                           /* 定时器定时周期，单位mS*/

/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          1460                                        /* 接收缓冲区的大小 */
#define CH563NET_DBG                           0                                          /* printf调试使能 */
/* CH563NET库TCP的MSS长度为536字节，即一个TCP包里的数据部分最长为536字节 */
/* TCP协议栈采用滑动窗口进行流控，窗口最大值为socket的接收缓冲区长度。在设定 */
/* RX_QUEUE_ENTRIES时要考虑MSS和窗口之间的关系，例如窗口值为4*MSS，则远端一次会发送 */
/* 4个TCP包，如果RX_QUEUE_ENTRIES小于4，则必然会导致数据包丢失，从而导致通讯效率降低 */
/* 建议RX_QUEUE_ENTRIES要大于( 窗口/MSS ),如果多个socket同时进行大批量发送数据，则 */
/* 建议RX_QUEUE_ENTRIES要大于(( 窗口/MSS )*socket个数) 在多个socket同时进行大批数据收发时 */
/* 为了节约RAM，请将接收缓冲区的长度设置为MSS */

                       
UINT8 MACAddr[6];                                                                         /* CH563MAC地址 */
UINT8 IPAddr[4];                                                                         /* CH563IP地址 */
UINT8 GWIPAddr[4];                                                                       /* CH563网关 */
UINT8 IPMask[4];                                                                         /* CH563子网掩码 */
                                                                                             
/* 常用变量定义 */
UINT8  SocketId,RecvBuffer[RECE_BUF_LEN];                                           
UINT8  SocketRecvBuf[CH563NET_MAX_SOCKET_NUM][RECE_BUF_LEN];                              /* socket接收缓冲区 */
UINT8 flag=0;                                                                             /*浏览器请求接收标志*/
/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                                             /* 以太网中断 */
    {                                                                                     /* 以太网中断中断服务函数 */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                                         /* 定时器中断 */
    {
         CH563NET_TimeIsr(CH563NETTIMEPERIOD);                                            /* 定时器中断服务函数 */
         R8_TMR0_INT_FLAG |= 0xff;                                                        /* 清除定时器中断标志 */
   }
}
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
void SysTimeInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * CH563NETTIMEPERIOD;                                      /* 设置为10MS定时 */
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
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                                    /* 开启TIM0中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;                                                     /* 开启ETH中断 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                                 /* 开启IRQ全局中断 */
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
    if (iError == CH563NET_ERR_SUCCESS) return;                                           /* 操作成功 */
#if CH563NET_DBG
    printf("Error: %02X\n", (UINT16)iError);                                              /* 显示错误 */
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
    if(CH563NET_GetVer() != CH563NET_LIB_VER)return 0xfc;                                 /* 获取库的版本号，检查是否和头文件一致 */
    CH563NETConfig = LIB_CFG_VALUE;                                                       /* 将配置信息传递给库的配置变量 */
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
* Function Name  : CH563NET_HandleSockInt
* Description    : socket中断处理函数
* Input          : sockeid  socket索引
*                ：initstat 中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
     
    if(initstat & SINT_STAT_RECV)                                                         /* 接收中断 */
    {
        len = CH563NET_SocketRecvLen(sockeid,NULL);                                       /* 查询长度 */
    
      
#if CH563NET_DBG
        printf("Receive Len = %02x\n",len);                           
#endif
        if(len)
        {
            CH563NET_SocketRecv(sockeid,RecvBuffer,&len);                                /* 将接收到的浏览器请求保存在RecvBuffer[]里*/
            flag  =  1;
            socket=sockeid;
            //printf("sockint:%d",socket);
        }
        
      
    }
    if(initstat & SINT_STAT_CONNECT)                                                      /* TCP连接中断 */
    {
#if CH563NET_DBG                                                                          /* 产生此中断表示TCP已经连接，可以进行收发数据 */
        printf("TCP Connect Success  %d\n",sockeid);                           
#endif
        /* 关于CH563NET_ModifyRecvBuf                                       */
        /* 在TCP SERVER模式下，如果检测到TCP连接，则CH563NET将会自动为      */
        /* 此连接分配一个socket信息表，此socket表可能没有分配接收缓冲区     */
        /* 所以在TCP Server模式下，一旦收到连接中断后，应用层应该立即调用   */
        /* CH563NET_ModifyRecvBuf函数来为socket分配一个缓冲区               */
        /* CH563NET_ModifyRecvBuf的作用就是为socket分配缓冲区，在其他模式下 */
        /* 同样可以使用，例如需要双缓冲时可以随时修改接收缓冲区的地址和长度 */
        /* 在TCP Server模式下，库会监听客户端的连接动作，如果socket表中     */
        /* 有空闲的信息列表且连接合法，就会允许此连接，否则会对连接Reset    */
    
        CH563NET_ModifyRecvBuf(sockeid,(UINT32)SocketRecvBuf[sockeid],RECE_BUF_LEN);  
    }
    if(initstat & SINT_STAT_DISCONNECT)                                                   /* TCP断开中断 */
    {
#if CH563NET_DBG                                                                          /* 产生此中断，CH563NET库内部会将此socket清除，置为关闭*/
        printf("TCP Disconnect %d\n",sockeid);                                            /* 应用曾需可以重新创建连接 */
#endif

    }
    if(initstat & SINT_STAT_TIM_OUT)                                                      /* TCP超时中断 */
    {
#if CH563NET_DBG                                                                          /* 产生此中断，CH563NET库内部会将此socket清除，置为关闭*/
        printf("TCP Timout  %d\n",sockeid);                                               /* 应用曾需可以重新创建连接 */
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
void CH563NET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                                   /* 读全局中断状态并清除 */           
    if(initstat & GINT_STAT_UNREACH)                                                      /* 不可达中断 */                     
    {                                                                                                                          
    }                                                                                                                          
   if(initstat & GINT_STAT_IP_CONFLI)                                                     /* IP冲突中断 */                     
   {                                                                                                                           
                                                                                                                               
   }                                                                                                                           
   if(initstat & GINT_STAT_PHY_CHANGE)                                                    /* PHY改变中断 */                    
   {                                                                                                                           
       i = CH563NET_GetPHYStatus();                                                       /* 获取PHY状态 */                    
       #if CH563NET_DBG                                                                                                        
         printf("GINT_STAT_PHY_CHANGE %02x\n",i);                                                                              
       #endif                                                                                                                  
   }                                                                                                                           
   if(initstat & GINT_STAT_SOCKET)                                                        /* Socket中断 */                     
   {                                                                                                                           
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                                                                           
       {                                                                                                                       
           socketinit = CH563NET_GetSocketInt(i);                                         /* 读socket中断并清零 */             
           if(socketinit)CH563NET_HandleSockInt(i,socketinit);                            /* 如果有中断则清零 */               
       }    
   }
}


void CH563NET_UdpServerRecv(struct _SCOK_INF *socinf,UINT32 ipaddr,UINT16 port,UINT8 *buf,UINT32 len)
{

    UINT8 ip_addr[4],i;               
    for(i=0;i<4;i++){
        ip_addr[i] = ipaddr&0xff;
        printf("%-4d",ip_addr[i]);
        ipaddr = ipaddr>>8;    
    }

    CH563NET_SocketUdpSendTo( socinf->SockIndex,buf,&len,ip_addr,port);
}

/*******************************************************************************
* Function Name  : CH563NET_CreatTcpSocket
* Description    : 创建TCP Server scoket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_Creat_Web_TcpSocket(void)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                                 /* 创建临时socket变量 */                                       
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                                      /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */       
   TmpSocketInf.SourPort = 80;                                                            /* 设置源端口 */                                               
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                               /* 设置socekt类型 */                                           
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                                     /* 创建socket，将返回的socket索引保存在SocketId中 */           
   mStopIfError(i);                                                                       /* 检查错误 */                                                 
   i = CH563NET_SocketListen(SocketId);                                                   /* TCP监听 */                                                  
   mStopIfError(i);                                                                       /* 检查错误 */                                                 
}

void CH563NET_Creat_Cfg_Socket(UINT8 mode,UINT8 *Desip,UINT16 Desport,UINT16 Srcport)     /*根据网页的配置信息，563建立相应的socket*/
{
  UINT8 i;

  SOCK_INF TmpSocketInf;
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));    
  printf("desport: %d, srcport: %d\n",Desport,Srcport);
  printf("desip:%d.%d.%d.%d\n",Desip[0],Desip[1],Desip[2],Desip[3]);
  printf("mode %d\n",mode);
  switch(mode)
  {
    
;
    case MODE_TcpServer:
         TmpSocketInf.ProtoType = PROTO_TYPE_TCP; 
         TmpSocketInf.SourPort =Srcport;    
         i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           
         mStopIfError(i);                                                             
         i = CH563NET_SocketListen(SocketId);                                         
         mStopIfError(i); 
          break;
    case MODE_TcpClient:        
         TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
         memcpy((void *)&TmpSocketInf.IPAddr,Desip,4);
         TmpSocketInf.SourPort =Srcport;
         TmpSocketInf.DesPort =Desport;
         TmpSocketInf.RecvStartPoint=(UINT32)SocketRecvBuf[3];
         TmpSocketInf.RecvBufLen= RECE_BUF_LEN;
         i=CH563NET_SocketCreat(&SocketId,&TmpSocketInf);
         mStopIfError(i);
         i=CH563NET_SocketConnect(SocketId);
         mStopIfError(i);
         break;
    default :
            break;      
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

void CH563_RESET()                                                              /*563软件复位*/
{
   printf("reset\n");
   R8_SAFE_ACCESS_SIG=0X57;
   R8_SAFE_ACCESS_SIG=0XAB;
   R8_GLOB_RST_CFG=0X40 | RB_GLOB_FORCE_RST;
   R8_SAFE_ACCESS_SIG=0;  
} 



void CH563_Default()                                                            /*563恢复全部出厂设置*/
{
   EEPROM_ERASE(Basic_Cfg_Address,4096);
   EEPROM_WRITE(Basic_Cfg_Address,Basic_Default,Basic_Cfg_Len);
   EEPROM_ERASE(Port_Cfg_Address,4096);
   EEPROM_WRITE(Port_Cfg_Address,Port_Default,Port_Cfg_Len);
   EEPROM_ERASE(Login_Cfg_Address,4096);
   EEPROM_WRITE(Login_Cfg_Address,Login_Default,Login_Cfg_Len);
   printf("563_default\n");

   CH563_RESET();
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


    UINT16 DESPORT,SRCPORT;
  
    mInitSTDIO( ); 
    R32_PB_PU  |=1<<6;
    R32_PB_DIR &=~(1<<6);

    if((R32_PB_PIN &(1<<6))==0){                                                         /*设置PB6为手动恢复出厂配置*/                                                                                         
      Delay_ms(500);                                                                                                                                                                                           
      if((R32_PB_PIN&(1<<6))==0){                                                                                                                                                                              
        while((R32_PB_PIN&(1<<6))==0);                                                                                                                                                                         
        CH563_Default();                                                                                                                                                                                       
      }                                                                                                                                                                                                        
    }                                                                                                                                                                                                          
                                                                                                                                                                                                               
    EEPROM_READ(Basic_Cfg_Address,Basic_CfgBuf,Basic_Cfg_Len);                           /*从EEPROM里读取配置信息*/                                                                                            
    EEPROM_READ(Port_Cfg_Address,Port_CfgBuf,Port_Cfg_Len);                                                                                                                                                    
    EEPROM_READ(Login_Cfg_Address,Login_CfgBuf,Login_Cfg_Len);                                                                                                                                                 
                                                                                                                                                                                                               
    if(Basic_CfgBuf->flag[0]!=0x57 || Basic_CfgBuf->flag[1]!=0xAB)                       /*判断网络配置信息标志位，若判断为错误，则说明，EEPROM里保存的配置信息错误，恢复出厂配置*/                            
      CH563_Default();                                                                                                                                                                                         
    else{                                                                                                                                                                                                      
          if(Basic_CfgBuf->flag[0]!=0x57 || Basic_CfgBuf->flag[1]!=0xAB)                 /*判断密码配置信息，同上*/                                                                                            
             CH563_Default();                                                                                                                                                                                  
          else{                                                                                                                                                                                                
              if(Basic_CfgBuf->flag[0]!=0x57 || Basic_CfgBuf->flag[1]!=0xAB)             /*判断密码配置信息，同上*/                                                                                            
               CH563_Default();                                                                                                                                                                                
           }                                                                                                                                                                                                   
        }                                                                                                                                                                                                      
                                                                                                                                                                                                               
    memcpy(MACAddr,Basic_CfgBuf->mac,6);                                                                                                                                                                       
    memcpy(IPAddr,Basic_CfgBuf->ip,4);                                                                                                                                                                         
    memcpy(IPMask,Basic_CfgBuf->mask,4);                                                                                                                                                                       
    memcpy(GWIPAddr,Basic_CfgBuf->gateway,4);                                                                                                                                                                  
    printf("ch563ip:\n");                                                                                                                                                                                      
    for(i=0;i<4;i++)                                                                                                                                                                                           
     printf("%d.",IPAddr[i]);                                                                                                                                                                                  
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                                                                                                                                                      
    mStopIfError(i);                                                                     /*563网络参数配置 */                                                                                                  
#if CH563NET_DBG
    printf("CH563NETLibInit Success\n");
#endif 
    http_request = (st_http_request*)RecvBuffer;       
    SysTimeInit();                                                                       /* 系统定时器初始化 */
    InitSysHal();                                                                        /* 初始化中断 */
    CH563NET_Creat_Web_TcpSocket();                                                      /* 建立web server */ 
    
    DESPORT=Port_CfgBuf->des_port[0]*256+Port_CfgBuf->des_port[1];
    SRCPORT=Port_CfgBuf->src_port[0]*256+Port_CfgBuf->src_port[1];
    CH563NET_Creat_Cfg_Socket(Port_CfgBuf->mode,Port_CfgBuf->des_ip,DESPORT  ,SRCPORT);       
    Init_Para_Tab();                                                                     /*初始化参数表*/                                                    
    while(1)
    {
        CH563NET_MainTask();                                                             /* CH563NET库主任务函数，需要在主循环中不断调用 */
        if(CH563NET_QueryGlobalInt())CH563NET_HandleGlobalInt();                         /* 查询中断，如果有中断，则调用全局中断处理函数 */
        Web_Server( );                                                                   /*Web 服务器处理程序*/        
    }
}

/*********************************** endfile **********************************/
