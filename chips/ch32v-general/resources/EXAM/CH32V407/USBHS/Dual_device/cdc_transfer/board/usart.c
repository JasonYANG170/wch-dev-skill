/********************************** (C) COPYRIGHT *******************************
* File Name          : usart.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/12
* Description        : Usart driver for ch32v407.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "usart.h"

/* @define */
#define RX_BUFFER_SIZE      2048

/* @global */
static size_t _rx_dma_offset;
static uint8_t _rx_buffer[RX_BUFFER_SIZE];

/**
 * @brief Usart driver initialization.
 *
 * @return usb_rst_e Execution result.
 */
usb_rst_e usart_init(void)
{
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_USART3, ENABLE);
    USART_DeInit(USART3);

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(DMA1_Channel2);
    memset(&DMA_InitStructure, 0, sizeof(DMA_InitStructure));
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)NULL;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel2, ENABLE);

    DMA_DeInit(DMA1_Channel3);
    memset(&DMA_InitStructure, 0, sizeof(DMA_InitStructure));
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)_rx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = RX_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    return USB_RST_OK;
}

/**
 * @brief Usart driver configuration.
 *
 * @param line_coding Line coding parameters.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usart_config(cdc_line_coding_t *line_coding)
{
    if (line_coding == NULL)
    {
        return USB_RST_NULL_PTR;
    }

    if (line_coding->bit_rate == 0 || line_coding->bit_rate > 3000000)
    {
        return USB_RST_ERROR_PARAMETER;
    }

    USART_InitTypeDef USART_InitStructure = {0};

    USART_DeInit(USART3);
    memset(&USART_InitStructure, 0, sizeof(USART_InitStructure));
    USART_InitStructure.USART_BaudRate = line_coding->bit_rate;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    switch (line_coding->stop_bits)
    {
    case CDC_LINE_CODING_STOP_BITS_1:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;

    case CDC_LINE_CODING_STOP_BITS_1_5:
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
        break;

    case CDC_LINE_CODING_STOP_BITS_2:
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
        break;

    default:
        return USB_RST_ERROR_PARAMETER;
    }

    switch (line_coding->parity)
    {
    case CDC_LINE_CODING_PARITY_NONE:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;

    case CDC_LINE_CODING_PARITY_ODD:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;

    case CDC_LINE_CODING_PARITY_EVEN:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;

    default:
        return USB_RST_ERROR_PARAMETER;
    }

    USART_Init(USART3, &USART_InitStructure);
    USART_DMACmd(USART3, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    USART_Cmd(USART3, ENABLE);

    return USB_RST_OK;
}

/**
 * @brief Usart data send.
 *
 * @param buf Data buffer pointer.
 * @param size Data size.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usart_send(void *buf, size_t size)
{
    if (buf == NULL)
    {
        return USB_RST_NULL_PTR;
    }

    if (size == 0)
    {
        return USB_RST_OK;
    }

    if (DMA_GetCurrDataCounter(DMA1_Channel2) == 0)
    {
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA1_Channel2->MADDR = (uint32_t)buf;
        DMA1_Channel2->CNTR = size;
        DMA_Cmd(DMA1_Channel2, ENABLE);
        return USB_RST_OK;
    }
    else
    {
        return USB_RST_FAILED;
    }
}

/**
 * @brief Usart data receive.
 *
 * @param buf Data buffer pointer.
 * @param max_size Maximum data size to receive.
 * @return size_t Number of bytes received.
 */
size_t usart_receive(void *buf, size_t max_size)
{
    if (buf == NULL || max_size == 0)
    {
        return 0;
    }

    uint8_t *p = (uint8_t *)buf;
    size_t copy_len;
    size_t cur_pos = RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
    size_t old_pos = _rx_dma_offset;

    if (cur_pos == old_pos)
    {
        return 0;
    }

    if (cur_pos > old_pos)
    {
        size_t data_len = cur_pos - old_pos;
        copy_len = (data_len > max_size) ? max_size : data_len;
        memcpy(p, &_rx_buffer[old_pos], copy_len);
        old_pos += copy_len;
    }
    else
    {
        size_t data_len = (RX_BUFFER_SIZE - old_pos) + cur_pos;
        copy_len = (data_len > max_size) ? max_size : data_len;

        size_t first_part = RX_BUFFER_SIZE - old_pos;
        if (copy_len <= first_part)
        {
            memcpy(p, &_rx_buffer[old_pos], copy_len);
            old_pos += copy_len;
        }
        else
        {
            memcpy(p, &_rx_buffer[old_pos], first_part);
            memcpy((uint8_t *)p + first_part, &_rx_buffer[0], copy_len - first_part);
            old_pos = copy_len - first_part;
        }
    }

    if (old_pos >= RX_BUFFER_SIZE)
    {
        old_pos -= RX_BUFFER_SIZE;
    }

    _rx_dma_offset = old_pos;

    return copy_len;
}
