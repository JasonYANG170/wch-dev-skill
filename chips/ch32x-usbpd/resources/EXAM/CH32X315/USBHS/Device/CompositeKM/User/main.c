/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/01/19
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

 /*
 * @Note
 * Composite Keyboard and Mouse Example:
 * This example uses PB11-PB14 and PA4-PA7 to simulate keyboard key pressing and mouse
 * movement respectively, active low.
 * At the same time, it also uses USART4 to receive the specified data sent from
 * the host to simulate the pressing and releasing of the following specific keyboard
 * keys. Data is sent in hexadecimal format and 1 byte at a time.
 * 'W' -> 0x1A
 * 'A' -> 0x04
 * 'S' -> 0x16
 * 'D' -> 0x07
 */

#include "debug.h"
#include "ch32x315_usbhs_device.h"
#include "usbd_composite_km.h"

uint8_t  KB_Data_Pack1[ 8 ] = { 0x00 };   
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
    printf( "USBHS Composite KM Device Test\r\n" );

	/* Initialize USART4 for receiving the specified keyboard data */
	USART4_Init( 115200 );
	printf( "USART4 Init OK!\r\n" );

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

    USBHS_Device_Init(ENABLE);
	USB_Sleep_Wakeup_CFG( );

    while(1)
    {
	    if( USBHS_DevEnumStatus )
	    {
	        /* Handle keyboard scan data */
	        KB_Scan_Handle(  );

	        /* Handle keyboard lighting */
	        KB_LED_Handle( );

            /* Handle mouse scan data */
            MS_Scan_Handle( );

            /* Handle USART4 receiving data */
            USART4_Receive_Handle( );
	    }
    }

}
