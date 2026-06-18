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
 *@Note
  Example routine to emulate a custom USB device (CH372 device).
  This routine demonstrates the use of a USBSS Device to emulate a custom device, the CH372,
  with endpoints 1/2 downlinking data and uploading via endpoints 2/1 respectively
  Endpoints 3 and 4 upload and download data separately respectively.
  The device can be operated using Bushund or other upper computer software.
  Note: This routine needs to be demonstrated in conjunction with the host software.

  If the USB is set to high-speed, an external crystal oscillator is recommended for the clock source.
*/

#include "debug.h"
#include "ch32x3x5_usbss_device.h"
#include "ch32x3x5_usbhs_device.h"

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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    printf("CH372Device Running On USBSS Controller\n");

    /* Disable SWD */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    USB_Timer_Init( );
    USBSS_Device_Init( ENABLE );

    while(1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if(USBHS_DevEnumStatus)
        {
            /* Data Transfer */
            if(RingBuffer_Comm.RemainPack)
            {
                if((USBHSD->UEP1_TX_CTRL & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_NAK)
                {
                    USBHSD->UEP1_TX_DMA = (uint32_t)&Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_HS_PACK_SIZE];
                    USBHSD->UEP1_TX_LEN = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];
                    USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL &= ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;

                    NVIC_DisableIRQ( USBHS_IRQn );
                    RingBuffer_Comm.RemainPack--;
                    RingBuffer_Comm.DealPtr++;
                    if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                    {
                        RingBuffer_Comm.DealPtr = 0;
                    }
                    NVIC_EnableIRQ( USBHS_IRQn );
                }
            }

            /* Monitor whether the remaining space is available for further downloads */
            if( RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART ))
            {
                if( RingBuffer_Comm.StopFlag )
                {
                    RingBuffer_Comm.StopFlag = 0;
                    USBHSD->UEP1_RX_CTRL = (USBHSD->UEP1_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                }
            }
        }
    }
}
