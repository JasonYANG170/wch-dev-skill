/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563网络部分初始化及MAC收发接口函数
*******************************************************************************/



/******************************************************************************/
/* 文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "NETSFR.H"
#include "NETMAC.H"

/******************************************************************************/
/* 全局变量定义 */
ETHERHEAD    ETH;                                                               /* mac层包头 */
ARPPACKAGE   ARP;                                                               /* arp数据包 */
MACCtrl      mac_cotrl;                                                         /* CH563MAC控制结构 */
struct  _RXBUFST    RxCtrl;                                                     /* RX接收控制信息表 */
struct  _TXBUFST    TxCtrl;                                                     /* TX接收控制信息表 */
UINT8V  SendSuc;                                                                /* 发送数据包 成功标志 */
UINT8V  PHYStatus;                                                              /* PHY状态 */

/*******************************************************************************
* Function Name  : PHY_ReadData
* Description    : 从PHY读取数据，数据宽度为16位
* Input          : regad PHY寄存器地址 
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 PHY_ReadData(UINT16 regad)
{
    R32_ETHE_PHYCR = RB_PHYAD(FTPHY_RB_PHYAD) | RB_REGAD(regad) | RB_MIIRD;     /* 向PHY控制寄存器写入PHY地址，PHY寄存器地址，以及发送读信号*/
    Delay_us(100);
    while( R32_ETHE_PHYCR & RB_MIIRD );                                         /* 等待读完成 */
    return( (UINT16)(R32_ETHE_PHYCR & RB_MIIRDATA) );
}

/*******************************************************************************
* Function Name  : CH563PHY_Process
* Description    : PHY处理函数，根据协商结果，来配置MACCR寄存器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563PHY_Process(void)
{
    UINT16 value;

    value = PHY_ReadData(FTPHY_REG_STATUS);
    if( (value&RB_LINK_STATUS)==0 ){
    }
    if( (value&RB_AUTO_NEG_CMP)==0 ){
    }
    value = PHY_ReadData(FTPHY_REG_CONTROL);
    if((value & RB_DUPLEX_MODE) > 0){
        R32_ETHE_MACCR |= RB_FULLDUP;
    }
    else{
        R32_ETHE_MACCR &= ~(RB_FULLDUP);
    }
    if((value & RB_SPEED_SEL) > 0){
        R32_ETHE_MACCR |= RB_SPEED_100;
    }
    else{
        R32_ETHE_MACCR &= ~(RB_SPEED_100);
    }
}

/*******************************************************************************
* Function Name  : CH563MAC_ParamInit
* Description    : 参数初始化函数，初始化全局变量
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563MAC_ParamInit(void)
{
    memset((void *)&TxCtrl,0,sizeof(TxCtrl));
    memset((void *)&RxCtrl,0,sizeof(RxCtrl));
    RxCtrl.RxDMAEnable = TRUE;
}

/*******************************************************************************
* Function Name  : MAC_InitTXDes
* Description    : 初始化发送描述，发送描述符必须16字节对齐
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_InitTXDes(void)
{

    UINT32 i;
    mac_txdes *tmp;

    mac_cotrl.txdes_top = (mac_txdes *)TxDesBuf;                                /* 将描述符顶部指向描述符缓冲区头部 */
    mac_cotrl.txdes_cur = mac_cotrl.txdes_top;                                  /* 当前描述符指向描述符缓冲区头部*/
    mac_cotrl.txdes_end = mac_cotrl.txdes_top + (TX_QUEUE_NUM - 1);             /* 最后一个描述符指向缓冲区尾部*/
    memset(mac_cotrl.txdes_top, 0,sizeof(TxDesBuf));                            /* 清空发送描述符 */
    mac_cotrl.txdes_end->txdes1 |= TXDES1_EDOTR;                                /* 最后一个描述符置为结束标志 */
    mac_cotrl.txdes_cur->txdes2 |= (UINT32)MACTxBuf[TxCtrl.SendIndex];          /* 发送缓冲区的基地址 */
    MACSendbuf = (UINT8 *)&MACTxBuf[TxCtrl.SendIndex][14];
    tmp = mac_cotrl.txdes_top;
    for(i = 0; i < TX_QUEUE_NUM; i++){                                          /* 将所有描述符用链表连接 */
        tmp->txdes3 = (UINT32)(tmp + 1);
        tmp = (mac_txdes *)tmp->txdes3;
    }
    mac_cotrl.txdes_end->txdes3 = (UINT32)mac_cotrl.txdes_top;
}

/*******************************************************************************
* Function Name  : MAC_InitRXDes
* Description    : 初始化接收描述，接收描述符必须16字节对齐
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_InitRXDes(void)
{
    int i;
    mac_rxdes *tmp;

    mac_cotrl.rxdes_top = (void *)RxDesBuf;                                     /* rxdes_top指向第一个描述符 */
    mac_cotrl.rxdes_cur = mac_cotrl.rxdes_top;                                  /* rxdes_cur指向第一个描述符 */
    mac_cotrl.rxdes_end = mac_cotrl.rxdes_top + (RX_QUEUE_NUM - 1);             /* rxdes_end指向最后一个描述符 */
    memset(mac_cotrl.rxdes_top, 0, sizeof(RxDesBuf));                           /* 缓冲区清空*/
    mac_cotrl.rxdes_end->rxdes1 |= RXDES1_EDORR;                                /* 最后一个描述符置为结束标志 */    
    mac_cotrl.rxdes_cur->rxdes2 |= (UINT32)MACRxBuf[RxCtrl.RecvIndex];          /* 接收缓冲区的基地址 */
    tmp = mac_cotrl.rxdes_top;                                                  /* 指向第一个描述符 */
    tmp->rxdes0 |= RXDES0_RXDMA_OWN;                                            /* 置MAC拥有本描述符使用权限 */
    for(i = 0; i < RX_QUEUE_NUM; i++){                                          /* 将所有描述符用链表连接 */
        tmp->rxdes1 |= RXDES1_RXBUF_SIZE( RX_BUF_SIZE );                        /* 接收缓冲区大小，单位为字节 */
        tmp->rxdes3 = (UINT32)(tmp + 1);                                        /* rxdes3保存下一个描述符地址 */
        tmp = (mac_rxdes *)tmp->rxdes3;                                         /* 指向下一个描述符地址 */ 
    }
    mac_cotrl.rxdes_end->rxdes3 = (UINT32)mac_cotrl.rxdes_top;                  /* rxdes3保存下一个描述符地址（首个描述符）*/
}

/*******************************************************************************
* Function Name  : CH563MAC_Open
* Description    : CH563MAC打开函数，本函数使用默认配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563MAC_Open(void)
{

    UINT32 i;

    i = R32_ETHE_ISR;                                                           /* 清除中断 */
    R32_ETHE_MACCR = RB_SW_RST;                                                 /* 复位MAC */ 
    MAC_InitRXDes( );                                                           /* 初始化接收描述符 */
    MAC_InitTXDes( );                                                           /* 初始化发送描述符 */

#ifdef  TX_QUEUE_NUM
    R32_ETHE_TXR_BADR = (UINT32)mac_cotrl.txdes_top;                            /* 将发送描述符地址写入R32_ETHE_TXR_BADR */
#else
    R32_ETHE_TXR_BADR = (UINT32)mac_cotrl.txdes_point;                          /* 将发送描述符地址写入R32_ETHE_TXR_BADR */
#endif

    R32_ETHE_RXR_BADR = (UINT32)mac_cotrl.rxdes_top;                            /* 将接收描述符地址写入R32_ETHE_RXR_BADR */
    R32_ETHE_IMR = RB_RPKT_FINISH_EN | RB_NORXBUF_EN | RB_XPKT_OK_EN |          /* 设置MAC中断使能寄存器 */
                   RB_XPKT_LOST_EN | RB_RPKT_LOST_EN | RB_PHYSTS_CHG_EN;
    i = (UINT32)(CH563MACAddr[2]<<24) + (UINT32)(CH563MACAddr[3]<<16) + (UINT32)(CH563MACAddr[4]<<8) + CH563MACAddr[5]; 
    R32_ETHE_MAC_LADR = i;                                                      /* 写MAC地址寄存器的低4节*/
    i = (UINT32)(CH563MACAddr[0]<<8) + CH563MACAddr[1]; 
    R32_ETHE_MAC_MADR = i & 0xffff;                                             /* 写MAC地址寄存器的高2节*/

    R32_ETHE_ITC   = RB_TXINT_THR(1);                                           /* 设置中断定时器 */
     R32_ETHE_APTC  = RB_RXINT_CNT(1);                                          /* 设置查询寄存器 */
    R32_ETHE_DBLAC = RB_RXFIFO_LTHR(2) | RB_RXFIFO_HTHR(6) | RB_RX_THR_EN |     /* 设置DMA突发长度 */
                     RB_INCR_SEL(2);
#if 1
    R32_ETHE_MACCR |= ( RB_RCV_ALL | RB_TDMA_EN | RB_RDMA_EN | RB_CRC_APD | 
                      RB_XMT_EN | RB_RCV_EN );                                  /* 设置MACCR寄存器 */
#else
    R32_ETHE_MACCR |= ( RB_TDMA_EN | RB_RDMA_EN | RB_CRC_APD | 
                      RB_XMT_EN | RB_RCV_EN );                                  /* 设置MACCR寄存器 */
#endif  
}

/*******************************************************************************
* Function Name  : CH563NET_Init
* Description    : 初始化CH56以太网部分
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_Init(void)
{
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* 必须关全局中断 */

    R32_PA_DIR |= (1<<18);                                                      /*PA18为LINK和ACK灯 */
    R8_MISC_CTRL_ETH |= RB_MISC_ETH_RST | RB_MISC_ETH_LED;

    R8_SAFE_ACCESS_SIG = 0x57;                                                  /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8;                                                  /* unlock step 2 */                   
    R8_SLP_WAKE_CTRL  &= 0x7f;                                                  /* 开启电源 */
    R8_SAFE_ACCESS_SIG = 0x00;                                                  /* 写保护 */

    Delay_ms( 50 );
    R8_MISC_CTRL_ETH &= ~RB_MISC_ETH_RST;                                       /* 复位结束*/
    Delay_ms( 200 );

    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */
    R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_ETH;                                         /* 打开时钟 */
    R8_SAFE_ACCESS_SIG = 0;                                                     /* 写保护 */
}

/*******************************************************************************
* Function Name  : NET_INIT
* Description    : 以太网初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NET_INIT( void )
{
    CH563MAC_ParamInit( );                                                      /* 变量初始化 */
    CH563NET_Init( );                                                           /* 开启以太网部分电源，时钟，elink灯 */
    CH563MAC_Open( );                                                           /* 初始化MAC */
    CH563PHY_Process( );                                                        /* 初始化PHY */
}

/*******************************************************************************
* Function Name  : MAC_RxSuccessHandle
* Description    : 以太网接收成功回调函数，主要是处理缓冲区标识，查询到当前缓冲区
                   没有空闲，则停止DMA接收直到缓冲区可用才开启。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_RxSuccessHandle(void)
{
    RxCtrl.RxBufStau[RxCtrl.RecvIndex] = BUF_IP_LOCK;                           /* 将此缓冲区锁定，权限交给上层 */
    RxCtrl.RxBufAddr[RxCtrl.RecvIndex]=((UINT32)MACRxBuf[RxCtrl.RecvIndex])+2;  /* 地址向后偏移2字节，每个报文前2个字节无效 */
    RxCtrl.RecvIndex++;                                                         /* MAC接收索引指向下一个接收缓冲区 */
    if(RxCtrl.RecvIndex >= RX_QUEUE_NUM){                                       /* 检查是否到尾部 */
        RxCtrl.RecvIndex = 0;
    }
    /* 处理接收缓冲区溢出的方法是丢失最早的数据包 */
    if(RxCtrl.RxBufStau[RxCtrl.RecvIndex] == BUF_IDLE){                         /* 如果下一个接收缓冲区空闲，则将接收描述符指向此缓冲区 */
        RxCtrl.RemainCout++;
        if(RxCtrl.RemainCout > RX_QUEUE_NUM){
            RxCtrl.RemainCout = RX_QUEUE_NUM;
        }
    }
    else{
    }
    RxCtrl.RxBufStau[RxCtrl.RecvIndex] = BUF_MAC_LOCK;                          /* 此缓冲区由MAC使用，MAC进行锁定 */
    mac_cotrl.rxdes_cur = (mac_rxdes *)mac_cotrl.rxdes_cur->rxdes3;             /* 将当前描述符指向下一个 */
    /* 初始化下一个描述符 */
    mac_cotrl.rxdes_cur->rxdes2 = (UINT32)MACRxBuf[RxCtrl.RecvIndex];    
    mac_cotrl.rxdes_cur->rxdes0 = RXDES0_RXDMA_OWN;
    RxCtrl.RxDMAEnable = TRUE;
}

/*******************************************************************************
* Function Name  : MAC_QueryInput
* Description    : 查询是否有可读数据包
* Input          : None
* Output         : None
* Return         : 返回剩余数据包个数，0表示没有数据
*******************************************************************************/
UINT8 MAC_QueryInput(void)
{
    return RxCtrl.RemainCout;                                                   /* 返回剩余包数据数目 */
}

/*******************************************************************************
* Function Name  : MAC_Xmit
* Description    : MAC发送函数，只能发送一包数据
* Input          : tx_addr  发送缓冲区地址
*                ：size     发送数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_Xmit( UINT32 size)
{
    TxCtrl.TxBufStat[TxCtrl.SendIndex] = BUF_MAC_LOCK;                         /* 修改发送信息列表 */
    if( mac_cotrl.txdes_cur->txdes0 & TXDES0_TXDMA_OWN ){
//        R32_ETHE_MACCR &= (~RB_TDMA_EN);
//        R32_ETHE_MACCR |= RB_TDMA_EN; 
    }
    mac_cotrl.txdes_cur->txdes1 &= TXDES1_EDOTR;
    mac_cotrl.txdes_cur->txdes1 |= ( TXDES1_TXBUF_SIZE(size) | TXDES1_LTS | 
                                   TXDES1_FTS | TXDES1_TXIC );
    mac_cotrl.txdes_cur->txdes0 = TXDES0_TXDMA_OWN;
    SendSuc = 1;
    TxCtrl.SendIndex++;
    if(TxCtrl.SendIndex >= TX_QUEUE_NUM){
        TxCtrl.SendIndex = 0;
    }
    if( (R32_ETHE_APTC & RB_TXINT_CNT(0xf)) == 0 ){
        R32_ETHE_TXPD = 0xffffffff;                                             /* 写任意数操作来触发MAC读发送描述符 */
    }
}

/*******************************************************************************
* Function Name  : MAC_SendOkHanld
* Description    : 由CH563MAC_Isr调用，CH563MAC发送完成中断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_SendOkHanld(void)
{                                                                                    
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* 关闭IRQ全局中断 */
    mac_cotrl.txdes_cur = (mac_txdes *)mac_cotrl.txdes_cur->txdes3;             /* 将发送描述符指向下一个 */
    mac_cotrl.txdes_cur->txdes2 |= (UINT32)MACTxBuf[TxCtrl.SendIndex];          /* 发送缓冲区的基地址 */
    MACSendbuf = (UINT8 *)&MACTxBuf[TxCtrl.SendIndex][14];
    SendSuc = 0;                                                                /* 发送成功标志清0 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启IRQ全局中断 */
}

/*******************************************************************************
* Function Name  : PHY_GetLinkStatus
* Description    : 获取PHY的连接状态，在PHY改变中断中调用，获取当前状态
* Input          : None
* Output         : None
* Return         : 返回连接状态
*******************************************************************************/
UINT8 PHY_GetLinkStatus(void)
{
    UINT8  status;
    UINT16 value;

    value = PHY_ReadData(FTPHY_REG_STATUS);                                     /* 读取状态寄存器 */
    if( (value & RB_LINK_STATUS) == 0 ){                                        /* 如果为断开状态 */
        status =  PHY_DISCONN;  
    }
    else{
        value = PHY_ReadData(FTPHY_REG_CONTROL);                                /* 读取PHY控制寄存器 */
        if((value & RB_DUPLEX_MODE) ){                                          /* 如果为全双工模式 */
            R32_ETHE_MACCR |= RB_FULLDUP;
            if((value & RB_SPEED_SEL) > 0){                                     /* 获取速度 */
                R32_ETHE_MACCR |= RB_SPEED_100;
                status = PHY_100M_FLL;                                          /* 全双工，100M */
            }
            else{ 
                R32_ETHE_MACCR &= ~RB_SPEED_100;
                status = PHY_10M_FLL;                                           /* 全双工，10M */
            }
        }
        else{                                                                   /* 如果为半双工模式 */
            R32_ETHE_MACCR &= ~(RB_FULLDUP);                                    /* 获取速度 */
            if((value & RB_SPEED_SEL) > 0){                                    
                R32_ETHE_MACCR |= RB_SPEED_100;
                status = PHY_100M_HALF;                                         /* 半双工，100M */
            }
            else{ 
                R32_ETHE_MACCR &= ~(RB_SPEED_100);
                status = PHY_10M_HALF;                                          /* 半双工，10M */
            }
        }
    }                   
    return status;
}

/*******************************************************************************
* Function Name  : MAC_RecvFinishHanld
* Description    : 由CH563MAC_Isr调用，接收完毕中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_RecvFinishHanld(void)
{

    if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_FRS){                               /* 是否是否为第一个描述符 */
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RX_ODD_NB){                     /* 半字节错误 */
#if  CH563_DEBUG
            printf("ERROR *** RX_ODD_NB\n");
#endif 
            return;
        }
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_CRC_ERR){                       /* CRC校验错误 */
#if  CH563_DEBUG
            printf("ERROR *** CRC\n");
#endif
            return;
        }
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RX_ERR){                        /* 接收描述符DES0错误 */
#if  CH563_DEBUG
            printf("ERROR *** RX_ERR\n");
#endif
            return;
        }
        RxCtrl.RxBufLen[RxCtrl.RecvIndex] = mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RFL;/* 读取长度，检查包长度是否超过TX_BUF_SIZE */
        if( RxCtrl.RxBufLen[RxCtrl.RecvIndex] > TX_BUF_SIZE){
#if  CH563_DEBUG
            printf("ERROR *** len large\n");
#endif
            return;
        }
    }
    else{                                                                       /* 不是第一包数据 */
#if  CH563_DEBUG
        printf("ERROR *** not the first package\n");
#endif
        return;
    }
    while(1){
        while(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RXDMA_OWN);                  /* 等待描述符权限 */
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_LRS){
            MAC_RxSuccessHandle();                                              /* 接收成功处理函数，主要更新控制列表 */
            break;
        }
        mac_cotrl.rxdes_cur->rxdes0 = RXDES0_RXDMA_OWN;                         /* 释放DMA，用于多个描述符，否则在CH563MAC_RxSuccessHandle已经跳转 */
        mac_cotrl.rxdes_cur = (mac_rxdes *)mac_cotrl.rxdes_cur->rxdes3;         /* 将当前描述符指向下一个 */
    }
}

/*******************************************************************************
* Function Name  : MAC_Input
* Description    : 读取接收数据，读取一帧数据
* Input          : buf  缓冲区,缓冲区的大小必须不小于RX_BUF_SIZE。
*                : len  数据包有效长度
* Output         : None
* Return         : 返回剩余数据包个数，0表示没有数据
*******************************************************************************/
UINT16 MAC_Input(UINT8 *buf,UINT16 *len)
{
    UINT8  *macdata;
    UINT16 mac_len;

    mac_len = RxCtrl.RxBufLen[RxCtrl.ReadIndex];                                /* 获取当前数据包长度 */
    if( mac_len < ETHER_HEAD_LEN ){
        *len = 0;
        return FALSE;
    }
    if(  mac_len > RECE_BUF_LEN ){
        mac_len = RECE_BUF_LEN;
    }
    macdata = (UINT8 *)RxCtrl.RxBufAddr[RxCtrl.ReadIndex];                      /* 获取当前数据包指针 */
    memcpy(ETH.buffer,macdata,ETHER_HEAD_LEN);                                  /* 将以太网包头复制出来 */
    /// 此处可以对mac地址数据长度作检测 /////////
    mac_len -= ETHER_HEAD_LEN;
    memcpy(buf,&macdata[ETHER_HEAD_LEN],mac_len);                               /* 将数据包复制到用户指定的缓冲区中 */
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* 关闭中断，修改接收信息列表 */
    RxCtrl.RxBufStau[RxCtrl.ReadIndex] = BUF_IDLE;                              /* 将当前列表置为空闲状态 */
    RxCtrl.ReadIndex++;                                                         /* 将索引+1 */
    if(RxCtrl.ReadIndex >= RX_QUEUE_NUM){                                       /* 检查索引范围 */
        RxCtrl.ReadIndex = 0;
    }
    if(RxCtrl.RemainCout){                                                      /* 剩余包个数-1 */
        RxCtrl.RemainCout--; 
    }
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启中断  */
    *len = mac_len;
    return TRUE;
}

/*******************************************************************************
* Function Name  : MAC_Output
* Description    : CH563MAC 发送数据
* Input          : buf  缓冲区,缓冲区的大小必须不得大于TX_BUF_SIZE，且不得小于60
*                : len  数据包有效长度
*                ：code mac层协议类型
* Output         : None
* Return         : 返回发送状态
*******************************************************************************/
UINT8 MAC_Output(UINT8 *buf,UINT16 *len,UINT16 code)
{
    UINT8   timecount = 0;
    UINT16  mac_len;
    
    if(buf) MACSendbuf = buf;
    mac_len = *len;
    if( mac_len > MAC_MAX_LEN ){                                                /* 检查长度，长度大于TX_BUF_SIZE，可能会导致系统错误 */
        mac_len = MAC_MAX_LEN; 
        *len = MAC_MAX_LEN;
    }
    memcpy( ETH.Head.SourMAC,CH563MACAddr,MACADDR_LEN );                        /* 填写本机mac地址 */
    ETH.Head.PROTOCOL = code;                                                   /* 填写协议类型 */
    memcpy( MACTxBuf[TxCtrl.SendIndex],ETH.buffer,ETHER_HEAD_LEN );             /* 将以太网包头复制到发送缓冲区 */
    mac_len += ETHER_HEAD_LEN; 
    if( mac_len < 60){
        memset(&MACTxBuf[mac_len],0,60 - mac_len);                              /* 将不足60的字节的部分补0 */
        mac_len = 60;                                                           /* 最小包为60字节 */
    }
    MAC_Xmit( mac_len);                                                         /* 为了节省RAM，可以直接用buf将数据发送，节省去复制时间，提高效率*/
    while(SendSuc){                                                             /* 发送完毕后，会产生发送成功中断CH563MAC_INT_XPKT_OK */
       timecount++;
       Delay_us(10);
       if((timecount > 200))return FALSE;                                       /* 最大超时时间约为2MS，超时返回FALSE(0) */
    }
    return TRUE;                                                                /* 成功返回TRUE(1) */
}

/*******************************************************************************
* Function Name  : GLOB_nTohs
* Description    : 16位大小端切换
* Input          : in 
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 GLOB_nTohs( UINT16 in ) 
{
    UINT16 out;
    out = (in<<8) | (in>>8);
    return out;
}

/*******************************************************************************
* Function Name  : MAC_Isr
* Description    : MAC中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_Isr(void)
{
    UINT32  int_status;
     
    int_status = R32_ETHE_ISR;
    if(int_status & RB_RPKT_FINISH) {                                           /* 接收完成中断 */  
        MAC_RecvFinishHanld( );                                     
    }
    if(int_status & RB_XPKT_FINISH){                                            /* 发送完成(TXDMA 将数据写入FIFO，不表示发送成功)*/

    }
    if(int_status & RB_XPKT_OK){                                                /* 数据发送成功 */
        MAC_SendOkHanld( );
    }      
    if(int_status & RB_PHYSTS_CHG){                                             /* PHY改变中断 */
        PHYStatus = PHY_GetLinkStatus( );                                       /* 获取PHY状态 */
#if  CH563_DEBUG
        printf("PHY change,status=%x\r\n",(UINT16)PHYStatus);
#endif
    }                                                                 
}

/*******************************************************************************
* Function Name  : ARP_Output
* Description    : 发送arp请求
* Input          : DestIP，发送的目的ip地址
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 ARP_Output( UINT8 *desip,UINT8 code )
{
    UINT16 arp_len;    

    arp_len = ARP_PACKAGE_LEN;                                               
    ARP.Package.HRDTYPE = GLOB_nTohs( ARP_HARDWARE_TYPE_ETHER );
    ARP.Package.PROTYPE = GLOB_nTohs( ARP_PROTOCOL_TYPE_IP );    
    ARP.Package.MACLEN  = MACADDR_LEN;    
    ARP.Package.IPLEN   = IPADDR_LEN;    
    ARP.Package.OPCODE  = GLOB_nTohs( code );                                   /* 数据包类型，（1 -arp请求，2-arp应答）*/
    memcpy(ARP.Package.TARGIP,desip,IPADDR_LEN);                                /* arp请求的目的ip地址 */
    memset(ARP.Package.TARGMAC,0,MACADDR_LEN);                                  /* arp请求的目的mac地址，为全0 */
    memcpy(ARP.Package.SENDIP,CH563IPAddr,IPADDR_LEN);                          /* 本机ip地址 */
    memcpy(ARP.Package.SENDMAC,CH563MACAddr,MACADDR_LEN);                       /* 本机mac地址 */
    memcpy( MACSendbuf,ARP.buffer,arp_len );                                    /* 将arp数据包复制到发送缓冲区 */
    if( code == ARP_CODE_REPLY ){
        memcpy(ETH.Head.DestMAC,ETH.Head.SourMAC,MACADDR_LEN);                  /* 填写目的mac地址 */
    }
    else memset(ETH.Head.DestMAC,0xff,MACADDR_LEN);                             /* 填写目的mac地址 */
    return ( MAC_Output( MACSendbuf,&arp_len,GLOB_nTohs( ETHER_TYPE_ARP ) ));
}

/*******************************************************************************
* Function Name  : ARP_Input
* Description    : 收到arp包
* Input          : DestIP，发送的目的ip地址
* Output         : None
* Return         : None
*******************************************************************************/
void ARP_Input( UINT8 *arpdata,UINT16 len )
{
    
    if( len < ARP_PACKAGE_LEN ) return;
    memcpy(ARP.buffer,arpdata,ARP_PACKAGE_LEN);
    if(    ARP.Package.HRDTYPE == GLOB_nTohs( ARP_HARDWARE_TYPE_ETHER ) &&
        ARP.Package.PROTYPE == GLOB_nTohs( ARP_PROTOCOL_TYPE_IP ) &&
        ARP.Package.MACLEN  == MACADDR_LEN &&    
        ARP.Package.IPLEN   == IPADDR_LEN ){
        if( ARP.Package.OPCODE  == GLOB_nTohs( ARP_CODE_QUEST )){
            if(  memcmp(ARP.Package.TARGIP,CH563IPAddr,IPADDR_LEN) == 0 ){
                ARP_Output( ARP.Package.TARGIP,ARP_CODE_REPLY );
            }
        }
        else{
            return ;
        }
    }    
}

/*********************************** endfile **********************************/
