/********************************** (C) COPYRIGHT *******************************
* File Name          : NETSFR.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/08/12
* Description        : Description: CH563MAC Non-OS firmware header file 
*******************************************************************************/



/******************************************************************************/
#ifndef __NETSFR_H__
#define __NETSFR_H__                            


#include "CH563BAS.H"

#define CH563MAC_BASE_ADDR 0x00406000

/******************************************************************************/
/* 
 * CH563MAC register offset definition 
 */								  
#define R32_ETHE_ISR             (*((PUINT32)(0x00 + CH563MAC_BASE_ADDR)))
#define 	  RB_RPKT_FINISH              (1 << 0)
#define 	  RB_NORXBUF                  (1 << 1)
#define 	  RB_XPKT_FINISH              (1 << 2)
#define 	  RB_NOTXBUF                  (1 << 3)
#define 	  RB_XPKT_OK                  (1 << 4)
#define 	  RB_XPKT_LOST                (1 << 5)
#define 	  RB_RPKT_SAV                 (1 << 6)
#define 	  RB_RPKT_LOST                (1 << 7)
#define 	  RB_PHYSTS_CHG               (1 << 9)
#define R32_ETHE_IMR             (*((PUINT32)(0x04 + CH563MAC_BASE_ADDR)))
#define       RB_RPKT_FINISH_EN         (1 << 0)
#define       RB_NORXBUF_EN             (1 << 1)
#define       RB_XPKT_FINISH_EN         (1 << 2)
#define       RB_NOTXBUF_EN             (1 << 3)
#define       RB_XPKT_OK_EN             (1 << 4)
#define       RB_XPKT_LOST_EN           (1 << 5)
#define       RB_ISR_RPKT_SAV_EN        (1 << 6)
#define       RB_RPKT_LOST_EN           (1 << 7)
#define       RB_PHYSTS_CHG_EN          (1 << 9)
#define R32_ETHE_MAC_MADR        (*((PUINT32)(0x08 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_MAC_LADR        (*((PUINT32)(0x0C + CH563MAC_BASE_ADDR)))
#define R32_ETHE_MAHT0           (*((PUINT32)(0x10 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_MAHT1           (*((PUINT32)(0x14 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_TXPD            (*((PUINT32)(0x18 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_RXPD            (*((PUINT32)(0x1C + CH563MAC_BASE_ADDR)))
#define R32_ETHE_TXR_BADR        (*((PUINT32)(0x20 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_RXR_BADR        (*((PUINT32)(0x24 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_ITC             (*((PUINT32)(0x28 + CH563MAC_BASE_ADDR)))
#define      RB_RXINT_CNT(x)            (((x) & 0xf) << 0)
#define      RB_RXINT_THR(x)            (((x) & 0x7) << 4)
#define      RB_RXINT_TIME_SEL          ( 1<<4 )
#define      RB_TXINT_CNT(x)            (((x) & 0xf) << 8)
#define      RB_TXINT_THR(x)            (((x) & 0x7) << 12)
#define      RB_TXINT_TIME_SEL          ( 1<<15 )
#define R32_ETHE_APTC            (*((PUINT32)(0x2C + CH563MAC_BASE_ADDR)))
#define      RB_RXINT_CNT(x)            (((x) & 0xf) << 0)
#define      RB_RXPOLL_TIME_SEL         ( 1<<4 )
#define      RB_TXPOLL_CNT(x)           (((x) & 0xf) << 8)
#define      RB_TXPOLL_TIME_SEL         ( 1<<12 )
#define R32_ETHE_DBLAC           (*((PUINT32)(0x30 + CH563MAC_BASE_ADDR)))
#define      RB_RXFIFO_LTHR(x)          ( ((x) & 0x7)<<3 )
#define      RB_RXFIFO_HTHR(x)          ( ((x) & 0x7)<<6 )
#define      RB_RX_THR_EN               ( 1<<9 )
#define      RB_INCR_SEL(x)             ( ((x) & 0x3)<<14 )
#define R32_REG_FPGA_VER         (*((PUINT32)(0x34 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_MACCR           (*((PUINT32)(0x88 + CH563MAC_BASE_ADDR)))
#define      RB_TDMA_EN                 (1 << 0)
#define      RB_RDMA_EN                 (1 << 1)
#define      RB_SW_RST                  (1 << 2)
#define      RB_LOOP_EN                 (1 << 3)
#define      RB_CRC_DIS                 (1 << 4)
#define      RB_XMT_EN                  (1 << 5)
#define      RB_ENRX_IN_HALFTX          (1 << 6)
#define      RB_RCV_EN                  (1 << 8)
#define      RB_HT_MULTI_EN             (1 << 9)
#define      RB_RX_RUNT                 (1 << 10)
#define      RB_RX_FTL                  (1 << 11)
#define      RB_RCV_ALL                 (1 << 12)
#define      RB_CRC_APD                 (1 << 14)
#define      RB_FULLDUP                 (1 << 15)
#define      RB_RX_MULTIPKT             (1 << 16)
#define      RB_RX_BROADPKT             (1 << 17)
#define      RB_SPEED_100               (1 << 18)
#define R32_ETHE_MACSR           (*((PUINT32)(0x8C + CH563MAC_BASE_ADDR)))
#define R32_ETHE_PHYCR           (*((PUINT32)(0x90 + CH563MAC_BASE_ADDR)))
#define      RB_MIIRDATA                0xffff			   // RO
#define      RB_PHYAD(x)                (((x) & 0x1f) << 16)
#define      RB_REGAD(x)                (((x) & 0x1f) << 21)
#define      RB_MIIRD                   (1 << 26)
#define      RB_MIIWD                   (1 << 27)
#define R32_ETHE_PHYWDATA        (*((PUINT32)(0x94 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_FCR             (*((PUINT32)(0x98 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_BPR             (*((PUINT32)(0x9C + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WOLCR           (*((PUINT32)(0xA0 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WOLSR           (*((PUINT32)(0xA4 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WFCRC           (*((PUINT32)(0xA8 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WFBM1           (*((PUINT32)(0xB0 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WFBM2           (*((PUINT32)(0xB4 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WFBM3           (*((PUINT32)(0xB8 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_WFBM4           (*((PUINT32)(0xBC + CH563MAC_BASE_ADDR)))
#define R32_REG_TS               (*((PUINT32)(0xC4 + CH563MAC_BASE_ADDR)))
#define R32_ETHE_DMAFIFOS        (*((PUINT32)(0xC8 + CH563MAC_BASE_ADDR)))

/*
 * 接收描述符
 */
#define RXDES0_RFL             0x7ff								    /* 接收的数据帧(长度) */
#define RXDES0_MULTICAST                (1 << 16)						    /* 接收的数据包(多播包) */
#define RXDES0_BROADCAST                (1 << 17)								/* 接收的数据包(广播包) */
#define RXDES0_RX_ERR                   (1 << 18)								/* 接收的数据包(错误) */
#define RXDES0_CRC_ERR                  (1 << 19)								/* CRC校验(错误) */
#define RXDES0_FTL                      (1 << 20)								/* 接收的数据包(长度大于1518字节) */
#define RXDES0_RUNT                     (1 << 21)								/* 接收的数据包(长度小于64字节) */
#define RXDES0_RX_ODD_NB                (1 << 22)								/* 接收的数据包(包含半字节) */
#define RXDES0_LRS                      (1 << 28)								/* 接收的数据包(最后一个数据段的描述符) */
#define RXDES0_FRS                      (1 << 29)								/* 接收的数据包(第一个数据段的描述符) */
#define RXDES0_RXDMA_OWN                (0x80000000)					    /* RXDMA使用权限位 */
#define RXDES1_RXBUF_SIZE(x)            ((x) & 0x7ff)
#define RXDES1_EDORR                    (0x80000000)//(1 << 31)

/*
 * 发送描述符
 */
#define TXDES0_TXPKT_LATECOL            (1 << 0)                 /* 该位置1表示最后一次冲突导致发送终止，仅当FTS = 1时，该位有效 */
#define TXDES0_TXPKT_EXSCOL             (1 << 1)									/* 产生的帧冲突 */
#define TXDES0_TXDMA_OWN                (0x80000000)							/* 描述符使用权限 */
#define TXDES1_TXBUF_SIZE(x)            ((x) & 0x7ff)						/* 发送缓冲区大小 */
#define TXDES1_LTS                      (1 << 27)								/* 置1表示当前描述符是发送包的最后一个数据段的描述符 */
#define TXDES1_FTS                      (1 << 28)								/* 置1表示当前描述符是发送包的第一个数据段的描述符 */
#define TXDES1_TX2FIC                   (1 << 29)								/* 数据写成功中断使能位 */
#define TXDES1_TXIC                     (1 << 30)								/* 帧传输是否完成标志位 */
#define TXDES1_EDOTR                    (0x80000000)						  /* 发送描述符是否为描述符队列的最后一个描述符  */

/* 
 * PHY基本模式控制寄存器(R16_PHY_BMCR) 
 */
#define RB_COLLISION_TEST                     (1<<7)
#define RB_DUPLEX_MODE                        (1<<8)
#define RB_RES_AUTO_NEG           			  (1<<9)
#define RB_ISOLATE           			      (1<<10)
#define RB_POWER_DOWN           			  (1<<11)
#define RB_AUTO_NEG            			      (1<<12)
#define RB_SPEED_SEL           				  (1<<13)
#define RB_LOOK_BACK           		 		  (1<<14)
#define RB_RESET   							  (1<<15)
          
/* 
 * PHY基本模式状态寄存器(R16_PHY_BMSR) 
 */
#define RB_EXTEN_CAP						  (1<<0)		   
#define RB_JAB_DET						      (1<<1)		   
#define RB_LINK_STATUS      				  (1<<2)
#define RB_AUTO_NEG_ABILITY					  (1<<3)		   
#define RB_REMOTE_FAULT			              (1<<4)		   
#define RB_AUTO_NEG_CMP    					  (1<<5)
#define RB_MF_PRE_SUP                         (1<<6)		   
#define RB_10BASE_T_HALF_DUPLX				  (1<<11)	
#define RB_10BASE_T_FULL_DUPLX 				  (1<<12)	
#define RB_100BASE_T_HALF_DUPLX				  (1<<13)	
#define RB_100BASE_TX_FULL_DUPLX			  (1<<14)	
#define RB_100BASE_T4	                      (1<<15)	

#endif
