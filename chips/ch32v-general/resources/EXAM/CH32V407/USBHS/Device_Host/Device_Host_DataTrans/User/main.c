/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/04/21
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 * @Note
 *This example demonstrates the data transmission function between USB devices and USB hosts.
 *USBHS1 (2) acts as the host for enumerating the keyboard, while USBHS2 (1) simulates the keyboard 
 *connection to the PC. On the PC, a device with consistent keyboard information is enumerated. 
 *
 *1. Data transmission function: When USBHS1 (2) is inserted into the keyboard and USBHS2 (1) is connected to 
 *the PC, a device with the same information as the keyboard will be enumerated on the PC. At this time, the keyboard can be used normally.
 *2. Supports macro definition for switching between host and device interfaces (HS1 host, HS2 device or HS1 device, HS2 host)
 *3. Supports sleep-wakeup function (computer wakes up the keyboard and the keyboard wakes up the computer), the chip enters a low-power mode during sleep
 *4. In any interface of the PC, press the Win key, then press F9 key to enter the company's official website
 *5. After pressing F2 key, press the numeric key, at this time, two identical keys will be uploaded (for example: 0 -> 00).
 *After pressing F3 key, press the numeric key, at this time, three identical keys will be uploaded (for example: 0 -> 000). 
 *After pressing F4 key, the numeric keys will return to normal (this function needs to be demonstrated in a txt document software).
*/

#include "debug.h"
#include "usb_host_config.h"
#include "device_host_trans.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf( "USB HOST DEVICE Data Trans Test\r\n" );

    /* Initialize TIM3 */
    TIM3_Init( 9, SystemCoreClock / 10000 - 1 );
    printf( "TIM3 Init OK!\r\n" );

    /* Initialize USBHS host */
#if DEF_USBHS_PORT_EN
    printf( "USBHS Host Init\r\n" );
    USBHS_Host_Init( ENABLE );
    USB_Sleep_Wakeup_CFG();
    memset( &RootHubDev[ DEF_USBHS_PORT_INDEX ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USBHS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL ].InterfaceNum, 0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof( HOST_CTL ) );
#endif

    while(1)
    {
        USBH_MainDeal( );
    }
}
