/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbhs_host.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/23
* Description        : USBHS host initialization operation
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "ch32h417_usbhs_host.h"
#include "ch32h417_enum.h"
#include "ch32h417_rcc.h"
__attribute__((aligned(4))) uint8_t  RxBuffer[ 64 ];     // IN, must even address
__attribute__((aligned(4))) uint8_t  TxBuffer[ 64 ];     // OUT, must even address

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes USB high-speed rcc.
 *
 * @param   sta - ENABLE: Open the USBHS PLL 
 *                DISABLE: Turn off the USBHS PLL 
 *
 * @return  none
 */
void USBHS_RCC_Init(FunctionalState sta)
{
    if (sta)
    {
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            /* Initialize USBHS 480M PLL */
            RCC_USBHS_PLLCmd(DISABLE);
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLSource_HSE);
            RCC_USBHSPLLReferConfig(RCC_USBHSPLLRefer_25M);
            RCC_USBHSPLLClockSourceDivConfig(RCC_USBHSPLL_IN_Div1);
            RCC_USBHS_PLLCmd(ENABLE);
            while(!(RCC->CTLR & (uint32_t)RCC_USBHS_PLLRDY));
        }
        /* Enable UTMI Clock */
        RCC_UTMIcmd(ENABLE);
        /* Enable USBHS Clock */
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, ENABLE);
    }
    else
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, DISABLE);
        RCC_UTMIcmd(DISABLE);
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            RCC_USBHS_PLLCmd(DISABLE);
        }
    }
}

/*********************************************************************
 * @fn      USBHS_Host_Init
 *
 * @brief   USB host mode initialized.
 *
 * @param   sta - ENABLE or DISABLE
 *
 * @return  none
 */
void USBHS_Host_Init( FunctionalState sta )
{   
    if( sta )
    {
        USBHS_RCC_Init(ENABLE);
        USBHSH->CFG = USBHS_RST_LINK | USBHS_UH_PHY_SUSPENDM;
        USBHSH->TX_DMA = (uint32_t)&TxBuffer;
        USBHSH->RX_DMA = (uint32_t)&RxBuffer;
        USBHSH->RX_MAX_LEN  = 512;
        USBHSH->PORT_CFG = USBHS_UH_PD_EN | USBHS_UH_HOST_EN;
        USBHSH->FRAME |= USBHS_UH_SOF_CNT_EN;
        USBHSH->CFG = USBHS_UH_SOF_EN | USBHS_UD_DMA_EN | USBHS_UD_PHY_SUSPENDM ;   
    }
    else
    {
        USBHSH->CFG = USBHS_UD_RST_LINK | USBHS_UD_RST_SIE | USBHS_UD_CLR_ALL;
        USBHS_RCC_Init(DISABLE);
    }
}

/*********************************************************************
 * @fn      USBHSH_CheckRootHubPortStatus
 *
 * @brief   USBHSH Chec kRoot Hub_Port Status.
 *
 * @return  none
 */
void USBHSH_CheckRootHubPortStatus( void )
{
    /* Detect USB devices plugged or unplugged */
    if( USBHSH->PORT_STATUS_CHG & USBHS_UHIF_PORT_CONNECT )
    {
        USBHSH->PORT_STATUS_CHG = USBHS_UHIF_PORT_CONNECT;

        if( USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT )                         // Detect that the USB device has been connected to the port
        {
			gDeviceConnectstatus = USB_INT_CONNECT;
        }
        else
        {
			gDeviceConnectstatus = USB_INT_DISCONNECT;
        }
    }
    else{

    }
}
