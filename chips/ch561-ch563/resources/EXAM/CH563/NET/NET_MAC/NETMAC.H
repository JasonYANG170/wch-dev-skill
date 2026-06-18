/********************************** (C) COPYRIGHT *******************************
* File Name          : EHCI_HCD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : Description: CH563MAC Non-OS firmware header file 
*******************************************************************************/



/******************************************************************************/
#ifndef __NETMAC_H__
#define __NETMAC_H__                            

/******************************************************************************/
#define ETHER_HEAD_LEN                     14
#define MAC_MAX_LEN                        1502
#define CH563_DEBUG                        1                                    /* 接收缓冲区队列数 */
#define RECE_BUF_LEN                       0x5ec                                /* 接收缓冲区的大小 */


/*
 * MAC层发送接收缓冲区相关参数定义
 */
/* 队列数不能小于2 */
#define RX_QUEUE_NUM                       3                                    /* 接收缓冲区队列数 */
#define TX_QUEUE_NUM                       2
#define RX_BUF_SIZE                        0x5ec                                /* 接收缓冲区大小 */
#define TX_BUF_SIZE                        0x5ec                                /* 发送缓冲区大小 */

#define BUF_IDLE                           0                                    /* 缓冲区空闲 */
#define BUF_MAC_LOCK                       1                                    /* 缓冲区MAC锁定 */
#define BUF_IP_LOCK                        2                                    /* 缓冲区IP锁定 */

#define IPADDR_LEN                         4
#define MACADDR_LEN                        6
#define NET_SOCKET_NUM                     20
/* 
 * PHY状态定义 
 */
#define PHY_DISCONN                         (1<<0)                              /* PHY断开 */
#define PHY_10M_FLL                         (1<<1)                              /* 10M全双工 */
#define PHY_10M_HALF                        (1<<2)                              /* 10M半双工 */
#define PHY_100M_FLL                        (1<<3)                              /* 100M全双工 */
#define PHY_100M_HALF                       (1<<4)                              /* 100M半双工 */
#define PHY_AUTO                            (1<<5)                              /* PHY自动模式，CMD10H_SET_PHY */
        
#define FTPHY_REG_CONTROL                  0x00
#define FTPHY_REG_STATUS                   0x01
#define FTPHY_RB_PHYAD                     0x10
/*
 * 全局变量定义
 */
extern UINT8   MACRxBuf[RX_QUEUE_NUM][RX_BUF_SIZE];                             /* MAC接收缓冲区，4字节对齐 */
extern UINT8   MACTxBuf[TX_QUEUE_NUM][TX_BUF_SIZE];                             /* MAC发送缓冲区，4字节对齐 */
extern UINT8   RecvBuf[RX_BUF_SIZE];
extern PUINT8  MACSendbuf;
extern UINT8V  PHYStatus;
extern UINT8V  SendSuc;                                                         /* 发送数据包 成功标志 */
extern UINT8   CH563MACAddr[MACADDR_LEN];                                       /* MAC地址 */
extern UINT8   CH563IPAddr[IPADDR_LEN];
extern UINT8   IPDestAddr[IPADDR_LEN];


/* 
 * RX接收控制信息表 
 */
struct _RXBUFST
{
    UINT8  RecvIndex;                                                           /* MAC层接收缓冲区的索引 */
    UINT8  ReadIndex;                                                           /* 读取缓冲区的索引值 */
    UINT8  RemainCout;                                                          /* 剩余未读取的数据包 */
    UINT8  RxDMAEnable;                                                         /* 接收DMA使能标志，在缓冲区溢出时，可以关闭RX DMA*/
    UINT8  RxBufStau[RX_QUEUE_NUM];                                             /* 接收缓冲区的状态 */
    UINT16 RxBufLen[RX_QUEUE_NUM];                                              /* 接收缓冲区数据的有效长度 */
    UINT32 RxBufAddr[RX_QUEUE_NUM];                                             /* 接收缓冲区的起始地址 */
};
extern struct  _RXBUFST    RxCtrl;                                     

/* 
 * TX接收控制信息表 
 */                                       

struct _TXBUFST
{                                                                               
    UINT8  SendIndex;                                                           /* 发送描述符索引 */
    UINT8  TxBufStat[TX_QUEUE_NUM];                                             /* 发送缓冲区状态 */
};
extern struct _TXBUFST    TxCtrl;                                      

/*
 * 发送描述符，16字节对齐
 */
typedef struct _CH563MAC_TXDES {
    UINT32 txdes0; /* 包含了帧状态和描述符权限 */
    UINT32 txdes1; /* 控制位与发送缓冲区大小 */
    UINT32 txdes2; /* 发送缓冲区的基地址 */
    UINT32 txdes3; /* 没有使用，软件用于保存下一个描述符地址 */
}mac_txdes;
extern UINT8  TxDesBuf[TX_QUEUE_NUM*16];

/*
 * 接收描述符，16字节对齐
 */
typedef struct _CH563MAC_RXDES 
{
    UINT32 rxdes0; /* 包含了帧状态和描述符权限 */
    UINT32 rxdes1; /* 控制位与发送缓冲区大小 */
    UINT32 rxdes2; /* 接收缓冲区的基地址 */
    UINT32 rxdes3; /* 没有使用，软件用于保存下一个描述符地址 */
}mac_rxdes;
extern UINT8  RxDesBuf[(RX_QUEUE_NUM )*16]; 

/*
 * CH563MAC控制结构
 */
typedef struct _CH563MAC_Control
{
    mac_txdes *txdes_top;                                                       /* 发送描述符顶部 */
    mac_txdes *txdes_cur;                                                       /* 当前发送描述符 */
    mac_txdes *txdes_end;                                                       /* 最后一个发送描述符 */
    mac_rxdes *rxdes_top;                                                       /* 接收描述符顶部 */
    mac_rxdes *rxdes_cur;                                                       /* 当前接收描述符 */
    mac_rxdes *rxdes_end;                                                       /* 最后一个描述符 */
}MACCtrl;
extern MACCtrl   mac_cotrl;                                       


typedef union _ETHERHEAD
{
    UINT8  buffer[ETHER_HEAD_LEN];
    struct
    {
        UINT8   DestMAC[MACADDR_LEN];
        UINT8   SourMAC[MACADDR_LEN];
        UINT16  PROTOCOL;
        #define ETHER_TYPE_IP            0x0800    
        #define ETHER_TYPE_ARP          0x0806
    }Head;
}ETHERHEAD;
extern ETHERHEAD    ETH;                                                        /* mac层包头 */
#define ARP_ENTRY_TIME_OUT            10
#define ARP_PACKAGE_LEN                28
#define ARP_ENTRY_MAX_NUM            8

/* arppacket->type */
#define ARP_CODE_QUEST                0x0001    
#define ARP_CODE_REPLY              0x0002
#define RARP_CODE_QUEST                0x0003
#define RARP_CODE_REPLY                0x0004

typedef union _ARPPACKAGE
{
    UINT8  buffer[ARP_PACKAGE_LEN];
    struct  
    {
        UINT16 HRDTYPE;        // 硬件类型
        #define ARP_HARDWARE_TYPE_ETHER     0x0001
        UINT16 PROTYPE;        //
        #define ARP_PROTOCOL_TYPE_IP        0x0800
        UINT8  MACLEN;        //
        UINT8  IPLEN;        //
        UINT16 OPCODE;        //
        UINT8  SENDMAC[MACADDR_LEN];
        UINT8  SENDIP[IPADDR_LEN];
        UINT8  TARGMAC[MACADDR_LEN];
        UINT8  TARGIP[IPADDR_LEN];
    }Package;
}ARPPACKAGE;

/* 
 * 函数声明
 */
UINT16 GLOB_nTohs( UINT16 in );

void    NET_INIT( void );

void    NET_TimeIsr( UINT16 time );

UINT16  PHY_ReadData(UINT16 regad);

void    MAC_InitTXDes(void);

void    MAC_Isr(void);

UINT8   MAC_QueryInput(void);

UINT16  MAC_Input(UINT8 *buf,UINT16 *slen);

void    MAC_RecvFinishHanld( void );

UINT8   MAC_Output(UINT8 *buf,UINT16 *len,UINT16 code);

UINT8 ARP_Output( UINT8 *desip,UINT8 code );

void ARP_Input( UINT8 *arpdata,UINT16 len );

#endif
