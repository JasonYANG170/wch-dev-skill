/********************************** (C) COPYRIGHT *******************************
* File Name          : board.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Board for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <stdio.h>
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "usbhs1_dcp.h"
#include "usbhs2_dcp.h"

#include "device/usbd_driver.h"

/* @define */
#define ROM_CFG_USERADR_ID              0x1FFFF7E8

/**
 * @brief Board initialization.
 *
 */
void board_init(void)
{
    SystemCoreClockUpdate();
    Delay_Init();

    USART_Printf_Init(115200);

    printf("======== Startup Information ========\r\n");
    printf("Compiled Time: %s %s\n", __DATE__, __TIME__);
    printf("RISC-V Compiler: %s\r\n", __VERSION__);
    printf("System Clock: %ld\r\n", SystemCoreClock);
    printf("=====================================\r\n\r\n");
}

/**
 * @brief Board delay in milliseconds.
 *
 * @param ms Milliseconds to delay.
 */
void board_delay_ms(uint32_t ms)
{
    Delay_Ms(ms);
}

/**
 * @brief Board get mac address.
 *
 * @param mac mac address buffer.
 */
void board_get_mac(uint8_t *mac)
{
    if (mac == NULL) return;

    uint8_t *mac_addr = (uint8_t *)(ROM_CFG_USERADR_ID + 5);

    for (uint8_t i = 0; i < 6; i++)
    {
        *mac++ = *mac_addr--;
    }
}

/**
 * @brief Board USB device initialization.
 *
 * @param index USB device index.
 * @param interrupt Interrupt enable flag.
 * @return usbd_handle_t* USB device handle.
 */
usbd_handle_t *board_usbd_init(uint8_t index, usb_bool_t interrupt)
{
    usb_rst_e rst;

    switch (index)
    {
    case 0:
        rst = usbhs1_dch_init(interrupt);
        if (rst == USB_RST_OK)
        {
            return &usbhs1d_handle;
        }
        break;

    case 1:
        rst = usbhs2_dch_init(interrupt);
        if (rst == USB_RST_OK)
        {
            return &usbhs2d_handle;
        }
        break;
    }

    return NULL;
}
