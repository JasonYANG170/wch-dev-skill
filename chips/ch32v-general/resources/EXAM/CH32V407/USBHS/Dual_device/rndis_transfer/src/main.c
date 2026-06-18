/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2026/02/10
* Description        : Main function file.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @include */
#include <string.h>
#include <assert.h>

#include "board.h"

#include "class/cdc/cdcd.h"
#include "class/cdc/cdc_rndis.h"

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

#define BUFFER_COUNT        8
#define BUFFER_SIZE         4096

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
    uint32_t tx_ok;
    uint32_t rx_ok;
    uint32_t tx_err;
    uint32_t rx_err;
} xfer_count_t;

typedef struct
{
    usbd_handle_t *h;
    cdcd_rndis_itf_t itf;
    uint16_t bulk_size;
    manage_t xfer_manage;
    xfer_count_t xfer_count;
    pack_t xfer_packs[BUFFER_COUNT];
    __attribute__((aligned(4))) uint8_t request_buf[1024];
    __attribute__((aligned(4))) uint8_t response_buf[1024];
    __attribute__((aligned(4))) uint8_t xfer_bufs[BUFFER_COUNT][BUFFER_SIZE];
} rndis_dev_t;

/* @global */
rndis_dev_t rndis_devs[2];
const uint32_t response_available[] = {RNDIS_RESPONSE_AVAILABLE, 0x00000000};

const uint32_t supported_oids[] =
{
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_XMIT_OK,
    OID_GEN_RCV_OK,
    OID_GEN_XMIT_ERROR,
    OID_GEN_RCV_ERROR,
    OID_GEN_RCV_NO_BUFFER,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
};

/* @function declaration */
void command_handle(rndis_dev_t *dev);
void status_upload_handle(rndis_dev_t *dev);
void tranxfer_handle(rndis_dev_t *dev);
void rndis_oid_query(rndis_dev_t *dev, void *buf, uint32_t oid, uint32_t *status, uint32_t *length);
void rndis_oid_set(rndis_dev_t *dev, const void *buf, uint32_t oid, uint32_t *status, uint32_t length);
void reset_callback(usbd_handle_t *h, uint32_t parameter);
usb_rst_e rndis_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
usb_rst_e rndis_itf_status(usbd_handle_t *h, const usb_req_t *req, const void *buf, size_t size);
endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size);

int main(void)
{
    cdcd_rndis_itf_t *itf;

    board_init();

    rndis_devs[0].itf.ctrl_itf = 0x00;
    rndis_devs[0].itf.data_itf = 0x01;
    rndis_devs[0].itf.ep_notif = 0x81;
    rndis_devs[0].itf.ep_in = 0x82;
    rndis_devs[0].itf.ep_out = 0x03;
    rndis_devs[0].itf.msg_stage = RNDIS_MSG_STAGE_IDLE;

    usbd_handle_t *h0 = board_usbd_init(USBD0_INDEX, USBD0_INTERRUPT);
    assert(h0 != NULL);

    rndis_devs[0].h = h0;
    assert(usbd_drv_init(h0) == USB_RST_OK);
    assert(usbd_enable(h0) == USB_RST_OK);

    itf = &rndis_devs[0].itf;
    assert(usbd_register_event_callback(h0, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_ctrl_callback(h0, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h0, itf->ctrl_itf, rndis_itf_setup, rndis_itf_status) == USB_RST_OK);
    assert(usbd_register_data_callback(h0, itf->ep_out, download_callback) == USB_RST_OK);

    assert(usbd_open(h0, USB_FALSE, USB_FALSE) == USB_RST_OK);

    rndis_devs[1].itf.ctrl_itf = 0x00;
    rndis_devs[1].itf.data_itf = 0x01;
    rndis_devs[1].itf.ep_notif = 0x81;
    rndis_devs[1].itf.ep_in = 0x82;
    rndis_devs[1].itf.ep_out = 0x03;
    rndis_devs[1].itf.msg_stage = RNDIS_MSG_STAGE_IDLE;

    usbd_handle_t *h1 = board_usbd_init(USBD1_INDEX, USBD1_INTERRUPT);
    assert(h1 != NULL);

    rndis_devs[1].h = h1;
    assert(usbd_drv_init(h1) == USB_RST_OK);
    assert(usbd_enable(h1) == USB_RST_OK);

    itf = &rndis_devs[1].itf;
    assert(usbd_register_event_callback(h1, USBD_CB_EVENT_RESET, reset_callback) == USB_RST_OK);
    assert(usbd_register_ctrl_callback(h1, 0x80, USB_REQ_CODE_GET_DESCRIPTOR, get_device_desc, NULL) == USB_RST_OK);
    assert(usbd_register_itf_callback(h1, itf->ctrl_itf, rndis_itf_setup, rndis_itf_status) == USB_RST_OK);
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

        rndis_dev_t *dev;

        for (uint8_t i = 0; i < sizeof(rndis_devs) / sizeof(rndis_dev_t); i++)
        {
            dev = &rndis_devs[i];

            if (usbd_is_enum(dev->h) != USB_TRUE) continue;

            command_handle(dev);

            status_upload_handle(dev);

            tranxfer_handle(dev);
        }
    }

    return 0;
}

void command_handle(rndis_dev_t *dev)
{
    if (dev->itf.msg_stage == RNDIS_MSG_STAGE_SETUP)
    {
        dev->itf.msg_stage = RNDIS_MSG_STAGE_IDLE;

        rndis_generic_t *msg = (rndis_generic_t *)dev->request_buf;

        switch (msg->MessageType)
        {
        case RNDIS_INITIALIZE_MSG:
        {
            dev->itf.init_status = USB_TRUE;
            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_initialize_msg_t *req = (rndis_initialize_msg_t *)dev->request_buf;
            rndis_initialize_cmplt_t *resp = (rndis_initialize_cmplt_t *)dev->response_buf;
            resp->MessageType = RNDIS_INITIALIZE_CMPLT;
            resp->MessageLength = sizeof(rndis_initialize_cmplt_t);
            resp->RequestId = req->RequestId;
            resp->Status = RNDIS_STATUS_SUCCESS;
            resp->MajorVersion = 0x00000001;
            resp->MinorVersion = 0x00000000;
            resp->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
            resp->Medium = RNDIS_MEDIUM_802_3;
            resp->MaxPacketsPerTransfer = 8;
            resp->MaxTransferSize = sizeof(dev->xfer_bufs[0]);
            resp->PacketAlignmentFactor = 2;
            resp->AfListOffset = 0;
            resp->AfListSize = 0;
            break;
        }

        case RNDIS_QUERY_MSG:
        {
            uint32_t status;
            uint32_t length;

            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_query_msg_t *req = (rndis_query_msg_t *)dev->request_buf;
            rndis_query_cmplt_t *resp = (rndis_query_cmplt_t *)dev->response_buf;
            rndis_oid_query(dev, dev->response_buf + sizeof(rndis_query_cmplt_t), req->Oid, &status, &length);
            resp->MessageType = RNDIS_QUERY_CMPLT;
            resp->MessageLength = sizeof(rndis_query_cmplt_t) + length;
            resp->RequestId = req->RequestId;
            resp->Status = status;
            resp->InformationBufferLength = length;
            resp->InformationBufferOffset = sizeof(rndis_query_cmplt_t) - sizeof(rndis_generic_t);
            break;
        }

        case RNDIS_SET_MSG:
        {
            uint32_t status;

            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_set_msg_t *req = (rndis_set_msg_t *)dev->request_buf;
            rndis_set_cmplt_t *resp = (rndis_set_cmplt_t *)dev->response_buf;
            rndis_oid_set(dev, dev->request_buf + sizeof(rndis_set_msg_t), req->Oid, &status,
                          req->InformationBufferLength);
            resp->MessageType = RNDIS_SET_CMPLT;
            resp->MessageLength = sizeof(rndis_set_cmplt_t);
            resp->RequestId = req->RequestId;
            resp->Status = status;
            break;
        }

        case RNDIS_RESET_MSG:
        {
            dev->itf.init_status = USB_FALSE;
            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_reset_cmplt_t *resp = (rndis_reset_cmplt_t *)dev->response_buf;
            resp->MessageType = RNDIS_RESET_CMPLT;
            resp->MessageLength = sizeof(rndis_reset_cmplt_t);
            resp->Status = RNDIS_STATUS_SUCCESS;
            resp->AddressingReset = 0x00000000;
            break;
        }

        case RNDIS_KEEPALIVE_MSG:
        {
            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_keepalive_msg_t *req = (rndis_keepalive_msg_t *)dev->request_buf;
            rndis_keepalive_cmplt_t *resp = (rndis_keepalive_cmplt_t *)dev->response_buf;
            resp->MessageType = RNDIS_KEEPALIVE_CMPLT;
            resp->MessageLength = sizeof(rndis_keepalive_cmplt_t);
            resp->RequestId = req->RequestId;
            resp->Status = RNDIS_STATUS_SUCCESS;
            break;
        }
        }
    }

    if (dev->itf.msg_stage == RNDIS_MSG_STAGE_NOTIFY)
    {
        if (usbd_upload(dev->h, dev->itf.ep_notif, response_available, sizeof(response_available)) == USB_RST_OK)
        {
            dev->itf.msg_stage = RNDIS_MSG_STAGE_STATUS;
        }
    }
}

void status_upload_handle(rndis_dev_t *dev)
{
    static usb_bool_t link_status = USB_FALSE;

    if (dev->itf.init_status && dev->itf.msg_stage == RNDIS_MSG_STAGE_IDLE)
    {
        usb_bool_t curr_status = usbd_is_enum(rndis_devs[0].h) && usbd_is_enum(rndis_devs[1].h) ? USB_TRUE : USB_FALSE;

        if (curr_status != link_status)
        {
            link_status = curr_status;

            dev->itf.msg_stage = RNDIS_MSG_STAGE_NOTIFY;
            rndis_indicate_status_t *resp = (rndis_indicate_status_t *)dev->response_buf;
            resp->MessageType = RNDIS_INDICATE_STATUS_MSG;
            resp->MessageLength = sizeof(rndis_indicate_status_t);
            resp->Status = curr_status ? RNDIS_STATUS_MEDIA_CONNECT : RNDIS_STATUS_MEDIA_DISCONNECT;
            resp->StatusBufferLength = 0;
            resp->StatusBufferOffset = 0;
        }
    }
}

void tranxfer_handle(rndis_dev_t *dev)
{
    if (dev->xfer_manage.count)
    {
        rndis_dev_t *dst_dev = dev == &rndis_devs[0] ? &rndis_devs[1] : &rndis_devs[0];

        pack_t *pack = &dev->xfer_packs[dev->xfer_manage.deal];

        if (pack->size >= dst_dev->bulk_size && usbd_is_burst_support(dst_dev->h) == USB_FALSE)
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

void rndis_oid_query(rndis_dev_t *dev, void *buf, uint32_t oid, uint32_t *status, uint32_t *length)
{
    switch (oid)
    {
    case OID_GEN_SUPPORTED_LIST:
    {
        memcpy(buf, supported_oids, sizeof(supported_oids));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(supported_oids);
        break;
    }

    case OID_GEN_HARDWARE_STATUS:
    {
        uint32_t hardware_status = 0x00000000;
        memcpy(buf, &hardware_status, sizeof(hardware_status));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(hardware_status);
        break;
    }

    case OID_GEN_MEDIA_SUPPORTED:
    {
        uint32_t media_supported = NDIS_MEDIUM_802_3;
        memcpy(buf, &media_supported, sizeof(media_supported));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(media_supported);
        break;
    }

    case OID_GEN_MEDIA_IN_USE:
    {
        uint32_t media_in_use = NDIS_MEDIUM_802_3;
        memcpy(buf, &media_in_use, sizeof(media_in_use));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(media_in_use);
        break;
    }

    case OID_GEN_MAXIMUM_FRAME_SIZE:
    {
        uint32_t maximum_frame_size = 1500;
        memcpy(buf, &maximum_frame_size, sizeof(maximum_frame_size));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(maximum_frame_size);
        break;
    }

    case OID_GEN_LINK_SPEED:
    {
        uint32_t link_speed = 480000000 / 100;
        memcpy(buf, &link_speed, sizeof(link_speed));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(link_speed);
        break;
    }

    case OID_GEN_TRANSMIT_BLOCK_SIZE:
    {
        uint32_t transmit_block_size = 1514;
        memcpy(buf, &transmit_block_size, sizeof(transmit_block_size));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(transmit_block_size);
        break;
    }

    case OID_GEN_RECEIVE_BLOCK_SIZE:
    {
        uint32_t receive_block_size = 1514;
        memcpy(buf, &receive_block_size, sizeof(receive_block_size));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(receive_block_size);
        break;
    }

    case OID_GEN_VENDOR_ID:
    {
        uint32_t vendor_id = 0x00FFFFFF;
        memcpy(buf, &vendor_id, sizeof(vendor_id));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(vendor_id);
        break;
    }

    case OID_GEN_VENDOR_DESCRIPTION:
    {
        const char *vendor_description = "WCH RNDIS Device";
        memcpy(buf, vendor_description, strlen(vendor_description) + 1);
        *status = RNDIS_STATUS_SUCCESS;
        *length = strlen(vendor_description) + 1;
        break;
    }

    case OID_GEN_VENDOR_DRIVER_VERSION:
    {
        uint32_t vendor_driver_version = 0x00001000;
        memcpy(buf, &vendor_driver_version, sizeof(vendor_driver_version));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(vendor_driver_version);
        break;
    }

    case OID_GEN_CURRENT_PACKET_FILTER:
    {
        uint32_t current_packet_filter = dev->itf.packet_filter;
        memcpy(buf, &current_packet_filter, sizeof(current_packet_filter));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(current_packet_filter);
        break;
    }

    case OID_GEN_MAXIMUM_TOTAL_SIZE:
    {
        uint32_t maximum_total_size = 1514;
        memcpy(buf, &maximum_total_size, sizeof(maximum_total_size));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(maximum_total_size);
        break;
    }

    case OID_GEN_MEDIA_CONNECT_STATUS:
    {
        uint32_t connect_status = usbd_is_enum(rndis_devs[0].h) && usbd_is_enum(rndis_devs[1].h) ?
                                  NDIS_MEDIA_STATE_CONNECTED : NDIS_MEDIA_STATE_DISCONNECTED;
        memcpy(buf, &connect_status, sizeof(connect_status));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(connect_status);
        break;
    }

    case OID_GEN_XMIT_OK:
    {
        uint32_t frames_xmit_ok = dev->xfer_count.tx_ok;
        memcpy(buf, &frames_xmit_ok, sizeof(frames_xmit_ok));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(frames_xmit_ok);
        break;
    }

    case OID_GEN_RCV_OK:
    {
        uint32_t frames_rcv_ok = dev->xfer_count.rx_ok;
        memcpy(buf, &frames_rcv_ok, sizeof(frames_rcv_ok));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(frames_rcv_ok);
        break;
    }

    case OID_GEN_XMIT_ERROR:
    {
        uint32_t xmit_error = dev->xfer_count.tx_err;
        memcpy(buf, &xmit_error, sizeof(xmit_error));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(xmit_error);
        break;
    }

    case OID_GEN_RCV_ERROR:
    {
        uint32_t rcv_error = dev->xfer_count.rx_err;
        memcpy(buf, &rcv_error, sizeof(rcv_error));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(rcv_error);
        break;
    }

    case OID_GEN_RCV_NO_BUFFER:
    {
        uint32_t rcv_no_buffer = 0;
        memcpy(buf, &rcv_no_buffer, sizeof(rcv_no_buffer));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(rcv_no_buffer);
        break;
    }

    case OID_802_3_PERMANENT_ADDRESS:
    case OID_802_3_CURRENT_ADDRESS:
    {
        uint8_t mac_addr[6] = {0};
        mac_addr[0] = 0x50;
        mac_addr[1] = 0x54;
        mac_addr[2] = 0x7B;
        mac_addr[3] = 0x43;
        mac_addr[4] = 0x84;
        mac_addr[5] = dev == &rndis_devs[0] ? 0x8A : 0x8B;
        memcpy(buf, mac_addr, sizeof(mac_addr));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(mac_addr);
        break;
    }

    case OID_802_3_MULTICAST_LIST:
    {
        uint32_t multicast_list = 0;
        memcpy(buf, &multicast_list, sizeof(multicast_list));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(multicast_list);
        break;
    }

    case OID_802_3_MAXIMUM_LIST_SIZE:
    {
        uint32_t maximum_list_size = 1;
        memcpy(buf, &maximum_list_size, sizeof(maximum_list_size));
        *status = RNDIS_STATUS_SUCCESS;
        *length = sizeof(maximum_list_size);
        break;
    }

    default:
    {
        *status = RNDIS_STATUS_NOT_SUPPORTED;
        *length = 0;
        break;
    }
    }
}

void rndis_oid_set(rndis_dev_t *dev, const void *buf, uint32_t oid, uint32_t *status, uint32_t length)
{
    *status = RNDIS_STATUS_FAILURE;

    switch (oid)
    {
    case OID_GEN_CURRENT_PACKET_FILTER:
    {
        if (length == sizeof(uint32_t))
        {
            *status = RNDIS_STATUS_SUCCESS;
            memcpy(&dev->itf.packet_filter, buf, length);
        }
        break;
    }

    case OID_GEN_SUPPORTED_LIST:
    case OID_GEN_HARDWARE_STATUS:
    case OID_GEN_MEDIA_SUPPORTED:
    case OID_GEN_MEDIA_IN_USE:
    case OID_GEN_MAXIMUM_FRAME_SIZE:
    case OID_GEN_LINK_SPEED:
    case OID_GEN_TRANSMIT_BLOCK_SIZE:
    case OID_GEN_RECEIVE_BLOCK_SIZE:
    case OID_GEN_VENDOR_ID:
    case OID_GEN_VENDOR_DESCRIPTION:
    case OID_GEN_VENDOR_DRIVER_VERSION:
    case OID_GEN_MAXIMUM_TOTAL_SIZE:
    case OID_GEN_MEDIA_CONNECT_STATUS:
    case OID_GEN_XMIT_OK:
    case OID_GEN_RCV_OK:
    case OID_GEN_XMIT_ERROR:
    case OID_GEN_RCV_ERROR:
    case OID_GEN_RCV_NO_BUFFER:
    case OID_802_3_PERMANENT_ADDRESS:
    case OID_802_3_CURRENT_ADDRESS:
    case OID_802_3_MULTICAST_LIST:
    case OID_802_3_MAXIMUM_LIST_SIZE:
        *status = RNDIS_STATUS_SUCCESS;
        break;
    }
}

void reset_callback(usbd_handle_t *h, uint32_t parameter)
{
    rndis_dev_t *dev = h == rndis_devs[0].h ? &rndis_devs[0] : &rndis_devs[1];

    dev->bulk_size = usbd_get_speed(h) == USB_SPEED_HIGH ? 512 : 64;

    dev->itf.init_status = USB_FALSE;
    dev->itf.msg_stage = RNDIS_MSG_STAGE_IDLE;
    memset(&dev->xfer_count, 0, sizeof(dev->xfer_count));

    memset(&dev->xfer_manage, 0, sizeof(dev->xfer_manage));
    memset(&dev->xfer_manage, 0, sizeof(dev->xfer_manage));

    usbd_endp_open(h, dev->itf.ep_notif, ENDP_TYPE_NORMAL, 64, NULL, ENDP_RESP_NAK);

    if (usbd_is_burst_support(h))
    {
        usbd_endp_open(h, dev->itf.ep_in, ENDP_TYPE_BURST, dev->bulk_size, NULL, ENDP_RESP_NAK);
        usbd_endp_open(h, dev->itf.ep_out, ENDP_TYPE_BURST, dev->bulk_size, dev->xfer_bufs[0], ENDP_RESP_ACK);
        usbd_endp_set_burst_size(h, dev->itf.ep_in, dev->bulk_size);
        usbd_endp_set_burst_size(h, dev->itf.ep_out, dev->bulk_size);
    }
    else
    {
        usbd_endp_open(h, dev->itf.ep_in, ENDP_TYPE_NORMAL, dev->bulk_size, NULL, ENDP_RESP_NAK);
        usbd_endp_open(h, dev->itf.ep_out, ENDP_TYPE_NORMAL, dev->bulk_size, dev->xfer_bufs[0], ENDP_RESP_ACK);
    }
}

usb_rst_e rndis_itf_setup(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size)
{
    rndis_dev_t *dev = h == rndis_devs[0].h ? &rndis_devs[0] : &rndis_devs[1];

    if (req->bRequest == 0x00)
    {
        *buf = dev->request_buf;
        *size = USB_MIN(sizeof(dev->response_buf), req->wLength);
        return USB_RST_OK;
    }

    if (req->bRequest == 0x01 && dev->itf.msg_stage == RNDIS_MSG_STAGE_STATUS)
    {
        *buf = dev->response_buf;
        rndis_generic_t *msg = (rndis_generic_t *)dev->response_buf;
        *size = USB_MIN(msg->MessageLength, req->wLength);
        return USB_RST_OK;
    }

    return USB_RST_FAILED;
}

usb_rst_e rndis_itf_status(usbd_handle_t *h, const usb_req_t *req, const void *buf, size_t size)
{
    rndis_dev_t *dev = h == rndis_devs[0].h ? &rndis_devs[0] : &rndis_devs[1];

    if (req->bRequest == 0x00 && dev->itf.msg_stage == RNDIS_MSG_STAGE_IDLE)
    {
        dev->itf.msg_stage = RNDIS_MSG_STAGE_SETUP;
    }

    if (req->bRequest == 0x01)
    {
        dev->itf.msg_stage = RNDIS_MSG_STAGE_IDLE;
    }

    return USB_RST_OK;
}

endp_resp_e download_callback(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size)
{
    rndis_dev_t *dev = h == rndis_devs[0].h ? &rndis_devs[0] : &rndis_devs[1];

    uint8_t load = dev->xfer_manage.load;
    pack_t *pack = &dev->xfer_packs[load];
    if (size >= dev->bulk_size && pack->size + dev->bulk_size < sizeof(dev->xfer_bufs[0]) &&
        usbd_is_burst_support(h) == USB_FALSE)
    {
        pack->size += size;
        usbd_endp_set_buf(h, dev->itf.ep_out, dev->xfer_bufs[load] + pack->size);
        return ENDP_RESP_ACK;
    }
    else
    {
        pack->addr = dev->xfer_bufs[load];
        pack->size += size;
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
}
