#ifndef __UPNP_H__
#define __UPNP_H__
/*=============================================================================*/
/* 自定义变量类型 */
/*-----------------------------------------------------------------------------*/
typedef unsigned long                             u32;
typedef unsigned short                            u16;
typedef unsigned char                             u8;
/*=============================================================================*/
/* 调试模式 */
/*-----------------------------------------------------------------------------*/
#define UPNP_ENABLE                               1                             /* 开启UPNP功能 */
#define UPNP_BUG_ENABLE                           1                             /* DEBUG使能 */
/*=============================================================================*/
/* SSDP功能禁用 */
/*-----------------------------------------------------------------------------*/
#define ENABLE_SSDP_FUN                           0                             /* 开启/禁用SSDP功能,UDP广播 */
/*=============================================================================*/
/* 端口范围 */
/*-----------------------------------------------------------------------------*/
#define UVC_IP_MIN_PORT                           40000
#define UVC_IP_MAX_PORT                           41000
/*=============================================================================*/
/* UPNP状态机定义 */
/*-----------------------------------------------------------------------------*/
#define DEF_UPNP_STOP                             0X00                          /* UPNP 停止 */
#define DEF_UPNP_START                            0X01                          /* UPNP 开启 */
#define DEF_UPNP_WAIT_URL                         0X02                          /* 等待URL地址 */
#define DEF_UPNP_GET_CONNECT                      0X03                          /* 发起GET的TCP连接 */
#define DEF_UPNP_GET_RECV                         0X04                          /* GET连接成功，并发送GET命令后，等待UPNP回应 */
#define DEF_UPNP_GET_CLOSEING                     0X05                          /* GET连接成功，接收完数据，等待关闭，关闭后立即启动POST */
#define DEF_UPNP_POST_CONNECT                     0X06                          /* 发起GET的TCP连接 */
#define DEF_UPNP_POST_RECV                        0X07                          /* POST等待接收数据，以便检查映射失败还是成功 */
#define DEF_UPNP_POST_CLOSEING                    0X08                          /* POST连接成功，接收完数据，等待关闭，关闭后立即启动POST */
/*=============================================================================*/
/* 内存块定义 */
/*-----------------------------------------------------------------------------*/
#define UPNP_MEM_SIZE                             170
/*=============================================================================*/
/* 超时定义 */
/*-----------------------------------------------------------------------------*/
#define UPNP_TIME_PERIOD                          250                           /* 定时器周期250MS*/
#define UPNP_TIME_SECOND                          1000                          /* 1S的时间 */
#define UPNP_TIMEOUT_10S                          (UPNP_TIME_SECOND*10)         /* 10S超时 */
#define UPNP_TIMEOUT_180S                         (UPNP_TIME_SECOND*10)        /* 180S超时 */
#define UPNP_TIMEOUT_20S                          (UPNP_TIME_SECOND*20)         /* 10S超时 */
#define UPNP_MAX_AGE_ADD                          (UPNP_TIME_SECOND*5)          /* 在MAX-AGE基础上加UPNP_MAX_AGE_ADD，防止误检测到UPNP设备离线 */
/*=============================================================================*/
/* 映射协议类型 */                                                                   
/*-----------------------------------------------------------------------------*/
#define MAP_TYPE_TCP                              0                            /* 映射协议类型为TCP */
#define MAP_TYPE_UDP                              1                            /* 映射协议类型为UDP */
/*=============================================================================*/
/*端口定义 */                                                                   
/*-----------------------------------------------------------------------------*/
#define DEF_UPNP_MULTI_SPORT                      1890                          /* 多播本地端口 */
#define DEF_UPNP_MULTI_DPORT                      1900                          /* 多播目的端口 */
/*=============================================================================*/
/* 索引有效标志 */                                                              
/*-----------------------------------------------------------------------------*/
#define SOCKET_ENABLE_FLAG                        0X80                          /* socket时是否有效 */
/*=============================================================================*/
/* 定义3中服务 */
/*-----------------------------------------------------------------------------*/
#define  UPNP_SERVER_WANIPCONNECT                 1
#define  UPNP_SERVER_WANCOMMINTER                 2
#define  UPNP_SERVER_LAY3FORWARD                  3
/*=============================================================================*/
/* 错误码 */
/*-----------------------------------------------------------------------------*/
#define ERR_UPNP_SUCCESS                          0
#define ERR_UPNP_FAIL                             1
#define ERR_UPNP_TCP_CONNECT                      0x30
#define ERR_UPNP_MEM_ERR                          0x31

/*=============================================================================*/
/* PNP标志位定义 */
/*-----------------------------------------------------------------------------*/
#define  DEF_UPNP_FIND                            (1<<0)
/*=============================================================================*/
/* 函数声明 */                                                                   
/*-----------------------------------------------------------------------------*/
typedef __packed struct _UP_CTRL
{
   u32  MaxAgeBack;                                                             /* MAX-AGE备份时间，供MAP端口计时 */
   u32  MaxAgeCount;                                                            /* 最大生存时间  */
   u32  MapTime;                                                                /* 发现包最大定时 */
   u32  TimeCount;                                                              /* 定时器当前时间 */
   u32  SSDPTime;                                                               /* SSDP超时未UDP专用定时器 */
   u8   *pRecv;                                                                 /* 接收数据指针 */
   u16  RecvLen;                                                                /* 接收数据的长度 */
   u16  RecvTotalLen;                                                           /* 接收数据的总长度 */
   u16  ExternalPort;                                                           /* UPNP 外部端口 */
   u16  InternelPort;                                                           /* UPNP内部端口 */
   u8   UpnpProType;                                                            /* UPNP协议类型，0为TCP,1为UDP*/
   u8   IPAddr[4];
   u8   GateIPAddr[4];
   u8   UdpSocket;                                                              /* UPNP的UDP端口，最高位为1表示有效 */
   u8   TcpSocket;                                                              /* UPNP的TCP端口，最高位为1表示有效 */
   u8   MaxAgeStart;                                                            /* 启动MaxAge计时 */
   u8   TimeInt;                                                                /* 定时器中断 */
   u8   UpnpMach;                                                               /* 状态机  */
   u8   UpnpFlags;                                                              /* 标志位，参考UPNP标志位定义 */
   const u8 *MapPortDes;                                                        /* MAP端口描述 */
   void (*AppUpnpCallBack)(u8 errcdoe);                                         /* 用户回调函数 */
}UPNP_CTRL;

/*=============================================================================*/
/* UPNP应用层结构体，启动时传递 */                                                                   
/*-----------------------------------------------------------------------------*/
typedef struct _UPNP
{
   u16  InternelPort;                                                           /* UPNP内部端口 */
   u16  ExternalPort;                                                           /* UPNP 外部端口 */
   u8   IPAddr[4];                                                              /* 映射端口的IP地址 */
   u8   UpnpProType;                                                            /* UPNP协议类型，0为TCP,1为UDP*/
   const u8 *MapPortDes;                                                        /* MAP端口描述 */
}UPNP_MAP;
/*=============================================================================*/
/* UPNP应用层结构体，初始化传递参数使用 */                                                                   
/*-----------------------------------------------------------------------------*/
typedef struct _UPNP_CFG
{
   u8  *UpnpMemory;
   u32  MemoryLen;
   u32  TcpMss;
   u8   GateIPAddr[4];                                                          /* 网关(UPNP设备)的IP地址*/
   void (*AppUpnpCallBack)(u8 errcdoe);                                         /* 用户回调函数 */
}UPNP_CFG;
/*=============================================================================*/
/* 函数声明 */                                                                   
/*-----------------------------------------------------------------------------*/
u8 CH563NET_UPNPInit(UPNP_CFG *upnpcfg);

void CH563NET_UPNPStart(UPNP_MAP *upnpmap);

u8 CH563NET_UPNPCheckSocketAvail(u8 socket);

void UPNP_CreatTcpPostSocket(void);

void CH563NET_UPNPTimeHandle(void);

void CH563NET_UPNPCreatUpdSocket(void);

void CH563NET_UPNPSendSSDPacket(void);

void CH563NET_UPNPRecvMultiBrocast(u8 socketid,u8 *buf);

void CH563NET_UPNPCreatTcpGetSocket(void);

u8 CH563NET_UPNPStartMapPort(void);

void CH563NET_UPNPInitPostCommand(u8 socketindex);

void CH563NET_UPNPTcpSendGetCommand(u8 socketindex);

void CH563NET_UPNPReceivePostCmd(u8 *buf,u32 len);

void CH563NET_UPNPReceiveGetCmd(u8 *buf,u32 len);

void CH563NET_UPNPReceiveDataHandle(u8 socket );

void CH563NET_UPNPMainTask(void);

void CH563NET_UPNPTCPDisConnectHandle(u8 socketid);
#endif