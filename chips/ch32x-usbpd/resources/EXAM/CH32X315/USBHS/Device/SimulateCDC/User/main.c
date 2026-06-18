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
 *@Note
  Example routine to emulate a simulate USB-CDC Device, USE USART2(PC12/PC13);
  Please note: This code uses the default serial port 1 for debugging,
  if you need to modify the debugging serial port, please do not use USART2

  If the USB is set to high-speed, an external crystal oscillator is recommended for the clock source.
*/

#include "debug.h"
#include "ch32x315_usbhs_device.h"
#include "UART.h"

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
    printf("SimulateCDC Running On USBHS Controller\n");

    RCC_Configuration( );

    /* Tim2 init */
    TIM2_Init();

    /* USART init */
    UART_Init(1, DEF_UARTx_BAUDRATE, DEF_UARTx_STOPBIT, DEF_UARTx_PARITY);
    
    USBHS_Device_Init(ENABLE);

    while(1)
    {
        UART_DataRx_Deal();
        UART_DataTx_Deal();
    }

}
