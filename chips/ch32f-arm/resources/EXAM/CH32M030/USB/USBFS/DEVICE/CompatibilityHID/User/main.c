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

/* @Note
 * Compatibility HID Example:
 * This program provides examples of the pass-through of USB-HID data and serial port
 *  data based on compatibility HID device. And the data returned by Get_Report request is
 *  the data sent by the last Set_Report request.Speed of UART1/2 is 115200bps.
 *
 * Interrupt/Control Transfers:
 *   UART1_RX   ---> Endpoint2
 *   Endpoint1  ---> UART1_TX
 *   Set_Report ---> UART1_TX
 *   Get_Report <--- last Set_Report packet
 *
 *   Note that the first byte is the valid data length and the remaining bytes are
 *   the transmission data for interrupt Transfers.
 *
 *  */

#include "debug.h"
#include "ch32m030_usbfs_device.h"
#include "usbd_compatibility_hid.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      Var_Init
 *
 * @brief   Software parameter initialisation
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

    /* Variables init */
    Var_Init();

    /* UART1 init */
    UART1_Init();
    UART1_DMA_Init();

    USBFS_RCC_Init();
    USBFS_Device_Init(ENABLE);

    /* Timer init */
    TIM2_Init();

    while(1)
    {
        if(USBFS_DevEnumStatus)
        {
            UART1_Rx_Service();
            UART1_Tx_Service();
            HID_Set_Report_Deal();
        }
    }
}
