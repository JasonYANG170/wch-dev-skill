/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        :本例程用于演示上位机软件通过UDP广播来配置CH563的功能.
                       563创建了一个UDP server来与上位机软件通信
*******************************************************************************/



/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ModuleConfig.h"
#include "CH563NET.H"
#include "ISPXT56X.H"
#define  CH563NET_DBG                          1

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
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN                          1600                              /* 接收缓冲区的大小 */                   
                                                                                                                         
/* CH563相关定义 */
UINT8 Default_MAC[6]={0X02,0X03,0X04,0X05,0X06,0X07};                     
UINT8 MACAddr[6];                                                               /* CH563MAC地址 */                       
UINT8 IPAddr[4];                                                                /* CH563IP地址 */                        
UINT8 GWIPAddr[4];                                                              /* CH563网关 */                          
UINT8 IPMask[4];                                                                /* CH563子网掩码 */                      
UINT8 SocketId;                                                                 /* 保存socket索引，可以不用定义 */       
UINT8 SocketRecvBuf[CH563NET_MAX_SOCKET_NUM ][RECE_BUF_LEN];                    /* socket接收缓冲区 */                   
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* 定义一个临时缓冲区 */                 
UINT8 Configbuf[MODULE_CFG_LEN];
pmodule_cfg CFG = (pmodule_cfg)Configbuf;
UINT8 socket_flag;                                                              /*用来显示上位机配置的是什么模式*/


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
        CH563NET_TimeIsr(CH563NETTIMEPERIOD);                                   /* 定时器中断服务函数 */
        R8_TMR0_INT_FLAG |= 0xff;                                               /* 清除定时器中断标志 */
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
#if CH563NET_DBG
    printf("Error: %02X\n", (UINT16)iError);                                    /* 显示错误 */
#endif    
}


void CH563_RESET()
{
#if CH563NET_DBG
    printf("reset \n");
#endif  
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */                     
    R8_GLOB_RST_CFG = 0x40 | RB_GLOB_FORCE_RST;                                 /*563软件复位*/  
    R8_SAFE_ACCESS_SIG = 0;                                                     /* 保护，禁止读写 */
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
UINT8 CH563NET_LibInit( UINT8 *ip, UINT8 *gwip, UINT8 *mask, UINT8 *macaddr)
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
    return (i);              /* 库初始化 */
}


void CH563NET_UdpServerRecv(struct _SCOK_INF *socinf,UINT32 ipaddr,UINT16 port,UINT8 *buf,UINT32 len)
{
    UINT8 i;    
     
    i =ParseConfigbuf(buf) ;                                                    /*解析数据*/
    if(i){
     CH563_RESET();
    }                 
}



void CH563NET_UdpSRecv(struct _SCOK_INF *socinf,UINT32 ipaddr,UINT16 port,UINT8 *buf,UINT32 len)
{
  UINT8 ip_addr[4],i;
  
  printf("ip:");
  for(i=0;i<4;i++){
    ip_addr[i]=ipaddr&0xff;
    printf("%-4d",ip_addr[i]);
    ipaddr=ipaddr>>8;
  }
#if CH563NET_DBG
  printf("port=%-8d  len=%-8d  sockeid=%-4d\n",port,len,socinf->SockIndex);
#endif  
  CH563NET_SocketUdpSendTo(socinf->SockIndex,buf,&len,ip_addr,port);
}

void CH563NET_Creat_Communication_Socket(void)
{   
   UINT8 i;                                                             
   UINT8 *desip=CFG->dest_ip;                                                   /* 目的IP地址 */
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,desip,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort = ((UINT16)CFG->dest_port[0]+(UINT16)CFG->dest_port[1]*256);                /* 设置目的端口 */
   TmpSocketInf.SourPort = ((UINT16)CFG->src_port[0]+(UINT16)CFG->src_port[1]*256);                 /* 设置源端口 */
   switch(CFG->type){
     case NET_MODULE_TYPE_TCP_S:     
          TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                              /* 设置socekt类型 */         
          socket_flag= NET_MODULE_TYPE_TCP_S;
          break;
     case NET_MODULE_TYPE_TCP_C:
          TmpSocketInf.ProtoType = PROTO_TYPE_TCP;          
          socket_flag= NET_MODULE_TYPE_TCP_C;
          break;
     case NET_MODULE_TYPE_UDP_S:
          TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
          TmpSocketInf.AppCallBack = CH563NET_UdpSRecv;
          socket_flag= NET_MODULE_TYPE_UDP_S;
          break;
     case NET_MODULE_TYPE_UDP_C:
          TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
          socket_flag= NET_MODULE_TYPE_UDP_C;
          break; 
     default:
          break;
   }     
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[1];                      /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
   switch(CFG->type){
     case NET_MODULE_TYPE_TCP_S:
          CH563NET_SocketListen(SocketId);
          printf("listening\n");
          break;
     case NET_MODULE_TYPE_TCP_C:
          CH563NET_SocketConnect(SocketId);
          printf("connecting\n");
          break;
     default:
          break;
   }

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
    UINT32 i;
    UINT32 len;
    UINT32 totallen;
    UINT8 *p = MyBuf;

    if(initstat & SINT_STAT_RECV) {                                             /* 接收中断 */
      len = CH563NET_SocketRecvLen(sockeid,NULL);                               /* 查询长度 */
#if CH563NET_DBG
      printf("Receive Len = %02x\n",len);                           
#endif
      totallen = len;
      CH563NET_SocketRecv(sockeid,MyBuf,&len);                                  /* 将接收缓冲区的数据读到MyBuf中*/
      while(1){      
         len = totallen;
         CH563NET_SocketSend(sockeid,p,&len);                                   /* 将MyBuf中的数据发送 */
         totallen -= len;                                                       /* 将总长度减去以及发送完毕的长度 */
         p += len;                                                              /* 将缓冲区指针偏移*/
         if(totallen)continue;                                                  /* 如果数据未发送完毕，则继续发送*/
         break;                                                                 /* 发送完毕，退出 */
      }           
    }
    if(initstat & SINT_STAT_CONNECT) {                                          /* TCP连接中断 */
 #if CH563NET_DBG
      printf("tcp connect\n");
#endif
      if(socket_flag==NET_MODULE_TYPE_TCP_S){
        CH563NET_ModifyRecvBuf(sockeid,(UINT32)SocketRecvBuf[sockeid],RECE_BUF_LEN);
      }                    
    }
    if(initstat & SINT_STAT_DISCONNECT){                                        /* TCP断开中断 */    
#if CH563NET_DBG
      printf("tcp disconnect\n");
#endif
      Delay_ms(200);
      CH563NET_Creat_Communication_Socket();      
    }
    if(initstat & SINT_STAT_TIM_OUT){                                           /* TCP超时中断 */
#if CH563NET_DBG
      printf("time out\n");
#endif
      Delay_ms(200);
      CH563NET_Creat_Communication_Socket();     
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
#if CH563NET_DBG
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
#if CH563NET_DBG
       printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif   
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
* Function Name  : CH563NET_CreatUdpSocket
* Description    : 创建UDP socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_CreatUdpSocket(void)
{
   UINT8 i;                                                             
   UINT8 desip[4] = {255,255,255,255};                                          /* 目的IP地址 */
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,desip,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort = 60000;                                                /* 设置目的端口 */
   TmpSocketInf.SourPort = 50000;                                               /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_UDP;                                     /* 设置socekt类型 */
   TmpSocketInf.AppCallBack = CH563NET_UdpServerRecv;
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[0];                       /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
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
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
    UINT8 i = 0;
  
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */   
    R32_PB_PU  |=1<<6;
    R32_PB_DIR &=~(1<<6);
    EEPROM_READ( 0x1000,CFG,MODULE_CFG_LEN );                                   /* 读取EEPROM里保存的配置信息 */   
    if(CFG->cfg_flag[0]!=checkcode1||CFG->cfg_flag[1]!=checkcode2){             /*校验EEPROM里的信息是否合法*/                                                                               /*若EEPROM里保存的信息不合法，或563还未经过上位机配置,那么就按照默认配置来初始化563*/                                                                                                         
      i = EEPROM_ERASE(0x1000,4096);                                                   
      i = EEPROM_WRITE(0x1000,Default_cfg,MODULE_CFG_LEN );                                       
      CH563_RESET();
    }
    i=CH56X_GetMac( MACAddr ); 
    if(i){                                                                      /*如果读取eeprom里的mac失败，那么563就使用默认MAC*/
      memcpy(MACAddr,Default_MAC,6);
    }
#if CH563NET_DBG    
    printf("MACAddr:");    
    for(i=0;i<6;i++){
      printf("%02x  ",MACAddr[i]);
    }
    printf("\n");
#endif
    memcpy(IPAddr,CFG->src_ip,sizeof(CFG->src_ip));         
    memcpy(GWIPAddr,CFG->getway,sizeof(CFG->getway));   
    memcpy(IPMask,CFG->mask,sizeof(CFG->mask));       
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* 库初始化 */
    mStopIfError(i);                                                            /* 检查错误 */
#if CH563NET_DBG
     printf(" 563 ip:%d.%d.%d.%d\n 563 gwip:%d.%d.%d.%d\n 563 mask:%d.%d.%d.%d\n", IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3],\
            GWIPAddr[0],GWIPAddr[1],GWIPAddr[2],GWIPAddr[3],IPMask[0],IPMask[1],IPMask[2],IPMask[3]); 
     printf("CH563IPLibInit Success\n");
#endif    
    SysTimeInit();                                                              /* 系统定时器初始化 */
    InitSysHal();                                                               /* 初始化中断 */
    CH563NET_CreatUdpSocket();                                                  /* 创建UDP Socket */
    if(CFG->type!=NET_MODULE_TYPE_NONE){                                        /*默认配置下不开启任何模式*/      
    CH563NET_Creat_Communication_Socket();                                      /*创建上位机配置模式下的socket*/
    }  
    if((R32_PB_PIN&(1<<6))==0) {                                                /*按键按下后，初始化563，执行默认配置*/ 
       Delay_ms(50);
       if((R32_PB_PIN&(1<<6))==0){      
				 while((R32_PB_PIN&(1<<6))==0);
       i = EEPROM_ERASE(0x1000,4096);
       i = EEPROM_WRITE( 0x1000,(pmodule_cfg)Default_cfg,MODULE_CFG_LEN );   
       CH563_RESET();
       }
    }
    while(1){
      CH563NET_MainTask();                                                      /* CH563NET库主任务函数，需要在主循环中不断调用 */
      if(CH563NET_QueryGlobalInt())CH563NET_HandleGlobalInt();                  /* 查询中断，如果有中断，则调用全局中断处理函数 */     
    }
}

/*********************************** endfile **********************************/
