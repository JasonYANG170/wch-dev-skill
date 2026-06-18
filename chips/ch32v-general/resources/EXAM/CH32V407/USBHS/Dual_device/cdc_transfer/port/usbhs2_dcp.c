/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs2_dcp.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/09
* Description        : Usb high speed device controller 2 port for ch32v407.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "debug.h"
#include "ch32v4x7.h"
#include "ch32v4x7_usb.h"

#include "usbhs2_dcp.h"

/* @define */
#define ENDP_MAX_LEN(ep)                *((volatile uint32_t *)&(USBHS2D->UEP0_MAX_LEN) + (ep))
#define ENDP_TX_LEN(ep)                 *((volatile uint16_t *)&(USBHS2D->UEP0_TX_LEN) + (ep) * 2)
#define ENDP_RX_LEN(ep)                 *((volatile uint16_t *)&(USBHS2D->UEP0_RX_LEN) + (ep) * 2)
#define ENDP_TX_CTRL(ep)                *((volatile uint8_t *)&(USBHS2D->UEP0_TX_CTRL) + (ep) * 4)
#define ENDP_RX_CTRL(ep)                *((volatile uint8_t *)&(USBHS2D->UEP0_RX_CTRL) + (ep) * 4)
#define ENDP_TX_DMA_ADDR(ep)            *((volatile uint32_t *)&(USBHS2D->UEP1_TX_DMA) + (ep - 1))
#define ENDP_RX_DMA_ADDR(ep)            *((volatile uint32_t *)&(USBHS2D->UEP1_RX_DMA) + (ep - 1))

/* @global */
usbd_handle_t usbhs2d_handle;
static usb_bool_t _interrupt = USB_FALSE;

/* @function declaration */
static usb_rst_e _enable(void);
static usb_rst_e _disable(void);
static usb_rst_e _open(void);
static usb_rst_e _close(void);
static usb_rst_e _get_event(usbd_event_t *e);
static usb_rst_e _clear_event(usbd_event_t *e);
static usb_rst_e _resume(void);
static usb_rst_e _set_address(uint8_t addr);
static usb_rst_e _interrupt_ctrl(usb_bool_t status);
static usb_rst_e _endp_open(usb_endp_t ep, endp_type_e type);
static usb_rst_e _endp_close(usb_endp_t ep);
static usb_rst_e _set_endp_status(usb_endp_t ep, endp_resp_e resp, endp_tog_e tog);
static usb_rst_e _set_endp_size(usb_endp_t ep, size_t size);
static usb_rst_e _set_endp_buf(usb_endp_t ep, const void *buf);
static usb_rst_e _set_endp_burst_size(usb_endp_t ep, size_t size);
static usb_rst_e _get_endp_status(usb_endp_t ep, endp_resp_e *resp, endp_tog_e *tog);
static usb_rst_e _get_endp_size(usb_endp_t ep, size_t *size);
static usb_rst_e _get_endp_buf(usb_endp_t ep, void **buf);
static usb_rst_e _get_endp_burst_size(usb_endp_t ep, size_t *size);

__attribute__((interrupt("WCH-Interrupt-fast"))) void USBHS2_IRQHandler(void)
{
    usbd_drv_task(&usbhs2d_handle);
}

usb_rst_e usbhs2_dch_init(usb_bool_t interrupt)
{
    _interrupt = interrupt;
    memset(&usbhs2d_handle, 0, sizeof(usbhs2d_handle));

    usbhs2d_handle.feature.burst_support = USB_TRUE;
    usbhs2d_handle.feature.ep0_size = USBHS2_DCP_ENDP0_SIZE;

    usbhs2d_handle.enable = _enable;
    usbhs2d_handle.disable = _disable;
    usbhs2d_handle.open = _open;
    usbhs2d_handle.close = _close;
    usbhs2d_handle.get_event = _get_event;
    usbhs2d_handle.clear_event = _clear_event;
    usbhs2d_handle.resume = _resume;
    usbhs2d_handle.set_address = _set_address;
    usbhs2d_handle.interrupt_ctrl = _interrupt_ctrl;
    usbhs2d_handle.endp_open = _endp_open;
    usbhs2d_handle.endp_close = _endp_close;
    usbhs2d_handle.set_endp_status = _set_endp_status;
    usbhs2d_handle.set_endp_size = _set_endp_size;
    usbhs2d_handle.set_endp_buf = _set_endp_buf;
    usbhs2d_handle.set_endp_burst_size = _set_endp_burst_size;
    usbhs2d_handle.get_endp_status = _get_endp_status;
    usbhs2d_handle.get_endp_size = _get_endp_size;
    usbhs2d_handle.get_endp_buf = _get_endp_buf;
    usbhs2d_handle.get_endp_burst_size = _get_endp_burst_size;

    return USB_RST_OK;
}

static usb_rst_e _enable(void)
{
    if ((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, DISABLE);
        RCC->CTLR &= ~RCC_USBHSPLLON;
        if (RCC->CTLR & RCC_HSEON)
        {
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCLKSource_HSE);
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCKREFCLK_25M);
        }
        else
        {
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCLKSource_HSI);
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLCKREFCLK_20M);
        }
        RCC->CTLR |= RCC_USBHSPLLON;
        while (!(RCC->CTLR & RCC_USBHSPLLRDY));
    }

    /* Enable UTMI Clock */
    RCC_UTMI2cmd(ENABLE);

    /* Enable USBHS Clock */
    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, ENABLE);

    USBHS2D->CONTROL = USBHS_UD_RST_LINK | USBHS_UD_PHY_SUSPENDM;
    USBHS2D->INT_EN = USBHS_UDIE_BUS_RST | USBHS_UDIE_SUSPEND | USBHS_UDIE_BUS_SLEEP | USBHS_UDIE_LPM_ACT |
                      USBHS_UDIE_TRANSFER | USBHS_UDIE_LINK_RDY;
    USBHS2D->UEP_TX_EN = 0;
    USBHS2D->UEP_RX_EN = 0;
    USBHS2D->BASE_MODE = USBHS_UD_SPEED_HIGH;
    USBHS2D->CONTROL = USBHS_UD_DMA_EN | USBHS_UD_LPM_EN | USBHS_UD_PHY_SUSPENDM;
    NVIC_EnableIRQ(USBHS2_IRQn);

    if (_interrupt)
    {
        NVIC_EnableIRQ(USBHS2_IRQn);
    }

    return USB_RST_OK;
}

static usb_rst_e _disable(void)
{
    if (_interrupt)
    {
        NVIC_DisableIRQ(USBHS2_IRQn);
    }

    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS2, DISABLE);
    RCC_UTMI2cmd(DISABLE);
    RCC->CTLR &= ~RCC_USBHSPLLON;
    return USB_RST_OK;
}

static usb_rst_e _open(void)
{
    USBHS2D->CONTROL |= USBHS_UD_DEV_EN;
    return USB_RST_OK;
}

static usb_rst_e _close(void)
{
    USBHS2D->CONTROL &= ~USBHS_UD_DEV_EN;
    return USB_RST_OK;
}

static usb_rst_e _get_event(usbd_event_t *e)
{
    uint8_t en = USBHS2D->INT_EN;
    uint8_t flag = USBHS2D->INT_FG;
    uint8_t status = USBHS2D->INT_ST;

    if (flag & USBHS_UDIF_TRANSFER)
    {
        uint8_t endp = status & USBHS_UDIS_EP_ID_MASK;
        uint8_t dir = status & USBHS_UDIS_EP_DIR;

        if (endp == 0x00 && !dir && (ENDP_RX_CTRL(0) & USBHS_UEP_R_SETUP_IS))
        {
            e->e = USBD_EVENT_SETUP;
        }
        else if (dir)
        {
            e->e = USBD_EVENT_XFER;
            e->xfer.ep = endp | 0x80;
            e->xfer.size = ENDP_TX_LEN(endp);
            e->xfer.buf = (endp == 0x00) ? (void *)USBHS2D->UEP0_DMA : (void *)ENDP_TX_DMA_ADDR(endp);
        }
        else if (ENDP_RX_CTRL(endp) & USBHS_UEP_R_TOG_MATCH)
        {
            e->e = USBD_EVENT_XFER;
            e->xfer.ep = endp & ~0x80;
            e->xfer.size = ENDP_RX_LEN(endp);
            ENDP_RX_LEN(endp) = 0;
            e->xfer.buf = (endp == 0x00) ? (void *)USBHS2D->UEP0_DMA : (void *)ENDP_RX_DMA_ADDR(endp);
        }
        else
        {
            ENDP_RX_CTRL(endp) = (ENDP_RX_CTRL(endp) & ~(USBHS_UEP_R_RES_MASK | USBHS_UEP_R_DONE)) |
                                 USBHS_UEP_R_RES_ACK;
        }
    }
    else if (en & flag & USBHS_UDIF_RX_SOF)
    {
        e->e = USBD_EVENT_SOF;
        e->sof.frame_no = USBHS2D->FRAME_NO & 0x07FF;
    }
    else if (flag & USBHS_UDIF_BUS_RST)
    {
        e->e = USBD_EVENT_RESET;
        Delay_Ms(10);
        e->reset.link_speed = (USBHS2D->MIS_ST & USBHS_UDMS_HS_MOD) ? USB_SPEED_HIGH : USB_SPEED_FULL;
    }
    else if (flag & USBHS_UDIF_SUSPEND)
    {
        if (USBHS2D->MIS_ST & USBHS_UDMS_SUSPEND)
        {
            e->e = USBD_EVENT_SUSPEND;
        }
        else
        {
            USBHS2D->INT_FG = USBHS_UDIF_SUSPEND;
        }
    }
    else
    {
        USBHS2D->INT_FG = flag;
    }

    return USB_RST_OK;
}

static usb_rst_e _clear_event(usbd_event_t *e)
{
    switch (e->e)
    {
    case USBD_EVENT_RESET:
        USBHS2D->INT_FG = USBHS_UDIF_BUS_RST;
        break;

    case USBD_EVENT_SUSPEND:
        USBHS2D->INT_FG = USBHS_UDIF_SUSPEND;
        break;

    case USBD_EVENT_SOF:
        USBHS2D->INT_FG = USBHS_UDIF_RX_SOF;
        break;

    case USBD_EVENT_SETUP:
        USBHS2D->UEP0_RX_CTRL &= ~USBHS_UEP_R_DONE;
        break;

    case USBD_EVENT_XFER:
        if (e->xfer.ep & 0x80)
        {
            ENDP_TX_CTRL(ENDP_NUM(e->xfer.ep)) &= ~USBHS_UEP_T_DONE;
        }
        else
        {
            ENDP_RX_CTRL(ENDP_NUM(e->xfer.ep)) &= ~USBHS_UEP_R_DONE;
        }
        break;

    default:
        return USB_RST_UNKNOW_EVENT;
    }

    return USB_RST_OK;
}

static usb_rst_e _resume(void)
{
    USBHS2D->WAKE_CTRL |= USBHS_UD_REMOTE_WKUP;
    return USB_RST_OK;
}

static usb_rst_e _set_address(uint8_t addr)
{
    USBHS2D->DEV_AD = addr & 0x7F;
    return USB_RST_OK;
}

static usb_rst_e _interrupt_ctrl(usb_bool_t status)
{
    if (_interrupt == USB_FALSE) return USB_RST_OK;

    if (status == USB_TRUE)
    {
        NVIC_EnableIRQ(USBHS2_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(USBHS2_IRQn);
        __asm volatile("fence.i");
    }

    return USB_RST_OK;
}

static usb_rst_e _endp_open(usb_endp_t ep, endp_type_e type)
{
    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS2_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    uint32_t bit = 1 << num;

    if (dir)
    {
        USBHS2D->UEP_TX_EN |= bit;

        switch (type)
        {
        case ENDP_TYPE_NORMAL:
            USBHS2D->UEP_TX_ISO &= ~bit;
            USBHS2D->UEP_TX_BURST &= ~bit;
            USBHS2D->UEP_TX_TOG_AUTO &= ~bit;
            break;

        case ENDP_TYPE_ISO:
            USBHS2D->UEP_TX_ISO |= bit;
            USBHS2D->UEP_TX_BURST &= ~bit;
            USBHS2D->UEP_TX_TOG_AUTO &= ~bit;
            break;

        case ENDP_TYPE_BURST:
            USBHS2D->UEP_TX_ISO &= ~bit;
            USBHS2D->UEP_TX_BURST |= bit;
            USBHS2D->UEP_TX_TOG_AUTO |= bit;
            break;
        }
    }
    else
    {
        USBHS2D->UEP_RX_EN |= bit;

        switch (type)
        {
        case ENDP_TYPE_NORMAL:
            USBHS2D->UEP_RX_ISO &= ~bit;
            USBHS2D->UEP_RX_BURST &= ~bit;
            USBHS2D->UEP_RX_TOG_AUTO &= ~bit;
            break;

        case ENDP_TYPE_ISO:
            USBHS2D->UEP_RX_ISO |= bit;
            USBHS2D->UEP_RX_BURST &= ~bit;
            USBHS2D->UEP_RX_TOG_AUTO &= ~bit;
            break;

        case ENDP_TYPE_BURST:
            USBHS2D->UEP_RX_ISO &= ~bit;
            USBHS2D->UEP_RX_BURST |= bit;
            USBHS2D->UEP_RX_TOG_AUTO |= bit;
            break;
        }
    }

    return USB_RST_OK;
}

static usb_rst_e _endp_close(usb_endp_t ep)
{
    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS2_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    uint32_t bit = 1 << num;

    if (dir)
    {
        USBHS2D->UEP_TX_EN &= ~bit;
        USBHS2D->UEP_TX_ISO &= ~bit;
    }
    else
    {
        USBHS2D->UEP_RX_EN &= ~bit;
        USBHS2D->UEP_RX_ISO &= ~bit;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_status(usb_endp_t ep, endp_resp_e resp, endp_tog_e tog)
{
    uint8_t resp_val[] = {USBHS_UEP_T_RES_ACK, USBHS_UEP_T_RES_NAK, USBHS_UEP_T_RES_NAK, USBHS_UEP_T_RES_STALL};
    uint8_t tog_val[] = {USBHS_UEP_T_TOG_DATA0, USBHS_UEP_T_TOG_DATA1, USBHS_UEP_T_TOG_DATA2, USBHS_UEP_T_TOG_MDATA};

    if (ENDP_DIR(ep))
    {
        ENDP_TX_CTRL(ENDP_NUM(ep)) = resp_val[resp] | tog_val[tog];
    }
    else
    {
        ENDP_RX_CTRL(ENDP_NUM(ep)) = resp_val[resp] | tog_val[tog];
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_size(usb_endp_t ep, size_t size)
{
    if (ENDP_DIR(ep))
    {
        ENDP_TX_LEN(ENDP_NUM(ep)) = size;
    }
    else
    {
        ENDP_MAX_LEN(ENDP_NUM(ep)) = size;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_buf(usb_endp_t ep, const void *buf)
{
    uint8_t num = ENDP_NUM(ep);

    if (num == 0)
    {
        USBHS2D->UEP0_DMA = (uint32_t)buf;
    }
    else if (ENDP_DIR(ep))
    {
        ENDP_TX_DMA_ADDR(num) = (uint32_t)buf;
    }
    else
    {
        ENDP_RX_DMA_ADDR(num) = (uint32_t)buf;
    }

    return USB_RST_OK;
}

static usb_rst_e _set_endp_burst_size(usb_endp_t ep, size_t size)
{
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS2_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    ENDP_MAX_LEN(num) = size;

    return USB_RST_OK;
}

static usb_rst_e _get_endp_status(usb_endp_t ep, endp_resp_e *resp, endp_tog_e *tog)
{
    uint8_t num = ENDP_NUM(ep);
    uint8_t resp_val[] = {ENDP_RESP_NAK, ENDP_RESP_STALL, ENDP_RESP_ACK, ENDP_RESP_NAK};
    uint8_t tog_val[] = {ENDP_TOG_DATA0, ENDP_TOG_DATA1, ENDP_TOG_DATA2, ENDP_TOG_MDATA};

    if (ENDP_DIR(ep))
    {
        *resp = resp_val[ENDP_TX_CTRL(num) & USBHS_UEP_T_RES_MASK];
        *tog = tog_val[(ENDP_TX_CTRL(num) & USBHS_UEP_T_TOG_MASK) >> 2];
    }
    else
    {
        *resp = resp_val[ENDP_RX_CTRL(num) & USBHS_UEP_T_RES_MASK];
        *tog = tog_val[(ENDP_RX_CTRL(num) & USBHS_UEP_T_TOG_MASK) >> 2];
    }

    return USB_RST_OK;
}

static usb_rst_e _get_endp_size(usb_endp_t ep, size_t *size)
{
    *size = ENDP_DIR(ep) ? ENDP_TX_LEN(ENDP_NUM(ep)) : ENDP_RX_LEN(ENDP_NUM(ep));
    return USB_RST_OK;
}

static usb_rst_e _get_endp_buf(usb_endp_t ep, void **buf)
{
    uint8_t num = ENDP_NUM(ep);

    if (num == 0)
    {
        *buf = (void *)USBHS2D->UEP0_DMA;
    }
    else
    {
        uint32_t addr = ENDP_DIR(ep) ? ENDP_TX_DMA_ADDR(num) : ENDP_RX_DMA_ADDR(num);
        *buf = (void *)addr;
    }

    return USB_RST_OK;
}

static usb_rst_e _get_endp_burst_size(usb_endp_t ep, size_t *size)
{
    uint8_t num = ENDP_NUM(ep);

    if (num >= USBHS2_DCP_MAX_ENDPOINTS) return USB_RST_UNSUPPORTED_ENDP;

    *size = ENDP_MAX_LEN(num);

    return USB_RST_OK;
}
