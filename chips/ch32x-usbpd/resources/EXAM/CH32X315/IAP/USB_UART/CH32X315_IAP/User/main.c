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

extern vu8 End_Flag;

#define UPGRADE_MODE_COMMAND   0
#define UPGRADE_MODE_IO        1

#define UPGRADE_MODE   UPGRADE_MODE_COMMAND
/*********************************************************************
 * @fn      IAP_2_APP
 *
 * @brief   IAP_2_APP program.
 *
 * @return  none
 */
void IAP_2_APP(void)
{ 
    USB_Init(DISABLE);
    Delay_Ms(50);
    printf("jump APP\r\n");
    Delay_Ms(50);
    GPIO_DeInit(GPIOA);
    USART_DeInit(USART2);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, DISABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_USART2,DISABLE);

    Delay_Ms(10);
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
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
	printf("IAP\r\n");

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
    if(*(uint32_t*)FLASH_Base  != 0xe339e339 )
    {
        if(*(uint32_t*)CalAddr != CheckNum)
        {
            IAP_2_APP();
            while(1);
        }
    }
#elif UPGRADE_MODE == UPGRADE_MODE_IO
    if(PA0_Check() == 0)
    {
        IAP_2_APP();
        while(1);
    }
#endif

    USART2_CFG(460800);
    /* USB20 device init */
    USB_Init(ENABLE);

	while(1)
	{
        if( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET){

            UART_Rx_Deal();
        }
        IWDG_ReloadCounter();

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
        if (End_Flag)
         {
            IAP_2_APP();
            while(1);
         }
#endif
	}
}
