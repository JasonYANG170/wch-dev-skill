/********************************** (C) COPYRIGHT *******************************
* File Name          : ethernet.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/01/19
* Description        : Ethernet driver for ch32v307.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "ch32v4x7.h"
#include "debug.h"

#include "board.h"
#include "ethernet.h"
#include "eth_driver.h"

#include "class/cdc/cdcd.h"

/* @global */
uint8_t MACAddr[6];

/**
 * @brief Ethernet driver initialization.
 *
 * @return usb_rst_e Execution result.
 */
usb_rst_e ethernet_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);

    board_get_mac(MACAddr);
    ETH_Init(MACAddr);

    return USB_RST_OK;
}

/**
 * @brief Ethernet main task.
 *
 */
void ethernet_task(void)
{
    void WCHNET_QueryPhySta(void);

    WCHNET_QueryPhySta();
}

/**
 * @brief Get MAC address from ROM configuration area.
 *
 * @param mac Pointer to store the MAC address.
 */
void ethernet_get_mac(uint8_t *mac)
{
    memcpy(mac, MACAddr, 6);
}

/**
 * @brief Set ethernet MAC filter.
 *
 * @param filter Filter value.
 */
void ethernet_mac_filter(uint8_t filter)
{
    /* Promiscuous Mode */
    // if (filter & CDC_ECM_PACKET_FILTER_PROMISCUOUS)
    // {
    //     ETH->MACFFR |= ETH_PromiscuousMode_Enable;
    //     ETH->MACFFR |= ETH_ReceiveAll_Enable;
    // }
    // else
    // {
    //     ETH->MACFFR &= ~ETH_PromiscuousMode_Enable;
    //     ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    // }
    // /* All Multicast Mode */
    // if (filter & CDC_ECM_PACKET_FILTER_ALL_MULTICAST)
    // {
    //     ETH->MACFFR |= ETH_MulticastFramesFilter_None;
    // }
    // else
    // {
    //     ETH->MACFFR &= ~ETH_MulticastFramesFilter_None;
    // }
    // /* Direct mode(Unicast) */
    // if (filter & CDC_ECM_PACKET_FILTER_DIRECTED)
    // {
    //     ETH->MACFFR &= ~ETH_UnicastFramesFilter_HashTable;
    //     ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    // }
    // /* Multicast Mode */
    // if (filter & CDC_ECM_PACKET_FILTER_MULTICAST)
    // {
    //     ETH->MACFFR &= ~ETH_MulticastFramesFilter_HashTable;
    //     ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    // }
    // /* BroadCast Mode */
    // if ((filter & CDC_ECM_PACKET_FILTER_BROADCAST) == RESET)
    // {
    //     ETH->MACFFR |= ETH_BroadcastFramesReception_Disable;
    //     ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    // }
    // else
    // {
    //     ETH->MACFFR &= ~ETH_BroadcastFramesReception_Disable;
    //     ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    // }
}

/**
 * @brief Check ethernet is linked.
 *
 * @return usb_bool_t Link status.
 */
usb_bool_t ethernet_is_linked(void)
{
    extern uint8_t volatile LinkSta;
    return LinkSta ? USB_TRUE : USB_FALSE;
}

/**
 * @brief Get ethernet link speed.
 *
 * @return uint32_t Link speed in bps.
 */
uint32_t ethernet_get_link_speed(void)
{
    extern uint8_t volatile LinkSta;
    return 100000000;
}

/**
 * @brief Send ethernet frame.
 *
 * @param buf Pointer to the data buffer.
 * @param size Size of the data to send.
 * @return usb_rst_e Execution result.
 */
usb_rst_e ethernet_send(void *buf, size_t size)
{
    return MACRAW_Tx(buf, size) == ETH_SUCCESS ? USB_RST_OK : USB_RST_FAILED;
}

/**
 * @brief Receive ethernet frame.
 *
 * @param buf Pointer to the data buffer.
 * @param size Pointer to store the size of received data.
 * @return usb_rst_e Execution result.
 */
usb_rst_e ethernet_receive(void *buf, size_t *size)
{
    usb_rst_e rst = USB_RST_FAILED;
    extern ETH_DMADESCTypeDef *pDMARxSet;

    if ((pDMARxSet->Status & ETH_DMARxDesc_OWN) == 0)
    {
        if (!(pDMARxSet->Status & ETH_DMARxDesc_ES) && \
            (pDMARxSet->Status & ETH_DMARxDesc_LS) && \
            (pDMARxSet->Status & ETH_DMARxDesc_FS))
        {
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            size_t length = ((pDMARxSet->Status & ETH_DMARxDesc_FL) >> 16) - 4;

            memcpy(buf, (void *)pDMARxSet->Buffer1Addr, length);
            *size = length;

            rst = USB_RST_OK;
        }

        pDMARxSet->Status = ETH_DMARxDesc_OWN;
        pDMARxSet = (ETH_DMADESCTypeDef *)pDMARxSet->Buffer2NextDescAddr;
    }

    return rst;
}

/**
 * @brief Control ethernet flow.
 *
 */
void ethernet_flow_control(void)
{
    ETH->MACFCR |= ETH_MACFCR_FCBBPA;
}

/**
 * @brief Timer2 Interrupt Handler.
 *
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void TIM2_IRQHandler(void)
{
    WCHNET_TimeIsr(WCHNETTIMERPERIOD);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/**
 * @brief Ethernet Interrupt Handler.
 *
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void ETH_IRQHandler(void)
{
    WCHNET_ETHIsr();
}
