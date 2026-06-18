/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbss_host.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/23
* Description        : This file contains the headers of the ch32h417_usbss handlers.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32H417_USBSS_H
#define __CH32H417_USBSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"

/******************************************************************************/
/*                                 USB 3.0                                    */
/******************************************************************************/

typedef enum {  Power_off = 0,
                Disconnected =1,
                DISABLED = 2,
                Training = 3,
                Enabled = 4,
                Resetting = 5,
                Error = 6,
                Loopback = 7,
                Compliance = 8
} PortState;

typedef struct  __attribute__((packed))USBSS_Port_Info {
    uint16_t port_change;
    uint16_t port_status;
    uint8_t set_u3_exit;
    uint8_t set_u3_enter;
    uint8_t set_u2_exit;
    uint8_t set_u2_enter;
    uint8_t set_u1_exit;
    uint8_t set_u1_enter;
    uint8_t set_warm_reset;
    uint8_t set_hot_reset;
    uint8_t port_rmt_wkup;
    uint8_t lp_mode;
    uint8_t DSPORT;
} USBSS_Port_Info;

// UEP_CR
#define EP_HALTED       (1<<7)
#define EP_CLR          (1<<6)
#define CHAIN_CLR       (1<<5)
#define ERDY_NUMP_MASK  (0x1f)
// UEP_ST
#define RES_ACK     (1<<4)


//#define USBSSH0_BASE      ((uint32_t)0x40038000)
#define USBSSH1_BASE      ((uint32_t)0x40038200)
#define USBSSH2_BASE      ((uint32_t)0x40038400)
#define USBSSH3_BASE      ((uint32_t)0x40038600)
#define USBSSH4_BASE      ((uint32_t)0x40038800)

#define USBSS_PHY_CFG_CR              (*((__IO uint32_t *)0x400341f8))          //这个地址才对
#define USBSS_PHY_CFG_DAT             (*((__IO uint32_t *)0x400341fc))      

#define USBSSH1_PHY_CFG_CR            (*((__IO uint32_t *)0x400383f8))
#define USBSSH1_PHY_CFG_DAT           (*((__IO uint32_t *)0x400383fc))

#define USBSSH2_PHY_CFG_CR            (*((__IO uint32_t *)0x400385f8))
#define USBSSH2_PHY_CFG_DAT           (*((__IO uint32_t *)0x400385fc))

#define USBSSH3_PHY_CFG_CR            (*((__IO uint32_t *)0x400387f8))
#define USBSSH3_PHY_CFG_DAT           (*((__IO uint32_t *)0x400387fc))

#define USBSSH4_PHY_CFG_CR            (*((__IO uint32_t *)0x400389f8))
#define USBSSH4_PHY_CFG_DAT           (*((__IO uint32_t *)0x400389fc))

//U3_ROOTx =((USBSSH_TypeDef *)(USBSS_BASE+i*0x100));

#define USBSSH1             ((USBSSH_TypeDef *) USBSSH1_BASE)
#define USBSSH2             ((USBSSH_TypeDef *) USBSSH2_BASE)
#define USBSSH3             ((USBSSH_TypeDef *) USBSSH3_BASE)
#define USBSSH4             ((USBSSH_TypeDef *) USBSSH4_BASE)

#define USBSS_DMA           ((uint32_t)0x20002000)

#define USBSSH1_TX_DMA      ((uint32_t)0x20002800)
#define USBSSH1_RX_DMA      ((uint32_t)0x20003000)
#define USBSSH2_TX_DMA      ((uint32_t)0x20003800)
#define USBSSH2_RX_DMA      ((uint32_t)0x20004000)
#define USBSSH3_TX_DMA      ((uint32_t)0x20004800)
#define USBSSH3_RX_DMA      ((uint32_t)0x20005000)
#define USBSSH4_TX_DMA      ((uint32_t)0x20005800)
#define USBSSH4_RX_DMA      ((uint32_t)0x20006000)

void USBSS_Endp_Init(void);
void USBSS_Endp_Deinit(void);
void USBSS_Endp_Disable(void);
void USBSSH_Init(void);
void USBSS_LINK_Handle( USBSSH_TypeDef *USBSSHx,uint8_t port_num );
//void usbssh_lmp_init(USBSSH_TypeDef *USBSSHx);
void send_setup_packet( USBSSH_TypeDef *USBSSHx );
void send_status_packet(USBSSH_TypeDef *USBSSHx);
void get_data( USBSSH_TypeDef *USBSSHx, uint32_t seq_num, uint8_t packet_num, uint32_t endp_num );
void send_data( USBSSH_TypeDef *USBSSHx, uint32_t seq_num, uint8_t packet_num, uint32_t endp_num, uint32_t tx_len );
void send_data_iso( USBSSH_TypeDef *USBSSHx, uint32_t seq_num, uint8_t packet_num, uint32_t endp_num, uint32_t tx_len );
void get_data_iso( USBSSH_TypeDef *USBSSHx, uint32_t seq_num, uint8_t packet_num, uint32_t endp_num );

void get_data_t( USBSSH_TypeDef *USBSSHx, uint32_t seq_num, uint8_t packet_num, uint32_t endp_num );

//void send_erdy( uint8_t endp, uint8_t nump, uint8_t dir );
void set_device_address( uint32_t address,USBSSH_TypeDef *USBSSx );
void switch_pwr_mode( USBSSH_TypeDef *USBSSHx,uint8_t pwr_mode );
void host_enum( uint32_t dev_addr, USBSSH_TypeDef *USBSSHx , uint32_t TX_DMAaddr);

void enter_loopback (void);
//void SetPortPwr( uint8_t port_num );

uint32_t USBSS_PHY_Cfg( uint8_t port_num, uint8_t addr, uint16_t data );
uint32_t USBSS_ReadPHYData(uint8_t port_num,uint8_t addr);
void USBSS_CFG_MOD(void);
void USBSS_PLL_Init( FunctionalState sta );


// LINK_AUTO MOD
#define U0_AUTO         (1<<0)
#define U1_AUTO         (1<<1)
#define U2_AUTO         (1<<2)
#define U3_AUTO         (1<<3)
#define DISABLED_AUTO   (1<<4)
#define RXDET_AUTO      (1<<5)
#define INACTIVE_AUTO   (1<<6)
//#define POLLING_AUTO    (1<<7)
#define TX_EQ_AUTO      (1<<8)
#define POLLING_AUTO    (1<<10)
#define U1_TOUT_AUTO    (1<<14)
#define U2_TOUT_AUTO    (1<<15)

/*---------------------------------------------*/
/* LINK_CFG */
#define U3_LINK_RESET               0x80000000
#define LINK_FORCE_RXTERM           0x00800000
#define LINK_FORCE_POLLING          0x00400000
#define LINK_TOUT_MODE              0x00200000
#define LINK_U2_ALLOW               0x00020000
#define LINK_U1_ALLOW               0x00010000
#define LINK_LTSSM_MODE             0x00008000
#define LINK_LOOPBACK_ACT           0x00004000
#define LINK_LOOPBACK_EN            0x00002000
#define LINK_U2_RXDET               0x00001000
#define LINK_CP78_SEL_MASK          0x00000C00
 #define LINK_CP78_SEL_190BITS      0x00000000
 #define LINK_CP78_SEL_120BITS      0x00000400
 #define LINK_CP78_SEL_50BITS       0x00000800
 #define LINK_CP78_SEL_250BITS      0x00000C00
#define LINK_TX_DEEMPH_MASK         0x00000300
 #define LINK_TX_DEEMPH_6DB         0x00000000
 #define LINK_TX_DEEMPH_3_5DB       0x00000100
#define LINK_TX_SWING               0x00000080
#define LINK_RX_EQ_EN               0x00000040
#define LINK_LFPS_RX_PD             0x00000020
#define LINK_COMPLIANCE_EN          0x00000010
#define LINK_PHY_RESET              0x00000008
#define LINK_SS_PLR_SWAP            0x00000004
#define LINK_RX_TERM_EN             0x00000002
#define LINK_DOWN_MODE              0x00000001

/* LINK_CTRL */
#define LINK_RX_TS_CFG_MASK         0xFF000000
#define LINK_TX_TS_CFG_MASK         0x00FF0000
 #define LINK_HOT_RESET             0x00010000
#define LINK_TX_LGO_U3              0x00008000
#define LINK_TX_LGO_U2              0x00004000
#define LINK_TX_LGO_U1              0x00002000
#define LINK_POLLING_EN             0x00001000
#define LINK_REG_ROUT_EN            0x00000800
#define LINK_LUP_LDN_EN             0x00000400
#define LINK_TX_UX_EXIT             0x00000200
#define LINK_TX_WARM_RESET          0x00000100
#define LINK_GO_RX_DET              0x00000080
#define LINK_GO_RECOVERY            0x00000040
#define LINK_GO_INACTIVE            0x00000020
#define LINK_GO_DISABLED            0x00000010
#define LINK_PD_MODE_MASK           0x00000003

/* LINK_INT_CTRL */
#define LINK_IE_STATE_CHG           0x80000000
#define LINK_IE_U1_TOUT             0x40000000
#define LINK_IE_U2_TOUT             0x20000000
#define LINK_IE_UX_FAIL             0x10000000
#define LINK_IE_TX_WARMRST          0x08000000
#define LINK_IE_UX_EXIT_FAIL        0x04000000
#define LINK_IE_RX_LMP_TOUT         0x00800000
#define LINK_IE_TX_LMP              0x00400000
#define LINK_IE_RX_LMP              0x00200000
#define LINK_IE_RX_DET              0x00100000
#define LINK_IE_LOOPBACK            0x00080000
#define LINK_IE_COMPLIANCE          0x00040000
#define LINK_IE_HPBUF_FULL          0x00020000
#define LINK_IE_HPBUF_EMPTY         0x00010000
#define LINK_IE_HOT_RST             0x00008000
#define LINK_IE_WAKEUP              0x00004000
#define LINK_IE_WARM_RST            0x00002000
#define LINK_IE_UX_EXIT             0x00001000
#define LINK_IE_TXEQ                0x00000800
#define LINK_IE_TERM_PRES           0x00000400
#define LINK_IE_UX_REJ              0x00000200
#define LINK_IE_U3_WK_TOUT          0x00000100
#define LINK_IE_GO_U0               0x00000080
#define LINK_IE_GO_U1               0x00000040
#define LINK_IE_GO_U2               0x00000020
#define LINK_IE_GO_U3               0x00000010
#define LINK_IE_DISABLE             0x00000008
#define LINK_IE_INACTIVE            0x00000004
#define LINK_IE_RECOVERY            0x00000002
#define LINK_IE_READY               0x00000001

/* LINK_INT_FLAG */
#define LINK_IF_STATE_CHG           0x80000000
#define LINK_IF_U1_TOUT             0x40000000
#define LINK_IF_U2_TOUT             0x20000000
#define LINK_IF_UX_FAIL             0x10000000
#define LINK_IF_TX_WARMRST          0x08000000
#define LINK_IF_UX_EXIT_FAIL        0x04000000
#define LINK_IF_RX_LMP_TOUT         0x00800000
#define LINK_IF_TX_LMP              0x00400000
#define LINK_IF_RX_LMP              0x00200000
#define LINK_IF_RX_DET              0x00100000
#define LINK_IF_LOOPBACK            0x00080000
#define LINK_IF_COMPLIANCE          0x00040000
#define LINK_IF_HPBUF_FULL          0x00020000
#define LINK_IF_HPBUF_EMPTY         0x00010000
#define LINK_IF_HOT_RST             0x00008000
#define LINK_IF_WAKEUP              0x00004000
#define LINK_IF_WARM_RST            0x00002000
#define LINK_IF_UX_EXIT             0x00001000
#define LINK_IF_TXEQ                0x00000800
#define LINK_IF_TERM_PRES           0x00000400
#define LINK_IF_UX_REJ              0x00000200
#define LINK_IF_U3_WK_TOUT          0x00000100
#define LINK_IF_GO_U0               0x00000080
#define LINK_IF_GO_U1               0x00000040
#define LINK_IF_GO_U2               0x00000020
#define LINK_IF_GO_U3               0x00000010
#define LINK_IF_DISABLE             0x00000008
#define LINK_IF_INACTIVE            0x00000004
#define LINK_IF_RECOVERY            0x00000002
#define LINK_IF_READY               0x00000001

/* LINK_STATUS */
#define LINK_HPBUF_EMPTY            0x80000000
#define LINK_HPBUF_FULL             0x40000000
#define LINK_HPBUF_IDLE             0x20000000
#define LINK_U3_SLEEP_ALLOW         0x00400000
#define LINK_U2_SLEEP_ALLOW         0x00200000
#define LINK_RXDET_SLEEP_ALLOW      0x00100000
#define LINK_WAKUP                  0x00080000
#define LINK_RX_LFPS                0x00040000
#define LINK_RX_DETECT              0x00020000
#define LINK_RX_UX_EXIT_REQ         0x00010000
#define LINK_STATE_MASK             0x00000F00
 #define LINK_STATE_U0              0x00000000
 #define LINK_STATE_U1              0x00000100
 #define LINK_STATE_U2              0x00000200
 #define LINK_STATE_U3              0x00000300
 #define LINK_STATE_DISABLE         0x00000400
 #define LINK_STATE_RXDET           0x00000500
 #define LINK_STATE_INACTIVE        0x00000600
 #define LINK_STATE_POLLING         0x00000700
 #define LINK_STATE_RECOVERY        0x00000800
 #define LINK_STATE_HOTRST          0x00000900
 #define LINK_STATE_COMPLIANCE      0x00000A00
 #define LINK_STATE_LOOPBACK        0x00000B00
#define LINK_TXEQ                   0x00000040
#define LINK_PD_MODE_ST_MASK        0x00000030
 #define LINK_PD_MODE_ST_P0         0x00000000
 #define LINK_PD_MODE_ST_P1         0x00000010
 #define LINK_PD_MODE_ST_P2         0x00000020
 #define LINK_PD_MODE_ST_P3         0x00000030
#define LINK_READY                  0x00000008
#define LINK_BUSY                   0x00000004
#define LINK_RX_WARM_RST            0x00000002
#define LINK_RX_TERM_PRES           0x00000001

/* LINK_LPM_CR */
#define LINK_LPM_TERM_PRESENT       0x00000800
#define LINK_LPM_TERM_CHG           0x00000400
#define LINK_LPM_EN                 0x00000200
#define LINK_LPM_RST                0x00000100

/* LINK_PORT_CAP */
#define LINK_LMP_RX_CAP_VLD         0x80000000
#define LINK_LMP_TX_CAP_VLD         0x40000000
#define LINK_SPEED_MASK             0x3F000000
#define LINK_PORT_CAP_MASK          0x00FFFFFF

/* USB_CONTROL */
#define USBSS_DEV_ADDR_MASK         0x7F000000
#define USBSS_UIE_FIFO_RXOV         0x00800000           
#define USBSS_UIE_FIFO_TXOV         0x00400000           
#define USBSS_UIE_ITP               0x00100000       
#define USBSS_UIE_RX_PING           0x00080000         
#define USBSS_UDIE_STATUS           0x00040000
#define USBSS_UHIE_NOTIF            0x00040000  
#define USBSS_UDIE_SETUP            0x00020000       
#define USBSS_UHIE_ERDY             0x00020000     
#define USBSS_UIE_TRANSFER          0x00010000           
#define USBSS_CHAIN_CONFLICT        0x00008000               
#define USBSS_TX_ERDY_MODE          0x00004000           
#define USBSS_HP_PEND_MASK          0x00000300    
 #define USBSS_HP_PENDING           0x00000200
#define USBSS_HOST_MODE             0x00000080       
#define USBSS_ITP_EN                0x00000040       
#define USBSS_SETUP_FLOW            0x00000020           
#define USBSS_DIR_ABORT             0x00000010       
#define USBSS_DMA_MODE              0x00000008       
#define USBSS_FORCE_RST             0x00000004       
#define USBSS_USB_CLR_ALL           0x00000002           
#define USBSS_DMA_EN                0x00000001  

/* USB_STATUS */
#define USBSS_HRX_RES_MASK          0xC0000000
#define USBSS_HTX_RES_MASK          0x00C00000
#define USBSS_EP_DIR_MASK           0x00001000
#define USBSS_EP_ID_MASK            0x00000700
#define USBSS_UIF_FIFO_RXOV         0x00000080  
#define USBSS_UIF_FIFO_TXOV         0x00000040           
#define USBSS_UIF_ITP               0x00000010       
#define USBSS_UIF_RX_PING           0x00000008           
#define USBSS_UDIF_STATUS           0x00000004           
#define USBSS_UHIF_NOTIF            0x00000004           
#define USBSS_UDIF_SETUP            0x00000002           
#define USBSS_UHIF_ERDY             0x00000002       
#define USBSS_UIF_TRANSFER          0x00000001   

/* USB_ITP */
#define USBSS_ITP_INTERVAL_MASK     0x00003FFF

/* USB_ITP_ADJ */
#define USBSS_ITP_DELTA             0x001FFF00
#define USBSS_ITP_DELAYED           0x00000080
#define USBSS_ITP_ADJ_CR_MASK       0x0000007F

/* UEP_TX_EN */
#define USBSS_EP15_TX_EN            0x00008000
#define USBSS_EP14_TX_EN            0x00004000
#define USBSS_EP13_TX_EN            0x00002000
#define USBSS_EP12_TX_EN            0x00001000
#define USBSS_EP11_TX_EN            0x00000800
#define USBSS_EP10_TX_EN            0x00000400
#define USBSS_EP9_TX_EN             0x00000200
#define USBSS_EP8_TX_EN             0x00000100
#define USBSS_EP7_TX_EN             0x00000080
#define USBSS_EP6_TX_EN             0x00000040
#define USBSS_EP5_TX_EN             0x00000020
#define USBSS_EP4_TX_EN             0x00000010
#define USBSS_EP3_TX_EN             0x00000008
#define USBSS_EP2_TX_EN             0x00000004
#define USBSS_EP1_TX_EN             0x00000002
#define USBSS_UH_TX_EN              0x00000002

/* UEP_RX_EN */
#define USBSS_EP15_RX_EN            0x00008000
#define USBSS_EP14_RX_EN            0x00004000
#define USBSS_EP13_RX_EN            0x00002000
#define USBSS_EP12_RX_EN            0x00001000
#define USBSS_EP11_RX_EN            0x00000800
#define USBSS_EP10_RX_EN            0x00000400
#define USBSS_EP9_RX_EN             0x00000200
#define USBSS_EP8_RX_EN             0x00000100
#define USBSS_EP7_RX_EN             0x00000080
#define USBSS_EP6_RX_EN             0x00000040
#define USBSS_EP5_RX_EN             0x00000020
#define USBSS_EP4_RX_EN             0x00000010
#define USBSS_EP3_RX_EN             0x00000008
#define USBSS_EP2_RX_EN             0x00000004
#define USBSS_EP1_RX_EN             0x00000002
#define USBSS_UH_RX_EN              0x00000002

#if 0
/* UEP0_TX_CTRL */
#define USBSS_UIF_EP0_TX_ACT        0x80000000
#define USBSS_EP0_TX_FLOW           0x02000000
#define USBSS_EP0_TX_PP             0x01000000
#define USBSS_EP0_TX_ERDY           0x00800000
#define USBSS_EP0_TX_RES            0x00600000
 #define USBSS_EP0_TX_NRDY          0x00000000
 #define USBSS_EP0_TX_DPH           0x00200000
 #define USBSS_EP0_TX_STALL         0x00400000
#define USBSS_EP0_TX_SEQ_MASK       0x1F000000 
 #define USBSS_NUMP_1               0x01000000 
 #define USBSS_NUMP_2               0x02000000
 #define USBSS_NUMP_3               0x03000000 
 #define USBSS_NUMP_4               0x04000000 
 #define USBSS_NUMP_5               0x05000000 
 #define USBSS_NUMP_6               0x06000000 
 #define USBSS_NUMP_7               0x07000000 
 #define USBSS_NUMP_8               0x08000000 
 #define USBSS_NUMP_9               0x09000000 
 #define USBSS_NUMP_10              0x0A000000 
 #define USBSS_NUMP_11              0x0B000000
 #define USBSS_NUMP_12              0x0C000000
 #define USBSS_NUMP_13              0x0D000000
 #define USBSS_NUMP_14              0x0E000000
 #define USBSS_NUMP_15              0x0F000000
 #define USBSS_NUMP_16              0x10000000 
#define USBSS_EP0_TX_LEN_MASK       0x000007FF

/* UEP0_RX_CTRL */
#define USBSS_UIF_EP0_RX_ACT        0x80000000
#define USBSS_EP0_RX_PP             0x01000000
#define USBSS_EP0_RX_ERDY           0x00800000
#define USBSS_EP0_RX_RES            0x00600000
 #define USBSS_EP0_RX_NRDY          0x00000000
 #define USBSS_EP0_RX_ACK           0x00200000
 #define USBSS_EP0_RX_STALL         0x00400000
#define USBSS_EP0_RX_SEQ_MASK       0x001F0000
#define USBSS_EP0_RX_LEN_MASK       0x000007FF

/* R8_UEPn_TX_CFG */
#define USBSS_EP_TX_CHAIN_AUTO      0x80
#define USBSS_EP_TX_FIFO_MODE       0x40
#define USBSS_EP_TX_FIFO_CFG        0x20
#define USBSS_EP_TX_EOB_MODE        0x08
#define USBSS_EP_TX_ERDY_AUTO       0x04
#define USBSS_EP_TX_SEQ_AUTO        0x02
#define USBSS_EP_TX_ISO_MODE        0x01

/* R8_UEPn_TX_CR */
#define USBSS_EP_TX_HALT            0x80
#define USBSS_EP_TX_CLR             0x40
#define USBSS_EP_TX_CHAIN_CLR       0x20
#define USBSS_EP_TX_ERDY_NUMP_MASK  0x1F
#endif

/* R8_UEPn_TX_SEQ */
#define USBSS_EP_TX_SEQ_NUM_MASK    0x1F

/* R8_UEPn_TX_ST */
#define USBSS_EP_TX_INT_FLAG        0x80
#define USBSS_EP_TX_FC_ST           0x40
#define USBSS_EP_TX_ERDY_REQ        0x20
#define USBSS_EP_TX_CHAIN_RES       0x10
#define USBSS_EP_TX_CHAIN_EN_MASK   0x0F

/* R8_UEPn_TX_CHAIN_CR */
#define USBSS_EP_TX_CUR_USE         0xC0               
#define USBSS_EP_TX_CUR_CFG         0x30               
#define USBSS_EP_TX_FORCE_RET       0x04             
#define USBSS_EP_TX_RET_SEL         0x03               

/* R8_UEPn_TX_CHAIN_ST */
#define USBSS_EP_TX_CHAIN_EN        0x80
#define USBSS_EP_TX_CHAIN_IF        0x40
#define USBSS_EP_TX_EOB_LPF         0x20
#define USBSS_EP_TX_NUMP_EMPTY      0x08
#define USBSS_EP_TX_DPH_PP          0x04
#define USBSS_EP_TX_CHAIN_NO_MASK   0x03

/* R8_UEPn_RX_CFG */
#define USBSS_EP_RX_CHAIN_AUTO      0x80
#define USBSS_EP_RX_FIFO_MODE       0x40
#define USBSS_EP_RX_FIFO_CFG        0x20
#define USBSS_EP_RX_EOB_MODE        0x08
#define USBSS_EP_RX_ERDY_AUTO       0x04
#define USBSS_EP_RX_SEQ_AUTO        0x02
#define USBSS_EP_RX_ISO_MODE        0x01

/* R8_UEPn_RX_CR */
#define USBSS_EP_RX_HALT            0x80
#define USBSS_EP_RX_CLR             0x40
#define USBSS_EP_RX_CHAIN_CLR       0x20
#define USBSS_EP_RX_ERDY_NUMP_MASK  0x1F

/* R8_UEPn_RX_SEQ */
#define USBSS_EP_RX_SEQ_NUM_MASK    0x1F

/* R8_UEPn_RX_ST */
#define USBSS_EP_RX_INT_FLAG        0x80
#define USBSS_EP_RX_FC_ST           0x40
#define USBSS_EP_RX_ERDY_REQ        0x20
#define USBSS_EP_RX_CHAIN_RES       0x10
#define USBSS_EP_RX_CHAIN_EN_MASK   0x0F

/* R8_UEPn_RX_CHAIN_CR */
#define USBSS_EP_RX_CUR_USE         0xC0               
#define USBSS_EP_RX_CUR_CFG         0x30               
#define USBSS_EP_RX_FORCE_RET       0x04             
#define USBSS_EP_RX_RET_SEL         0x03               

/* R8_UEPn_RX_CHAIN_ST */
#define USBSS_EP_RX_CHAIN_EN        0x80
#define USBSS_EP_RX_CHAIN_IF        0x40
#define USBSS_EP_RX_LPF_FLAG        0x20
#define USBSS_EP_RX_ISO_PKT_ERR     0x10
#define USBSS_EP_RX_NUMP_EMPTY      0x08
#define USBSS_EP_RX_DPH_PP          0x04
#define USBSS_EP_RX_CHAIN_NO_MASK   0x03

/* R32_UH_TX_CTRL */
#define USBSS_UH_TX_ACT             0x80000000
#define USBSS_UH_TX_ISO             0x40000000
#define USBSS_UH_TX_SETUP           0x20000000
#define USBSS_UH_TX_STATUS          0x10000000
#define USBSS_UH_TX_LPF             0x00800000
#define USBSS_UH_TX_RES             0x00600000
 #define USBSS_UH_TX_NRDY           0x00000000
 #define USBSS_UH_TX_ACK            0x00200000
 #define USBSS_UH_TX_STALL          0x00400000
#define USBSS_UH_TX_SEQ             0x001F0000
#define USBSS_UH_TX_EP              0x0000F000
#define USBSS_UH_TX_LEN_MASK        0x000007FF  

/* R32_UH_RX_CTRL */
#define USBSS_UH_RX_ACT             0x80000000
#define USBSS_UH_RX_ISO             0x40000000
#define USBSS_UH_RX_NUMP            0x1F000000
#define USBSS_UH_RX_RES             0x00600000
 #define USBSS_UH_RX_NRDY           0x00000000
 #define USBSS_UH_RX_ACK            0x00200000
 #define USBSS_UH_RX_STALL          0x00400000
#define USBSS_UH_RX_SEQ             0x001F0000
#define USBSS_UH_RX_EP              0x0000F000
#define USBSS_UH_RX_LEN_MASK        0x000007FF   

/* R32_HOST_STATUS */
#define USBSS_UH_ITP_PRESAGE        0x000C0000
#define USBSS_UH_RX_ISO_PKT_ERR     0x00020000
#define USBSS_UH_RX_EOB_LPF         0x00010000
#define USBSS_UH_RX_ERDY_DIR        0x00008000
#define USBSS_UH_RX_ERDY_NUMP       0x00001F00
#define USBSS_UH_RX_ERDY_EP         0x000000F0

#define TOUT_MODE       (1<<21)

#define HUB_DEPTH_MASK      (7<<24)
#define HUB_DEPTH_INVLD     (7<<24)

#define PM_MASK         ((uint32_t)0x00000003)
#define POWER_MODE_0    ((uint32_t)0x00000004)
#define POWER_MODE_1    ((uint32_t)0x00000005)
#define POWER_MODE_2    ((uint32_t)0x00000006)
#define POWER_MODE_3    ((uint32_t)0x00000007)

// #define GO_DISABLED     (1<<4)
// #define GO_INACTIVE     (1<<5)
// #define GO_RECOVERY     (1<<6)
// #define GO_RX_DET       (1<<7)

//#define POLLING_EN          ((uint32_t)1<<12)

// link int en

// link int flag

// USB CONTROL

#define USB_INT_EP_MASK     (0x7<<8)
//#define USB_INT_RES_MASK    (0x3<<14)
//#define USB_RES_ACK         (0x0<<14)
//#define USB_RES_ERDY        (0x1<<14)
//#define USB_RES_NRDY        (0x2<<14)
//#define USB_RES_STALL       (0x3<<14)

#define USB_TX_RES_MASK     (0x3<<22)
#define TX_RES_ACK          (0x0<<22)
#define TX_RES_FAILED       (0x1<<22)
#define TX_RES_NRDY         (0x2<<22)
#define TX_RES_STALL        (0x3<<22)

#define USB_RX_RES_MASK     (0x3<<30)
#define RX_RES_ACK          (0x0<<30)
#define RX_RES_FAILED       (0x1<<30)
#define RX_RES_NRDY         (0x2<<30)
#define RX_RES_STALL        (0x3<<30)

#define USB_SEQ_MASK        (0x1f<<21)

// #define USB_INT_ST_MASK      0xf<<16
// #define USB_SEQ_MATCH        0x1<<16

#define USB_TX_LPF          (0x1<<28)

#define USB_STATUS_IS       (0x1<<29)
#define USB_SETUP_IS        (0x1<<30)
#define USB_TX_DIR          (0x1<<12)


#define USB_ERDY_ENDP_MASK  (0xf<<4)
#define USB_ERDY_NUMP_MASK  (0x1f<<8)
// #define USB_ITP_PREAGE       (0x1<<31)


#define EP0_EN            (1<<0)
#define EP1_EN            (1<<1)
#define EP2_EN            (1<<2)
#define EP3_EN            (1<<3)
#define EP4_EN            (1<<4)
//#define EP5_EN            (1<<5)
//#define EP6_EN            (1<<6)
//#define EP7_EN            (1<<7)

#define EP1_ISO           (1<<17)
#define EP2_ISO           (1<<18)
#define EP3_ISO           (1<<19)
#define EP4_ISO           (1<<20)
#define EP5_ISO           (1<<21)
#define EP6_ISO           (1<<22)
#define EP7_ISO           (1<<23)

#define TX_ERDY_ACT         (1<<0)
#define TX_ERDY_DIR         (1<<1)
#define TX_ERDY_ENDP_0      (0<<2)
#define TX_ERDY_ENDP_1      (1<<2)
#define TX_ERDY_ENDP_2      (2<<2)
#define TX_ERDY_ENDP_3      (3<<2)
#define TX_ERDY_ENDP_4      (4<<2)
#define TX_ERDY_ENDP_5      (5<<2)
#define TX_ERDY_ENDP_6      (6<<2)
#define TX_ERDY_ENDP_7      (7<<2)

#define TX_ERDY_NUMP_0      (0<<6)
#define TX_ERDY_NUMP_1      (1<<6)
#define TX_ERDY_NUMP_2      (2<<6)
#define TX_ERDY_NUMP_3      (3<<6)
#define TX_ERDY_NUMP_4      (4<<6)
#define TX_ERDY_NUMP_5      (5<<6)
#define TX_ERDY_NUMP_6      (6<<6)
#define TX_ERDY_NUMP_7      (7<<6)
#define TX_ERDY_NUMP_8      (8<<6)

// host
#define UH_T_EN             (1<<1)
#define UH_R_EN             (1<<1)
//#define UH_R_ISO            (1<<17)
//#define UH_T_ISO            (1<<17)

#define UH_ISO_MODE         (1<<30)
#define UH_TX_SETUP         (0x1<<29)
#define UH_TX_STATUS        (0x1<<28)
#define UH_TX_LPF           (0x1<<23)
#define UH_RTX_VALID        (0x1<<21)

#define UH_RX_EOB           (0x1<<16)
#define UH_ISO_RX_ERR       (0x1<<17)

#define UH_INT_FLAG         (1<<31)


#define USB_ACT_FLAG        0x01
#define USB_SETUP_FLAG      0x02
#define USB_STATUS_FLAG     0x04
#define USB_RX_PING_FLAG    0x08
#define USB_ITP_FLAG        0x10
#define USB_TXOV_FLAG       0x40
#define USB_RXOV_FLAG       0x80
#define USB_ERDY_FLAG       0x02
#define USB_NOTIF_FLAG      0x04

// PING-TP
#define TP_HP           4

#define PING_TP         7
#define PING_DIR        (1<<7)
#define PING_ENDP       (2<<8)

#define DEV_NOTIF_TP    6
#define FUNC_WAKE       (1<<4)

// LMP
#define LMP_HP          0
// LMP subtype
#define LMP_SUBTYPE_MASK        (0xf<<5)
#define LMP_SET_LINK_FUNC       (0x1<<5)
#define LMP_U2_INACT_TOUT       (0x2<<5)
#define LMP_VENDOR_TEST         (0x3<<5)
#define LMP_PORT_CAP            (0x4<<5)
#define LMP_PORT_CFG            (0x5<<5)
#define LMP_PORT_CFG_RES        (0x6<<5)

#define LMP_LINK_SPEED          (1<<9)

#define NUM_HP_BUF      (4<<0)
#define DOWN_STREAM     (1<<16)
#define UP_STREAM       (2<<16)
#define TIE_BRK         (1<<20)

#define DEF_USBSSD_UEP0_SIZE           512
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP0_Tx_Buf[ DEF_USBSSD_UEP0_SIZE ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP0_Rx_Buf[ DEF_USBSSD_UEP0_SIZE ];

#ifdef __cplusplus
}
#endif


#endif /* __ch32h417_USBSS_H */

