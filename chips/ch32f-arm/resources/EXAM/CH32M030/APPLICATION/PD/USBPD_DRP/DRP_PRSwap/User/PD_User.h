/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_User.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/10/01
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __PD_USER_H__
#define __PD_USER_H__

/*******************************************************************************/

#include "debug.h"
#include "ch32m030_pd.h"

/******************************************************************************/

#define Example_Type        Drp_Example_01  

#define Src_Example_01      0x01        /* OK */
#define Src_Example_02      0x02        /* OK */
#define Src_Example_03      0x03        /* OK */
#define Src_Example_04      0x04        /* OK */

#define Snk_Example_01      0x11        /* OK */        
#define Snk_Example_02      0x12        /* OK */
#define Snk_Example_03      0x13        /* OK */
#define Snk_Example_04      0x14        /* OK */
#define Snk_Example_05      0x15        /* OK */

#define Drp_Example_01      0x21        /* OK */
#define Drp_Example_02      0x22        /* OK */

/******************************************************************************/
#define DEF_USE_PD_POWER_DEDUCT		0
#define DEF_PD_POWER_DEDUCT			10000		//2000*15W
#define DEF_PD_CURR_LIMIT			50			//50*10 mA

/******************************************************************************/
extern u32 SrcCapCnt;
extern u32 SrcCap[7];
extern u32 tGetSrcCap,  fGetSrcCap;
extern u32 tGetVdmIdent, fGetVdmIdent;
extern u32 tGetNTDADC;

extern u32 DRSwapch, tDRSwap;
extern u32 PRSwapch, tPRSwap;
/******************************************************************************/
/* PD Voltage control type User processing */
void PD_User_ChangeVoltTypeSet(void);

/* PD connection detection user processing */
void PD_User_Dev_Attatched( u8 ch );
void PD_User_Dev_Unattatched( u8 ch );

/* Source user processing */
void PD_User_Src_Rx_Disc_Ident(void);
void PD_User_Emk_Rx_Disc_Ident(void);

void PD_User_Src_VoltChange(u8 v_change);
void PD_User_Src_Tx_PS_RDY(void);
void PD_User_Src_Rx_SinkCap(void);
void PD_User_Src_RX_DRS_Accept(void);

/* Sink user processing */
void PD_User_Snk_Rx_SrcCap(void);
void PD_User_Snk_Rx_EPR_SrcCap(void);
u32 PD_User_Snk_Tx_Request(void);
void PD_User_Snk_Rx_PS_RDY(void);

/* Timer 1ms user processing */
void PD_User_Timer(void);

#endif
