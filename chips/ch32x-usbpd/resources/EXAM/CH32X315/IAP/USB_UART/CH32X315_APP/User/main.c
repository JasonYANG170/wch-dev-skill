/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * IAP routine: this routine support USB and UART mode,
 * and you can choose the command method or the IO method to jump to the APP .
 * Key  parameters: CalAddr - address in flash (same in APP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.60.
 *       For this chip has 2 USBHS, you can choose anyone you what by changing the definition "USBHS_CONTROLLER".
 *       DEF_USB_IAP_MODE - USB Vendor or USB HID mode.
 */

#include "debug.h"
#include "ch32x3x5_usbhs_device.h"
#include "ch32x3x5_gpio.h"
#include "iap.h"
#include "usb_inf.h"


#define UPGRADE_MODE_COMMAND   0
#define UPGRADE_MODE_IO        1

#define UPGRADE_MODE   UPGRADE_MODE_COMMAND
/*********************************************************************
 * @fn      APP_2_IAP
 *
 * @brief   APP_2_IAP program.
 *
 * @return  none
 */
void APP_2_IAP(void)
{
    NVIC_SystemReset();
    while(1){
    }
}
/*********************************************************************
 * @fn      USART1_IT_CFG
 *
 * @brief   USART1 IT configuration.
 *
 * @return  none
 */
void USART2_IT_CFG(void)
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_SetPriority(USART2_IRQn,0);
    NVIC_EnableIRQ(USART2_IRQn);
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      USART3_IRQHandler
 *
 * @brief   This function handles USART3 global interrupt request.
 *
 * @return  none
 */
void USART2_IRQHandler(void)
{
    if( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET){

        UART_Rx_Deal();
    }
}

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
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("In APP\r\n");

    USART2_CFG(460800);
    /* USB20 device init */
    USB_Init(ENABLE);
	USART2_IT_CFG();

	while(1)
	{
#if 0
        if( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
        {
            UART_Rx_Deal();
        }
#endif

        if(*(vu32*)CalAddr == CheckNum)
        {
             Delay_Ms(10);
             APP_2_IAP();
             while(1);
        }
	}
}
