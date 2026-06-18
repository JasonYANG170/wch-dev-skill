/********************************** (C) COPYRIGHT *******************************
* File Name          : button.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Button driver for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "button.h"

/**
 * @brief Button driver initialization.
 *
 * @return usb_rst_e Execution result.
 */
usb_rst_e btn_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    return USB_RST_OK;
}

/**
 * @brief Button scan.
 *
 * @return uint32_t Scan result bit map.
 */
uint32_t btn_scan(void)
{
    uint32_t temp = 0;

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET) temp |= (1 << 0);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET) temp |= (1 << 1);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == RESET) temp |= (1 << 2);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == RESET) temp |= (1 << 3);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == RESET) temp |= (1 << 4);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == RESET) temp |= (1 << 5);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == RESET) temp |= (1 << 6);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == RESET) temp |= (1 << 7);

    return temp;
}
