/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/19
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * @Note
 * Composite Keyboard and Mouse Example:
 * This example uses PA10-PA13 and PA4-PA7 to simulate keyboard key pressing and mouse
 * movement respectively, active low.
 */

/*******************************************************************************/
/* Header Files */
#include "ch32m030_usbfs_device.h"
#include "usbd_composite_km.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main( void )
{
    /* Initialize system configuration */
    Delay_Init( );
    USART_Printf_Init( 115200 );
    printf( "SystemClk:%d\r\n", SystemCoreClock );

    /* Initialize GPIO for keyboard scan */
    KB_Scan_Init( );
    KB_Sleep_Wakeup_Cfg( );
    printf( "KB Scan Init OK!\r\n" );

    /* Initialize GPIO for mouse scan */
    MS_Scan_Init( );
    MS_Sleep_Wakeup_Cfg( );
    printf( "MS Scan Init OK!\r\n" );

    /* Initialize timer for Keyboard and mouse scan timing */
    TIM3_Init( 1, SystemCoreClock / 10000 - 1 );
    printf( "TIM3 Init OK!\r\n" );

    /* Initialize USBFS interface to communicate with the host  */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );
    USB_Sleep_Wakeup_CFG( );

    printf("USBFS Composite KM Device Test\r\n");

    while( 1 )
    {
        if( USBFS_DevEnumStatus )
        {
            /* Handle keyboard scan data */
            KB_Scan_Handle(  );

            /* Handle keyboard lighting */
            KB_LED_Handle( );

            /* Handle mouse scan data */
            MS_Scan_Handle( );
        }
    }
}
