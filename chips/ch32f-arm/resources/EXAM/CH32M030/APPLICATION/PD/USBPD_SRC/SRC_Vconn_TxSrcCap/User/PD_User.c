/********************************** (C) COPYRIGHT *******************************
 * File Name          : PD_User.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2025/12/06
 * Description        : PD user uses function
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "PD_User.h"
#include "libPD_CH32M030.h"

u32 SrcCap[] =
{
    SPR_Fixed_PDO (5, 3, 0x0A0),
};
u32 SrcCapCnt = 1;


/* SrcCap list */
u32 SrcCap_5V[1] =
{
    SPR_Fixed_PDO (5, 3, 0x0A0),
};

u32 SrcCap_5V_9V[2] =
{
    SPR_Fixed_PDO (5, 3, 0x0A0),
    SPR_Fixed_PDO (9, 3, 0),
};

u32 SrcCap_5_20V[5] =
{
    SPR_Fixed_PDO (5, 3, 0x0A0),
    SPR_Fixed_PDO (9, 3, 0),
    SPR_Fixed_PDO (12, 3, 0),
    SPR_Fixed_PDO (15, 3, 0),
    SPR_Fixed_PDO (20, 3, 0)
};

/*********************************************************************
 * @fn      USB_VID_Checks
 *
 * @brief   Check USB Vendor ID
 *
 * @param   VID - USB Vendor ID number
 *
 * @return  none
 */
void USB_VID_Checks (u16 VID)
{
    u8 vid_name[10];
    switch (VID) {
        case 0x1A86:
            memcpy (vid_name, "WCH\0", 3);
            break;
        case 0x2B01:
            memcpy (vid_name, "Zimi\0", 4);
            break;
        default:
            memcpy (vid_name, "Unknown\0", 7);
            break;
    }
    printf ("USB VID:0x%04x, %s\n", VID, vid_name);
}

/*********************************************************************
 * @fn      PD_User_ChangeVoltTypeSet
 *
 * @brief   Voltage control type selection
 *
 * @return  none
 */
void PD_User_ChangeVoltTypeSet(void)
{
    /* Voltage control type selection */
    PORT[PHYch].PHY.ChangeVoltType =  PD_VC_PDPassThrh;	
}

/*********************************************************************
 * @fn      PD_User_Dev_Attatched
 *
 * @brief   PD connect user process
 *
 * @param   ch - Port number
 *
 * @return  none
  */
void PD_User_Dev_Attatched( u8 ch )
{

    if ( ch ) 	/* Port1 */
	{
        if ( !PORT[ch].DEVICE.DevStat )
	{   // Src access, wait for SrcCap
			Port_Sel( ch );
			pProt_Wait_SrcCap();
			if ( PORT[1-ch].DEVICE.ConnectStat ) return;	/* Port0 is connected, no power on */
			C1_VBUS_EN;		
		} 
        else 
        {
			/* Only SNK characters in the routine */
		}
	} 
    else 
	    /* Port0 */   
		{
		 if ( !PORT[ch].DEVICE.DevStat )
		 {   // Src access, wait for SrcCap
				Port_Sel( ch );
			pProt_Wait_SrcCap();
			C0_VBUS_EN;
		} 
        else 
        //Sink access
        {
            (PORT[PHYch].PHY.ChangeVoltType == PD_VC_DCDCISink )?(C0_DCDC_OUT_VBUS_EN):(C0_VBUS_EN);
			Port_Sel( ch );
            pProt_TX_SOP1_REQ_IDENT();/* After device-side access, send Vconn message, judge Vconn content */
		}	
	}
}

/*********************************************************************
 * @fn      PD_User_Dev_Unattatched
 *
 * @brief   PD remove user processing
 *
 * @param   ch - Port number
 *
 * @return  none
 */
void PD_User_Dev_Unattatched( u8 ch )
{
 
}

/**
   _____    ____    ______
  / ___/   / __ \  / ____/
  \__ \   / /_/ / / /
 ___/ /  / _, _/ / /___
/____/  /_/ |_|  \____/
*/

/*********************************************************************
 * @fn      PD_User_Emk_Rx_Disc_Ident
 *
 * @brief   After receiving the VDM Disc Ident from Emk, the user processes
 *
 * @return  none
 */
void PD_User_Emk_Rx_Disc_Ident (void)
{
    u32 rxVDO[7];
    printf ("RX_SOP1_PD_VDM_DiscIdent!\n");
    printf ("----- Rx SOP' Disc Ident ------\n");
    /* Save the received IDHeaderVDO|CertStatVDO|ProductVDO|ProductTypeVDO*/
    memset (rxVDO, 0, sizeof (rxVDO));
    memcpy (rxVDO, &PD_RX_BUF[PHYch][3], (rxHeader[PHYch]->NDO - 1) * 4);

    for (u8 i = 0; i < rxHeader[PHYch]->NDO - 1; i++)
        printf ("[%d] 0x%08x ", i, rxVDO[i]);
    printf ("\n");

    // printf("USB VID:%04x\n", rxVDO[0]&0xFFFF);
    USB_VID_Checks (rxVDO[0] & 0xFFFF);

    PORT[PHYch].DEVICE.EmarkerMaxCurr = (rxVDO[rxHeader[PHYch]->NDO - 2] >> 5) & 0x3;
    printf ("Cable Support %dA!\n", 2 * PORT[PHYch].DEVICE.EmarkerMaxCurr + 1);

    printf ("-------------------------------\n");

    /* After VDO parsing, process Srccap according to emk: present 5A/present 3A/not present */
    /* Default 5V3A, get Emk information inside if support 5A then generate 5V 9V two gears Srcc */
              
    if(PORT[PHYch].DEVICE.EmarkerMaxCurr == 2)   /* 5A adds 9V3A PDO */
        SrcCapCnt = 2;

    Set_TXSrcCap(PHYch, SrcCap_5V_9V, SrcCapCnt);
}

/*********************************************************************
 * @fn      PD_User_Src_VoltChange
 *
 * @brief   Receive Request and then adjust pressure user handle
 *
 * @return  none
 */
void PD_User_Src_VoltChange (u8 v_change)
{

}

/*********************************************************************
 * @fn      PD_User_Src_Tx_PS_RDY
 *
 * @brief   Send PS Rdy then user handle
 *
 * @return  none
 */ 
void PD_User_Src_Tx_PS_RDY (void)
{

}

/*********************************************************************
 * @fn      PD_User_Src_Rx_SinkCap
 *
 * @brief   After receiving the SinkCap, the user processes
 *
 * @return  none
 */ 
void PD_User_Src_Rx_SinkCap (void)
{
    u32 rxSinkCap[7];
    printf ("----- Rx SinkCap ------\n");
    st_SrcCap_Fixed *pSrcCap;
    st_SrcCap_PPS *pSrcCap1;
    /* Save the received SinkCap */
    memset (rxSinkCap, 0, sizeof (rxSinkCap));
    memcpy (rxSinkCap, &PD_RX_BUF[PHYch][1], rxHeader[PHYch]->NDO * 4);
    for (u8 i = 0; i < rxHeader[PHYch]->NDO; i++) 
    {
        printf ("[%d] 0x%08x", i, rxSinkCap[i]);
        if ((rxSinkCap[i] >> 30) == 0x0)
        { 
            pSrcCap = (st_SrcCap_Fixed *)&rxSinkCap[i];
            printf (", %.2fV %.2fA\n", (double)pSrcCap->Voltage / 20, (double)pSrcCap->MaxCurrent / 100);
        } 
        else if ((rxSinkCap[i] >> 28) == 0xC)
        { /* APDO:PPS */
            pSrcCap1 = (st_SrcCap_PPS *)&rxSinkCap[i];
            printf (", %.2f-%.2fV %.2fA\n", (double)pSrcCap1->MinVoltPPS / 10, (double)pSrcCap1->MaxVoltPPS / 10, (double)pSrcCap1->MaxCurrentPPS / 20);
        } 
        else
        {
            printf ("\n");
        }
    }
    printf ("----------------------\n");
}

/*********************************************************************
 * @fn      PD_User_Src_RX_DRS_Accept
 *
 * @brief   SRC receive DRS accept
 *
 * @return  none
 */ 
void PD_User_Src_RX_DRS_Accept (void)
{

}


/**
   _____    _   __    __ __
  / ___/   / | / /   / //_/
  \__ \   /  |/ /   / ,<
 ___/ /  / /|  /   / /| |
/____/  /_/ |_/   /_/ |_|
*/

/*********************************************************************
 * @fn      PD_User_Snk_Rx_SrcCap
 *
 * @brief   User processing after receiving Srccap
 *
 * @return  none
 */ 
void PD_User_Snk_Rx_SrcCap (void)
{

    printf ("----- [PHY%d] Rx SrcCap ------\n", PHYch);

    /* Print received PDO */
    st_SrcCap_Fixed *pSrcCap;
    st_SrcCap_PPS *pSrcCap1;

    for (u8 i = 0; i < PORT[PHYch].PHY.rxSrcCapCnt; i++) {

        // printf("0x%x",PORT[PHYch].PHY.rxSrcCap[i]);

        if ((PORT[PHYch].PHY.rxSrcCap[i] >> 30) == 0x0) { /* Fixed */

            pSrcCap = (st_SrcCap_Fixed *)&PORT[PHYch].PHY.rxSrcCap[i];
            printf ("[%d] %.2fV %.2fA\n", i + 1, (double)pSrcCap->Voltage / 20, (double)pSrcCap->MaxCurrent / 100);
        } else if ((PORT[PHYch].PHY.rxSrcCap[i] >> 28) == 0xC) { /* APDO:PPS */

            pSrcCap1 = (st_SrcCap_PPS *)&PORT[PHYch].PHY.rxSrcCap[i];
            printf ("[%d] %.2f-%.2fV %.2fA\n", i + 1, (double)pSrcCap1->MinVoltPPS / 10, (double)pSrcCap1->MaxVoltPPS / 10, (double)pSrcCap1->MaxCurrentPPS / 20);
        }
    }

    /* Check if SrcCap supports EPR */
    pSrcCap = (st_SrcCap_Fixed *)&PORT[PHYch].PHY.rxSrcCap[0];

    if (pSrcCap->EPRMode)
        PORT[PHYch].PHY.fEPR_Support = 1;

    if (PORT[PHYch].PHY.fEPR_Support == 0)
        printf ("Not ");

    printf ("Support EPR.\n");
    printf ("----------------------\n");

}

/*********************************************************************
 * @fn      PD_User_Snk_Rx_SrcCap_DeductPower
 *
 * @brief   Receiving Srccap reserved power processing
 *
 * @return  none
 */ 
void PD_User_Snk_Rx_SrcCap_DeductPower (u8 EN_Deduct)
{

    st_SrcCap_Fixed *pSrcCap, *pSrcCap1;
    ;

    // Reserve SrcCap generation
    for (u8 i = 0; i < PORT[PHYch].PHY.rxSrcCapCnt; i++) {

        pSrcCap = (st_SrcCap_Fixed *)&PORT[PHYch].PHY.rxSrcCap[i];
        pSrcCap1 = (st_SrcCap_Fixed *)&PORT[PHYch].PHY.txSrcCap[i];

        if (!i) {

            pSrcCap1->USBComm = pSrcCap1->UnconstrainedPower = 1;
            pSrcCap1->DualRolePower = pSrcCap1->DualRoleData = 1;
            // if(EPR_State[PHYch].EPR_Support) pSrcCap1->EPRMode = 1;		//Saved -15w SrcCap with EPR
        }

        // (-15W to panel)
        pSrcCap1->Voltage = pSrcCap->Voltage;


        if (EN_Deduct) {

            // Reserve 15W
            if (pSrcCap->Voltage * pSrcCap->MaxCurrent > DEF_PD_POWER_DEDUCT) {
                pSrcCap1->MaxCurrent = (u32)(((pSrcCap->Voltage * pSrcCap->MaxCurrent) - DEF_PD_POWER_DEDUCT) / pSrcCap->Voltage);
                if (pSrcCap1->MaxCurrent < DEF_PD_CURR_LIMIT)
                    pSrcCap1->MaxCurrent = DEF_PD_CURR_LIMIT;  // Otherwise, keep 0.5A
            } else if (pSrcCap->MaxCurrent >= DEF_PD_CURR_LIMIT)
                pSrcCap1->MaxCurrent = DEF_PD_CURR_LIMIT;      // Otherwise, keep 0.5A
            else
                pSrcCap1->MaxCurrent = pSrcCap->MaxCurrent;
            // Below 0.5A retain the original value
            // else;
        } else  // No spare power
            pSrcCap1->MaxCurrent = pSrcCap->MaxCurrent;
    }
}

/*********************************************************************
 * @fn      PD_User_Snk_Tx_Request
 *
 * @brief   Send Request file position; user processing
 *
 * @return  none
 */ 
u32 PD_User_Snk_Tx_Request (void)
{
    u32 ObjPos = 0;

    if (PORT[1 - PHYch].PHY.VoltChanging)
        ObjPos = PORT[1 - PHYch].PHY.savedRequest.ObjectPos; /* There is a request for voltage adjustment on the other port, and the request for the other port's req request pos */
    else
        ObjPos = 1;

    return ObjPos;
}

/*********************************************************************
 * @fn      PD_User_Snk_Rx_PS_RDY
 *
 * @brief   User processing after receiving PS Rdy
 *
 * @return  none
 */ 
void PD_User_Snk_Rx_PS_RDY (void)
{

}

/*********************************************************************
 * @fn      PD_User_Snk_Rx_EPR_SrcCap
 *
 * @brief   User processing after receiving EPR SrcCap
 *
 * @return  none
 */ 
void PD_User_Snk_Rx_EPR_SrcCap (void)
{

    // printf("Rx EPR Done!\n");
}

/*********************************************************************
 * @fn      PD_User_Src_Rx_Disc_Ident
 *
 * @brief   After receiving VDM Disc Ident
 *
 * @return  none
 */ 
void PD_User_Src_Rx_Disc_Ident (void)
{

    u32 rxVDO[7];

    printf ("RX_SOP_PD_VDM_DiscIdent!\n");
    printf ("----- Rx SOP Disc Ident ------\n");
    /* Save the received IDHeaderVDO|CertStatVDO|ProductVDO|ProductTypeVDO */
    memset (rxVDO, 0, sizeof (rxVDO));
    memcpy (rxVDO, &PD_RX_BUF[PHYch][3], (rxHeader[PHYch]->NDO - 1) * 4);

    for (u8 i = 0; i < rxHeader[PHYch]->NDO - 1; i++)
        printf ("[%d] 0x%08x ", i, rxVDO[i]);
    printf ("\n");

    USB_VID_Checks (rxVDO[0] & 0xFFFF);
    printf ("------------------------------\n");
}


/**
  ______    ____    __  ___    ______    ____
 /_  __/   /  _/   /  |/  /   / ____/   / __ \
  / /      / /    / /|_/ /   / __/     / /_/ /
 / /     _/ /    / /  / /   / /___    / _, _/
/_/     /___/   /_/  /_/   /_____/   /_/ |_|
 */

 /*********************************************************************
 * @fn      PD_User_Timer
 *
 * @brief   User processing within 1ms interrupt
 *
 * @return  none
 */ 
void PD_User_Timer (void)
{

}
