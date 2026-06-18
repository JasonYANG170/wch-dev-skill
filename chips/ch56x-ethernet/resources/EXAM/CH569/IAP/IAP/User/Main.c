/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/07/31
 * Description 		 :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *GPIO routine
 *
 */

#include "CH56x_common.h"
#include "iap.h"
#include "CH56x_usb20.h"

#define  FREQ_SYS   80000000

extern uint8_t End_Flag;

void IAP_2_APP()
{
	printf( "Jump APP\r\n" );

	PFIC_EnableIRQ( SWI_IRQn );
	mDelaymS( 20 );
	PFIC_SetPendingIRQ( SWI_IRQn );
}

/*******************************************************************************
 * @fn       DebugInit
 *
 * @brief    Initializes the UART1 peripheral.
 *
 * @param    baudrate: UART1 communication baud rate.
 *
 * @return   None
 */
void DebugInit(UINT32 baudrate)
{
	UINT32 x;
	UINT32 t = FREQ_SYS;

	x = 10 * t * 2 / 16 / baudrate;
	x = ( x + 5 ) / 10;
	R8_UART1_DIV = 1;
	R16_UART1_DL = x;
	R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
	R8_UART1_LCR = RB_LCR_WORD_SZ;
	R8_UART1_IER = RB_IER_TXD_EN;
	R32_PA_SMT |= ( 1 << 8 ) | ( 1 << 7 );
	R32_PA_DIR |= ( 1 << 8 );
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main()
{
	SystemInit( FREQ_SYS );
	Delay_Init( FREQ_SYS );

	/* Configure serial debugging */
	DebugInit( 115200 );
	PRINT( "Start @ChipID=%02X\r\n" , R8_CHIP_ID );
	PRINT("IAP Program\r\n");

	if(R8_GLOB_RESET_KEEP == 0xaa){
		IAP_2_APP();
		while(1);
	}

	GPIOA_ModeCfg( GPIO_Pin_3 , GPIO_ModeIN_Floating );

	if (  GPIOA_ReadPortPin(GPIO_Pin_3) != RESET )
	{
		IAP_2_APP();
		while(1);
	}

	USART3_CFG( 57600 );

	USB20_Device_Init(ENABLE);
	PFIC_EnableIRQ(USBHS_IRQn);

	while(1)
	{

		if( (UART3_GetLinSTA() & RB_LSR_DATA_RDY) != RESET)
		{
			UART_Rx_Deal();
		}
		if (End_Flag)
		{
			PRINT( "Transmit complately\r\n");
			mDelaymS(10);
			R8_GLOB_RESET_KEEP = 0xaa;
			R8_SAFE_ACCESS_SIG = 0x57;
			R8_SAFE_ACCESS_SIG = 0xA8;
			R8_RST_WDOG_CTRL |= 0x41;

			while(1);
		}
	}
}

/*********************************************************************
 * @fn      SW_Handler
 *
 * @brief   This function handles Software exception.
 *
 * @return  none
 */
void SW_Handler(void) {
    __asm("li  a6, 0x4000");
    __asm("jr  a6");

    while(1);
}

