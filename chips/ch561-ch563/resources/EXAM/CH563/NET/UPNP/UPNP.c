/********************************** (C) COPYRIGHT *******************************
* File Name          : upnp.c
* Author             : lht
* Version            : V1.0
* Date               : 2013/04/12
* Description        : IGD设备upnp注册端口
*                      
*******************************************************************************/

/* include files*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "CH563NET.H"
#include "UPNP.H"
#define DBG                             0
#if DBG
#define  cprintf(format,arg...)         printf(format,##arg)
#else
#define  cprintf(format,arg...)         do{ if(0)printf(format,##arg);}while(0);
#endif
/* 全局常量定义 */
u8 const Discover[97] = 
{
    0x4D, 0x2D, 0x53, 0x45, 0x41, 0x52, 0x43, 0x48, 0x20, 0x2A, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2F, 
    0x31, 0x2E, 0x31, 0x0D, 0x0A, 0x48, 0x6F, 0x73, 0x74, 0x3A, 0x32, 0x33, 0x39, 0x2E, 0x32, 0x35, 
    0x35, 0x2E, 0x32, 0x35, 0x35, 0x2E, 0x32, 0x35, 0x30, 0x3A, 0x31, 0x39, 0x30, 0x30, 0x0D, 0x0A, 
    0x53, 0x54, 0x3A, 0x75, 0x70, 0x6E, 0x70, 0x3A, 0x72, 0x6F, 0x6F, 0x74, 0x64, 0x65, 0x76, 0x69, 
    0x63, 0x65, 0x0D, 0x0A, 0x4D, 0x61, 0x6E, 0x3A, 0x22, 0x73, 0x73, 0x64, 0x70, 0x3A, 0x64, 0x69, 
    0x73, 0x63, 0x6F, 0x76, 0x65, 0x72, 0x22, 0x0D, 0x0A, 0x4D, 0x58, 0x3A, 0x33, 0x0D, 0x0A, 0x0D, 
    0x0A
};
/**< SSDP Header */
const char SSDP[] = "\
M-SEARCH * HTTP/1.1\r\n\
Host:239.255.255.250:1900\r\n\
ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n\
Man:\"ssdp:discover\"\r\n\
MX:3\r\n\
\r\n\
";
u8 const IP_ANY[4] =  {255,255,255,255}; 
u8 const IP_ANY1[4] = {239,255,255,250}; 
/*******************************************************************************/
UPNP_CTRL  UpnpCtrl;
u8 *UpnpSockRecvBuf;                                                            /* socket接收缓冲区 */
u8 *UpnpSockSendBuf;                                                            /* 发送缓冲区 */
u8 *UpnpTmpBuf;                                                                 /* 临时缓冲区 */
u32 UpnpFlashAddr;
u8 UrlBuf[170];
u8 UrlDirBuf[170];
u8 UrlCtrlBuf[170];
static u16 TotalBufLen;
static u16 SocketRecvBufLen;
static u16 SocketSendBufLen;
static u16 UpnpTmpBufLen;

/*******************************************************************************
* Function Name  : strcasestr
* Description    : 搜索字符串，忽略大小写
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    int c1, c2;

    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while ((--n > 0) && c1 == c2 && c1 != 0);
    return c1 - c2;
}

char* strcasestr(const char* str, const char* subStr)
{
    u16 len = strlen(subStr);
    
    if(len == 0)
    {
        return NULL;                                 
    }
    while(*str)
    {
        if(strncasecmp(str, subStr, len) == 0)       
        {
            return (u8 *)str;
        }
        str++;
    }
    return NULL;
}
/*******************************************************************************
* Function Name  : CH563NET_InetNtoa
* Description    : IP地址转换为字符
* Input          : ipddr ---- 需要转换的IP地址
* Output         :  cp   ---- 输出IP地址字符串
* Return         : None
*******************************************************************************/
void CH563NET_InetNtoa(char *cp,u8 *ipddr)
{
    char *str = cp;
    char inv[3];
    char *rp;
    u8 *ap;
    u8 rem;
    u8 n;
    u8 i;

    rp = str;
    ap = ipddr;
    for(n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (UINT8)10;
            *ap /= (UINT8)10;
            inv[i++] = '0' + rem;
        } while(*ap);
        while(i--)
        *rp++ = inv[i];
        *rp++ = '.';
        ap++;
    }
    *--rp = 0;
}

/*******************************************************************************
* Function Name  : UVCIP_GetPort
* Description    : 获取当前用端口
* Input          : None
* Output         : None
* Return         : 当前可用的端口号
*******************************************************************************/
u16 UVCIP_GetPort(void)
{
    static usedport = UVC_IP_MIN_PORT;

    if(usedport >= UVC_IP_MAX_PORT)usedport = UVC_IP_MIN_PORT;
    return(usedport++);
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPMainTask
* Description    : UPNP主任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPMainTask(void)
{
   CH563NET_UPNPTimeHandle();
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPInit
* Description    : UPNP模块初始化
* Input          : upnpcfg ---- 配置
* Output         : None
* Return         : 执行结果
*******************************************************************************/
u8 CH563NET_UPNPInit(UPNP_CFG *upnpcfg)
{
    u32 addr;
    u16 i;

    i = sizeof(SSDP);
    memset(&UpnpCtrl,0,sizeof(UpnpCtrl));                                       /* 将结构体清零 */
    UpnpCtrl.GateIPAddr[0] = upnpcfg->GateIPAddr[0];
    UpnpCtrl.GateIPAddr[1] = upnpcfg->GateIPAddr[1];
    UpnpCtrl.GateIPAddr[2] = upnpcfg->GateIPAddr[2];
    UpnpCtrl.GateIPAddr[3] = upnpcfg->GateIPAddr[3];
    UpnpCtrl.AppUpnpCallBack = upnpcfg->AppUpnpCallBack;
    TotalBufLen = upnpcfg->MemoryLen;
    SocketRecvBufLen = SocketSendBufLen = 2* (upnpcfg->TcpMss); 
    UpnpTmpBufLen = TotalBufLen - SocketRecvBufLen - SocketSendBufLen;  
    if(TotalBufLen < (SocketRecvBufLen + SocketSendBufLen))return ERR_UPNP_MEM_ERR;
    if(UpnpTmpBufLen < SocketRecvBufLen)return ERR_UPNP_MEM_ERR;     
    UpnpSockRecvBuf = upnpcfg->UpnpMemory;                                      /* 头部为socket接收缓冲区 */
    UpnpSockSendBuf = UpnpSockRecvBuf + SocketRecvBufLen;                       /* 接收缓冲区后为发送缓冲区 */
    UpnpTmpBuf = UpnpSockSendBuf + SocketSendBufLen;                            /* 发送缓冲区之后为临时缓冲区 */
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPTimeIsr
* Description    : UPNP定时器中断，250MS调用一次
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTimeIsr(void)
{
    UpnpCtrl.TimeInt = 1;                                                       /* 设置中断事件标志向量 */
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPTimeOutHandle
* Description    : UPNP定时器中断事件处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTimeOutHandle(void)
{
    if(UpnpCtrl.UpnpMach == DEF_UPNP_STOP)return;
    switch(UpnpCtrl.UpnpMach)
    {
        case DEF_UPNP_START:                                                    /* 状态在启动阶段，计时到后，开始启动 */
        case DEF_UPNP_WAIT_URL:
        case DEF_UPNP_GET_CONNECT:
        case DEF_UPNP_GET_RECV:
        case DEF_UPNP_GET_CLOSEING:
        case DEF_UPNP_POST_CONNECT:
        case DEF_UPNP_POST_RECV:
        case DEF_UPNP_POST_CLOSEING:
        default:                                                                /* 连接超时错误 */
            UpnpCtrl.UpnpFlags &= ~DEF_UPNP_FIND;
            UpnpCtrl.MapTime = 0;
            if(UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG )
            {
               CH563NET_SocketClose(UpnpCtrl.TcpSocket & ~SOCKET_ENABLE_FLAG,0);    
            }
            UpnpCtrl.UpnpMach = DEF_UPNP_START;
        break;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPTimeHandle
* Description    : UPNP计时函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTimeHandle(void)
{
    if(UpnpCtrl.TimeInt)
    {
        UpnpCtrl.TimeInt = 0;                                                   /* 清除中断事件标志向量 */
        if(UpnpCtrl.UpnpMach == DEF_UPNP_STOP)return;
        if(UpnpCtrl.SSDPTime)                                                   /* SSDP为UDP专用定时器 */
        {
            UpnpCtrl.SSDPTime -= UPNP_TIME_PERIOD;
            if(UpnpCtrl.SSDPTime == 0)
            {
                if(!(UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG))                  /* 检查UDP是否存在，不存在则重新创建 */
                {
                   CH563NET_UPNPCreatUpdSocket(); 
                }
                if(UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG)
                {
                    if(!(UpnpCtrl.UpnpFlags &= DEF_UPNP_FIND))
                    {
                        cprintf("NO Find Upnp Device\n");
                    }
                    CH563NET_UPNPSendSSDPacket();
                }
                UpnpCtrl.SSDPTime = UPNP_TIMEOUT_180S;
            }

        }
        if(UpnpCtrl.TimeCount)
        {
            UpnpCtrl.TimeCount -= UPNP_TIME_PERIOD;                             /* 将计数减小 */
            if(UpnpCtrl.TimeCount == 0)                                         /* 计数时间到 */
             {                                                                  
                 CH563NET_UPNPTimeOutHandle();                                 /* 超时处理函数 */
             }                                                                  
        }                                                                       
        if(UpnpCtrl.MaxAgeCount)                                                
        {                                                                       
            UpnpCtrl.MaxAgeCount -= UPNP_TIME_PERIOD;                           /* 最大生存时间计数 */
            if(UpnpCtrl.MaxAgeCount == 0)                                       /* 生存时间结束 */
            {                                                                   
                 cprintf("Upnp device Remove \n");                               
                 if(UpnpCtrl.UpnpFlags & DEF_UPNP_FIND)                         
                 {                                                              
                   UpnpCtrl.UpnpFlags &= ~DEF_UPNP_FIND;                        /* 生存期结束后，设备仍然未收到UPNP设备的SSDP，将发现标志清除 */
                 }
            }
        }
        if(UpnpCtrl.MapTime)                                                    /* 此定时器是用在端口映射，映射一段时间后需要重新映射 */
        {                                                                       
            UpnpCtrl.MapTime -= UPNP_TIME_PERIOD;                               
            if(UpnpCtrl.MapTime == 0)                                           /* 计时完毕，再次MAP端口 */
            {                                                                   
                 cprintf("Map Time end and Map Port\n");                         
                 UpnpCtrl.TimeCount = 0;                                        /*stop timer */
                 UpnpCtrl.TcpSocket = 0;
                 CH563NET_UPNPStartMapPort();
            } 
        }
    }
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPStart
* Description    : 启动UPNP
* Input          : upnpmap ---- 需要映射的端口信息
* Output         : None
* Return         : None
*******************************************************************************/
//CH563NET_UPNPUPNPFunStart
void CH563NET_UPNPStart(UPNP_MAP *upnpmap)
{
    u8 *p = "http://192.168.1.1:1900/igd.xml";

    if(UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG )
    {
        CH563NET_SocketClose(UpnpCtrl.UdpSocket & ~SOCKET_ENABLE_FLAG,0);    
    }
    if(UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG )
    {
        CH563NET_SocketClose(UpnpCtrl.TcpSocket & ~SOCKET_ENABLE_FLAG,0);    
    }
//    memset(&UpnpCtrl,0,sizeof(UpnpCtrl));
    UpnpCtrl.TimeCount = 0;                                                     /* 初始化 */
    UpnpCtrl.MaxAgeCount = 0;                                                   /* 最大生存时间清零 */
    UpnpCtrl.UpnpMach = DEF_UPNP_START;                                         /* 状态机进入开始状态 */
    UpnpCtrl.SSDPTime = UPNP_TIMEOUT_180S;                                      /* SSDP发现包超时为3分钟 */
    UpnpCtrl.ExternalPort = upnpmap->ExternalPort;                              
    UpnpCtrl.InternelPort = upnpmap->InternelPort;                              
    UpnpCtrl.UpnpProType  = upnpmap->UpnpProType;                               
    UpnpCtrl.MapPortDes   = upnpmap->MapPortDes;                                
    UpnpCtrl.IPAddr[0] = upnpmap->IPAddr[0];
    UpnpCtrl.IPAddr[1] = upnpmap->IPAddr[1];
    UpnpCtrl.IPAddr[2] = upnpmap->IPAddr[2];
    UpnpCtrl.IPAddr[3] = upnpmap->IPAddr[3];

    UpnpCtrl.UpnpFlags = 0;                                                     /* 将标志位清除 */
#if ENABLE_SSDP_FUN
    CH563NET_UPNPCreatUpdSocket();                                              /* 创建UDP的socket */

    if(UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG)                                 /* 判断socket是否创建成功 */
    {                                                                           /* 成功，则发送SSDP,(搜索UPNP设备 ) */
        CH563NET_UPNPSendSSDPacket();                                                
        UpnpCtrl.SSDPTime = UPNP_TIMEOUT_180S;                                  /* 180S内，未等到URL，则再次发送SSDP */
    }                                                                          
    else                                                                       
    {                                                                          
        UpnpCtrl.SSDPTime = UPNP_TIMEOUT_20S;                                   /* 初始定时器，20S后再次重试创建 */
    }
    UpnpCtrl.UpnpMach = DEF_UPNP_START;
#endif
    memset(UrlBuf,0,sizeof(UrlBuf));                                            /* 将缓冲区全部清零 */
    p = &UrlBuf[1];
    sprintf(p,"http://%d.%d.%d.%d:1900/igd.xml",UpnpCtrl.GateIPAddr[0],UpnpCtrl.GateIPAddr[1],UpnpCtrl.GateIPAddr[2],UpnpCtrl.GateIPAddr[3]);
    UrlBuf[0] = strlen(p) + 1;    
    CH563NET_UPNPCreatTcpGetSocket();   
    UpnpCtrl.MapTime = 0;
    UpnpCtrl.TimeCount = UPNP_TIMEOUT_10S;                                      /* 进入超时 */
    UpnpCtrl.UpnpMach = DEF_UPNP_GET_CONNECT;                                   /* 进入等待连接 */
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPPStop
* Description    : 停止UPNP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPPStop(void)
{
    UpnpCtrl.UpnpMach = DEF_UPNP_STOP;
    
    if(UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG)                                 /* 关闭TCP socket */
    {
         UpnpCtrl.TcpSocket &= ~ SOCKET_ENABLE_FLAG;
         CH563NET_SocketClose(UpnpCtrl.TcpSocket,1);     
    }
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPCheckSocketAvail
* Description    : 本函数主要是在接收中断里判断是否为UPNP的接口数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 CH563NET_UPNPCheckSocketAvail(u8 socket)
{
    if(UpnpCtrl.UpnpMach == DEF_UPNP_STOP)return FALSE;                        /* upnp funciton may be stoped */
    socket |= SOCKET_ENABLE_FLAG;                                              /* if socket is available ,the socket index highest bit is 1 */
    if((socket == UpnpCtrl.UdpSocket) || (socket == UpnpCtrl.TcpSocket))       /* upnp use two sockets */
    {
        return TRUE;
    }
    else FALSE;
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPCreatUpdSocket
* Description    : 建立一个多播UDP连接
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPCreatUpdSocket(void)
{
   u8 desip[4];                                                 
   u8 i,uSocketid;
   SOCK_INF  TmpSocketInf;
   desip[0] = 0xff;
   desip[1] = 0xff;
   desip[2] = 0xff;
   desip[3] = 0xff;

   memset(&TmpSocketInf,0,sizeof(TmpSocketInf));                                /* clear local variable*/
   memcpy((void *)TmpSocketInf.IPAddr,desip,4);                                 /* set destination  ip address */
   TmpSocketInf.SourPort = 1900;                                                
   TmpSocketInf.DesPort = 1025;                                                 
   TmpSocketInf.ProtoType = PROTO_TYPE_UDP;                                     /* protocol type is udp */
   TmpSocketInf.RecvStartPoint = (UINT32)UpnpSockRecvBuf;                       /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = SocketRecvBufLen ;                                 /* 设置接收缓冲区的接收长度 */
   i = CH563NET_SocketCreat(&uSocketid,&TmpSocketInf);                          /* creat upnp socket */
   CH563NET_SetSocketTTL(uSocketid,1);                                          /* 将TTL 设置为1 */
   if(i != CH563NET_ERR_SUCCESS)return;                                         /* check  creat socket succeccful ,if not ,next creat again */
   UpnpCtrl.UdpSocket = uSocketid | 0x80;                                       /* save socket index,and high bit set 1*/   
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPSendSSDPacket
* Description    : 发送UPNP发现包
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPSendSSDPacket(void)
{
   u8 i;
   u32 len;

   if(!(UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG))return;                        /* UPNP的socket可能已经被关闭了 */
   i = UpnpCtrl.UdpSocket & ~SOCKET_ENABLE_FLAG;
   len = sizeof(Discover);
   CH563NET_SocketUdpSendTo(i,(u8 *)Discover,(u32 *)(&len),\
   (u8 *)IP_ANY1,DEF_UPNP_MULTI_DPORT);                                         /* 向目的端口多播SSD数据包 */
}
/*******************************************************************************
* Function Name  : MultiBrocast
* Description    : 建立一个多播UDP连接
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPRecvMultiBrocast(u8 socketid,u8 *buf)
{
    u8 *p;
    u8 *lp;
    u8 i = 0;
    u8 ipstr[20];
    u8 addr[4];
    u16 port;
    u8  server = 0; 
    UPNP_MAP upnpmap;


    p = buf;
    if(lp = strcasestr(p,"ssdp:alive"))                                         /*  ssdp广播包， */
    {
       i = 0;
    }
    else if(lp = strcasestr(buf,"ssdp:byebye"))                                 /* 宣告设备不可用 */
    {
        cprintf("byebye\n");
        cprintf("%s\n",buf);
        i = 1;
    }
    else return;
    p = buf;
    /* 第一步查找服务类型 */
    if(lp = strcasestr(p, "service:WANIPConnection"))                           /* 查找是否为WANIPConnection服务 */
    {
         cprintf("Fine WANIPConnection server\n");
         server = UPNP_SERVER_WANIPCONNECT;
         if(i)                                                                  /* WANIPConnection 设备移除 */
         {
             cprintf("device bye bye\n");
             upnpmap.ExternalPort = UpnpCtrl.ExternalPort;  
             upnpmap.InternelPort = UpnpCtrl.InternelPort;
             upnpmap.UpnpProType  = UpnpCtrl.UpnpProType; 
             upnpmap.MapPortDes   = UpnpCtrl.MapPortDes;  
             upnpmap.IPAddr[0]= UpnpCtrl.IPAddr[0]  ;
             upnpmap.IPAddr[1]= UpnpCtrl.IPAddr[1]  ;
             upnpmap.IPAddr[2]= UpnpCtrl.IPAddr[2]  ;
             upnpmap.IPAddr[3]= UpnpCtrl.IPAddr[3]  ;
             //CH563NET_UPNPStart(&upnpmap);
             UpnpCtrl.UpnpFlags  &=  ~DEF_UPNP_FIND;
             return;
         }
    }
    else return;                                                                /* 仅识别WANIPConnection服务，其他不处理 */
    /* 第二步查找服务类型的URL，需要找出IP,PORT，ROOT DIR等三个变量 */
    if((lp = strcasestr(p,"LOCATION:")))                                        /* 找到location位置 */
    {
       u8 blen;
       u8 *urlbuf;
       u8 wrflag = 0;

       p = strcasestr(lp,"http://");                                            /*保存URL */
       lp = strstr(p,".xml");
       i = lp - p + 4;
       blen = UrlBuf[0];//FlashOneByteRead(FlashUrlAddr);
       if(blen != i)
       {
            wrflag = 1;
       }
       else
       {
           for(blen = 0; blen < i; blen++)
          {
              //if(p[blen] != FlashOneByteRead(FlashUrlAddr + 1 + blen))/* 第0个字节保存了长度，所以在对比的时候要FlashCtrlUrlAddr + 1*/
              if(p[blen] != UrlBuf[1 + blen])                                   /* 第0个字节保存了长度，所以在对比的时候要FlashCtrlUrlAddr + 1*/
              {
                  wrflag = 1;
                  break;   
             }
          }
       }
       if(wrflag)
       {
           if(i > (UPNP_MEM_SIZE - 1))return;                                 /* 长度不够，不保存数据，防止溢出，导致程序异常 */
           cprintf("****************Url Change and save it\n");
           urlbuf = UrlBuf;
           *urlbuf++ = i;                                                       /* 将长度保存 */
           memset(urlbuf,0,UPNP_MEM_SIZE - 1);
           memcpy(urlbuf,p,i);
           cprintf("Url:%s\n",urlbuf);
           UpnpCtrl.UpnpFlags  &=  ~DEF_UPNP_FIND;                              /* UPNP设备改变，则立即将发现标志清除 */ 
       }                                                                        /* 清除此标志后，会发生GET命令重新获取control URL */
       p += sizeof("http://") - 1;                                              /* 其格式为http://192.168.1.1:1900/xxx.xml */                                   
       if(lp = strcasestr(p,":"))
       {
          i = lp - p;
          memset(ipstr,0,20);
          memcpy(ipstr,p,i);
          CH563NET_Aton(ipstr,addr);
          lp++;                                                                 /* lp指向':'*/
          p = strcasestr(lp,"/");                                               /*P指向'/'，中间为端口字符 */
          i = p - lp;                                                           /* 计算端口字符串的大小 */ 
          memset(ipstr,0,20);                                                   
          memcpy(ipstr,lp,i);                                                   /* 将字符串提取到缓冲区中 */
          port = atoi(ipstr);
          lp = strstr(lp,"/");
          p = lp;
          lp = strstr(lp,".xml");
          i = lp - p + 4;                                                       /* 计算根路径的长度 */
        
          if(server == UPNP_SERVER_WANIPCONNECT)                     
          {
              p = buf;
              /* 查找MAX-AGE,MAX-age为宣告有效时间，此值会记录到 */
              /* UpnpCtrl.MaxAgeCount中，且每次收到max-age后，都 */
              /* 将UpnpCtrl.MaxAgeCount刷新，如果MaxAgeCount为0，*/
              /* 则表示UPNP设备消失了。在实际处理时，为了防止此  */
              /* 值计数到0，误检测到设备不在线，所以加上了5秒时间*/
              if((lp = strcasestr(p, "CACHE-CONTROL:")))           
              {
                  lp = strcasestr(p,"max-age=");
                  if(lp)
                  {
                      lp += sizeof("max-age=") - 1; 
                      UpnpCtrl.MaxAgeCount = atoi(lp) * 1000;
                      UpnpCtrl.MaxAgeCount += UPNP_MAX_AGE_ADD;
                      cprintf("UpnpCtrl.MaxAgeCount = %d\n",UpnpCtrl.MaxAgeCount);                     
                  }
                  else
                  {
                      UpnpCtrl.MaxAgeCount = 100 * 1000;                        /* 防止没找到，默认为100S */
                  } 
              }
              else
              {
                  UpnpCtrl.MaxAgeCount = 100 * 1000;                            /* 防止没找到，默认为100S */
              }                                                                 
              UpnpCtrl.MaxAgeBack = UpnpCtrl.MaxAgeCount;                       /* 备份MAX-AGE，在注册完毕后使用 */
              UpnpCtrl.SSDPTime = UPNP_TIMEOUT_180S;                            /* 刷新SSDP时间列表 */
              cprintf("Find UPNP_SERVER_WANIPCONNECT\n");                       
              if(!(UpnpCtrl.UpnpFlags & DEF_UPNP_FIND))                         /* 如果UPNP设备不存在，在获取control 的URL*/
              {
                  cprintf("Fine New Upnp Device and Creat Get Socket\n");
                  if(UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG)                   /* 可能此时UPNP在用TCP socket此时不可以再次使用了 */
                  {
                     if((UpnpCtrl.UpnpMach >= DEF_UPNP_POST_CONNECT)&&\
                         (UpnpCtrl.UpnpMach <= DEF_UPNP_POST_CLOSEING))         /* 此时可能在POST状态，GET的优先级比较高，将POST连接复位 */
                     {
                         UpnpCtrl.TcpSocket &= ~ SOCKET_ENABLE_FLAG;
                         CH563NET_SocketClose(UpnpCtrl.TcpSocket,1);     
                     }
                     else return;                                               /* 说明在GET状态，则直接返回 */  
                  }
                  CH563NET_UPNPCreatTcpGetSocket();   
                  UpnpCtrl.MapTime = 0;
                  UpnpCtrl.TimeCount = UPNP_TIMEOUT_10S;                        /* 进入超时 */
                  UpnpCtrl.UpnpMach = DEF_UPNP_GET_CONNECT;                     /* 进入等待连接 */
              }
          }
       }
    }
}


/*******************************************************************************
* Function Name  : CH563NET_UPNPTcpSendGetCommand
* Description    : 初始化GET命令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTcpSendGetCommand(u8 socketindex)
{
   u8 *buf;
#define TMP_LEN   60
   u8 tmp[TMP_LEN];
   u8 *p,*urlbuf;
   u8 *lp;
   u8 i;
   u32 len;

   memset(UpnpSockSendBuf,0,SocketSendBufLen);
   buf = UpnpSockSendBuf;
   memcpy(buf,"GET ",strlen("GET "));
   urlbuf = (u8 *)&UrlBuf[1];//(u8 *)(UpnpFlashAddr + FLASH_LOCAT_URL + 1);
   p = strcasestr(urlbuf,"http://");                                 /* 通过URL查找到IP地址 */
   p += sizeof("http://");
   p = strcasestr(p,"/");
   lp = strcasestr(p,".xml"); 
   i = lp - p + sizeof(".xml");
   memset(tmp,0,TMP_LEN);
   memcpy(tmp,p,i);
   cprintf("Dir:%s\n",tmp);
   strcat(buf,tmp);
   strcat(buf," HTTP/1.1\r\n");
   strcat(buf,"HOST: ");
   p = strcasestr(urlbuf,"http://");
   p += sizeof("http://") - 1;
   lp = strcasestr(p,"/");
   i = lp - p;
   memset(tmp,0,TMP_LEN);
   memcpy(tmp,p,i);
   cprintf("Host:%s\n",tmp);
   strcat(buf,tmp);
   strcat(buf,"\r\nACCEPT-LANGUAGE: en\r\n\r\n");
   len = strlen(buf);
   CH563NET_SocketSend(socketindex,buf,(u32 *)(&len)); 
}


/*******************************************************************************
* Function Name  : CH563NET_UPNPCreatTcpGetSocket
* Description    : UPNP创建GET TCP Sockets
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPCreatTcpGetSocket(void)
{
    u8 i;
    u8 *p,*lp,*urlbuf;
    u8 tmp[60];
    u8 addr[4];
    u16 port;
    SOCK_INF  TmpSocketInf;
    
    cprintf("CH563NET_UPNPCreatTcpGetSocket\n");
    urlbuf = (u8 *)&UrlBuf[1];//(u8 *)(UpnpFlashAddr + FLASH_LOCAT_URL + 1);    /* 直接将地址指向FLASH中 */ 
    p = strcasestr(urlbuf,"http://");                                           /* 通过URL查找到IP地址 */
    p += sizeof("http://") - 1;                                                 /* 格式为HTTP://+IP+:+PORT */
    lp = strcasestr(p,":");                                                     /* 查找冒号 */
    i = lp - p;                                                                 /* 计算长度 */
    memset(tmp,0,20);                                                           /* 将临时缓冲区全部清零 */
    memcpy(tmp,p,i);                                                            /* 将IP字符串复制到临时缓冲区中 */
    cprintf("ipstr %s\n",tmp);                                                  
    CH563NET_Aton(tmp,addr);                                                    /* 将字符传转换为IP地址 */
    cprintf("IP: %d.%d.%d.%d\n",addr[0],addr[1],addr[2],addr[3]);               /* 将IP调试打印 */
    lp++;                                                                       
    p = strcasestr(lp,"/");                                                     /* 查找端口 */
    i = p - lp;                                                                 
    memset(tmp,0,20);                                                           /* 将临时缓冲区全部清零 */
    memcpy(tmp,lp,i);                                                           /* 将IP字符串复制到临时缓冲区中 */
    port = atoi(tmp);                                                           /* 计算端口 */
    cprintf("Port = %d\n",port);                                                
                                                                                
   memset(&TmpSocketInf,0,sizeof(TmpSocketInf));                                /* 将临时缓冲区清除 */
   memcpy((void *)TmpSocketInf.IPAddr,addr,4);                                  /* 设置目的IP地址 */
   TmpSocketInf.SourPort = UVCIP_GetPort();                                     /* 设置源端口 */
   TmpSocketInf.DesPort = port;                                                 /* 设置目的端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 协议类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)UpnpSockRecvBuf;                       /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = SocketRecvBufLen ;                                 /* 设置接收缓冲区的接收长度 */
   UpnpCtrl.TcpSocket = 0X00;                                                   /* 将TCP标志清零 */
   i = CH563NET_SocketCreat(&UpnpCtrl.TcpSocket,&TmpSocketInf);                 /* 创建TCP socket */
   if(i != CH563NET_ERR_SUCCESS) 
   {
      cprintf("Creat Socket Fail %02x\n",i);
      return;                                                                   /* 创建失败直接返回 */
   }
   cprintf("CH563NET_SocketCreat success %d\n\n",UpnpCtrl.TcpSocket);
   UpnpCtrl.TcpSocket |= SOCKET_ENABLE_FLAG;                                    /* 创建成功，置socket为有效 */

   i = CH563NET_SocketConnect(UpnpCtrl.TcpSocket & ~SOCKET_ENABLE_FLAG );
   if(i != CH563NET_ERR_SUCCESS)
   {
      cprintf(" Socket Connect Fail %02x\n",i);
      return;                                                                   /* 创建失败直接返回 */
   }
   cprintf("CH563NET_SocketConnect success\n");
}
/*******************************************************************************
* Function Name  : CH563NET_UPNPReceiveGetCmd
* Description    : GET命令接收函数，循环接收，接收完毕后分析URL
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPReceiveGetCmd(u8 *buf,u32 len)
{
    u8  *p = buf;
    u8  *lp;
    u8  *wp; 
    u16  i;
    u8  fag = 0;
    //cprintf("UpnpCtrl.RecvTotalLen = %02x\n",UpnpCtrl.RecvTotalLen);
    //cprintf("%s\n",buf);
    if(UpnpCtrl.RecvTotalLen == 0)                                              /* 接收的总长度为0，表示还计算出接收的数据长度 ，为第一包数据 */
    {                                                                           /* 计算出总长度 */
         p = strcasestr(p, "Content-Length:");                                  /* 找到长度内部标志 */
         if(p)                                                                  /* 计算长度  */
         {                                                                      
             u32 xlen = 0;                                                      
             //cprintf("Find Content-Length:\n");                               
             lp = strcasestr(p, "\r\n");                                        /* 找到本行结束 */
             p += sizeof("Content-Length:") - 1;                                /* 将指针偏移到正确位置 */
             i = *lp;                                                           /* 备份*lp */
             *lp = 0;                                                           
             xlen = atoi(p);                                                    
             *lp = i;                                                           /* 还原lp */
             cprintf("Pose Receive Cmd = %d\n",xlen);
             UpnpCtrl.RecvTotalLen = xlen;
             
             UpnpCtrl.RecvLen = 0;
             p = buf;
             if(p = strcasestr(p,"<?xml version=\"1.0\""))
             {
                 //cprintf("find <?xml version=\"1.0\"?>\n");
                 UpnpCtrl.RecvLen = len - (p - buf);
                 UpnpCtrl.pRecv = UpnpTmpBuf; 
                 memcpy(UpnpCtrl.pRecv,p,UpnpCtrl.RecvLen);
                 UpnpCtrl.pRecv += UpnpCtrl.RecvLen;
                 //cprintf("First packet len = %d",UpnpCtrl.RecvLen);
             }
         }

         return;
    }
    else
    {
        i = 0;
        if(UpnpCtrl.RecvLen < UpnpTmpBufLen)
        {
            i = UpnpTmpBufLen - UpnpCtrl.RecvLen; 
        }
        else 
        {
            if(UpnpCtrl.AppUpnpCallBack)UpnpCtrl.AppUpnpCallBack(ERR_UPNP_MEM_ERR); /* tmp内存分配的过少 */   
        }
        i = i > len? len:i;
        if(i)memcpy(UpnpCtrl.pRecv,buf,i);
        UpnpCtrl.RecvLen += len;
        UpnpCtrl.pRecv += len;
        //cprintf("UpnpCtrl.RecvLen = %d\n",UpnpCtrl.RecvLen);
        if(UpnpCtrl.RecvLen < UpnpCtrl.RecvTotalLen)return;                      /* 数据未处理完，则等待继续读数据 */
    }
    p = UpnpTmpBuf;
    while(1)
    {
        //cprintf("%s\n",p);
        p = strcasestr(p, "<serviceList>");                                     /* 服务列表的开始地址 */
        lp = strcasestr(p, "</serviceList>");                                   /* 服务列表的结束地址 */
        if((p == NULL)||(lp == NULL))
        {
            cprintf("nono find serviceList = %d   %d\n",p,lp);
            break;                         /* 未找到服务列表 */
        }
        if(wp = strcasestr(p, "service:WANIPConnection"))                       /* 说明是service:WANIPConnection服务列表 */
        {
            u8 blen;
            u8 *CtrlUrlBuf;
            u8 wrflag = 0;

            if(wp = strcasestr(wp, "<controlURL>"))                             /* 找到控制URL*/
            {
                lp = strcasestr(wp, "</controlURL>");                           /* 找到控制URL的结束 */
                if(lp == NULL)break;
                wp = strcasestr(wp, "/");                                       /* 找到<controlURL>后的第一个'/'*/
                i = lp - wp;
                
                blen = UrlDirBuf[0];//FlashOneByteRead(FlashCtrlUrlAddr);
                if(blen != i)wrflag = 1;
                else
                {
                     for(blen = 0; blen < i; blen++)
                     {
                        // if(wp[blen] != FlashOneByteRead(FlashCtrlUrlAddr + 1 + blen))/* 第0个字节保存了长度，所以在对比的时候要FlashCtrlUrlAddr + 1*/
                         if(wp[blen] != UrlDirBuf[1 + blen])                    /* 第0个字节保存了长度，所以在对比的时候要FlashCtrlUrlAddr + 1*/
                         {
                            wrflag = 1;
                            break;   
                         }
                     }
                }
                if(wrflag)
                {
                    if(i > (UPNP_MEM_SIZE - 1))return;                        /* 检查长度，防止字符串过长，导致程序异常 */
                    //FlashBytes(UpnpFlashAddr,UpnpSockSendBuf,512);
                    CtrlUrlBuf =  UrlDirBuf;//UpnpSockSendBuf + FLASH_LOCAT_CTIRL_URL;
                    *CtrlUrlBuf++ = i;                                          /* 将长度保存 */
                    memset(CtrlUrlBuf,0,UPNP_MEM_SIZE - 1);
                    memcpy(CtrlUrlBuf,wp,i);
                    //FlashWrite(UpnpFlashAddr,(u8 *)UpnpSockSendBuf, 512);
                    cprintf("Control Url:%s\n",CtrlUrlBuf); 
                }
                fag = 1;
                UpnpCtrl.UpnpFlags |= DEF_UPNP_FIND;                            /* 找到UPNP设备,状态机不做任何改变  */
                break; 
            }
            else break;
        }
        else p = lp;
        if(p > UpnpCtrl.pRecv)break;
    }
    UpnpCtrl.UpnpMach = DEF_UPNP_GET_CLOSEING;                                  /* 置为等待关闭状态 */
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPInitPostCommand
* Description    : 初始化Post命令
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPInitPostCommand(u8 socketindex)
{
   u8 *buf;
#define TMP_LEN   60
   u8 tmp[TMP_LEN];
   u8 *p,*urlbuf;
   u8 *lp;
   u8 i;
   u32 len,blen;
   u8 *CtrlUrlBuf;
   u32 *ip;
   u8 sysip[4];

   memset(UpnpSockSendBuf,0,SocketSendBufLen);
   buf = UpnpSockSendBuf;
   memcpy(buf,"POST ",strlen("POST "));                                         /* POST命令 */
   CtrlUrlBuf = (u8 *)&UrlDirBuf[1];//(u8 *)(UpnpFlashAddr + FLASH_LOCAT_CTIRL_URL + 1);/* 将地址直接指向FLASH中 */
   strcat(buf,CtrlUrlBuf);                                                      /* 填入CTRL URL*/
   strcat(buf," HTTP/1.1\r\n");
   strcat(buf,"HOST: ");

   urlbuf = (u8 *)&UrlBuf[1];//(u8 *)(UpnpFlashAddr + FLASH_LOCAT_URL + 1);     /* 将地址直接指向FLASH中 */

   p = strcasestr(urlbuf,"http://");
   p += sizeof("http://") - 1;
   lp = strcasestr(p,"/");
   i = lp - p;
   memset(tmp,0,TMP_LEN);
   memcpy(tmp,p,i);                                                    
   cprintf("Host:%s\n",tmp);
   strcat(buf,tmp);                                                             /* 填入HOST:192.168.1.1:1900 */
   strcat(buf,"\r\n");
   /* 填入长度值*/
   
   len = 607;
   len += strlen(UpnpCtrl.MapPortDes);
   if(UpnpCtrl.UpnpProType == MAP_TYPE_TCP)len += strlen("TCP");
   else len += strlen("UDP");

   memset(tmp,0,TMP_LEN);
   sprintf(tmp,"%d",UpnpCtrl.InternelPort);
   len += strlen(tmp);


   memset(tmp,0,TMP_LEN);
   sprintf(tmp,"%d",UpnpCtrl.ExternalPort);
   len += strlen(tmp);

   memset(tmp,0,TMP_LEN);
   sysip[0] = UpnpCtrl.IPAddr[0];
   sysip[1] = UpnpCtrl.IPAddr[1];
   sysip[2] = UpnpCtrl.IPAddr[2];
   sysip[3] = UpnpCtrl.IPAddr[3];
   CH563NET_InetNtoa(tmp,sysip);
   len += strlen(tmp);
   blen = len;
   cprintf("len len len = %d\n",blen);
   
   strcat(buf,"Content-Length: ");
   len = strlen(buf);

    sprintf(&buf[len],"%d\r\n",blen);
   strcat(buf,"Content-Type: text/xml; charset=\"utf-8\"\r\n");
   strcat(buf,"SOAPACTION: \"urn:schemas-upnp-org:service:WANIPConnection:1#AddPortMapping\"\r\n\r\n");
   len = strlen(buf);
   //cprintf("222222222222222 = %d\n",len);
   CH563NET_SocketSend(socketindex,buf,(u32 *)(&len)); 
   //cprintf("%s\n",buf);
   buf = UpnpSockSendBuf;                                                       /* 上一步已经将POST头部封装完毕，开始封装命令 */
   memset(UpnpSockSendBuf,0,SocketSendBufLen);

   memcpy(buf,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n",strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"));
   //memcpy(buf,"<s:Envelope\r\n",strlen("<s:Envelope\r\n"));
   //strcat(buf,"<s:Envelope\r\n");
   strcat(buf,"<s:Envelope");
   strcat(buf," xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"");
//   strcat(buf,"    xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"");

   strcat(buf," s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n");
   strcat(buf,"<s:Body>\r\n");
   strcat(buf,"<u:AddPortMapping xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">\r\n");
   strcat(buf,"<NewRemoteHost></NewRemoteHost>\r\n");

   strcat(buf,"<NewExternalPort>");
   
   len = strlen(buf);                                                           /* 计算当前的buf字符串的长度 */
   sprintf(&buf[len],"%d",UpnpCtrl.ExternalPort);                               /* 外部端口 */
   strcat(buf,"</NewExternalPort>\r\n");                                        /* 行结束 */

   strcat(buf,"<NewProtocol>");
   if(UpnpCtrl.UpnpProType == 0) strcat(buf,"TCP");
   else  strcat(buf,"UDP");

   strcat(buf,"</NewProtocol>\r\n");                                            /* 行结束 */
                                                                                
   strcat(buf,"<NewInternalPort>");                                             /* 内部端口 */
   len = strlen(buf);                                                           
   sprintf(&buf[len],"%d",UpnpCtrl.InternelPort);                               /* 内部端口 */
   strcat(buf,"</NewInternalPort>\r\n");                                        /* 行结束 */

   strcat(buf,"<NewInternalClient>");                                           /* 内部主机IP地址 */
   sysip[0] = UpnpCtrl.IPAddr[0];
   sysip[1] = UpnpCtrl.IPAddr[1];
   sysip[2] = UpnpCtrl.IPAddr[2];
   sysip[3] = UpnpCtrl.IPAddr[3];
   len = strlen(buf);
   CH563NET_InetNtoa(&buf[len],sysip);
  
   strcat(buf,"</NewInternalClient>\r\n");                                      /* 行结束 */

   strcat(buf,"<NewEnabled>1</NewEnabled>\r\n");                                /* 开启UPNP端口映射，为0表示关闭 */

   strcat(buf,"<NewPortMappingDescription>");                                   /* 映射端口描述  */
   strcat(buf,UpnpCtrl.MapPortDes);
   strcat(buf,"</NewPortMappingDescription>\r\n");                              /* 行结束 */

   strcat(buf,"<NewLeaseDuration>1600</NewLeaseDuration>\r\n");         
   strcat(buf,"</u:AddPortMapping>\r\n");         
   strcat(buf,"</s:Body>\r\n");         
   strcat(buf,"</s:Envelope>\r\n");         
   strcat(buf,"\r\n");         
   len = strlen(buf);
   blen = len;
   CH563NET_SocketSend(socketindex,buf,(u32 *)(&len));
   if(len == blen)
   {
      cprintf("Send post success\n");
   } 
}

/*******************************************************************************
* Function Name  : UPNP_CreatTcpPostSocket
* Description    : 创建POST的socket连接
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UPNP_CreatTcpPostSocket(void)
{
    u8 i;
    u8 *p,*lp,*urlbuf;
    u8 tmp[60];
    u8 addr[4];
    u16 port;
    SOCK_INF  TmpSocketInf;
    
    cprintf("Creat Post Socket\n");
    //FlashBytes(UpnpFlashAddr,UpnpSockSendBuf,512);
    urlbuf =  (u8 *)&UrlBuf[1];//UpnpSockSendBuf + FLASH_LOCAT_URL + 1;
    p = strcasestr(urlbuf,"http://");                                           /* 通过URL查找到IP地址 */
    p += sizeof("http://") - 1;                                                 /* 格式为HTTP://+IP+:+PORT */
    lp = strcasestr(p,":");                                                     /* 查找冒号 */
    i = lp - p;                                                                 /* 计算长度 */
    memset(tmp,0,20);                                                           /* 将临时缓冲区全部清零 */
    memcpy(tmp,p,i);                                                            /* 将IP字符串复制到临时缓冲区中 */
    cprintf("ipstr %s\n",tmp);                                                  
    CH563NET_Aton((const char *)tmp,addr);                                      /* 将字符传转换为IP地址 */
    cprintf("IP: %d.%d.%d.%d\n",addr[0],addr[1],addr[2],addr[3]);               /* 将IP调试打印 */
    lp++;                                                                       
    p = strcasestr(lp,"/");                                                     /* 查找端口 */
    i = p - lp;                                                                 
    memset(tmp,0,20);                                                           /* 将临时缓冲区全部清零 */
    memcpy(tmp,lp,i);                                                           /* 将IP字符串复制到临时缓冲区中 */
    port = atoi(tmp);                                                           /* 计算端口 */
    cprintf("Port = %d\n",port);                                                
                                                                                
   memset(&TmpSocketInf,0,sizeof(TmpSocketInf));                                /* 将临时缓冲区清除 */
   memcpy((void *)TmpSocketInf.IPAddr,addr,4);                                  /* 设置目的IP地址 */
   TmpSocketInf.SourPort = UVCIP_GetPort();                                     /* 设置源端口 */
   TmpSocketInf.DesPort = port;                                                 /* 设置目的端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 协议类型 */
   TmpSocketInf.RecvStartPoint = (UINT32)UpnpSockRecvBuf;                       /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = SocketRecvBufLen ;                                 /* 设置接收缓冲区的接收长度 */
   i = CH563NET_SocketCreat(&UpnpCtrl.TcpSocket,&TmpSocketInf);
   if(i != CH563NET_ERR_SUCCESS) 
   {
      cprintf("Creat Fail %02x\n",i);
      return;                                                                   /* 创建失败直接返回 */
   }
   UpnpCtrl.TcpSocket |= SOCKET_ENABLE_FLAG;                                    /* 创建成功，置socket为有效 */
   i = CH563NET_SocketConnect(UpnpCtrl.TcpSocket & ~SOCKET_ENABLE_FLAG);
   if(i != CH563NET_ERR_SUCCESS) 
   {
      cprintf("Connect Fail %02x\n",i);
      return;                                                                   /* 创建失败直接返回 */
   }
}
/*******************************************************************************
* Function Name  : CH563NET_UPNPStartMapPort
* Description    : APP可以调用此函数，来进行端口映射
* Input          : None
* Output         : None
* Return         : TRUE 失败，FALSE表示成功
*******************************************************************************/
u8 CH563NET_UPNPStartMapPort(void)
{
    if(UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG)                                 /* 可能此时UPNP在用TCP socket此时不可以再次使用了 */
    {                                                               
       if((UpnpCtrl.UpnpMach < DEF_UPNP_POST_CONNECT)|| (UpnpCtrl.UpnpMach > DEF_UPNP_POST_CLOSEING)) /* 此时可能在GET状态，GET的优先级比较高，如果在GET状态则不会创建POST */
        {                                                                       /* 判断是否在GET状态 */
           UpnpCtrl.TcpSocket &= ~ SOCKET_ENABLE_FLAG;                          /* 不在GET状态，则将TCP直接复位 */
           CH563NET_SocketClose(UpnpCtrl.TcpSocket,1);     
           UpnpCtrl.TcpSocket = 0;
        }
        else return TRUE;  
    }
    if(UpnpCtrl.UpnpFlags & DEF_UPNP_FIND)                                      /* 检查UPNP设备是否已经存在 */
    {                                                                           /* */
        UPNP_CreatTcpPostSocket();                                              /* 创建 POST的SOCKET*/
        UpnpCtrl.TimeCount = UPNP_TIMEOUT_10S;                                  /* 10秒超时 */
        UpnpCtrl.UpnpMach = DEF_UPNP_POST_CONNECT;                              /* 进入等待POST连接状态 */
        return FALSE;
    }
    else return  TRUE;
}


/*******************************************************************************
* Function Name  : CH563NET_UPNPReceivePostCmd
* Description    : Post命令接收函数，循环接收，接收完毕后分析UPNP是否成功
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPReceivePostCmd(u8 *buf,u32 len)
{
    
    u8  *p = buf;
    u8  *lp;
    u8  *wp; 
    u16  i;
    u8  fag = 0;

#if 1
    if(UpnpCtrl.RecvTotalLen == 0)                                              /* 接收的总长度为0，表示还计算出接收的数据长度 ，为第一包数据 */
    {                                                                           /* 计算出总长度 */
         
         if(p = strcasestr(p, "OK"))
         {
            cprintf("*****************\n");
            cprintf("UPNP Successful\n");
            cprintf("*****************\n");
            if(UpnpCtrl.AppUpnpCallBack)UpnpCtrl.AppUpnpCallBack(ERR_UPNP_SUCCESS);
            UpnpCtrl.MapTime = UpnpCtrl.MaxAgeBack;
         }
         else
         {
            p = buf;
            if(p = strcasestr(p, "Error"))
            {
                UpnpCtrl.MapTime = 0;
                UpnpCtrl.UpnpFlags &= ~DEF_UPNP_FIND;                           /* 本次未注册成功，将设备清除 */
                if(UpnpCtrl.AppUpnpCallBack)UpnpCtrl.AppUpnpCallBack(ERR_UPNP_FAIL);
                cprintf("*****************\n");
                cprintf("UPNP Fail\n");
                cprintf("*****************\n");
            }   
         }
         p = buf;
         p = strcasestr(p, "Content-Length:");                                  /* 找到长度内部标志 */
         if(p)                                                                  /* 计算长度  */
         {                                                                      
             u32 xlen = 0;                                                      
             cprintf("Find Content-Length:\n");                                 
             lp = strcasestr(p, "\r\n");                                        /* 找到本行结束 */
             p += sizeof("Content-Length:") - 1;                                /* 将指针偏移到正确位置 */
             i = *lp;                                                           /* 备份*lp */
             *lp = 0;                                                           
             xlen = atoi(p);                                                    
             *lp = i;                                                           /* 还原lp */
             cprintf("Pose Receive Cmd = %d\n",xlen);
             UpnpCtrl.RecvTotalLen = xlen;
             cprintf("UpnpCtrl.RecvTotalLen = %d\n",xlen);
             UpnpCtrl.RecvLen = 0;
             p = buf;
             if(p = strcasestr(p,"<?xml version=\"1.0\""))
             {
                 cprintf("find <?xml version=\"1.0\"?>\n");
                 UpnpCtrl.RecvLen = len - (p - buf);
                 UpnpCtrl.pRecv = UpnpTmpBuf; 
                 memcpy(UpnpCtrl.pRecv,p,UpnpCtrl.RecvLen);
                 UpnpCtrl.pRecv += UpnpCtrl.RecvLen;
                 cprintf("First packet len = %d",UpnpCtrl.RecvLen);
             }
         }
         return;
    }
    else
    {
        memcpy(UpnpCtrl.pRecv,buf,len);
        UpnpCtrl.RecvLen += len;
        UpnpCtrl.pRecv += len;
        cprintf("UpnpCtrl.RecvLen = %d\n",UpnpCtrl.RecvLen);
        if(UpnpCtrl.RecvLen < UpnpCtrl.RecvTotalLen)return;                     /* 数据未处理完，则等待继续读数据 */
    }
    UpnpCtrl.UpnpMach = DEF_UPNP_POST_CLOSEING;                                 /* 置为等待关闭状态 */
 #endif
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPTCPConnectHandle
* Description    : 处理UPNP的TCP连接事件，由外部调用
* Input          : socket 索引
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTCPConnectHandle(u8 socketid)
{
    switch(UpnpCtrl.UpnpMach)
    {
        case DEF_UPNP_GET_CONNECT:                                              /* GET连接成功 */
             cprintf("CH563NET_UPNPTcpSendGetCommand.....\n");                  
             CH563NET_UPNPTcpSendGetCommand(socketid);                          /* 发送GET命令 */
             UpnpCtrl.pRecv = UpnpTmpBuf;                                       /* 初始化接收指针 */
             UpnpCtrl.RecvLen = 0;                                              /* 初始化接收长度 */
             UpnpCtrl.RecvTotalLen = 0;                                         /* 初始化接收总长度 */
             UpnpCtrl.UpnpMach = DEF_UPNP_GET_RECV;                             /* 等待接收 GET的应答 */
             break;                                                                  
       case DEF_UPNP_POST_CONNECT:                                              /* POST连接成功 */
            CH563NET_UPNPInitPostCommand(socketid);                             /* 启动UPNP注册 */
            UpnpCtrl.pRecv = UpnpTmpBuf;                                        /* 初始化接收指针 */
            UpnpCtrl.RecvLen = 0;                                               /* 初始化接收长度 */
            UpnpCtrl.RecvTotalLen = 0;                                          /* 初始化接收总长度 */
            UpnpCtrl.UpnpMach = DEF_UPNP_POST_RECV;                             /* 等待接收 POST的应答 */
            break;
        default:
            break;
    }
}
/*******************************************************************************
* Function Name  : CH563NET_UPNPTCPDisConnectHandle
* Description    : 处理UPNP的TCP断开连接事件，由外部调用,由断开或者超时调用
* Input          : socket 索引
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPTCPDisConnectHandle(u8 socketid)
{
    u8  i;

    switch(UpnpCtrl.UpnpMach)
    {
        case DEF_UPNP_GET_CLOSEING:                                             /* GET等待关闭，进入下一个状态 */
             UpnpCtrl.TcpSocket &= ~ SOCKET_ENABLE_FLAG;
             i = CH563NET_SocketClose(socketid,0);
             cprintf("Get Socket Close %02x\n",i);
             UpnpCtrl.TimeCount = 0;                                            /*关闭连接时清除定时器stop timer */
             UpnpCtrl.TcpSocket = 0;
             UpnpCtrl.UpnpMach = DEF_UPNP_START;
             CH563NET_UPNPStartMapPort();                                       /* 启动映射 */
        break;
        case DEF_UPNP_POST_CLOSEING:                                            /* POST等待断开 */
             i = CH563NET_SocketClose(socketid,0);
             UpnpCtrl.UpnpMach = DEF_UPNP_START;
             UpnpCtrl.TcpSocket = 0;
             cprintf("Post connect close \n");
             UpnpCtrl.TcpSocket = 0;
             UpnpCtrl.TimeCount = 0;                                            /*stop timer */
            break;
        default:
             CH563NET_SocketClose(socketid,1);
             if(UpnpCtrl.AppUpnpCallBack)UpnpCtrl.AppUpnpCallBack(ERR_UPNP_TCP_CONNECT);
             UpnpCtrl.TcpSocket = 0;
             UpnpCtrl.TimeCount = 0;
             UpnpCtrl.MapTime = 0;                                              /*stop timer */
             UpnpCtrl.UpnpFlags &= DEF_UPNP_FIND;                               /* 在意外的情况下连接被关闭，则UPNP清除 */
        break;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_UPNPReceiveDataHandle
* Description    : 接收数据处理函数，由外部调用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_UPNPReceiveDataHandle(u8 socket)
{
    u32 rlen;
    u32 addr;

    if((UpnpCtrl.UdpSocket & SOCKET_ENABLE_FLAG)&&\
      (socket == (UpnpCtrl.UdpSocket & ~SOCKET_ENABLE_FLAG)))
    {
        rlen = CH563NET_SocketRecvLen(socket,&addr);
        CH563NET_UPNPRecvMultiBrocast(socket & ~SOCKET_ENABLE_FLAG,(u8 *)addr);
        CH563NET_SocketRecv(socket,NULL,&rlen);
    }
    else if((UpnpCtrl.TcpSocket & SOCKET_ENABLE_FLAG)&&\
            (socket == (UpnpCtrl.TcpSocket & ~SOCKET_ENABLE_FLAG)))
    {
        rlen = CH563NET_SocketRecvLen(socket,&addr);
        switch(UpnpCtrl.UpnpMach)
        {
            case DEF_UPNP_GET_RECV:
                CH563NET_UPNPReceiveGetCmd((u8 *)addr,rlen);
                CH563NET_SocketRecv(socket,NULL,&rlen);
            break;
            case DEF_UPNP_POST_RECV:
                cprintf("Post\n");
                CH563NET_UPNPReceivePostCmd((u8 *)addr,rlen);
                CH563NET_SocketRecv(socket,NULL,&rlen);
            break;
        }
    }
    else
    {
        rlen = CH563NET_SocketRecvLen(socket,&addr);
        CH563NET_SocketRecv(socket,NULL,&rlen);
    }
}
