/********************************** (C) COPYRIGHT *******************************
* File Name          : POWER.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563 USB相关寄存器定义
*******************************************************************************/



#ifndef __CH563USBSFR_H__
#define __CH563USBSFR_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* usb addresses                                                              */
#define USB_BASE_ADDR              (0x00404000)

/**********************************************************************************************************************************************/
/************************* Host Controller Registers(Address = 0x000---0x07F) **************************/

/******************************************************************************/
/* EHCI Capability Registers */

/* HC Capability Register(Address = 0x000) */
#define HC_CAPLENGTH			   (*((PUINT32V)(USB_BASE_ADDR + 0x000)))	    // RO(32h01000010), HC Capability Register
#define R8_HC_CAPLENGTH			   (*((PUINT8V) (USB_BASE_ADDR + 0x000)))	    // RO(8h10), Capability Register Length
#define R16_HC_HCIVERSION		   (*((PUINT16V)(USB_BASE_ADDR + 0x003)))           // RO(16h0100), Host Controller Version Number  

/* HC Structural Register(Address = 0x004) */
#define HC_HCSPARAMS			   (*((PUINT32V)(USB_BASE_ADDR + 0x004)))	    // RO(32h00000001), HC Structural Parameters

/* HC Capability Register(Address = 0x008) */
#define HC_HCCPARAMS			   (*((PUINT32V)(USB_BASE_ADDR + 0x008)))	    // RO(32h00000006), HC Capability Parameters
#define  RB_ASYN_SCH_PARK_CAP      (1 << 2)                      		   	 	// RO(1b1), Asynchronous Schedule Park Capability
#define  RB_PROG_FR_LIST_FLAG      (1 << 1)                      		   		// RO(1b1), Programmable Frame List Flag

/******************************************************************************/
/* EHCI Operational Registers */

/* HC USB Command Register(Address = 0x010) */
#define HC_USBCMD			       (*((PUINT32V)(USB_BASE_ADDR + 0x010)))	    // RW(32h00080b00), HC USB Command Register

/* HC USB Status Register(Address = 0x014) */
#define HC_USBSTS			       (*((PUINT32V)(USB_BASE_ADDR + 0x014)))	    // RW(32h00001000), HC USB Status Register

/* HC USB Interrupt Enable Register(Address = 0x018) */
#define HC_USBINT_EN			   (*((PUINT32V)(USB_BASE_ADDR + 0x018)))	    // RW(32h00000000), HC USB Interrupt Enable Register

/* HC Frame Index Register(Address = 0x01C) */
#define HC_FRINDEX			       (*((PUINT32V)(USB_BASE_ADDR + 0x01C)))	    // RW(32h00000000), HC Frame Index Register

/* HC Periodic Frame List Base Address Register(Address = 0x024) */
#define HC_PERIODICLISTBASE		   (*((PUINT32V)(USB_BASE_ADDR + 0x024)))	    // RW(32h00000000), HC Periodic Frame List Base Address Register

/* HC Current Asynchronous List Address Register(Address = 0x028) */
#define HC_ASYNCHRONOUS  		   (*((PUINT32V)(USB_BASE_ADDR + 0x028)))	    // RW(32h00000000), HC Current Asynchronous List Address Register

/* HC Port Status and Control Register(Address = 0x030) */
#define HC_PORTSC			       (*((PUINT32V)(USB_BASE_ADDR + 0x030)))	    // RW(32h00000000), HC Port Status and Control Register

/* HC Miscellaneous Register(Address = 0x040) */
#define HC_MISCELLANEOUS           (*((PUINT32V)(USB_BASE_ADDR + 0x040)))	    // RW(32h00000181), HC Miscellaneous Register

/* HC Full-Speed(FS) EOF1 Timing Point Register(Address = 0x044) */
#define HC_FS_EOF1_T_POINT    	   (*((PUINT32V)(USB_BASE_ADDR + 0x044)))	    // RW(32h00000000), HC Full-Speed(FS) EOF1 Timing Point Register

/* HC High-Speed(HS) EOF1 Timing Point Register(Address = 0x048) */
#define HC_HS_EOF1_T_POINT    	   (*((PUINT32V)(USB_BASE_ADDR + 0x048)))	    // RW(32h00000000), HC High-Speed(HS) EOF1 Timing Point Register

/**********************************************************************************************************************************************/
/**************************  On-The-Go Controller Registers(Address = 0x080---0x0BF) ************************* */

/* OTG Control Status Register(Address = 0x080) */
#define OTG_CONTROL_STATUS         (*((PUINT32V)(USB_BASE_ADDR + 0x080)))	    // RW(32h00310020), OTG Control Status Register

/* OTG Interrupt Status Register(Address = 0x084) */
#define OTG_INT_STATUS         	   (*((PUINT32V)(USB_BASE_ADDR + 0x084)))	    // RW(32h00000040), OTG Interrupt Status Register

/* OTG Interrupt Enable Register(Address = 0x088) */
#define OTG_INT_EBABLE         	   (*((PUINT32V)(USB_BASE_ADDR + 0x088)))	    // RW(32h00000000), OTG Interrupt Enable Register

/**********************************************************************************************************************************************/
/**************************  Global Controller Registers(Address = 0x0C0---0x0FF) ************************* */

/* Global HC/OTG/DEV Interrupt Status Register(Address = 0x0C0) */
#define USB_GL_INT_STATUS          (*((PUINT32V)(USB_BASE_ADDR + 0x0C0)))	    // RW(32h00000000), Global HC/OTG/DEV Interrupt Status Register
#define  RB_HC_INT    		  	   (1 << 2)                      		        // RWC(1b0), HC Interrupt
#define  RB_OTG_INT    		  	   (1 << 1)                      		   		// RWC(1b0), OTG Interrupt
#define  RB_DEV_INT    		  	   (1 << 0)                      		   		// RWC(1b0), Device Interrupt

/* Global Mask of HC/OTG/DEV Interrupt Register(Address = 0x0C4) */
#define USB_GL_MASK_INT            (*((PUINT32V)(USB_BASE_ADDR + 0x0C4)))	    // RW(32h00000000), Global Mask of HC/OTG/DEV Interrupt Register

/**********************************************************************************************************************************************/
/**************************  Device Controller Registers(Address = 0x100---0x1FF) ************************* */

/* Device Main Control Register(Address = 0x100) */
#define USB_DEV_CONTROL            (*((PUINT32V)(USB_BASE_ADDR + 0x100)))	    // RW(32h00000020), Device Main Control Register
#define  RB_FORCE_FS      		   (1 << 9)                      		   		// RW(1b0), Force the devcie to full_speed
#define  RB_HS_EN      		   	   (1 << 6)                      		   		// RO(1b0), High_Speed Status
#define  RB_CHIP_EN      		   (1 << 5)                      		   		// RW(1b1), Chip Enable
#define  RB_SFRST      		   	   (1 << 4)                      		   		// RW(1b0), Device Software Reset
#define  RB_GOSUSP      		   (1 << 3)                      		   		// RW(1b0), Go Suspend
#define  RB_GLINT_EN      		   (1 << 2)                      		   		// RW(1b0), Global Interrupt Enable
#define  RB_HALF_SPEED    		   (1 << 1)                      		   		// RW(1b0), Half Speed Enable
#define  RB_CAP_RMWAKUP    		   (1 << 0)                      		   		// RW(1b0), Capability of Remote Wakeup

/* Device Address Register(Address = 0x104) */
#define USB_DEV_ADDRESS            (*((PUINT32V)(USB_BASE_ADDR + 0x104)))	    // RW(32h00000000), Device Address Register
#define  RB_AFT_CONF    		   (1 << 7)                      		   		// RW(1b0), After Set Configuration

/* Device Test Register(Address = 0x108) */
#define USB_DEV_TEST          	   (*((PUINT32V)(USB_BASE_ADDR + 0x108)))	    // RW(32h00000000), Device Test Register
#define  RB_DISGENSOF    		   (1 << 6)                      		   		// RW(1b0), Disable the generation of SOF
#define  RB_TST_MOD    		   	   (1 << 5)                      		  	 	// RW(1b0), Test Mode
#define  RB_TST_DISTOG    		   (1 << 4)                      		   		// RW(1b0), Disable Toggle Sequence
#define  RB_TST_DISCRC    		   (1 << 3)                      		   		// RW(1b0), Disable CRC
#define  RB_TST_CLREA    		   (1 << 2)                      		   		// RW(1b0), Clear External Side Address
#define  RB_TST_LPCX    		   (1 << 1)                      		   		// RW(1b0), Loop-back Test for Control Endopint
#define  RB_TST_CLRFF    		   (1 << 0)                      		   		// RW(1b0), Clear FIFO

/* Device SOF Frame Number Register(Address = 0x10C) */
#define USB_DEV_SOF_NUM            (*((PUINT32V)(USB_BASE_ADDR + 0x10C)))	    // RO(32h00000000), Device SOF Frame Number Register

/* Device SOF Mask Timer Register(Address = 0x110) */
#define USB_DEV_SOF_MASKTIMER      (*((PUINT32V)(USB_BASE_ADDR + 0x110)))	    // RW(32h0000044C), Device SOF Mask Timer Register

/* PHY Test Mode Selector Register(Address = 0x114) */
#define USB_PHY_TEST_MODE          (*((PUINT32V)(USB_BASE_ADDR + 0x114)))	    // RW(32h00000000), PHY Test Mode Selector Register
#define  RB_TST_PKT    		   	   (1 << 4)                      		   		// RW(1b0), Test Mode for Packet
#define  RB_TST_SE0NAK    		   (1 << 3)                      		   		// RW(1b0), 
#define  RB_TST_KSTA    		   (1 << 2)                      		   		// RW(1b0), 
#define  RB_TST_JSTA    		   (1 << 1)                      		   		// RW(1b0), 
#define  RB_UNPLUG    		   	   (1 << 0)                      		   		// RW(1b0), 

/* Device Vendor-Specific I/O Control Register(Address = 0x118) */
#define USB_DEV_VCS_CONTROL        (*((PUINT32V)(USB_BASE_ADDR + 0x118)))	    // RW(32h00000020), Device Vendor-Specific I/O Control Register

/* Device CX Configuration Status Register(Address = 0x11C) */
#define USB_DEV_CX_CONF_STATUS     (*((PUINT32V)(USB_BASE_ADDR + 0x11C)))	   	// RO(), Device CX Configuration Status Register

/* Device CX Configuration and FIFO Empty Status Register(Address = 0x120) */
#define USB_DEV_CX_CONF_FIFO_STATUS (*((PUINT32V)(USB_BASE_ADDR + 0x120)))	    // RW(32h00000F20), Device CX Configuration and FIFO Empty Status Register
#define  RB_CX_EMP    		   	   (1 << 5)                      		   		// RO(1b1), CX FIFO is Empty
#define  RB_CX_FUL    		   	   (1 << 4)                      		   		// RO(1b0), CX FIFO is Full
#define  RB_CX_CLR    		   	   (1 << 3)                      		   		// RW(1b0), Clear CX FIFO Data
#define  RB_CX_STL    		   	   (1 << 2)                      		   		// RW(1b0), Stall CX
#define  RB_TST_PKDONE    		   (1 << 1)                      		   		// RW(1b0), Dtat Transfer is Done for Test Packet
#define  RB_DONE    		   	   (1 << 0)                      		   		// RW(1b0), Dtat Transfer is Done for CX

/* Device Idle Counter Register(Address = 0x124) */
#define USB_DEV_IDLE_COUNTER 	   (*((PUINT32V)(USB_BASE_ADDR + 0x124)))	    // RW(32h00000000), Device Idle Counter Register

/* Device Mask of Interrupt Grop Register(Address = 0x130) */
#define USB_DEV_MASK_INT_GROP 	   (*((PUINT32V)(USB_BASE_ADDR + 0x130)))	    // RW(32h00000000), Device Mask of Interrupt Grop Register
#define  RB_MINT_G2    		   	   (1 << 2)                      		   		// RW(1b0), Mask of interrupt of source group 2
#define  RB_MINT_G1    		   	   (1 << 1)                      		   		// RW(1b0), Mask of interrupt of source group 1
#define  RB_MINT_G0    		   	   (1 << 0)                      		   		// RW(1b0), Mask of interrupt of source group 0

/* Device Mask of Interrupt Source Grop 0 Register(Address = 0x134) */
#define USB_DEV_MASK_INT_GROP0 	   (*((PUINT32V)(USB_BASE_ADDR + 0x134)))	    // RW(32h00000000), Device Mask of Interrupt Source Grop 0 Register
#define  RB_MCX_COMABORT_INT       (1 << 5)                      		   		// RW(1b0), Mask interrupt of the control transfer command abort
#define  RB_MCX_COMFAIL_INT        (1 << 4)                      		   		// RW(1b0), Mask interrupt of the host emits extra IN or OUT data interrupt bit
#define  RB_MCX_COMEND             (1 << 3)                      		   		// RW(1b0), Mask the host end of a command(Entering the status stage) interrupt bit
#define  RB_MCX_OUT_INT            (1 << 2)                      		   		// RW(1b0), Mask the interrupt bits of endpoint 0 for OUT
#define  RB_MCX_IN_INT             (1 << 1)                      		   		// RW(1b0), Mask the interrupt bits of endpoint 0 for IN
#define  RB_MCX_SETUP_INT          (1 << 0)                      		   		// RW(1b0), Mask endpoint 0 setup data received interrupt bit

/* Device Mask of Interrupt Source Grop 1 Register(Address = 0x138) */
#define USB_DEV_MASK_INT_GROP1 	   (*((PUINT32V)(USB_BASE_ADDR + 0x138)))	    // RW(32h00000000), Device Mask of Interrupt Source Grop 1 Register
#define  RB_MF3_IN_INT             (1 << 19)                      		   		// RW(1b0), Mask the IN interrupt bits of FIFO 3
#define  RB_MF2_IN_INT             (1 << 18)                      		   		// RW(1b0), Mask the IN interrupt bits of FIFO 2
#define  RB_MF1_IN_INT             (1 << 17)                      		   		// RW(1b0), Mask the IN interrupt bits of FIFO 1
#define  RB_MF0_IN_INT             (1 << 16)                      		   		// RW(1b0), Mask the IN interrupt bits of FIFO 0
#define  RB_MF3_SPK_INT            (1 << 7)                      		   		// RW(1b0), Mask the Short Packet interrupt of FIFO 3
#define  RB_MF3_OUT_INT            (1 << 6)                      		   		// RW(1b0), Mask the OUT interrupt of FIFO 3
#define  RB_MF2_SPK_INT            (1 << 5)                      		   		// RW(1b0), Mask the Short Packet interrupt of FIFO 2
#define  RB_MF2_OUT_INT            (1 << 4)                      		   		// RW(1b0), Mask the OUT interrupt of FIFO 2
#define  RB_MF1_SPK_INT            (1 << 3)                      		   		// RW(1b0), Mask the Short Packet interrupt of FIFO 1
#define  RB_MF1_OUT_INT            (1 << 2)                      		   		// RW(1b0), Mask the OUT interrupt of FIFO 1
#define  RB_MF0_SPK_INT            (1 << 1)                      		   		// RW(1b0), Mask the Short Packet interrupt of FIFO 0
#define  RB_MF0_OUT_INT            (1 << 0)                      		   		// RW(1b0), Mask the OUT interrupt of FIFO 0

/* Device Mask of Interrupt Source Grop 2 Register(Address = 0x13C) */
#define USB_DEV_MASK_INT_GROP2 	   (*((PUINT32V)(USB_BASE_ADDR + 0x13C)))	    // RW(32h00000000), Device Mask of Interrupt Source Grop 2 Register
#define  RB_MDEV_WAKEUP_BYBUS      (1 << 10)                      		   		// RW(1b0), Mask Dev_Wakeup_byVBUS interrupt 
#define  RB_MDEV_IDLE      		   (1 << 9)                      		   		// RW(1b0), Mask Dev_Idle interrupt
#define  RB_MDMA_ERROR     		   (1 << 8)                      		   		// RW(1b0), Mask DMA Error interrupt
#define  RB_MDMA_CMPLT     		   (1 << 7)                      		   		// RW(1b0), Mask DMA Completion interrupt
#define  RB_MRX0BYTE_INT     	   (1 << 6)                      		   		// RW(1b0), Mask Received Zero-length Data Packet interrupt
#define  RB_MTX0BYTE_INT     	   (1 << 5)                      		   		// RW(1b0), Mask Transferred Zero-length Data Packet interrupt
#define  RB_MSEQ_ABORT_INT     	   (1 << 4)                      		   		// RW(1b0), Mask ISO Sequential Abort interrupt
#define  RB_MSEQ_ERR_INT     	   (1 << 3)                      		   		// RW(1b0), Mask ISO Sequential Error interrupt
#define  RB_MRESMQ_INT       	   (1 << 2)                      		   		// RW(1b0), Mask Resume interrupt
#define  RB_MSUSP_INT       	   (1 << 1)                      		   		// RW(1b0), Mask Suspend interrupt
#define  RB_MUSBRST_INT       	   (1 << 0)                      		   		// RW(1b0), Mask USB Reset interrupt
	
/* Device Interrupt Grop Register(Address = 0x140) */
#define USB_DEV_INT_GROP 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x140)))	    // RO(32h00000000), Device Interrupt Grop Register
#define  RB_INT_G2    		   	   (1 << 2)                      		   		// RO(1b0), interrupt of source group 2
#define  RB_INT_G1    		   	   (1 << 1)                      		   		// RO(1b0), interrupt of source group 1
#define  RB_INT_G0    		   	   (1 << 0)                      		   		// RO(1b0), interrupt of source group 0

/* Device Interrupt Source Grop 0 Register(Address = 0x144) */
#define USB_DEV_INT_GROP0 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x144)))	    // RW(32h00000000), Device Interrupt Source Grop0 Register
#define  RB_CX_COMABORT_INT       (1 << 5)                      		   		// RW(1b0), interrupt of the control transfer command abort
#define  RB_CX_COMFAIL_INT        (1 << 4)                      		   		// RW(1b0), interrupt of the host emits extra IN or OUT data interrupt bit
#define  RB_CX_COMEND             (1 << 3)                      		   		// RW(1b0), the host end of a command(Entering the status stage) interrupt bit
#define  RB_CX_OUT_INT            (1 << 2)                      		   		// RW(1b0), the interrupt bits of endpoint 0 for OUT
#define  RB_CX_IN_INT             (1 << 1)                      		   		// RW(1b0), the interrupt bits of endpoint 0 for IN
#define  RB_CX_SETUP_INT          (1 << 0)                      		   		// RW(1b0), endpoint 0 setup data received interrupt bit

/* Device Interrupt Source Grop 1 Register(Address = 0x148) */
#define USB_DEV_INT_GROP1 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x148)))	    // RO(32h00000000), Device Interrupt Source Grop1 Register
#define  RB_F3_IN_INT             (1 << 19)                      		   		// RW(1b0), the IN interrupt bits of FIFO 3
#define  RB_F2_IN_INT             (1 << 18)                      		   		// RW(1b0), the IN interrupt bits of FIFO 2
#define  RB_F1_IN_INT             (1 << 17)                      		   		// RW(1b0), the IN interrupt bits of FIFO 1
#define  RB_F0_IN_INT             (1 << 16)                      		   		// RW(1b0), the IN interrupt bits of FIFO 0
#define  RB_F3_SPK_INT            (1 << 7)                      		   		// RW(1b0), the Short Packet interrupt of FIFO 3
#define  RB_F3_OUT_INT            (1 << 6)                      		   		// RW(1b0), the OUT interrupt of FIFO 3
#define  RB_F2_SPK_INT            (1 << 5)                      		   		// RW(1b0), the Short Packet interrupt of FIFO 2
#define  RB_F2_OUT_INT            (1 << 4)                      		   		// RW(1b0), the OUT interrupt of FIFO 2
#define  RB_F1_SPK_INT            (1 << 3)                      		   		// RW(1b0), the Short Packet interrupt of FIFO 1
#define  RB_F1_OUT_INT            (1 << 2)                      		   		// RW(1b0), the OUT interrupt of FIFO 1
#define  RB_F0_SPK_INT            (1 << 1)                      		   		// RW(1b0), the Short Packet interrupt of FIFO 0
#define  RB_F0_OUT_INT            (1 << 0)                      		   		// RW(1b0), the OUT interrupt of FIFO 0

/* Device Interrupt Source Grop 2 Register(Address = 0x14C) */
#define USB_DEV_INT_GROP2 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x14C)))	    // RW(32h00000000), Device Interrupt Source Grop2 Register
#define  RB_DEV_WAKEUP_BYBUS       (1 << 10)                      		   		// RW(1b0), Dev_Wakeup_byVBUS interrupt 
#define  RB_DEV_IDLE      		   (1 << 9)                      		   		// RW(1b0), Dev_Idle interrupt
#define  RB_DMA_ERROR     		   (1 << 8)                      		   		// RW(1b0), DMA Error interrupt
#define  RB_DMA_CMPLT     		   (1 << 7)                      		   		// RW(1b0), DMA Completion interrupt
#define  RB_RX0BYTE_INT     	   (1 << 6)                      		   		// RW(1b0), Received Zero-length Data Packet interrupt
#define  RB_TX0BYTE_INT     	   (1 << 5)                      		   		// RW(1b0), Transferred Zero-length Data Packet interrupt
#define  RB_ISO_SEQ_ABORT_INT      (1 << 4)                      		   		// RW(1b0), ISO Sequential Abort interrupt
#define  RB_ISO_SEQ_ERR_INT        (1 << 3)                      		   		// RW(1b0), ISO Sequential Error interrupt
#define  RB_RESMQ_INT       	   (1 << 2)                      		   		// RW(1b0), Resume interrupt
#define  RB_SUSP_INT       	   	   (1 << 1)                      		   		// RW(1b0), Suspend interrupt
#define  RB_USBRST_INT       	   (1 << 0)                      		   		// RW(1b0), USB Reset interrupt

/* Device Receive Zero-Length Data Packet Register(Address = 0x150) */
#define USB_DEV_RECV_0_LEN_PACK    (*((PUINT32V)(USB_BASE_ADDR + 0x150)))	    // RW(32h00000000), Device Receive Zero-Length Data Packet Register
#define  RB_RX0BYTE_EP8       	   (1 << 7)                      		   		// RW(1b0), Endpoint 8 receives a zero-length data packet
#define  RB_RX0BYTE_EP7       	   (1 << 6)                      		   		// RW(1b0), Endpoint 7 receives a zero-length data packet
#define  RB_RX0BYTE_EP6       	   (1 << 5)                      		   		// RW(1b0), Endpoint 6 receives a zero-length data packet
#define  RB_RX0BYTE_EP5       	   (1 << 4)                      		   		// RW(1b0), Endpoint 5 receives a zero-length data packet
#define  RB_RX0BYTE_EP4       	   (1 << 3)                      		   		// RW(1b0), Endpoint 4 receives a zero-length data packet
#define  RB_RX0BYTE_EP3       	   (1 << 2)                      		   		// RW(1b0), Endpoint 3 receives a zero-length data packet
#define  RB_RX0BYTE_EP2       	   (1 << 1)                      		   		// RW(1b0), Endpoint 2 receives a zero-length data packet
#define  RB_RX0BYTE_EP1       	   (1 << 0)                      		   		// RW(1b0), Endpoint 1 receives a zero-length data packet

/* Device Transfer Zero-Length Data Packet Register(Address = 0x154) */
#define USB_DEV_SEND_0_LEN_PACK    (*((PUINT32V)(USB_BASE_ADDR + 0x154)))	    // RW(32h00000000), Device Transfer Zero-Length Data Packet Register
#define  RB_TX0BYTE_EP8       	   (1 << 7)                      		   		// RW(1b0), Endpoint 8 transfers a zero-length data packet
#define  RB_TX0BYTE_EP7       	   (1 << 6)                      		   		// RW(1b0), Endpoint 7 transfers a zero-length data packet
#define  RB_TX0BYTE_EP6       	   (1 << 5)                      		   		// RW(1b0), Endpoint 6 transfers a zero-length data packet
#define  RB_TX0BYTE_EP5       	   (1 << 4)                      		   		// RW(1b0), Endpoint 5 transfers a zero-length data packet
#define  RB_TX0BYTE_EP4       	   (1 << 3)                      		   		// RW(1b0), Endpoint 4 transfers a zero-length data packet
#define  RB_TX0BYTE_EP3       	   (1 << 2)                      		   		// RW(1b0), Endpoint 3 transfers a zero-length data packet
#define  RB_TX0BYTE_EP2       	   (1 << 1)                      		   		// RW(1b0), Endpoint 2 transfers a zero-length data packet
#define  RB_TX0BYTE_EP1       	   (1 << 0)                      		   		// RW(1b0), Endpoint 1 transfers a zero-length data packet

/* Device Isochronous Sequential Error/Abort Register(Address = 0x158) */
#define USB_DEV_ISO_ERR_ABORT      (*((PUINT32V)(USB_BASE_ADDR + 0x158)))	    // RW(32h00000000), Device Isochronous Sequential Error/Abort Register
#define  RB_ISO_SEQ_ERR_EP8        (1 << 23)                      		   		// RW(1b0), Endpoint 8 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP7        (1 << 22)                      		   		// RW(1b0), Endpoint 7 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP6        (1 << 21)                      		   		// RW(1b0), Endpoint 6 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP5        (1 << 20)                      		   		// RW(1b0), Endpoint 5 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP4        (1 << 19)                      		   		// RW(1b0), Endpoint 4 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP3        (1 << 18)                      		   		// RW(1b0), Endpoint 3 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP2        (1 << 17)                      		   		// RW(1b0), Endpoint 2 encounters an Isochronous Sequential Error
#define  RB_ISO_SEQ_ERR_EP1        (1 << 16)                      		   		// RW(1b0), Endpoint 1 encounters an Isochronous Sequential Error
#define  RB_ISO_ABT_ERR_EP8        (1 << 7)                      		   		// RW(1b0), Endpoint 8 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP7        (1 << 6)                      		   		// RW(1b0), Endpoint 7 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP6        (1 << 5)                      		   		// RW(1b0), Endpoint 6 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP5        (1 << 4)                      		   		// RW(1b0), Endpoint 5 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP4        (1 << 3)                      		   		// RW(1b0), Endpoint 4 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP3        (1 << 2)                      		   		// RW(1b0), Endpoint 3 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP2        (1 << 1)                      		   		// RW(1b0), Endpoint 2 encounters an Isochronous Sequential abort
#define  RB_ISO_ABT_ERR_EP1        (1 << 0)                      		   		// RW(1b0), Endpoint 1 encounters an Isochronous Sequential abort

/* Device IN Endpoint 1 MaxPackSize Register(Address = 0x160) */
#define USB_DEV_EP1_IN_MAXPKS      (*((PUINT32V)(USB_BASE_ADDR + 0x160)))	    // RW(32h00000200), Device IN Endpoint 1 MaxPackSize Register
#define  RB_TX0BYTE_IEPx           (1 << 15)                      		   		// RW(1b0), Transfer a Zero-length Data Packet from Endpoint 1 to USB Host
#define  RB_RSTG_IEPx              (1 << 12)                      		   		// RW(1b0), Reset Toggle Sequence for IN Endpoint 1
#define  RB_STL_IEPx               (1 << 11)                      		   		// RW(1b0), Stall IN Endpoint 1

/* Device IN Endpoint 2 MaxPackSize Register(Address = 0x164) */
#define USB_DEV_EP2_IN_MAXPKS      (*((PUINT32V)(USB_BASE_ADDR + 0x164)))	    // RW(32h00000200), Device IN Endpoint 2 MaxPackSize Register

/* Device IN Endpoint 3 MaxPackSize Register(Address = 0x168) */
#define USB_DEV_EP3_IN_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x168)))	    // RW(32h00000200), Device IN Endpoint 3 MaxPackSize Register

/* Device IN Endpoint 4 MaxPackSize Register(Address = 0x16C) */
#define USB_DEV_EP4_IN_MAXPKS      (*((PUINT32V)(USB_BASE_ADDR + 0x16C)))	    // RW(32h00000200), Device IN Endpoint 4 MaxPackSize Register

/* Device IN Endpoint 5 MaxPackSize Register(Address = 0x170) */
#define USB_DEV_EP5_IN_MAXPKS  	   (*((PUINT32V)(USB_BASE_ADDR + 0x170)))	    // RW(32h00000200), Device IN Endpoint 5 MaxPackSize Register

/* Device IN Endpoint 6 MaxPackSize Register(Address = 0x174) */
#define USB_DEV_EP6_IN_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x174)))	    // RW(32h00000200), Device IN Endpoint 6 MaxPackSize Register

/* Device IN Endpoint 7 MaxPackSize Register(Address = 0x178) */
#define USB_DEV_EP7_IN_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x178)))	    // RW(32h00000200), Device IN Endpoint 7 MaxPackSize Register

/* Device IN Endpoint 8 MaxPackSize Register(Address = 0x17C) */
#define USB_DEV_EP8_IN_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x17C)))	    // RW(32h00000200), Device IN Endpoint 8 MaxPackSize Register

/* Device OUT Endpoint 1 MaxPackSize Register(Address = 0x180) */
#define USB_DEV_EP1_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x180)))	    // RW(32h00000200), Device OUT Endpoint 1 MaxPackSize Register
#define  RB_RSTG_OEPx              (1 << 12)                      		   		// RW(1b0), Reset Toggle Sequence for OUT Endpoint 1
#define  RB_STL_OEPx               (1 << 11)                      		   		// RW(1b0), Stall OUT Endpoint 1

/* Device OUT Endpoint 2 MaxPackSize Register(Address = 0x184) */
#define USB_DEV_EP2_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x184)))	    // RW(32h00000200), Device OUT Endpoint 2 MaxPackSize Register

/* Device OUT Endpoint 3 MaxPackSize Register(Address = 0x188) */
#define USB_DEV_EP3_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x188)))	    // RW(32h00000200), Device OUT Endpoint 3 MaxPackSize Register

/* Device OUT Endpoint 4 MaxPackSize Register(Address = 0x18C) */
#define USB_DEV_EP4_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x18C)))	    // RW(32h00000200), Device OUT Endpoint 4 MaxPackSize Register

/* Device OUT Endpoint 5 MaxPackSize Register(Address = 0x190) */
#define USB_DEV_EP5_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x190)))	    // RW(32h00000200), Device OUT Endpoint 5 MaxPackSize Register

/* Device OUT Endpoint 6 MaxPackSize Register(Address = 0x194) */
#define USB_DEV_EP6_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x194)))	    // RW(32h00000200), Device OUT Endpoint 6 MaxPackSize Register

/* Device OUT Endpoint 7 MaxPackSize Register(Address = 0x198) */
#define USB_DEV_EP7_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x198)))	    // RW(32h00000200), Device OUT Endpoint 7 MaxPackSize Register

/* Device OUT Endpoint 8 MaxPackSize Register(Address = 0x19C) */
#define USB_DEV_EP8_OUT_MAXPKS 	   (*((PUINT32V)(USB_BASE_ADDR + 0x19C)))	    // RW(32h00000200), Device OUT Endpoint 8 MaxPackSize Register

/* Device Endpoint 1---4 Map Register(Address = 0x1A0) */
#define USB_DEV_EP1_4_MAP 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x1A0)))	    // RW(32h33333333), Device Endpoint 1---4 Map Register

/* Device Endpoint 5---8 Map Register(Address = 0x1A4) */
#define USB_DEV_EP5_8_MAP 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x1A4)))	    // RW(32h3FFFFFFF), Device Endpoint 5---8 Map Register

/* Device FIFO Map Register(Address = 0x1A8) */
#define USB_DEV_FIFO_MAP 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x1A8)))	    // RW(32h0F0F0F0F), Device FIFO Map Register

/* Device FIFO Configuration Register(Address = 0x1AC) */
#define USB_DEV_FIFO_CONFIG 	   (*((PUINT32V)(USB_BASE_ADDR + 0x1AC)))	    // RW(32h00000000), Device FIFO Configuration Register

/* Device FIFO 0 Instruction and Byte Count Register(Address = 0x1B0) */
#define USB_DEV_FIFO0_INFO 	   	   (*((PUINT32V)(USB_BASE_ADDR + 0x1B0)))	    // RW(32h00000000), Device FIFO 0 Instruction and Byte Count Register

/* Device FIFO 1 Instruction and Byte Count Register(Address = 0x1B4) */
#define USB_DEV_FIFO1_INFO 	       (*((PUINT32V)(USB_BASE_ADDR + 0x1B4)))	    // RW(32h00000000), Device FIFO 1 Instruction and Byte Count Register

/* Device FIFO 2 Instruction and Byte Count Register(Address = 0x1B8) */
#define USB_DEV_FIFO2_INFO 	       (*((PUINT32V)(USB_BASE_ADDR + 0x1B8)))	    // RW(32h00000000), Device FIFO 2 Instruction and Byte Count Register

/* Device FIFO 3 Instruction and Byte Count Register(Address = 0x1BC) */
#define USB_DEV_FIFO3_INFO 	       (*((PUINT32V)(USB_BASE_ADDR + 0x1BC)))	    // RW(32h00000000), Device FIFO 3 Instruction and Byte Count Register

/* Device DMA Target FIFO Number Register(Address = 0x1C0) */
#define USB_DEV_DMA_TARGET_FIFO    (*((PUINT32V)(USB_BASE_ADDR + 0x1C0)))	    // RW(32h00000000), Device DMA Target FIFO Number Register

/* Device DMA Controller Parameter Setting 1 Register(Address = 0x1C8) */
#define USB_DEV_DMA_SET1    	   (*((PUINT32V)(USB_BASE_ADDR + 0x1C8)))	    // RW(32h00000000), Device DMA Controller Parameter Setting 1 Register
#define  RB_DEVPHY_SUSPEND         (1 << 31)                      		   		// RW(1b0), Device Transceiver Suspend Mode
#define  RB_CLRFIFO_DMAABORT       (1 << 4)                      		   		// RW(1b0), Clear FIFO when DMA_ABORT
#define  RB_DMA_ABORT              (1 << 3)                      		   		// RW(1b0), DMA ABORT
#define  RB_DMA_IO                 (1 << 2)                      		   		// RW(1b0), DMA I/O to I/O
#define  RB_DMA_TYPE               (1 << 1)                      		   		// RW(1b0), DMA Type
#define  RB_DMA_START              (1 << 0)                      		   		// RW(1b0), DMA Start

/* Device DMA Controller Parameter Setting 2 Register(Address = 0x1CC) */
#define USB_DEV_DMA_SET2    	   (*((PUINT32V)(USB_BASE_ADDR + 0x1CC)))	    // RW(32h00000000), Device DMA Controller Parameter Setting 2 Register

/* Device DMA Controller Parameter Setting 3 Register(Address = 0x1D0) */
#define USB_DEV_DMA_SET3    	   (*((PUINT32V)(USB_BASE_ADDR + 0x1D0)))	    // RO(32h00000000), Device DMA Controller Parameter Setting 3 Register
#define USB_DEV_SETUP_CMD_RPORT    (*((PUINT32V)(USB_BASE_ADDR + 0x1D0)))	    // RO(32h00000000), Device DMA Controller Parameter Setting 3 Register


#ifdef __cplusplus
}
#endif

#endif

/* ********************************************************************************************************************* */
