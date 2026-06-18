/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_usbpd.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/03/04
 * Description        : This file contains all the functions prototypes for the
 *                      USBPD firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32M030_USBPD_H
#define __CH32M030_USBPD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch32m030.h"

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif

 /******************************************************************************/
/* Related macro definitions */

/* Define the return value of the function */
#ifndef  SUCCESS
#define  SUCCESS                   0
#endif
#ifndef  FAIL
#define  FAIL                      0xFF
#endif

/* Register Bit Definition */
/* USBPD->CONFIG */
#define PD_FILT_ED          (1<<0)             /* PD pin input filter enable */
#define PD_ALL_CLR          (1<<1)             /* Clear all interrupt flags */
#define CC_SEL              (1<<2)             /* Select PD communication port */
#define CC_SEL_CC1          (0<<2)             /* Select PD communication port1 */
#define CC_SEL_CC2          (1<<2)             /* Select PD communication port2 */
#define PD_DMA_EN           (1<<3)             /* Enable DMA for USBPD */
#define PD_RST_EN           (1<<4)             /* PD mode reset command enable */
#define WAKE_POLAR          (1<<5)             /* PD port wake-up level */
#define IE_PD_IO            (1<<10)            /* PD IO interrupt enable */
#define IE_RX_BIT           (1<<11)            /* Receive bit interrupt enable */
#define IE_RX_BYTE          (1<<12)            /* Receive byte interrupt enable */
#define IE_RX_ACT           (1<<13)            /* Receive completion interrupt enable */
#define IE_RX_RESET         (1<<14)            /* Reset interrupt enable */
#define IE_TX_END           (1<<15)            /* Transfer completion interrupt enable */

/* USBPD->CONTROL */
#define PD_TX_EN            (1<<0)             /* USBPD transceiver mode and transmit enable */
#define BMC_START           (1<<1)             /* BMC send start signal */
#define RX_STATE_0          (1<<2)             /* PD received state bit 0 */
#define RX_STATE_1          (1<<3)             /* PD received state bit 1 */
#define RX_STATE_2          (1<<4)             /* PD received state bit 2 */
#define DATA_FLAG           (1<<5)             /* Cache data valid flag bit */
#define TX_BIT_BACK         (1<<6)             /* Indicates the current bit status of the BMC when sending the code */
#define BMC_BYTE_HI         (1<<7)             /* Indicates the current half-byte status of the PD data being sent and received */

/* USBPD->TX_SEL */
#define TX_SEL1             (0<<0)
#define TX_SEL1_SYNC1       (0<<0)             /* 0-SYNC1 */
#define TX_SEL1_RST1        (1<<0)             /* 1-RST1 */
#define TX_SEL2_Mask        (3<<2)
#define TX_SEL2_SYNC1       (0<<2)             /* 00-SYNC1 */
#define TX_SEL2_SYNC3       (1<<2)             /* 01-SYNC3 */
#define TX_SEL2_RST1        (2<<2)             /* 1x-RST1 */
#define TX_SEL3_Mask        (3<<4)
#define TX_SEL3_SYNC1       (0<<4)             /* 00-SYNC1 */
#define TX_SEL3_SYNC3       (1<<4)             /* 01-SYNC3 */
#define TX_SEL3_RST1        (2<<4)             /* 1x-RST1 */
#define TX_SEL4_Mask        (3<<6)
#define TX_SEL4_SYNC2       (0<<6)             /* 00-SYNC2 */
#define TX_SEL4_SYNC3       (1<<6)             /* 01-SYNC3 */
#define TX_SEL4_RST2        (2<<6)             /* 1x-RST2 */

/* USBPD->STATUS */
#define BMC_AUX_Mask        (3<<0)              /* Clear BMC auxiliary information */
#define BMC_AUX_INVALID     (0<<0)              /* 00-Invalid */
#define BMC_AUX_SOP0        (1<<0)              /* 01-SOP0 */
#define BMC_AUX_SOP1_HRST   (2<<0)              /* 10-SOP1 hard reset */
#define BMC_AUX_SOP2_CRST   (3<<0)              /* 11-SOP2 cable reset */
#define BUF_ERR             (1<<2)              /* BUFFER or DMA error interrupt flag */
#define IF_RX_BIT           (1<<3)              /* Receive bit or 5bit interrupt flag */
#define IF_RX_BYTE          (1<<4)              /* Receive byte or SOP interrupt flag */
#define IF_RX_ACT           (1<<5)              /* Receive completion interrupt flag */
#define IF_RX_RESET         (1<<6)              /* Receive reset interrupt flag */
#define IF_TX_END           (1<<7)              /* Transfer completion interrupt flag */

/* USBPD->PORT_CC1 */
/* USBPD->PORT_CC2 */
#define CC_CMPO             (1<<0)               /* CC port comparator analogue input */
#define CC_PD               (1<<1)               /* CC port pull-down resistor enable */
#define CC_PU_Mask          (3<<2)               /* Clear CC port pull-up current */
#define CC_NO_PU            (0<<2)               /* 00-Prohibit pull-up current */
#define CC_PU_330           (1<<2)               /* 01-330uA */
#define CC_PU_180           (2<<2)               /* 10-180uA */
#define CC_PU_80            (3<<2)               /* 11-80uA */
#define CC_LVE              (1<<4)               /* CC port output low voltage enable */
#define CC_CVS_Mask         (3<<5)               /* Clear CC_CVS*/
#define CC_NO_CVS           (0<<5)               /* 00-closed */
#define CC_CVS_22           (1<<5)               /* 01-0.22V */
#define CC_CVS_44           (0<<5)               /* 00-0.44V */
#define CC_CVS_66           (2<<5)               /* 10-0.66V */
#define CC_CVS_123          (3<<5)               /* 11-1.23V */
#define CC_CE               (1<<7)

/*USBPD_CC_HVT*/
#define USBPD0_HVT_Mask     (3<<18)
#define USBPD0_CC_HVT       (1<<18)
#define USBPD1_CC_HVT       (1<<19)

/**********************************************************
* PD transceiver PHY pull-up limit configuration bits:
* 1-Direct use of VDD for GPIO applications or PD applications with VDD voltage of 3.3V
* 0-LDO buck enabled, limited to approx 3.3V, for PD applications with VDD more than 4V
* ********************************************************/

/* Control Message Types */
#define DEF_TYPE_RESERVED          0x00
#define DEF_TYPE_GOODCRC           0x01                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GOTOMIN           0x02                                         /* Send By: Source */
#define DEF_TYPE_ACCEPT            0x03                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_REJECT            0x04                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_PING              0x05                                         /* Send By: Source */
#define DEF_TYPE_PS_RDY            0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SRC_CAP       0x07                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_SNK_CAP       0x08                                         /* Send By: Source,DRP */
#define DEF_TYPE_DR_SWAP           0x09                                         /* Send By: Source,Sink */
#define DEF_TYPE_PR_SWAP           0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_VCONN_SWAP        0x0B                                         /* Send By: Source,Sink */
#define DEF_TYPE_WAIT              0x0C                                         /* Send By: Source,Sink */
#define DEF_TYPE_SOFT_RESET        0x0D                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET        0x0E                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET_CMP    0x0F                                         /* Send By: Source,Sink */
#define DEF_TYPE_NOT_SUPPORT       0x10                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GET_SRC_CAP_EX    0x11                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_STATUS        0x12                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_STATUS_R      0X02                                         /* ext=1 */
#define DEF_TYPE_FR_SWAP           0x13                                         /* Send By: Sink */
#define DEF_TYPE_GET_PPS_STATUS    0x14                                         /* Send By: Sink */
#define DEF_TYPE_GET_CTY_CODES     0x15                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SNK_CAP_EX    0x16                                         /* Send By: Source,DRP */
#define DEF_TYPE_GET_SRC_INFO      0x17                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_REVISION      0x18                                         /* Send By: Source,Sink */

/* Data Message Types */
#define DEF_TYPE_SRC_CAP           0x01                                         /* Send By: Source,Dual-Role Power */
#define DEF_TYPE_REQUEST           0x02                                         /* Send By: Sink */
#define DEF_TYPE_BIST              0x03                                         /* Send By: Tester,Source,Sink */
#define DEF_TYPE_SNK_CAP           0x04                                         /* Send By: Sink,Dual-Role Power */
#define DEF_TYPE_BAT_STATUS        0x05                                         /* Send By: Source,Sink */
#define DEF_TYPE_ALERT             0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_CTY_INFO      0x07                                         /* Send By: Source,Sink */
#define DEF_TYPE_ENTER_USB         0x08                                         /* Send By: DFP */
#define DEF_TYPE_EPR_REQUEST       0x09                                         /* Send By: Sink */
#define DEF_TYPE_EPR_MODE          0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_SRC_INFO          0x0B                                         /* Send By: Source */
#define DEF_TYPE_REVISION          0x0C                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_VENDOR_DEFINED    0x0F                                         /* Send By: Source,Sink,Cable Plug */

/* Vendor Define Message Command */
#define DEF_VDM_DISC_IDENT         0x01
#define DEF_VDM_DISC_SVID          0x02
#define DEF_VDM_DISC_MODE          0x03
#define DEF_VDM_ENTER_MODE         0x04
#define DEF_VDM_EXIT_MODE          0x05
#define DEF_VDM_ATTENTION          0x06
#define DEF_VDM_DP_S_UPDATE        0x10
#define DEF_VDM_DP_CONFIG          0x11

/* PD Revision */
#define DEF_PD_REVISION_10         0x00
#define DEF_PD_REVISION_20         0x01
#define DEF_PD_REVISION_30         0x02

/* PD PHY Channel */
#define DEF_PD_CC1                 0x00
#define DEF_PD_CC2                 0x01

/* PD Tx Status */
#define DEF_PD_TX_OK               0x00
#define DEF_PD_TX_FAIL             0x01

/* PDO INDEX */
#define PDO_INDEX_1                1
#define PDO_INDEX_2                2
#define PDO_INDEX_3                3
#define PDO_INDEX_4                4
#define PDO_INDEX_5                5

/******************************************************************************/

#define UPD_TMR_TX_48M    (80-1)                                             /* timer value for USB PD BMC transmittal @Fsys=48MHz */
#define UPD_TMR_RX_48M    (120-1)                                            /* timer value for USB PD BMC receiving @Fsys=48MHz */
#define UPD_TMR_TX_24M    (40-1)                                             /* timer value for USB PD BMC transmittal @Fsys=24MHz */
#define UPD_TMR_RX_24M    (60-1)                                             /* timer value for USB PD BMC receiving @Fsys=24MHz */
#define UPD_TMR_TX_12M    (20-1)                                             /* timer value for USB PD BMC transmittal @Fsys=12MHz */
#define UPD_TMR_RX_12M    (30-1)                                             /* timer value for USB PD BMC receiving @Fsys=12MHz */

#define MASK_PD_STAT      0x03                                               /* Bit mask for current PD status */
#define PD_RX_SOP0        0x01                                               /* SOP0 received */
#define PD_RX_SOP1_HRST   0x02                                               /* SOP1 or Hard Reset received */
#define PD_RX_SOP2_CRST   0x03                                               /* SOP2 or Cable Reset received */

#define UPD_SOP0          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC1 | TX_SEL4_SYNC2 )     /* SOP1 */
#define UPD_SOP1          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC3 | TX_SEL4_SYNC3 )     /* SOP2 */
#define UPD_SOP2          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC3 | TX_SEL3_SYNC1 | TX_SEL4_SYNC3 )     /* SOP3 */
#define UPD_HARD_RESET    ( TX_SEL1_RST1  | TX_SEL2_RST1  | TX_SEL3_RST1  | TX_SEL4_RST2  )     /* Hard Reset*/
#define UPD_CABLE_RESET   ( TX_SEL1_RST1  | TX_SEL2_SYNC1 | TX_SEL3_RST1  | TX_SEL4_SYNC3 )     /* Cable Reset*/


#define bCC_CMP_22        0X01
#define bCC_CMP_44        0X02
#define bCC_CMP_66        0X08
#define bCC_CMP_123       0X20
#define bCC_CMP_220       0X40

#define		PD_Ctrl_Reserved			0x00
#define		PD_Ctrl_GoodCRC				0x01
#define		PD_Ctrl_GotoMin				0x02
#define		PD_Ctrl_Accept				0x03
#define		PD_Ctrl_Reject				0x04
#define		PD_Ctrl_Ping				0x05
#define		PD_Ctrl_PS_Ready			0x06
#define		PD_Ctrl_GetSrcCap			0x07
#define		PD_Ctrl_GetSinkCap			0x08
#define		PD_Ctrl_DRSwap				0x09
#define		PD_Ctrl_PRSwap				0x0A
#define		PD_Ctrl_VconnSwap			0x0B
#define		PD_Ctrl_Wait				0x0C
#define		PD_Ctrl_SoftReset			0x0D
#define		PD_Ctrl_DataReset			0x0E
#define		PD_Ctrl_DataResetComplete	0x0F
#define		PD_Ctrl_NotSupported		0x10
#define		PD_Ctrl_GetSrcCapExtended	0x11
#define		PD_Ctrl_GetStatus			0x12
#define		PD_Ctrl_FRSwap				0x13
#define		PD_Ctrl_GetPPSStatus		0x14
#define		PD_Ctrl_GetCountryCodes		0x15
#define		PD_Ctrl_GetSinkCapExtended	0x16
#define		PD_Ctrl_GetSrcInfo			0x17
#define		PD_Ctrl_GetRevision			0x18

#define		PD_Data_Reserved1			0x00
#define		PD_Data_SrcCap				0x01
#define		PD_Data_Request				0x02
#define		PD_Data_BIST				0x03
#define		PD_Data_SinkCap				0x04
#define		PD_Data_BatteryStatus		0x05
#define		PD_Data_Alert				0x06
#define		PD_Data_GetCountryInfo		0x07
#define		PD_Data_EnterUSB			0x08
#define		PD_Data_EPRRequest			0x09
#define		PD_Data_EPRMode				0x0A
#define		PD_Data_SrcInfo				0x0B
#define		PD_Data_Revision			0x0C
#define		PD_Data_Reserved2			0x0D
#define		PD_Data_Reserved3			0x0E
#define		PD_Data_VendorDefined		0x0F

#define		PD_Ext_Reserved				0x00
#define		PD_Ext_SrcCapExtended		0x01
#define		PD_Ext_Status				0x02
#define		PD_Ext_GetBatteryCap		0x03
#define		PD_Ext_GetBatteryStatus		0x04
#define		PD_Ext_BattertCap			0x05
#define		PD_Ext_GetManufacturerInfo	0x06
#define		PD_Ext_ManufacturerInfo		0x07
#define		PD_Ext_SecurityRequest		0x08
#define		PD_Ext_SecurityResponse		0x09
#define		PD_Ext_FWUpdateRequest		0x0A
#define		PD_Ext_FWUpdateResponse		0x0B
#define		PD_Ext_PPSStatus			0x0C
#define		PD_Ext_SinkCapExtended		0x0F
#define		PD_Ext_ExtendControl		0x10
#define		PD_Ext_EPRSrcCap			0x11
#define		PD_Ext_EPRSinkCap  		    0x12

#define     PD_VC_DCDCISink             0x01
#define     PD_VC_PDPassThrh            0x02

#define		PD_VDM_DiscoverIdentity		0x01
#define		PD_VDM_DiscoverSVIDs		0x02
#define		PD_VDM_DiscoverModes		0x03
#define		PD_VDM_EnterModes			0x04
#define		PD_VDM_ExitModes			0x05
#define		PD_VDM_Attention			0x06

#define 	DEF_Unstructured_VDM	    0
#define 	DEF_Structured_VDM			1

/* VDM */
#define     DEF_VDM_REQ                 0
#define     DEF_VDM_ACK                 1
#define     DEF_VDM_NAK                 2
#define     DEF_VDM_BUSY                3

/* EPRMode Action */
#define EPRMode_Enter                   0x01
#define EPRMode_Enter_Acked             0x02
#define EPRMode_Enter_Succeed           0x03
#define EPRMode_Enter_Fail              0x04
#define EPRMode_Exit                    0x05

/* PD Revision */
#define PD_Rev2         		0x01
#define PD_Rev3         		0x02

#define DevRole_Sink			0
#define DevRole_Src				1
#define DevRole_DRP				2
#define DevRole_DRP_TrySink		3
#define DevRole_DRP_TrySrc		4

#ifdef __cplusplus
}
#endif

#endif
