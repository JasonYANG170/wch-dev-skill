/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_driver_core.c
* Author             : WCH
* Version            : V1.1
* Date               : 2026/01/14
* Description        : Usb device driver core file.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>

#include "device/usbd_driver.h"

/* @function declaration */
static void _reset_parameter(usbd_handle_t *h);
static void _setup_process(usbd_handle_t *h, usbd_event_t *e);
static void _data_process(usbd_handle_t *h, usbd_event_t *e);
static void _ctrl_xfer_process(usbd_handle_t *h, usbd_event_t *e);
static usb_rst_e _register_standard_request(usbd_handle_t *h);
static usb_rst_e _set_address_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _clear_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _get_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _get_status_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _set_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _set_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
static usb_rst_e _set_config_status(usbd_handle_t *h, const usb_req_t *req, const void *buf, size_t size);

usb_rst_e usbd_drv_init(usbd_handle_t *h)
{
    USBD_LOGI("===========================================");
    USBD_LOGI(" USB Device Driver Init");
    USBD_LOGI(" Version: v%d.%d", (USB_DEVICE_DRIVER_VERSION >> 8) & 0xFF, USB_DEVICE_DRIVER_VERSION & 0xFF);
    USBD_LOGI(" Build Time: %s %s", __DATE__, __TIME__);
    USBD_LOGI("===========================================");

    if (!h)
    {
        USBD_LOGE("Init handle is NULL!");
        return USB_RST_NULL_PTR;
    }

    if (h->feature.ep0_size != 8 && h->feature.ep0_size != 64)
    {
        USBD_LOGE("Init EP0 size error!");
        return USB_RST_ERROR_PARAMETER;
    }

    h->open_status = USB_FALSE;

    memset(&h->ctrl_table, 0, sizeof(h->ctrl_table));
    memset(&h->itf_table, 0, sizeof(h->itf_table));
    memset(&h->upload_cbs, 0, sizeof(h->upload_cbs));
    memset(&h->download_cbs, 0, sizeof(h->download_cbs));
    _reset_parameter(h);

    usb_rst_e rst = _register_standard_request(h);
    if (rst != USB_RST_OK)
    {
        USBD_LOGE("Register standard request error!");
        return rst;
    }

    return USB_RST_OK;
}

usb_rst_e usbd_drv_task(usbd_handle_t *h)
{
    usbd_event_t e;
    e.e = USBD_EVENT_NONE;

    if (h->get_event(&e) == USB_RST_OK)
    {
        switch (e.e)
        {
        case USBD_EVENT_NONE:
            break;

        case USBD_EVENT_RESET:
            USBD_LOGI("Bus reset is %s speed.",
                      (e.reset.link_speed == USB_SPEED_HIGH) ? "high" :
                      (e.reset.link_speed == USB_SPEED_FULL) ? "full" :
                      (e.reset.link_speed == USB_SPEED_LOW) ? "low" : "unknown");

            _reset_parameter(h);
            h->set_address(0x00);
            h->feature.link_speed = e.reset.link_speed;
            h->clear_event(&e);
            if (h->event_cbs[USBD_CB_EVENT_RESET])
            {
                h->event_cbs[USBD_CB_EVENT_RESET](h, 0);
            }
            break;

        case USBD_EVENT_SUSPEND:
            USBD_LOGI("Bus suspend");
            h->clear_event(&e);
            if (h->event_cbs[USBD_CB_EVENT_SUSPEND] && h->feature.remote_wakeup_support &&
                h->feature.remote_wakeup_enable)
            {
                h->event_cbs[USBD_CB_EVENT_SUSPEND](h, 0);
            }
            break;

        case USBD_EVENT_SOF:
            h->clear_event(&e);
            if (h->event_cbs[USBD_CB_EVENT_SOF])
            {
                h->event_cbs[USBD_CB_EVENT_SOF](h, e.sof.frame_no);
            }
            break;

        case USBD_EVENT_SETUP:
            _setup_process(h, &e);
            h->clear_event(&e);
            break;

        case USBD_EVENT_XFER:
            if (ENDP_NUM(e.xfer.ep) == 0x00)
            {
                _ctrl_xfer_process(h, &e);
            }
            else
            {
                _data_process(h, &e);
            }
            h->clear_event(&e);
            break;
        }
    }

    return USB_RST_OK;
}

usb_rst_e usbd_enable(usbd_handle_t *h)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->enable) return USB_RST_NULL_PTR;
    if (h->enable_status) return USB_RST_OK;

    usb_rst_e rst = h->enable();
    if (rst != USB_RST_OK)
    {
        USBD_LOGE("Enable failed!");
    }

    h->enable_status = (rst == USB_RST_OK) ? USB_TRUE : USB_FALSE;
    return rst;
}

usb_rst_e usbd_disable(usbd_handle_t *h)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->disable) return USB_RST_NULL_PTR;
    if (!h->enable_status) return USB_RST_OK;

    usb_rst_e rst = h->disable();
    h->enable_status = USB_FALSE;
    h->open_status = USB_FALSE;
    return rst;
}

usb_rst_e usbd_open(usbd_handle_t *h, usb_bool_t self_power, usb_bool_t remote_wakeup)
{
    usb_rst_e rst;

    if (!h)
    {
        USBD_LOGE("Open handle is NULL!");
        return USB_RST_NULL_PTR;
    }

    if (!h->enable_status)
    {
        USBD_LOGE("Device not enabled!");
        return USB_RST_NOT_ENABLE;
    }

    if (!h->open || !h->endp_open)
    {
        USBD_LOGE("Open func is NULL!");
        return USB_RST_NULL_PTR;
    }

    if (h->open_status) return USB_RST_OK;

    h->feature.self_powered = self_power ? USB_TRUE : USB_FALSE;
    h->feature.remote_wakeup_support = remote_wakeup ? USB_TRUE : USB_FALSE;

    rst = h->open();
    if (rst != USB_RST_OK)
    {
        USBD_LOGE("Open controller error!");
        return rst;
    }

    rst = h->endp_open(0x80, ENDP_TYPE_NORMAL);
    if (rst != USB_RST_OK)
    {
        USBD_LOGE("Open EP 0x80 error!");
        return rst;
    }

    rst = h->endp_open(0x00, ENDP_TYPE_NORMAL);
    if (rst != USB_RST_OK)
    {
        USBD_LOGE("Open EP 0x00 error!");
        return rst;
    }

    rst = h->set_endp_buf(0x80, h->ctrl_ctx.buf);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_buf(0x00, h->ctrl_ctx.buf);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_size(0x00, sizeof(usb_req_t));
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_status(0x80, ENDP_RESP_NAK, ENDP_TOG_DATA0);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_status(0x00, ENDP_RESP_ACK, ENDP_TOG_DATA0);
    if (rst != USB_RST_OK) return rst;

    h->open_status = USB_TRUE;

    return USB_RST_OK;
}

usb_rst_e usbd_close(usbd_handle_t *h)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->close) return USB_RST_NULL_PTR;
    if (!h->enable_status) return USB_RST_NOT_ENABLE;
    if (!h->open_status) return USB_RST_OK;

    h->open_status = USB_FALSE;
    return h->close();
}

usb_bool_t usbd_is_enable(usbd_handle_t *h)
{
    if (!h) return USB_FALSE;
    return h->enable_status;
}

usb_bool_t usbd_is_open(usbd_handle_t *h)
{
    if (!h) return USB_FALSE;
    return h->open_status;
}

usb_bool_t usbd_is_enum(usbd_handle_t *h)
{
    if (!h) return USB_FALSE;
    return h->enum_status;
}

usb_bool_t usbd_is_burst_support(usbd_handle_t *h)
{
    return h->feature.burst_support;
}

usb_speed_e usbd_get_speed(usbd_handle_t *h)
{
    if (!h) return USB_SPEED_UNKNOW;
    return h->feature.link_speed;
}

usb_rst_e usbd_resume(usbd_handle_t *h)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->enable_status) return USB_RST_NOT_ENABLE;
    if (!h->open_status) return USB_RST_NOT_OPEN;
    if (!h->resume) return USB_RST_NULL_PTR;
    if (!h->feature.remote_wakeup_support || !h->feature.remote_wakeup_enable) return USB_RST_FAILED;

    return h->resume();
}

usb_rst_e usbd_interrupt_ctrl(usbd_handle_t *h, usb_bool_t status)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->interrupt_ctrl) return USB_RST_NULL_PTR;

    return h->interrupt_ctrl(status);
}

usb_rst_e usbd_endp_open(usbd_handle_t *h, usb_endp_t ep, endp_type_e type, size_t size, const void *buf,
                         endp_resp_e resp)
{
    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);

    if (!h) return USB_RST_NULL_PTR;
    if (!h->enable_status) return USB_RST_NOT_ENABLE;
    if (!h->endp_open || !h->set_endp_buf || !h->set_endp_size || !h->set_endp_status) return USB_RST_NULL_PTR;
    if (num == 0) return USB_RST_UNSUPPORTED_ENDP;

    usb_rst_e rst;

    rst = h->set_endp_size(ep, size);
    if (rst != USB_RST_OK) return rst;

    rst = h->set_endp_buf(ep, buf);
    if (rst != USB_RST_OK) return rst;

    rst = h->endp_open(ep, type);
    if (rst != USB_RST_OK) return rst;

    if (dir)
    {
        h->in_tog[num] = ENDP_TOG_DATA0;
        h->in_sta[num] = ENDP_STA_IDLE;
        rst = h->set_endp_status(ep, resp, ENDP_TOG_DATA0);
        if (rst != USB_RST_OK) return rst;
    }
    else
    {
        h->out_tog[num] = ENDP_TOG_DATA0;
        h->out_sta[num] = ENDP_STA_IDLE;
        rst = h->set_endp_status(ep, resp, ENDP_TOG_DATA0);
        if (rst != USB_RST_OK) return rst;
    }

    return USB_RST_OK;
}

usb_rst_e usbd_endp_close(usbd_handle_t *h, usb_endp_t ep)
{
    if (!h) return USB_RST_NULL_PTR;
    if (!h->enable_status) return USB_RST_NOT_ENABLE;
    if (!h->endp_close) return USB_RST_NULL_PTR;

    return h->endp_close(ep);
}

usb_rst_e usbd_endp_set_status(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp, endp_tog_e tog)
{
    return h->set_endp_status(ep, resp, tog);
}

usb_rst_e usbd_endp_set_size(usbd_handle_t *h, usb_endp_t ep, size_t size)
{
    return h->set_endp_size(ep, size);
}

usb_rst_e usbd_endp_set_buf(usbd_handle_t *h, usb_endp_t ep, const void *buf)
{
    return h->set_endp_buf(ep, buf);
}

usb_rst_e usbd_endp_set_burst_size(usbd_handle_t *h, usb_endp_t ep, size_t size)
{
    if (!h) return USB_RST_NULL_PTR;
    if (h->feature.burst_support == USB_FALSE) return USB_RST_UNSUPPORTED_BURST;
    if (h->set_endp_burst_size == NULL) return USB_RST_UNSUPPORTED_BURST;

    return h->set_endp_burst_size(ep, size);
}

usb_rst_e usbd_register_event_callback(usbd_handle_t *h, usbd_cb_event_e event, event_cb cb)
{
    if (!h) return USB_RST_NULL_PTR;
    if (event >= USBD_CB_EVENT_COUNT) return USB_RST_UNSUPPORTED_EVENT;

    h->event_cbs[event] = cb;
    return USB_RST_OK;
}

usb_rst_e usbd_register_ctrl_callback(usbd_handle_t *h, uint8_t bmRequestType, uint8_t bRequest, ctrl_setup_cb setup,
                                      ctrl_status_cb status)
{
    if (!h) return USB_RST_NULL_PTR;
    if (h->ctrl_table.count >= USBD_CTRL_TABLE_MAX_ITEMS) return USB_RST_OVERFLOW;

    size_t i;
    for (i = 0; i < h->ctrl_table.count; i++)
    {
        if (h->ctrl_table.items[i].bmRequestType == bmRequestType &&
            h->ctrl_table.items[i].bRequest == bRequest)
        {
            /* Existing item will be overwritten */
            break;
        }
    }
    if (i >= h->ctrl_table.count)
    {
        h->ctrl_table.count++;
    }

    usbd_ctrl_item_t *item = &h->ctrl_table.items[i];
    item->bmRequestType = bmRequestType;
    item->bRequest = bRequest;
    item->setup = setup;
    item->status = status;
    return USB_RST_OK;
}

usb_rst_e usbd_register_itf_callback(usbd_handle_t *h, uint16_t itf, ctrl_setup_cb setup, ctrl_status_cb status)
{
    if (!h) return USB_RST_NULL_PTR;
    if (itf >= sizeof(h->itf_table) / sizeof(h->itf_table[0])) return USB_RST_OVERFLOW;

    h->itf_table[itf].setup = setup;
    h->itf_table[itf].status = status;
    return USB_RST_OK;
}

usb_rst_e usbd_register_data_callback(usbd_handle_t *h, usb_endp_t ep, data_xfer_cb cb)
{
    if (!h) return USB_RST_NULL_PTR;

    uint8_t dir = ENDP_DIR(ep);
    uint8_t num = ENDP_NUM(ep);
    if (num >= 16) return USB_RST_UNSUPPORTED_ENDP;

    if (dir)
    {
        h->upload_cbs[num] = cb;
    }
    else
    {
        h->download_cbs[num] = cb;
    }
    return USB_RST_OK;
}

usb_rst_e usbd_upload(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size)
{
    uint8_t num = ENDP_NUM(ep);

    switch (h->in_sta[num])
    {
    case ENDP_STA_IDLE:
        h->in_sta[num] = ENDP_STA_BUSY;
        h->set_endp_buf(ep, buf);
        h->set_endp_size(ep, size);
        h->set_endp_status(ep, ENDP_RESP_ACK, h->in_tog[num]);
        return USB_RST_OK;
        break;

    case ENDP_STA_BUSY:
        return USB_RST_ENDP_BUSY;
        break;

    case ENDP_STA_STALL:
        return USB_RST_ENDP_STALL;
        break;
    }

    return USB_RST_FAILED;
}

usb_rst_e usbd_download(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size)
{
    uint8_t num = ENDP_NUM(ep);

    switch (h->out_sta[num])
    {
    case ENDP_STA_IDLE:
    case ENDP_STA_BUSY:
        h->out_sta[num] = ENDP_STA_BUSY;
        h->set_endp_buf(ep, buf);
        h->set_endp_size(ep, size);
        h->set_endp_status(ep, ENDP_RESP_ACK, h->out_tog[num]);
        return USB_RST_OK;
        break;

    case ENDP_STA_STALL:
        return USB_RST_ENDP_STALL;
        break;
    }

    return USB_RST_FAILED;
}

uint16_t usbd_get_version(void)
{
    return USB_DEVICE_DRIVER_VERSION;
}

static void _reset_parameter(usbd_handle_t *h)
{
    h->enum_status = USB_FALSE;

    memset(&h->in_sta, 0, sizeof(h->in_sta));
    memset(&h->out_sta, 0, sizeof(h->out_sta));
    memset(&h->in_tog, 0, sizeof(h->in_tog));
    memset(&h->out_tog, 0, sizeof(h->out_tog));

    h->feature.address = 0x00;
    h->feature.link_speed = USB_SPEED_FULL;
    h->feature.config_num = 0x00;
    h->feature.test_mode = USB_FALSE;
    h->feature.remote_wakeup_enable = USB_FALSE;
}

static void _setup_process(usbd_handle_t *h, usbd_event_t *e)
{
    usb_rst_e rst = USB_RST_FAILED;
    usbd_ctrl_ctx_t *ctx = &h->ctrl_ctx;

    memcpy(&ctx->req, ctx->buf, sizeof(ctx->req));
    USBD_LOGI("Setup packet: bmRequestType=0x%02X, bRequest=0x%02X, wValue=0x%04X, wIndex=0x%04X, wLength=%d.",
              ctx->req.bmRequestType, ctx->req.bRequest, ctx->req.wValue, ctx->req.wIndex, ctx->req.wLength);

    ctrl_status_cb status = NULL;

    /* Priority query interface control requests */
    if (ctx->req.bmRequestType_bit.recipient == USB_REQ_RCPT_INTERFACE &&
        ctx->req.wIndex < sizeof(h->itf_table) / sizeof(h->itf_table[0]) &&
        h->itf_table[ctx->req.wIndex].setup)
    {
        rst = h->itf_table[ctx->req.wIndex].setup(h, &ctx->req, &ctx->buf_ptr, &ctx->size);
        status = h->itf_table[ctx->req.wIndex].status;
    }
    else
    {
        usbd_ctrl_item_t *item;
        for (size_t i = 0; i < h->ctrl_table.count; i++)
        {
            item = &h->ctrl_table.items[i];

            if (item->bmRequestType == ctx->req.bmRequestType && item->bRequest == ctx->req.bRequest)
            {
                rst = item->setup ? item->setup(h, &ctx->req, &ctx->buf_ptr, &ctx->size) : USB_RST_NULL_PTR;
                status = item->status;
                break;
            }
        }
    }

    if (rst == USB_RST_OK)
    {
        ctx->offset = 0;
        ctx->tog = ENDP_TOG_DATA1;
        ctx->stage = CTRL_STAGE_SETUP;
        ctx->dir = ctx->req.bmRequestType_bit.direction ? 1 : 0;
        ctx->cb = status;
        _ctrl_xfer_process(h, e);
    }
    else
    {
        /* Unsupported control transmission */
        USBD_LOGW("Unsupported control request.");
        h->set_endp_status(0x00, ENDP_RESP_STALL, ENDP_TOG_DATA1);
        h->set_endp_status(0x80, ENDP_RESP_STALL, ENDP_TOG_DATA1);
    }
}

static void _data_process(usbd_handle_t *h, usbd_event_t *e)
{
    uint8_t dir = ENDP_DIR(e->xfer.ep);
    uint8_t num = ENDP_NUM(e->xfer.ep);
    uint16_t size = e->xfer.size;
    endp_resp_e resp = ENDP_RESP_NAK;

    if (dir)
    {
        h->in_sta[num] = ENDP_STA_IDLE;
        h->in_tog[num] ^= ENDP_TOG_DATA1;
        if (h->upload_cbs[num])
        {
            resp = h->upload_cbs[num](h, e->xfer.ep, e->xfer.buf, size);
        }
        h->set_endp_status(e->xfer.ep, resp, h->in_tog[num]);
    }
    else
    {
        h->out_sta[num] = ENDP_STA_IDLE;
        h->out_tog[num] ^= ENDP_TOG_DATA1;
        if (h->download_cbs[num])
        {
            resp = h->download_cbs[num](h, e->xfer.ep, e->xfer.buf, size);
        }
        h->set_endp_status(e->xfer.ep, resp, h->out_tog[num]);
    }
}

static void _ctrl_xfer_process(usbd_handle_t *h, usbd_event_t *e)
{
    size_t size;
    usb_rst_e rst = USB_RST_OK;
    usbd_ctrl_ctx_t *ctx = &h->ctrl_ctx;

    switch (ctx->stage)
    {
    case CTRL_STAGE_SETUP:
    case CTRL_STAGE_DATA:
        /* Device to host control transfer */
        if (ctx->dir)
        {
            if (ctx->size > 0)
            {
                size = USB_MIN(ctx->size, h->feature.ep0_size);
                memcpy(ctx->buf, (uint8_t *)ctx->buf_ptr + ctx->offset, size);
                ctx->offset += size;
                ctx->size -= size;
                h->set_endp_size(0x80, size);
                h->set_endp_status(0x80, ENDP_RESP_ACK, ctx->tog);
                ctx->tog ^= ENDP_TOG_DATA1;
                ctx->stage = CTRL_STAGE_DATA;
            }
            else
            {
                if (ctx->cb)
                {
                    rst = ctx->cb(h, &ctx->req, ctx->buf_ptr, ctx->offset);
                }

                h->set_endp_size(0x80, 0);
                h->set_endp_size(0x00, sizeof(usb_req_t));
                h->set_endp_status(0x00, rst == USB_RST_OK ? ENDP_RESP_ACK : ENDP_RESP_STALL, ENDP_TOG_DATA1);
                ctx->stage = CTRL_STAGE_STATUS;
            }
        }
        /* Host to device control transfer */
        else
        {
            if (ctx->stage == CTRL_STAGE_DATA)
            {
                h->get_endp_size(0x00, &size);
                size = USB_MIN(ctx->size, h->feature.ep0_size);
                memcpy((uint8_t *)ctx->buf_ptr + ctx->offset, ctx->buf, size);
                ctx->offset += size;
                ctx->size -= size;
            }

            if (ctx->size > 0)
            {
                size = USB_MIN(ctx->size, h->feature.ep0_size);
                h->set_endp_size(0x00, size);
                h->set_endp_status(0x00, ENDP_RESP_ACK, ctx->tog);
                ctx->tog ^= ENDP_TOG_DATA1;
                ctx->stage = CTRL_STAGE_DATA;
            }
            else
            {
                if (ctx->cb)
                {
                    rst = ctx->cb(h, &ctx->req, ctx->buf_ptr, ctx->offset);
                }

                h->set_endp_size(0x80, 0);
                h->set_endp_size(0x00, sizeof(usb_req_t));
                h->set_endp_status(0x80, rst == USB_RST_OK ? ENDP_RESP_ACK : ENDP_RESP_STALL, ENDP_TOG_DATA1);
                ctx->stage = CTRL_STAGE_STATUS;
            }
        }
        break;

    case CTRL_STAGE_STATUS:
        if (ctx->req.bmRequestType == 0x00 && ctx->req.bRequest == USB_REQ_CODE_SET_ADDRESS)
        {
            USBD_LOGI("Set address: %d.", h->feature.address);
            h->set_address(h->feature.address);
        }
        break;
    }
}

static usb_rst_e _register_standard_request(usbd_handle_t *h)
{
    usb_rst_e rst;

    rst = usbd_register_ctrl_callback(h, 0x00, USB_REQ_CODE_SET_ADDRESS, _set_address_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x00, USB_REQ_CODE_SET_FEATURE, _set_feature_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x00, USB_REQ_CODE_CLEAR_FEATURE, _clear_feature_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x00, USB_REQ_CODE_SET_CONFIGURATION, _set_config_setup, _set_config_status);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x80, USB_REQ_CODE_GET_STATUS, _get_status_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x80, USB_REQ_CODE_GET_CONFIGURATION, _get_config_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x02, USB_REQ_CODE_SET_FEATURE, _set_feature_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x02, USB_REQ_CODE_CLEAR_FEATURE, _clear_feature_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    rst = usbd_register_ctrl_callback(h, 0x82, USB_REQ_CODE_GET_STATUS, _get_status_setup, NULL);
    if (rst != USB_RST_OK) return rst;

    return USB_RST_OK;
}

static usb_rst_e _clear_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_REMOTE_WAKEUP)
    {
        h->feature.remote_wakeup_enable = USB_FALSE;
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_TEST_MODE)
    {
        /* TODO */
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT &&
        req->wValue == USB_REQ_FEATURE_ENDPOINT_HALT)
    {
        usb_endp_t ep = req->wIndex & 0x00FF;
        uint8_t dir = ENDP_DIR(ep);
        uint8_t num = ENDP_NUM(ep);

        if (dir)
        {
            h->in_tog[num] = ENDP_TOG_DATA0;
            h->in_sta[num] = ENDP_STA_IDLE;
            h->set_endp_status(ep, ENDP_RESP_NAK, ENDP_TOG_DATA0);
        }
        else
        {
            h->out_tog[num] = ENDP_TOG_DATA0;
            h->out_sta[num] = ENDP_STA_IDLE;
            h->set_endp_status(ep, ENDP_RESP_ACK, ENDP_TOG_DATA0);
        }

        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static usb_rst_e _get_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    h->ctrl_ctx.temp_data = h->feature.config_num;
    *buf = (void *)&h->ctrl_ctx.temp_data;
    *size = USB_MIN(sizeof(uint8_t), req->wLength);
    return USB_RST_OK;
}

static usb_rst_e _get_status_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE)
    {
        h->ctrl_ctx.temp_data = (h->feature.self_powered ? 0x0001 : 0x0000) |
                                (h->feature.remote_wakeup_enable ? 0x0002 : 0x0000);
        *buf = (void *)&h->ctrl_ctx.temp_data;
        *size = USB_MIN(sizeof(uint16_t), req->wLength);
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT)
    {
        usb_rst_e rst;
        usb_endp_t ep = req->wIndex & 0x00FF;
        endp_resp_e resp;
        endp_tog_e tog;

        rst = h->get_endp_status(ep, &resp, &tog);
        if (rst != USB_RST_OK)
        {
            return rst;
        }

        h->ctrl_ctx.temp_data = resp == ENDP_RESP_STALL ? 0x0001 : 0x0000;
        *buf = (void *)&h->ctrl_ctx.temp_data;
        *size = USB_MIN(sizeof(uint16_t), req->wLength);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static usb_rst_e _set_address_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    *size = 0;
    h->feature.address = req->wValue & 0x00FF;
    return USB_RST_OK;
}

static usb_rst_e _set_config_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    *size = 0;
    h->feature.config_num = req->wValue & 0x00FF;
    return USB_RST_OK;
}

static usb_rst_e _set_feature_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_REMOTE_WAKEUP)
    {
        h->feature.remote_wakeup_enable = USB_TRUE;
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_DEVICE &&
        req->wValue == USB_REQ_FEATURE_TEST_MODE)
    {
        /* TODO */
        return USB_RST_OK;
    }

    if (req->bmRequestType_bit.recipient == USB_REQ_RCPT_ENDPOINT &&
        req->wValue == USB_REQ_FEATURE_ENDPOINT_HALT)
    {
        usb_endp_t ep = req->wIndex & 0x00FF;

        uint8_t dir = ENDP_DIR(ep);
        uint8_t num = ENDP_NUM(ep);

        if (dir)
        {
            h->in_sta[num] = ENDP_STA_STALL;
        }
        else
        {
            h->out_sta[num] = ENDP_STA_STALL;
        }

        h->set_endp_status(ep, ENDP_RESP_STALL, ENDP_TOG_DATA0);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

static usb_rst_e _set_config_status(usbd_handle_t *h, const usb_req_t *req, const void *buf, size_t size)
{
    h->enum_status = USB_TRUE;
    USBD_LOGI("Enumeration completed, config is %d.", h->feature.config_num);

    if (h->event_cbs[USBD_CB_EVENT_ENUM_COMPLETED])
    {
        h->event_cbs[USBD_CB_EVENT_ENUM_COMPLETED](h, 0);
    }

    return USB_RST_OK;
}
