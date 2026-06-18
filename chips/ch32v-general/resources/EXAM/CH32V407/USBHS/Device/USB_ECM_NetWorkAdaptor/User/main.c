/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/02/03
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 *General:
 *  This code emulates a CDC-ECM compliant 100M Ethernet adapter for linux, android or unix-like systems, Using the CH32V4x7 series
 *  monolithic i.e. built-in USBHS controller and 100M MAC. The Ethernet MAC of CH32V4x7 series monolithic i.e. has
 *  built-in 100Megabit MAC+PHY, 100M MAC.
 *USB Features:
 *  1,Update ETH status via end-point 1 every 200ms.
 *  2,Uploading data received from the MAC via endpoint 2 in CDC-ECM format.
 *  3,Receive Ethernet packets in CDC-ECM format via endpoint 3 and send them via MAC.
 *  4,Supported ECM type commands: SetEthernetPacketFilter only.
 *ETH Features:
 *  1,Ethernet MAC address obtained from chip unique code.
 *  2,Using built-in 100M MAC.
 */

/*What changed: 
 * 1,Update eth-drivers, use the same way as Ethernet Evts.
 * 2,Adjust the writing of other code according to the new stye of eth-drivers;
 * */

#include "cdc_ecm.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    uint8_t i;

    SystemCoreClockUpdate( );
    Delay_Init( );
    USART_Printf_Init( 921600 );
    
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USB Simulated NetWork Adaptor, Based on CDC-ECM\r\n" );

    /* MAC&Phy Initialize */
    ETH_GetMacAddr( MACAddr );
        MACAddr[0] = 0x50;
    MACAddr[1] = 0x54;
    MACAddr[2] = 0x7B;
    MACAddr[3] = 0x43;
    MACAddr[4] = 0x84;
    MACAddr[5] = 0x8A;
    printf( "MAC address:" );
    for( i=0; i<6; i++ )
    {
        printf( "%x ",MACAddr[ i ] );
    }
    printf("\r\n");
    /* change mac-addr to ecm/ncm format */
    MACAddr_Change_To_SNDesc( MACAddr );

    USBHS_Device_Init( ENABLE );

    while(1)
    {
        USBETH_Main( );
    }
}

