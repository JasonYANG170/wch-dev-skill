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
/* @Note
 * Compatibility HID Example:
 * This program provides examples of the pass-through of USB-HID data and serial port
 *  data based on compatibility HID device. And the data returned by Get_Report request is
 *  the data sent by the last Set_Report request.Speed of UART2 is 115200bps.
 *
 * Interrupt Transfers:
 *   UART4_RX   ---> Endpoint2
 *   Endpoint1  ---> UART2_TX
 *
 *   Note that the 1(FS) or 2(HS) bytes is the valid data length and the remaining bytes are
 *   the transmission data for interrupt Transfers.
 *
 * Control Transfers:
 *   Set_Report ---> Debug UART TX
 *   Get_Report <--- last Set_Report packet
 *

 * If the USB is set to high-speed, an external crystal oscillator is recommended for the clock source.
 *  */


#include "debug.h"
#include "ch32x315_usbhs_device.h"
#include "usbd_compatibility_hid.h"

/*********************************************************************
 * @fn      Var_Init
 *
 * @brief   Software parameter initialization
 *
 * @return  none
 */
void Var_Init(void)
{
    uint16_t i;
    RingBuffer_Comm.LoadPtr = 0;
    RingBuffer_Comm.StopFlag = 0;
    RingBuffer_Comm.DealPtr = 0;
    RingBuffer_Comm.RemainPack = 0;
    for(i=0; i<DEF_Ring_Buffer_Max_Blks; i++)
    {
        RingBuffer_Comm.PackLen[i] = 0;
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
    USART_Printf_Init(921600);
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("CompatibilityHID Running On USBHS Controller\n");
    /* Variables init */
    Var_Init( );

    TIM2_Init( );

    /* UART init */
    UART_Init( );
    UART_DMA_Init( );
    USBHS_Device_Init(ENABLE);

    while(1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if (USBHS_DevEnumStatus)
        {
            UART_Rx_Service();
            UART_Tx_Service();
            HID_Set_Report_Deal();
        }
    }

}
