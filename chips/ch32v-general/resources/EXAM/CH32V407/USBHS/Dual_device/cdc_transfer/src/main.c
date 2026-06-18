/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/01/15
* Description        : Main function file.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * 功能说明 / Description:
 * 本程序为双设备 CDC 数据透传示例，演示如何在同一开发板上使用两个独立的 USB 设备控制器
 * （Device Controller 0 与 Device Controller 1）建立两个 CDC-ACM 接口，并在设备间实现数据缓存与
 * 透传。程序使用环形缓冲区保存从一端接收的数据包，然后按包大小分段通过另一端上传到主机。
 * 板上 LED 用于指示各通道的数据活动状态。
 *
 * 宏说明 / Macros:
 * - `USBD0_INDEX` / `USBD1_INDEX`：分别选择要初始化的两个 USB 设备控制器索引，默认 `0` 和 `1`。
 *   当开发板的 USB 控制器数量不同于默认值时，请将其设置为 `0`..(`BOARD_USBDC_COUNT - 1`) 的有效索引。
 * - `USBD0_INTERRUPT` / `USBD1_INTERRUPT`：控制各自 USB 驱动的工作模式。设置为 `1` 表示使用中断驱动（ISR
 *   处理 USB 事件），设置为 `0` 表示使用轮询/任务驱动（主循环中调用 `usbd_drv_task(usbdX_handle)`）。
 *
 * 使用方法 / Usage:
 * 1. 连接开发板到主机并烧录程序，主机会识别为两个 CDC-ACM 设备接口。
 * 2. 主机向任意一端的 CDC 端点发送数据（OUT），设备会把接收的数据保存到对应的环形缓冲区，随后由
 *    程序从缓冲区读取并通过对端的 IN 端点上传到另一侧主机，实现双向透传。
 * 3. 文件中的 `dc0_to_dc1_bufs` 与 `dc1_to_dc0_bufs` 为预分配的缓存数组，`BUFFER_COUNT` 控制环形队列深度；
 *    缓冲区大小由 `BUFFER_SIZE` 定义，通常为 与 `_bulk_size` 相同或更大，以容纳一次接收的完整包。
 * 4. LED 指示：LED0 指示从 DC0 接收并上传到 DC1 的数据活动，LED1 指示从 DC1 接收并上传到 DC0 的数据活动。
 *
 * 其他说明 / Notes:
 * - 缓冲区已按 4 字节对齐 (`aligned(4)`)，避免 DMA/对齐问题。
 * - `bulk_size0` 与 `bulk_size1` 根据 USB 速率自动选择（高速 512B，其他 64B）。
 * - 本示例侧重于演示缓冲与透传机制；在真实产品中请注意并发/线程安全、内存限制、错误处理与超时策略。
 */

/* @include */
#include <string.h>
#include <assert.h>

#include "board.h"
#include "led.h"

#include "class/cdc/cdcd.h"

#include "descriptor.h"

/* @define */
#ifndef USBD0_INDEX
#define USBD0_INDEX         0
#endif

#ifndef USBD0_INTERRUPT
#define USBD0_INTERRUPT     1
#endif

#if USBD0_INDEX >= BOARD_USBDC_COUNT
#error Not supported this USBD_INDEX!
#endif

#ifndef USBD1_INDEX
#define USBD1_INDEX         1
#endif

#ifndef USBD1_INTERRUPT
#define USBD1_INTERRUPT     1
#endif

#if USBD1_INDEX >= BOARD_USBDC_COUNT
#error Not supported this USBD_INDEX!
#endif

#define LED0                0x01
#define LED1                0x02

#define BUFFER_COUNT        8
#define BUFFER_SIZE         512

/* @struct */
typedef struct
{
    uint8_t *addr;
    uint32_t size;
} pack_t;

typedef struct
{
    uint8_t load;
    uint8_t deal;
    uint8_t stop;
    uint8_t count;
} manage_t;

typedef struct
{
    usbd_handle_t *h;
    cdcd_acm_itf_t itf;
    uint16_t bulk_size;
    manage_t xfer_manage;
    pack_t xfer_packs[BUFFER_COUNT];
    __attribute__((aligned(4))) uint8_t xfer_bufs[BUFFER_COUNT][BUFFER_SIZE];
} acm_dev_t;

/* @global */
acm_dev_t acm_devs[2];

/* @function declaration */
void tranxfer_handle(acm_dev_t *dev);
void reset_callback(usbd_handle_t *h, uint32_t parameter);
usb_rst_e cdc_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size);

int main(void)
{
    cdcd_acm_itf_t *itf;

    board_init();

    acm_devs[0].itf.itf_num = 0x00;
    acm_devs[0].itf.ep_notif = 0x81;
    acm_devs[0].itf.ep_in = 0x82;
    acm_devs[0].itf.ep_out = 0x03;
    acm_devs[0].itf.line_state = 0x00;

    assert(led_init() == USB_RST_OK);

    usbd_handle_t *h0 = board_usbd_init(USBD0_INDEX, USBD0_INTERRUPT);
    assert(h0 != NULL);

    acm_devs[0].h = h0;
    assert(usbd_drv_init(h0) == USB_RST_OK);
    assert(usbd_enable(h0) == USB_RST_OK);

    itf = &acm_devs[0].itf;
    assert(usbd_register_event_callback(h0, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_ctrl_callback(h0, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h0, itf->itf_num, cdc_itf_setup, NULL) == USB_RST_OK);
    assert(usbd_register_data_callback(h0, itf->ep_out, download_callback) == USB_RST_OK);

    assert(usbd_open(h0, USB_FALSE, USB_FALSE) == USB_RST_OK);

    acm_devs[1].itf.itf_num = 0x00;
    acm_devs[1].itf.ep_notif = 0x81;
    acm_devs[1].itf.ep_in = 0x82;
    acm_devs[1].itf.ep_out = 0x03;
    acm_devs[1].itf.line_state = 0x00;

    assert(led_init() == USB_RST_OK);

    usbd_handle_t *h1 = board_usbd_init(USBD1_INDEX, USBD1_INTERRUPT);
    assert(h1 != NULL);

    acm_devs[1].h = h1;
    assert(usbd_drv_init(h1) == USB_RST_OK);
    assert(usbd_enable(h1) == USB_RST_OK);

    itf = &acm_devs[1].itf;
    assert(usbd_register_event_callback(h1, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_ctrl_callback(h1, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h1, itf->itf_num, cdc_itf_setup, NULL) == USB_RST_OK);
    assert(usbd_register_data_callback(h1, itf->ep_out, download_callback) == USB_RST_OK);

    assert(usbd_open(h1, USB_FALSE, USB_FALSE) == USB_RST_OK);

    while (1)
    {
#if !(USBD0_INTERRUPT)
        usbd_drv_task(h0);
#endif

#if !(USBD1_INTERRUPT)
        usbd_drv_task(h1);
#endif

        acm_dev_t *dev;

        for (uint8_t i = 0; i < sizeof(acm_devs) / sizeof(acm_dev_t); i++)
        {
            dev = &acm_devs[i];

            if (usbd_is_enum(dev->h) != USB_TRUE) continue;

            tranxfer_handle(dev);
        }
    }

    return 0;
}

void tranxfer_handle(acm_dev_t *dev)
{
    if (dev->xfer_manage.count)
    {
        acm_dev_t *dst_dev = dev == &acm_devs[0] ? &acm_devs[1] : &acm_devs[0];

        pack_t *pack = &dev->xfer_packs[dev->xfer_manage.deal];

        if (pack->size > dst_dev->bulk_size)
        {
            if (usbd_upload(dst_dev->h, dst_dev->itf.ep_in, pack->addr, dst_dev->bulk_size) == USB_RST_OK)
            {
                pack->addr += dst_dev->bulk_size;
                pack->size -= dst_dev->bulk_size;
            }
        }
        else if (usbd_upload(dst_dev->h, dst_dev->itf.ep_in, pack->addr, pack->size) == USB_RST_OK)
        {
            pack->size = 0;
            dev->xfer_manage.deal = (dev->xfer_manage.deal + 1) % BUFFER_COUNT;

            usbd_interrupt_ctrl(dev->h, USB_FALSE);
            dev->xfer_manage.count--;
            usbd_interrupt_ctrl(dev->h, USB_TRUE);
        }
    }

    if (dev->xfer_manage.stop && dev->xfer_manage.count < BUFFER_COUNT - 2)
    {
        usbd_interrupt_ctrl(dev->h, USB_FALSE);
        if (usbd_download(dev->h, dev->itf.ep_out, dev->xfer_bufs[dev->xfer_manage.load], dev->bulk_size) == USB_RST_OK)
        {
            dev->xfer_manage.stop = 0;
        }
        usbd_interrupt_ctrl(dev->h, USB_TRUE);
    }
}

void reset_callback(usbd_handle_t *h, uint32_t parameter)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];

    memset(&dev->xfer_manage, 0, sizeof(dev->xfer_manage));
    for (uint8_t i = 0; i < BUFFER_COUNT; i++)
    {
        dev->xfer_packs[i].addr = dev->xfer_bufs[i];
        dev->xfer_packs[i].size = 0;
    }
    dev->bulk_size = usbd_get_speed(h) == USB_SPEED_HIGH ? 512 : 64;
    usbd_endp_open(h, dev->itf.ep_notif, ENDP_TYPE_NORMAL, 8, NULL, ENDP_RESP_NAK);
    usbd_endp_open(h, dev->itf.ep_in, ENDP_TYPE_NORMAL, dev->bulk_size, NULL, ENDP_RESP_NAK);
    usbd_endp_open(h, dev->itf.ep_out, ENDP_TYPE_NORMAL, dev->bulk_size, dev->xfer_bufs[0], ENDP_RESP_ACK);
}

usb_rst_e cdc_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];

    switch (req->bmRequestType_bit.type)
    {
    case USB_REQ_TYPE_CLASS:
        switch (req->bRequest)
        {
        case CDC_REQ_SET_LINE_CODING:
        case CDC_REQ_GET_LINE_CODING:
            *buf = (void *)&dev->itf.line_coding;
            *size = USB_MIN(sizeof(dev->itf.line_coding), req->wLength);
            return USB_RST_OK;

        case CDC_REQ_SET_CONTROL_LINE_STATE:
            *size = 0;
            dev->itf.line_state = req->wValue & 0x00FF;
            return USB_RST_OK;
        }
        break;
    }

    return USB_RST_FAILED;
}

endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size)
{
    acm_dev_t *dev = h == acm_devs[0].h ? &acm_devs[0] : &acm_devs[1];

    uint8_t load = dev->xfer_manage.load;
    pack_t *pack = &dev->xfer_packs[load];

    pack->addr = dev->xfer_bufs[load];
    pack->size = size;
    dev->xfer_manage.load = (load + 1) % BUFFER_COUNT;
    dev->xfer_manage.count++;
    if (dev->xfer_manage.count >= BUFFER_COUNT - 2)
    {
        dev->xfer_manage.stop = 1;
        return ENDP_RESP_NAK;
    }
    else
    {
        usbd_endp_set_buf(h, dev->itf.ep_out, dev->xfer_bufs[dev->xfer_manage.load]);
        return ENDP_RESP_ACK;
    }
}
