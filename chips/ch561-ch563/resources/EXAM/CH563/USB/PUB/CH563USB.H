/********************************** (C) COPYRIGHT *******************************
* File Name          : CH563USB.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB Define Head File
*******************************************************************************/


#ifndef __CH563USB_H__
#define __CH563USB_H__

#include "CH563BAS.H"

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************************************************************* */
/* USB define & constants */

#ifndef USB_DEFAULT_MAX_PKT
#define USB_DEFAULT_MAX_PKT 	   		64
#endif
#ifndef MAX_USB_STRING_LEN
#define MAX_USB_STRING_LEN  			255
#endif

/* 连接设备速度定义 */
#define USB_DEV_SPEED_LS		       	0x01									/* 当前为低速设备 */
#define USB_DEV_SPEED_FS		       	0x00									/* 当前为全速设备 */
#define USB_DEV_SPEED_HS		       	0x02									/* 当前为高速设备 */

/* USB package PID */
#ifndef DEF_USB_PID_SETUP
#define DEF_USB_PID_NULL    			0x00   									/* reserved */
#define DEF_USB_PID_PRE     			0x0C
#define DEF_USB_PID_ERR     			0x0C
#define DEF_USB_PID_SPLIT   			0x08
#define DEF_USB_PID_PING    			0x04
#define DEF_USB_PID_SOF     			0x05
#define DEF_USB_PID_SETUP   			0x0D
#define DEF_USB_PID_IN      			0x09
#define DEF_USB_PID_OUT     			0x01
#define DEF_USB_PID_ACK     			0x02
#define DEF_USB_PID_NAK     			0x0A
#define DEF_USB_PID_STALL   			0x0E
#define DEF_USB_PID_NYET    			0x06
#define DEF_USB_PID_DATA0   			0x03
#define DEF_USB_PID_DATA1   			0x0B
#define DEF_USB_PID_DATA2   			0x07
#define DEF_USB_PID_MDATA   			0x0F
#endif

/* USB device classes */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED  		0x00
#define USB_DEV_CLASS_AUDIO     		0x01
#define USB_DEV_CLASS_COMMUNIC  		0x02
#define USB_DEV_CLASS_HUMAN_IF  		0x03
#define USB_DEV_CLASS_MONITOR   		0x04
#define USB_DEV_CLASS_PHYSIC_IF 		0x05
#define USB_DEV_CLASS_POWER     		0x06
#define USB_DEV_CLASS_PRINTER   		0x07
#define USB_DEV_CLASS_STORAGE   		0x08
#define USB_DEV_CLASS_HUB       		0x09
#define USB_DEV_CLASS_VEN_SPEC  		0xFF
#endif

/* USB descriptor type */
#ifndef USB_DEV_DESCR_TYPE
#define USB_DEV_DESCR_TYPE      		0x01
#define USB_CFG_DESCR_TYPE      		0x02
#define USB_STR_DESCR_TYPE      		0x03
#define USB_ITF_DESCR_TYPE      		0x04
#define USB_ENDP_DESCR_TYPE     		0x05
#define USB_PWR_DESCR_TYPE      		0x06
#define USB_DEV_QUAL_DESCR_TYPE 		0x06
#define USB_CFG_PWR_DESCR_TYPE  		0x07
#define USB_ITF_PWR_DESCR_TYPE  		0x08
#define USB_HID_DESCR_TYPE      		0x21
#define USB_REPORT_DESCR_TYPE   		0x22
#define USB_PHYSIC_DESCR_TYPE   		0x23
#endif

/* USB standard request codes, bReqType bit6=0 & bit5=0 */
#ifndef USB_REQ_GET_DESCR
#define USB_REQ_GET_STATUS   			0x00
#define USB_REQ_CLR_FEATURE  			0x01
#define USB_REQ_SET_FEATURE  			0x03
#define USB_REQ_SET_ADDRESS  			0x05
#define USB_REQ_GET_DESCR    			0x06
#define USB_REQ_SET_DESCR    			0x07
#define USB_REQ_GET_CONFIG   			0x08
#define USB_REQ_SET_CONFIG   			0x09
#define USB_REQ_GET_INTERF   			0x0A
#define USB_REQ_SET_INTERF   			0x0B
#define USB_REQ_SYNC_FRAME   			0x0C
#endif

/* USB request type */
#ifndef USB_REQ_TYPE_STAND
#define USB_REQ_TYPE_READ    			0x80
#define USB_REQ_TYPE_WRITE   			0x00
#define USB_REQ_TYPE_MASK    			0x60   									/* bit mask for request type */
#define USB_REQ_TYPE_STAND   			0x00
#define USB_REQ_TYPE_CLASS   			0x20
#define USB_REQ_TYPE_VENDOR  			0x40
#endif

/* bReqType.Type */
#ifndef USB_BMREQ_STANDARD
#define USB_BMREQ_STANDARD   			0
#define USB_BMREQ_CLASS      			1
#define USB_BMREQ_VENDOR     			2
#endif

/* bReqType.Recipient */
#ifndef USB_BMREQ_TO_DEV
#define USB_BMREQ_TO_DEV     			0
#define USB_BMREQ_TO_ITF     			1
#define USB_BMREQ_TO_ENDP   	 		2
#define USB_BMREQ_TO_OTHER  	 		3
#endif

/* Values for bmAttributes field of an endpoint descriptor */
#ifndef USB_ENDP_TYPE_MASK
#define USB_ENDP_DIR_MASK    			0x80
#define USB_ENDP_TYPE_MASK   			0x03
#define USB_ENDP_TYPE_CTRL   			0x00
#define USB_ENDP_TYPE_ISOCH  			0x01
#define USB_ENDP_TYPE_BULK   			0x02
#define USB_ENDP_TYPE_INTER  			0x03
#endif

/* values for the bits returned by the USB_REQ_GET_STATUS command */
#ifndef USB_GETSTAT_REMOTE_WAKE_EN
#define USB_GETSTAT_SELF_POWERED    	0x01
#define USB_GETSTAT_REMOTE_WAKE_EN  	0x02
#endif

/* definitions for bits in the bmAttributes field of a configuration descriptor */
#ifndef USB_CFG_REMOTE_WAKEUP
#define USB_CFG_POWERED_MASK        	0xC0
#define USB_CFG_BUS_POWERED         	0x80
#define USB_CFG_SELF_POWERED        	0x40
#define USB_CFG_REMOTE_WAKEUP       	0x20
#endif

/* USB defined Feature selectors */
#ifndef USB_FEATURE_ENDP_STALL
#define USB_FEATURE_ENDP_STALL      	0x00
#define USB_FEATURE_REM_WAKEUP      	0x01
#endif
#ifndef USB_FEATURE_ITF_PWR_D0
#define USB_FEATURE_ITF_PWR_D0      	0x02
#define USB_FEATURE_ITF_PWR_D1      	0x03
#define USB_FEATURE_ITF_PWR_D2      	0x04
#define USB_FEATURE_ITF_PWR_D3      	0x05
#define USB_SUPPORT_D0_COMMAND      	0x01
#define USB_SUPPORT_D1_COMMAND      	0x02
#define USB_SUPPORT_D2_COMMAND      	0x04
#define USB_SUPPORT_D3_COMMAND      	0x08
#define USB_SUPPORT_D1_WAKEUP       	0x10
#define USB_SUPPORT_D2_WAKEUP       	0x20
#endif

/* ********************************************************************************************************************* */
/* Base Structures */

#ifndef BM_REQUEST_TYPE
/* BitMask for bReqType of USB_SETUP_REQ */
typedef union _BM_REQUEST_TYPE 
{
	struct _BM 
	{
		int  Recipient:2;
		int  Reserved:3;
		int  Type:2;
		int  Direct:1;
	}BM;
	UINT8  b8;
}BM_REQ_TYPE, *PBM_REQ_TYPE;

#endif

#ifndef USB_SETUP_REQ
typedef struct _USB_SETUP_REQ 
{
	UINT8  bReqType;
	UINT8  bRequest;
	UINT8  wValueL;
	UINT8  wValueH;
	UINT8  wIndexL;
	UINT8  wIndexH;
	UINT8  wLengthL;
	UINT8  wLengthH;
}USB_SETUP_REQ, *PUSB_SETUP_REQ;
#endif

#ifndef USB_DEV_DESCR
typedef struct _USB_DEVICE_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  bcdUSBL;
	UINT8  bcdUSBH;
	UINT8  bDeviceClass;
	UINT8  bDeviceSubClass;
	UINT8  bDeviceProtocol;
	UINT8  bMaxPacketSize0;
	UINT8  idVendorL;
	UINT8  idVendorH;
	UINT8  idProductL;
	UINT8  idProductH;
	UINT8  bcdDeviceL;
	UINT8  bcdDeviceH;
	UINT8  iManufacturer;
	UINT8  iProduct;
	UINT8  iSerialNumber;
	UINT8  bNumConfigurations;
}USB_DEV_DESCR, *PUSB_DEV_DESCR;
#endif

#ifndef USB_CFG_DESCR
typedef struct _USB_CONFIG_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  wTotalLengthL;
	UINT8  wTotalLengthH;
	UINT8  bNumInterfaces;
	UINT8  bConfigurationValue;
	UINT8  iConfiguration;
	UINT8  bmAttributes;
	UINT8  MaxPower;
}USB_CFG_DESCR, *PUSB_CFG_DESCR;
#endif

#ifndef USB_ITF_DESCR
typedef struct _USB_INTERF_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  bInterfaceNumber;
	UINT8  bAlternateSetting;
	UINT8  bNumEndpoints;
	UINT8  bInterfaceClass;
	UINT8  bInterfaceSubClass;
	UINT8  bInterfaceProtocol;
	UINT8  iInterface;
}USB_ITF_DESCR, *PUSB_ITF_DESCR;
#endif

#ifndef USB_ENDP_DESCR
typedef struct _USB_ENDPOINT_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  bEndpointAddress;
	UINT8  bmAttributes;
	UINT8  wMaxPacketSizeL;
	UINT8  wMaxPacketSizeH;
	UINT8  bInterval;
}USB_ENDP_DESCR, *PUSB_ENDP_DESCR;
#endif

#ifndef USB_CFG_DESCR_LONG
typedef struct _USB_CONFIG_DESCRIPTOR_LONG 
{
	USB_CFG_DESCR  cfg_descr;
	USB_ITF_DESCR  itf_descr;
	USB_ENDP_DESCR endp_descr[1];
}USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;
#endif

#ifndef USB_COMM_DESCR
typedef struct _USB_COMMON_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
}USB_COMM_DESCR, *PUSB_COMM_DESCR;
#endif

#ifndef USB_HUB_DESCR
typedef struct _USB_HUB_DESCRIPTOR 
{
	UINT8  bDescriptorLength;
	UINT8  bDescriptorType;
	UINT8  bNumberOfPorts;
	UINT8  wHubCharacteristicsL;
	UINT8  wHubCharacteristicsH;
	UINT8  bPowerOnToPowerGood;
	UINT8  bHubControlCurrent;
	UINT8  bRemoveAndPowerMask[1];
}USB_HUB_DESCR, *PUSB_HUB_DESCR;
#endif

#ifndef USB_DEV_QUAL_DESCR
typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  bcdUSBL;
	UINT8  bcdUSBH;
	UINT8  bDeviceClass;
	UINT8  bDeviceSubClass;
	UINT8  bDeviceProtocol;
	UINT8  bMaxPacketSize0;
	UINT8  bNumConfigurations;
	UINT8  bReserved;
}USB_DEV_QUAL_DESCR, *PUSB_DEV_QUAL_DESCR;
#endif

#ifndef USB_CFG_PWR_DESCR
typedef struct _USB_CONFIGURATION_POWER_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  SelfPowerConsumedD0[3];
	UINT8  bPowerSummaryId;
	UINT8  bBusPowerSavingD1;
	UINT8  bSelfPowerSavingD1;
	UINT8  bBusPowerSavingD2;
	UINT8  bSelfPowerSavingD2; 
	UINT8  bBusPowerSavingD3;
	UINT8  bSelfPowerSavingD3; 
	UINT8  TransitionTimeFromD1L;
	UINT8  TransitionTimeFromD1H;
	UINT8  TransitionTimeFromD2L;
	UINT8  TransitionTimeFromD2H;
	UINT8  TransitionTimeFromD3L;
	UINT8  TransitionTimeFromD3H;
}USB_CFG_PWR_DESCR, *PUSB_CFG_PWR_DESCR;
#endif

#ifndef USB_ITF_PWR_DESCR
typedef struct _USB_INTERFACE_POWER_DESCRIPTOR 
{
	UINT8  bLength;
	UINT8  bDescriptorType;
	UINT8  bmCapabilitiesFlags;
	UINT8  bBusPowerSavingD1;
	UINT8  bSelfPowerSavingD1;
	UINT8  bBusPowerSavingD2;
	UINT8  bSelfPowerSavingD2; 
	UINT8  bBusPowerSavingD3;
	UINT8  bSelfPowerSavingD3; 
	UINT8  TransitionTimeFromD1L;
	UINT8  TransitionTimeFromD1H;
	UINT8  TransitionTimeFromD2L;
	UINT8  TransitionTimeFromD2H;
	UINT8  TransitionTimeFromD3L;
	UINT8  TransitionTimeFromD3H;
}USB_ITF_PWR_DESCR, *PUSB_ITF_PWR_DESCR;
#endif

/*********************************************************************************************************************/
/*********************************************************************************************************************/
/* 以下为EHCI寄存器相关定义 */
/* 具体细节见<< Enhanced Host Controller Interface Specification for Universal Serial Bus >> */

#define EHCI_STRUCTURE_TYPE_QH	   0x00
#define EHCI_STRUCTURE_TYPE_QTD    0x01
#define EHCI_STRUCTURE_TYPE_ITD    0x02
#define EHCI_STRUCTURE_TYPE_SITD   0x03

/* Section 2.2 Host Controller Capability Registers */
#ifndef EHCI_CAPABILITY
typedef struct _EHCI_CAPABILITY
{
	/* HC_CAPLENGTH: offset 0x00 */
	UINT32 hc_caplength;														
#define RB_HC_LENGTH( p )		   ( ( ( p ) >> 00 ) & 0x00FF )					/* bits 7:0 */
#define RB_HC_VERSION( p )		   ( ( ( p ) >> 16 ) & 0xFFFF )					/* bits 31:16 */

	/* HC_HCSPARAMS: offset 0x04 */
	UINT32 hc_hcsparams;     													

	/* HC_HCCPARAMS: offset 0x08 */
	UINT32 hc_hccparams;      													
}EHCI_CAPABILITY, *PEHCI_CAPABILITY;
#endif


/* Section 2.3 Host Controller Operational Registers */ 
#ifndef EHCI_REGISTERS
typedef struct _EHCI_REGISTERS
{
	/* HC_USBCMD: offset 0x10 */
	UINT32 hc_usbcmd;															
																				/* bits 31:24---Reserved */
#define RB_INT_THRC( c )		   ( c << 16 )									/* bits 23:16---Interrupt Threshold Control */	
																			    /* bits 15:12---Reserved */												
#define RB_ASYN_PK_EN		   	   ( 1 << 11 )									/* bits 11------Asynchronous Schedule Park Mode Enable */
																				/* bits 10------Reserved */
#define RB_ASYN_PK_CNT( c )		   ( ( c & 0x03 ) << 8 )						/* bits 9:8-----Asynchronous Schedule Park Mode Count */
																				/* bits 7-------Reserved */
#define RB_INT_OAAD				   ( 1 << 6 )								    /* bits 6-------Interrupt on Async Advance Doorbell */
#define RB_ASCH_EN				   ( 1 << 5 )									/* bits 5-------Asynchronous Schedule Enable */
#define RB_PSCH_EN			       ( 1 << 4 )									/* bits 4-------Periodic Schedule Enable */
#define RB_FRL_SIZE( c )		   ( ( c & 0x03 ) << 2 )						/* bits 3:2-----Frame List Size */ 						
#define RB_HC_RESET				   ( 1 << 1 )									/* bits 1-------Host Controller Reset */	
#define RB_RS					   ( 1 << 0 )									/* bits 0-------Run/Stop */

	/* HC_USBSTS: offset 0x14 */
	UINT32 hc_usbsts;															
																				/* bits 31:16---Reserved */	
#define RB_ASCH_STS				   ( 1 << 15 )									/* bits 15------Asynchronous Schedule Status */
#define RB_PSCH_STS				   ( 1 << 14 )									/* bits 14------Periodic Schedule Status */
#define RB_RECLAMATIOM 			   ( 1 << 13 )									/* bits 13------Reclamation */
#define RB_HCHALTED			       ( 1 << 12 )									/* bits 12------HCHalted */
																			    /* bits 11:6----Reserved */
#define RB_INT_OAA				   ( 1 << 5 )									/* bits 5-------Interrupt on Async Advance */
#define RB_H_STSERR			       ( 1 << 4 )									/* bits 4-------Host System Error */
#define RB_FRL_ROL				   ( 1 << 3 )									/* bits 3-------Frame List Rollover */
#define RB_PO_CHG_DET			   ( 1 << 2 )									/* bits 2-------Port Change Detect */
#define RB_USBERR_INT			   ( 1 << 1 )									/* bits 1-------USB Error Interrupt (USBERRINT) */
#define RB_USB_INT				   ( 1 << 0 )									/* bits 0-------USB Interrupt (USBINT) */

	/* HC_USBINT_EN: offset 0x18 */
	UINT32 hc_usbint_en;	
																				/* bits 31:6----Reserved */
#define RB_INT_OAA_EN			   ( 1 << 5 )									/* bits 5-------Interrupt on Async Advance Enable */
#define RB_H_SYSERR_EN			   ( 1 << 4 )									/* bits 4-------Host System Error Enable */
#define RB_FRL_ROL_EN			   ( 1 << 3 )									/* bits 3-------Frame List Rollover Enable */
#define RB_PO_CHG_INT_EN		   ( 1 << 2 )									/* bits 2-------Port Change Interrupt Enable */
#define RB_USBERR_INT_EN		   ( 1 << 1 )								    /* bits 1-------USB Error Interrupt Enable */
#define RB_USB_INT_EN			   ( 1 << 0 )									/* bits 0-------USB Interrupt Enable */

	/* HC_FRINDEX: offset 0x1C */
	UINT32 hc_frindex;	

	/* CTRLDSSEGMENT: offset 0x20 */
	UINT32 hc_segment;															/* Reserved */

	/* HC_PERIODICLISTBASE: offset 0x24 */
	UINT32 hc_periodiclistbase;													/* bits 31:12---Periodic Frame List Base Address */
																				/* bits 11:0----Reserved */
	/* HC_ASYNCHRONOUS: offset 0x28 */
	UINT32 hc_asynchronous;														

	/* CONFIGFLAG: offset 0x2C */
	UINT32 configured_flag;														/* Reserved */

	/* HC_PORTSC: offset 0x30 */
	UINT32 hc_portsc;															
																				/* bits 31:21---Reserved */
#define RB_HC_TST_PKDONE		   ( 1 << 20 )									/* bits 20------Wake on Connect Enable (WKCNNT_E) */
#define RB_PORT_TEST( c )		   ( ( c & 0x0F ) << 16 )						/* bits 19:16---Port Test Control */
																				/* bits 15:12---Reserved */
#define RB_LINE_STS( c )		   ( ( c & 0x03 ) << 10 )						/* bits 11:10---Port Test Control */
																				/* bits 9-------Reserved */
#define RB_PO_RESET				   ( 1 << 8 )									/* bits 8-------Port Reset */
#define RB_PO_SUSP				   ( 1 << 7 )									/* bits 7-------Suspend */
#define RB_F_PO_RESM			   ( 1 << 6 )									/* bits 6-------Force Port Resume */
																				/* bits 5:4-----Reserved */
#define RB_PO_EN_CHG			   ( 1 << 3 )									/* bits 3-------Port Enable/Disable Change */
#define RB_PO_EN				   ( 1 << 2 )									/* bits 2-------Port Enabled/Disabled */
#define RB_CONN_CHG			       ( 1 << 1 )									/* bits 1-------Connect Status Change */
#define RB_CONN_STS				   ( 1 << 0 )									/* bits 0-------Current Connect Status */

	/* offset 0x34---offset 0x3C */
	UINT32 reserve[ 3 ];														/* Reserved */														

	/* HC_MISCELLANEOUS: offset 0x40 */
	UINT32 hc_miscellaneous;
	
	/* HC_FS_EOF1_T_POINT: offset 0x44 */
	UINT32 hc_fs_eof1_point;
	
	/* HC_HS_EOF1_T_POINT: offset 0x48 */
	UINT32 hc_hs_eof1_point;
	
}EHCI_REGISTERS, *PEHCI_REGISTERS;
#endif

/*********************************************************************************************************************/
/*********************************************************************************************************************/
/* 以下为EHCI各种数据结构相关定义 */
/* 具体细节见<< Enhanced Host Controller Interface Specification for Universal Serial Bus >> */

/***************************************************************************
 * EHCI Specification Section 3.6
 * QH: describes control/bulk/interrupt endpoints
 * See Fig 3-7 "Queue Head Structure Layout".
 * These appear in both the async and (for interrupt) periodic schedules.
****************************************************************************/
#ifndef EHCI_QH
typedef struct _EHCI_QH
{
	/* Next_qHD_Pointer Word */
	UINT32 hw_next;																/* see EHCI 3.6.1 */
#define QH_TYP_ITD				   0x00											/* 00=iTD(s实时传输描述符) */
#define QH_TYP_QH				   0x01											/* 01=QH(队列头) */
#define QH_TYP_SITD				   0x02											/* 10=siTD(SPLIT事物处理的实时传输描述符) */
#define QH_TYP_FSTN				   0x03											/* 11=FSTN(s实时传输描述符) */
		
	UINT32 hw_info1;       														/* see EHCI 3.6.2 */
#define QH_RL					   0xF0000000L
#define	QH_HEAD				       0x00008000
#define QH_DTC_QH			       0x00000000
#define QH_DTC_TD			       0x00004000
#define QH_EP_SPEED_HS		       0x00002000									/* 当前端点为高速(位13:12=10 ) */
#define QH_EP_SPEED_FS		       0x00000000									/* 当前端点为全速(位13:12=00 ) */
#define QH_EP_SPEED_LS		       0x00001000									/* 当前端点为低速(位13:12=01 ) */
#define QH_EP_NUM_0			       0x00000000
#define QH_DADDR				   DEFAULT_USB_OP_ADDR

	UINT32 hw_info2;        													/* see EHCI 3.6.2 */
#define	QH_MULT( c )			   ( ( c & 0x03 ) << 30 )
#define	QH_HUBPORT( c )	   		   ( c << 23 )
#define	QH_HUBADDR( c )			   ( c << 16 )

	UINT32 hw_current;															/* qtd list - see EHCI 3.6.4 */

	/* qtd overlay (hardware parts of a struct ehci_qtd) */
	UINT32 hw_qtd_next;
	
	UINT32 hw_alt_next;

	UINT32 hw_token;
#define	QH_IOC					   0x00008000									/* interrupt on complete */	
	
	UINT32 hw_buf[ 5 ];
}EHCI_QH, *PEHCI_QH;
#endif

/***************************************************************************
 * EHCI Specification Section 3.5
 * QTD: describe data transfer components (buffer, direction, ...)
 * See Fig 3-6 "Queue Element Transfer Descriptor Block Diagram".
 * These are associated only with "QH" (Queue Head) structures,
 * used with control, bulk, and interrupt transfers.
****************************************************************************/
#ifndef EHCI_QTD
typedef struct _EHCI_QTD
{
	UINT32 hw_next;																/* see EHCI 3.5.1 */

	UINT32 hw_alt_next;    														/* see EHCI 3.5.2 */

	UINT32 hw_token;       														/* see EHCI 3.5.3 */
#define	QTD_TOGGLE			       0x80000000									/* data toggle */
#define	QTD_IOC				       ( 1 << 15 )									/* interrupt on complete */
#define QTD_CERR_3			       0x00000C00
#define QTD_CERR_2			       0x00000800
#define QTD_CERR_1			       0x00000400
#define QTD_CERR_0			       0x00000000
#define	QTD_CERR( c )		       ( ( c >> 10 ) & 0x03 )
#define	QTD_PID( c )		       ( ( c >> 8 ) & 0x03 )
#define QTD_PID_OUT				   0x00000000
#define QTD_PID_IN			       0x00000100
#define QTD_PID_SETUP		       0x00000200
#define	QTD_STS_ACTIVE		       ( 1 << 7 )									/* HC may execute this */
#define	QTD_STS_HALT		       ( 1 << 6 )									/* halted on error */
#define	QTD_STS_DBE			       ( 1 << 5 )									/* data buffer error (in HC) */
#define	QTD_STS_BABBLE			   ( 1 << 4 )									/* device was babbling (qtd halted) */
#define	QTD_STS_XACT			   ( 1 << 3 )									/* device gave illegal response */
#define	QTD_STS_MMF			       ( 1 << 2 )									/* incomplete split transaction */
#define	QTD_STS_STS			       ( 1 << 1 )									/* split transaction state */
#define	QTD_STS_PING		       ( 1 << 0 )									/* issue PING? */

	UINT32 hw_buf[ 5 ];        													/* see EHCI 3.5.4 */	
}EHCI_QTD, *PEHCI_QTD;
#endif

/***************************************************************************
 * EHCI Specification Section 3.3
 * Fig 3-4 "Isochronous Transaction Descriptor (iTD)"
 * Schedule records for high speed iso xfers
****************************************************************************/
#ifndef EHCI_ITD
typedef struct _EHCI_ITD
{
	UINT32 hw_next;           													/* see EHCI 3.3.1 */

	UINT32 hw_transaction[ 8 ]; 												/* see EHCI 3.3.2 */
#define ITD_ISOC_ACTIVE    		   ( 1 << 31 )        							/* activate transfer this slot */
#define ITD_ISOC_BUF_ERR   		   ( 1 << 30 )        							/* Data buffer error */
#define ITD_ISOC_BABBLE    		   ( 1 << 29 )        							/* babble detected */
#define ITD_ISOC_XACTERR           ( 1 << 28 )        							/* XactErr - transaction error */
#define	ITD_LENGTH( c )            ( ( c >> 16 ) & 0x0fff )
#define	ITD_IOC					   ( 1 << 15 )									/* interrupt on complete */

	UINT32 hw_buf[ 7 ];															/* see EHCI 3.3.3 */
}EHCI_ITD, *PEHCI_ITD;
#endif

/***************************************************************************
 * EHCI Specification Section 3.4
 * siTD, aka split-transaction isochronous Transfer Descriptor
 *       ... describe full speed iso xfers through TT in hubs
 * see Figure 3-5 "Split-transaction Isochronous Transaction Descriptor (siTD)
****************************************************************************/
#ifndef EHCI_SITD
typedef struct _EHCI_SITD
{
	UINT32 hw_next;

	UINT32 hw_fullspeed_ep;														/* EHCI table 3-9 */

	UINT32 hw_uframe;															/* EHCI table 3-10 */

	UINT32 hw_results;															/* EHCI table 3-11 */
#define	SITD_IOC			       ( 1 << 31 )									/* interrupt on completion */
#define	SITD_PAGE				   ( 1 << 30 )									/* buffer 0/1 */
#define	SITD_LENGTH( c )		   ( 0x3FF & ( c >> 16 ) )
#define	SITD_STS_ACTIVE		       ( 1 << 7 )									/* HC may execute this */
#define	SITD_STS_ERR		       ( 1 << 6 )									/* error from TT */
#define	SITD_STS_DBE		       ( 1 << 5 )									/* data buffer error (in HC) */
#define	SITD_STS_BABBLE		       ( 1 << 4 )									/* device was babbling */
#define	SITD_STS_XACT		       ( 1 << 3 )									/* illegal IN response */
#define	SITD_STS_MMF		       ( 1 << 2 )									/* incomplete split transaction */
#define	SITD_STS_STS		       ( 1 << 1 )									/* split transaction state */

	UINT32 hw_buf[ 2 ];															/* EHCI table 3-12 */

	UINT32 hw_backpointer;														/* EHCI table 3-13 */
} EHCI_SITD, *PEHCI_SITD;
#endif

/***************************************************************************
 * EHCI Specification Section 3.7
 * Periodic Frame Span Traversal Node (FSTN)
 * Manages split interrupt transactions (using TT) that span frame boundaries
 * into uframes 0/1; see 4.12.2.2.  In those uframes, a "save place" FSTN
 * makes the HC jump (back) to a QH to scan for fs/ls QH completions until
 * it hits a "restore" FSTN; then it returns to finish other uframe 0/1 work.
****************************************************************************/  
#ifndef EHCI_FSTN
typedef struct _EHCI_FSTN
{
	UINT32 hw_next;																/* any periodic q entry */

	UINT32 hw_prev;																/* qh or EHCI_LIST_END */
}EHCI_FSTN, *PEHCI_FSTN;
#endif


#ifdef __cplusplus
}
#endif

#endif
