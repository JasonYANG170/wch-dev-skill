/********************************** (C) COPYRIGHT *******************************
* File Name          : led.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Led driver for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "led.h"

/**
 * @brief Led driver initialization.
 *
 * @return usb_rst_e Execution result.
 */
usb_rst_e led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    return USB_RST_OK;
}

/**
 * @brief Led control.
 *
 * @param bit Led control bit map.
 */
void led_ctrl(uint32_t bit)
{
    (bit & (1 << 0)) ? GPIO_ResetBits(GPIOB, GPIO_Pin_0) : GPIO_SetBits(GPIOB, GPIO_Pin_0);
    (bit & (1 << 1)) ? GPIO_ResetBits(GPIOB, GPIO_Pin_1) : GPIO_SetBits(GPIOB, GPIO_Pin_1);
}
