/********************************** (C) COPYRIGHT *******************************
 * File Name		  : libPD_CH32M030.h
 * Author			  : WCH
 * Version			  : V1.0.0
 * Date			      : 2025/10/09
 * Description		  : lib header.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/


 #ifndef USBPD_LIBPD_CH32M030_H_
#define USBPD_LIBPD_CH32M030_H_

/*******************************************************************************/
#include "debug.h"

/******************************************************************************/

#define PD0                     0
#define PD1                     1

#define Port0                   0
#define Port1                   1

#define C0_PIN_CC1              GPIO_Pin_0	
#define C0_PIN_CC2              GPIO_Pin_1

#define C1_PIN_CC1              GPIO_Pin_2	
#define C1_PIN_CC2              GPIO_Pin_3

#define C0_VBUS_EN              GPIO_ResetBits ( GPIOB, GPIO_Pin_9 )
#define C0_VBUS_DS              GPIO_SetBits ( GPIOB, GPIO_Pin_9 )

#define C1_VBUS_EN              GPIO_ResetBits ( GPIOB, GPIO_Pin_11 )
#define C1_VBUS_DS              GPIO_SetBits ( GPIOB, GPIO_Pin_11 )

#define C0_DCDC_IN_VBUS_EN    	GPIO_ResetBits ( GPIOB, GPIO_Pin_13 )     
#define C0_DCDC_IN_VBUS_DS  	GPIO_SetBits ( GPIOB, GPIO_Pin_13 )       

#define C0_DCDC_OUT_VBUS_EN    	GPIO_ResetBits ( GPIOB, GPIO_Pin_15 )     
#define C0_DCDC_OUT_VBUS_DS    	GPIO_SetBits ( GPIOB, GPIO_Pin_15 )  



/* ISRC definition */
#define Get_ISRC1_NTC_ADC 	Get_ADC_Val(ADC_Channel_5)
#define Get_ISRC2_NTC_ADC 	Get_ADC_Val(ADC_Channel_6)

#define 	printf(format, ...) 		printf(format, ##__VA_ARGS__)
// #define 	printf(format, ...)

#define SPR_Fixed_PDO(Volt, Curr, Others)       	(u32)( ( ((Volt==5)?(Others<<20):(0<<20)) | ((int)(Volt * 20)<<10) | (int)(Curr * 100) ) | (0x00000000) )

/******************************************************************************/
//PD message header
typedef struct {
	union {
		u16 Data;
		struct {
			u16 MsgType:5;
			u16 PortDataRole:1;
			u16 SpecRevision:2;
			u16 PortPwrRole:1;
			u16 MsgID:3;
			u16 NDO:3;
			u16 Extended:1;
		};
	};
} st_Prot_Header;

//PD Extend message header
typedef struct {
	union {
		u16 Data;
		struct {
			u16 DataSize:9;
			u16 Reserved:1;
			u16 RequestChunk:1;
			u16 ChunkNumber:4;
			u16 Chunked:1;
		};
	};
} st_Extended_Header;

typedef struct {
	union {
		u32 Data;
		struct {
			u32 MaxCurrent:10;
			u32 Voltage:10;
			u32 PeakCurrent:2;
			u32 Reserved:1;
			u32 EPRMode:1;
			u32 UnchunkedExtended:1;
			u32 DualRoleData:1;
			u32 USBComm:1;
			u32 UnconstrainedPower:1;
			u32 USBSuspend:1;
			u32 DualRolePower:1;
			u32 FixedSupply:2;
		};
	};
} st_SrcCap_Fixed;

//PPS
typedef struct {
    union {
        u32 Data;
        struct {
            u32 MaxCurrentPPS:7;
            u32 Reserved1:1;
            u32 MinVoltPPS:8;
            u32 Reserved2:1;
            u32 MaxVoltPPS:8;
            u32 Reserved3:2;
            u32 PowerLimited:1;
            u32 SPR_PPS:2;
            u32 FixedSupply:2;
        };
    };
} st_SrcCap_PPS;

//AVS
typedef struct {
    union {
        u32 Data;
        struct {
            u32 PDP:8;
            u32 MinVoltAVS:8;
            u32 Reserved4:1;
            u32 MaxVoltAVS:9;
            u32 PeakCurrenAVS:2;
            u32 AVS:2;
            u32 FixedSupply:2;
        };
    };
} st_SrcCap_AVS;

//PD RDO,Fixed Supply
typedef struct {
	union {
		u32 Data;
		struct {
			u32 MaxCurrent:10;
			u32 Current:10;
			u32 Reserved:2;
			u32 EPRMode:1;
			u32 UnchunkedExtended:1;
			u32 NoUSBSuspend:1;
			u32 USBComm:1;
			u32 CapbilityMismatch:1;
			u32 GiveBack:1;
			u32 ObjectPos:4;
		};
	};
} st_Request_Fixed;

typedef struct {
    union {
        u32 Data;
        struct {
            u32 Current:7;
            u32 Reserved:2;
            u32 Volt:12;
            u32 Reserved1:1;
            u32 EPRmode:1;
            u32 UnchunkedExtended:1;
            u32 NoUSBSuspend:1;
            u32 USBComm:1;
            u32 CapbilityMismatch:1;
            u32 Reserved2:1;
            u32 ObjectPos:4;
        };
    };
} st_Request_PPS;

//EPR
typedef struct {
    union {
        u32 Data;
        struct {
            u32 Reserved:16;
            u32 data:8;
            u32 Action:8;
        };

    };
} st_EPR_Mode;

//PD port hardware status structure, containing PHY and DEVICE
typedef struct {
	/* PD PHY struct */
	struct PHY {
				    //  SOP		SOP'	SOP"	HRST	CRST
        u32 RxSop;   //  01      10      11      10      11
        u32 TxSop;   //  0x00    0x50    0x44    0xFF    0x73
		
		/* BMC layer processing */
        u32 EmarkID;
        u32 TxMsgID;
        u32 RxMsgID;
        u32 RetryCnt;
        u32 PDExist;                // First packet message does not check MsgID
        u16 IdleCnt;                // PD Idle number

        u32 SrcCapCnt;
        st_Prot_Header Header;      // Current Header default value, including DRP status
		
		/* GCRC,MSG Counting processing */
        u32 WaitTxGcrc;
        u32 WaitRxGcrc;

        volatile u32 WaitMsgTx;     // Waiting for message to be sent;
        volatile u16 MsgTxCnt;      // Message sending timing ms

        volatile u32 WaitMsgRx;     // Waiting for message reception sign;
        volatile u16 MsgRxCnt;      // Message receive timeoutms

        volatile u32 Wait;
        volatile u16 WaitCnt;

		/* receive, timeout function pointer */
		void (*pRxFinish)();		//Received successfully: Message received and has been replied to GoodCRC
		void (*pRxTimeout)();		//Receive timeout: failed to receive a message within the specified time, without judging the type of message received
		void (*pTxFinish)();		//Sent successfully: The message has been successfully sent and has been received with GoodCRC
		void (*pTxTimeout)();		//Send timeout: failed to receive correct GoodCRC within the number of retries
		void (*pRxHRST)();			//Received HRST; and sent HRST

		/* Receive Srccap processing */
		u32 rxSrcCap[10];
        u32 rxEPRCap[14];
		u32 rxSrcCapCnt;
        u32 rxEPRCapCnt;
		u32 rxSrcCapFixedMaxPos;
		u32 savedRealObjpos;
		st_Request_Fixed savedRequest;

		/* Send Srccap processing */
		u32 txSrcCap[10];
        u32 txEPRCap[14];
		u32 txSrcCapCnt;
        u32 txEPRCapCnt;

		/* Pressure regulating treatment */
		volatile u16 isnkSaved;	//Real-time pressure adjustment value
		u8 ChangeVoltType;		//Voltage regulation type
		u8 VoltChanging;		//Voltage adjustment bit;
		u8 tSrcTransition;		//Waiting to start pressure regulation 	25-35
    	u32 vDCDC;				//DCDC Actual voltage

		/* VDM and PD establish processing */
		u32 VDMEstablished;
        u32 VDMEnterMode;

		u32 rxVDM[5];
	    u8  rxVDMCnt;

		/* EPR message flag */
		u32 fEPR_Support;       /* Is EPR supported? */
		u32 fFEPR_Loading;      /* EPR application is underway */   
		u32 fEPR_Enter;         /* Enter EPR mode */
		u32 fEPR_Exit;          /* Exit EPR mode */
		u32 fEPR_Fail;          /* Failed EPR mode */ 

		u32 actEPRMode;
		u32 expEPRMode;

		/* EPR Alive */
		u32 ExtCtrlType;		/* Send KeepAlive type */

		u16 tKeepAlive;         /* Send KeepAlive message count */
	} PHY;

	/* PD Device struct */
	struct DEVICE {

        void (*pDevChk)();
        void (*pUserUnattached)();      // Connect successfully call user program
        void (*pUserAttached)();        // Connect removal call user program

		volatile u32 Attaching;
		volatile u32 Unattaching;
		volatile u32 ConnectStat;

        u32 DevRole;                    // Sink   Src   DRP   DRP.TrySink   DRP.TrySrc
		u32 DevStat;		            //0:Sink   1:Src
	    u32 fEmarker;					//Read eMarker when VCONN is present
	    u32 EmarkerMaxCurr;				//VCONNE Mmk supports maximum current   1:3A	2:5A

		u16 Cnt;
		u16 TryCnt;
		u16 Timeout;

		uint8_t *PortCC;		        //Port control register of the used CC
		uint32_t GpioCC;		        //GPIO of the used CC

	} DEVICE;

} st_PD_Port;




/******************************************************************************/
extern  USBPD_TypeDef *USBPD[2];

extern st_Prot_Header *rxHeader[2];
extern st_Prot_Header *txHeader[2];
extern st_Prot_Header *gcHeader[2];

extern st_EPR_Mode *rxEPRMode[2];
extern st_Extended_Header *rxExtHeader[2];
extern st_Extended_Header *txExtHeader[2];

extern st_PD_Port PORT[2];
extern volatile u32 PHYch;

extern __attribute__ ((aligned(4))) uint16_t PD_RX_BUF[2][36];
extern __attribute__ ((aligned(4))) uint16_t PD_TX_BUF[2][36];
extern __attribute__ ((aligned(4))) uint16_t PD_GC_BUF0, PD_GC_BUF1;





/******************************************************************************/

void Set_DevChk(u8 ch, u8 role);

u16 Get_ADC_Val(u8 ch);
void Port_Sel(u8 ch);

void Prot_NULL(void);
void PD_RX_Reserved(void);
void PD_TX_NotSupported(void);
void PD_TX_Reject(void);
void pProt_TX_Wait(void);

void PD_TMR_Init(void);

void TMR_DRSwap(void);
void TMR_PRSwap(void);
void TMR_DCDCChange(void);
void TMR_KeepAlive(void);

void pDevice_Attached(u8 ch);
void pDevice_Unattached(u8 ch);
void pProt_IDLE(void);				

void pProt_TX_SOP1_REQ_IDENT(void);
void pProt_TX_SOP1_REQ_IDENT_OFF(void);

void pProt_TX_SrcCap(void);
void pProt_Wait_Request(void);
void pProt_RX_Request(void);
void pProt_TX_Accept(void);
void pVolt_DCDCChangeIsink(void);
void pProt_Set_Volt_Change(void);
void pProt_Volt_Change(void);
void pProt_EPR_Volt_Change(void);
void pProt_TX_PS_RDY(void);
void pProt_TX_PSR_Fnsh(void);

void Set_TXSrcCap( u8 ch, u32 *pSrcCap, u8 num );
void pProt_Wait_SrcCap(void);
void pProt_RX_SrcCap(void);
void pProt_TX_Request(void);
void pProt_TX_PPSRequest();
void pProt_RX_Accept(void);
void pProt_RX_PS_RDY(void);

void pProt_TX_PRSwap(void);
void pProt_RX_PRS_Accept(void);
void pProt_RX_PRS_PSR(void);
void pProt_RX_PRS_PSR_Fnsh(void);

void pProt_TX_DRSwap(void);
void pProt_RX_DRS_Accept(void);
void pProt_RX_DRSwap(void);
void pProt_RX_DRSwap_Fnsh(void);


void pProt_TX_GetSrcCap(void);
void pProt_TX_GetSinkCap(void);
void pProt_TX_SinkCap(void);
void pProt_RX_SinkCap(void);

void pProt_TX_SoftRst(void);
void pProt_SoftRst_RX_Accept(void);
void pProt_RX_SoftRst(void);
void pProt_Excute_SoftRst(void);

void pProt_TX_REQ_IDENT(void);

void pProt_TX_EPRMode(void);
void pProt_RX_EPRMode(void);
void pProt_RX_EPRCap(void);
void pProt_TX_EPRRequest(void);

void pProt_TX_ExtCtrl(void);
void pProt_RX_ExtCtrl(void);
void pProt_TX_KeepAlive(void);



#endif