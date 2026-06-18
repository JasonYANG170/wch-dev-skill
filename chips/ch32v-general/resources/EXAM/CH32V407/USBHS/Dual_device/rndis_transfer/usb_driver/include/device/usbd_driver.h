/********************************** (C) COPYRIGHT *******************************
* File Name          : usbd_driver.h
* Author             : WCH
* Version            : V1.1
* Date               : 2026/01/14
* Description        : Usb device driver headfile.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __USBD_DRIVER_H_
#define __USBD_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "usb_define.h"
#include "device/usbd_config.h"

/* @define */
#define USB_DEVICE_DRIVER_VERSION           (0x0101)

#ifdef USBD_DRIVER_LOG_INFO
#define USBD_LOGI(format, ...)              printf("[USBD INFO]: " format "\r\n", ##__VA_ARGS__)
#else
#define USBD_LOGI(format, ...)
#endif

#ifdef USBD_DRIVER_LOG_WARNING
#define USBD_LOGW(format, ...)              printf("[USBD WARNING]: " format "\r\n", ##__VA_ARGS__)
#else
#define USBD_LOGW(format, ...)
#endif

#ifdef USBD_DRIVER_LOG_ERROR
#define USBD_LOGE(format, ...)              printf("[USBD ERROR]: " format "\r\n", ##__VA_ARGS__)
#else
#define USBD_LOGE(format, ...)
#endif

/* @enum */
typedef enum
{
    USBD_EVENT_NONE,
    USBD_EVENT_RESET,
    USBD_EVENT_SUSPEND,
    USBD_EVENT_SOF,
    USBD_EVENT_SETUP,
    USBD_EVENT_XFER,
    USBD_EVENT_COUNT,
} usbd_event_e;

typedef enum
{
    USBD_CB_EVENT_RESET,
    USBD_CB_EVENT_SUSPEND,
    USBD_CB_EVENT_SOF,
    USBD_CB_EVENT_ENUM_COMPLETED,
    USBD_CB_EVENT_COUNT,
} usbd_cb_event_e;

/* @struct */
typedef struct
{
    usbd_event_e e;

    union
    {
        struct
        {
            usb_speed_e link_speed;
        } reset;

        struct
        {
            uint32_t frame_no;
        } sof;

        struct
        {
            usb_endp_t ep;
            size_t size;
            void *buf;
        } xfer;
    };
} usbd_event_t;

typedef struct
{
    uint8_t address;
    uint8_t link_speed;
    uint8_t config_num;
    usb_bool_t test_mode;
    usb_bool_t self_powered;
    usb_bool_t burst_support;
    usb_bool_t remote_wakeup_enable;
    usb_bool_t remote_wakeup_support;
    uint16_t ep0_size;
} usbd_feature_t;

typedef struct usbd_handle usbd_handle_t;

typedef void (*event_cb)(usbd_handle_t *h, uint32_t parameter);
typedef usb_rst_e (*ctrl_setup_cb)(usbd_handle_t *h, const usb_req_t *req, void **buf, size_t *size);
typedef usb_rst_e (*ctrl_status_cb)(usbd_handle_t *h, const usb_req_t *req, const void *buf, size_t size);
typedef endp_resp_e (*data_xfer_cb)(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size);

typedef struct
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    ctrl_setup_cb setup;
    ctrl_status_cb status;
} usbd_ctrl_item_t;

typedef struct
{
    ctrl_setup_cb setup;
    ctrl_status_cb status;
} usbd_itf_item_t;

typedef struct
{
    size_t count;
    usbd_ctrl_item_t items[USBD_CTRL_TABLE_MAX_ITEMS];
} usbd_ctrl_table_t;

typedef struct
{
    uint8_t dir;
    uint8_t tog;
    uint8_t stage;
    size_t size;
    size_t offset;
    void *buf_ptr;
    ctrl_status_cb cb;
    uint32_t temp_data;
    usb_req_t req;
    __attribute__((aligned(4))) uint8_t buf[64];
} usbd_ctrl_ctx_t;

struct usbd_handle
{
    usb_bool_t enable_status;
    usb_bool_t open_status;
    usb_bool_t enum_status;

    usbd_feature_t feature;

    usbd_ctrl_ctx_t ctrl_ctx;
    usbd_ctrl_table_t ctrl_table;
    usbd_itf_item_t itf_table[USBD_ITF_TABLE_MAX_ITEMS];

    uint8_t in_sta[16];
    uint8_t out_sta[16];

    uint8_t in_tog[16];
    uint8_t out_tog[16];

    event_cb event_cbs[USBD_CB_EVENT_COUNT];

    data_xfer_cb upload_cbs[16];
    data_xfer_cb download_cbs[16];

    usb_rst_e (*enable)(void);
    usb_rst_e (*disable)(void);

    usb_rst_e (*open)(void);
    usb_rst_e (*close)(void);

    usb_rst_e (*get_event)(usbd_event_t *e);
    usb_rst_e (*clear_event)(usbd_event_t *e);

    usb_rst_e (*resume)(void);

    usb_rst_e (*set_address)(uint8_t addr);
    usb_rst_e (*interrupt_ctrl)(usb_bool_t status);

    usb_rst_e (*endp_open)(usb_endp_t ep, endp_type_e type);
    usb_rst_e (*endp_close)(usb_endp_t ep);

    usb_rst_e (*set_endp_status)(usb_endp_t ep, endp_resp_e resp, endp_tog_e tog);
    usb_rst_e (*set_endp_size)(usb_endp_t ep, size_t size);
    usb_rst_e (*set_endp_buf)(usb_endp_t ep, const void *buf);
    usb_rst_e (*set_endp_burst_size)(usb_endp_t ep, size_t size);

    usb_rst_e (*get_endp_status)(usb_endp_t ep, endp_resp_e *resp, endp_tog_e *tog);
    usb_rst_e (*get_endp_size)(usb_endp_t ep, size_t *size);
    usb_rst_e (*get_endp_buf)(usb_endp_t ep, void **buf);
    usb_rst_e (*get_endp_burst_size)(usb_endp_t ep, size_t *size);
};

/* @function declaration */

/**
 * @brief Usb device driver initialization.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 * @note Make sure the handle has been initialized before executing this function.
 */
usb_rst_e usbd_drv_init(usbd_handle_t *h);

/**
 * @brief Usb device driver core task.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 * @note This function can be executed in an interrupt or within a loop.
 */
usb_rst_e usbd_drv_task(usbd_handle_t *h);

/**
 * @brief Enable usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_enable(usbd_handle_t *h);

/**
 * @brief disable usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_disable(usbd_handle_t *h);

/**
 * @brief Open usb device controller.
 *
 * @param h Usb device driver handle object.
 * @param self_power Usb device self power feature support flag.
 * @param remote_wakeup Usb device remote wakeup feature support flag.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_open(usbd_handle_t *h, usb_bool_t self_power, usb_bool_t remote_wakeup);

/**
 * @brief Close usb device controller.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_close(usbd_handle_t *h);

/**
 * @brief Usb device driver enable status.
 *
 * @param h Usb device driver handle object.
 * @return usb_bool_t USB_FALSE: is not enabled, USB_TRUE: is enabled.
 */
usb_bool_t usbd_is_enable(usbd_handle_t *h);

/**
 * @brief Usb device driver open status.
 *
 * @param h Usb device driver handle object.
 * @return usb_bool_t USB_FALSE: is not opened, USB_TRUE: is opened.
 */
usb_bool_t usbd_is_open(usbd_handle_t *h);

/**
 * @brief Usb device driver enumeration status.
 *
 * @param h Usb device driver handle object.
 * @return usb_bool_t USB_FALSE: is not enumerated, USB_TRUE: is enumerated.
 */
usb_bool_t usbd_is_enum(usbd_handle_t *h);

/**
 * @brief Usb device controller support endpoint for burst.
 *
 * @param h Usb device driver handle object.
 * @return usb_bool_t USB_FALSE: is not support, USB_TRUE: is support.
 */
usb_bool_t usbd_is_burst_support(usbd_handle_t *h);

/**
 * @brief Usb device connection speed.
 *
 * @param h Usb device driver handle object.
 * @return usb_speed_e Return usb device connection speed enumeration.
 */
usb_speed_e usbd_get_speed(usbd_handle_t *h);

/**
 * @brief Send resume signal.
 *
 * @param h Usb device driver handle object.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_resume(usbd_handle_t *h);

/**
 * @brief Usb device interrupt control.
 *
 * @param h Usb device driver handle object.
 * @param status Interrupt status.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_interrupt_ctrl(usbd_handle_t *h, usb_bool_t status);

/**
 * @brief Open usb controller endpoint.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param type Endpoint type.
 * @param size Endpoint size.
 * @param buf Endpoint data buffer.
 * @param resp Endpoint response.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_open(usbd_handle_t *h, usb_endp_t ep, endp_type_e type, size_t size, const void *buf,
                         endp_resp_e resp);

/**
 * @brief Close usb controller endpoint.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_close(usbd_handle_t *h, usb_endp_t ep);

/**
 * @brief Set endpoint status.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param resp Endpoint response.
 * @param tog Endpoint toggle.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_status(usbd_handle_t *h, usb_endp_t ep, endp_resp_e resp, endp_tog_e tog);

/**
 * @brief Set endpoint size.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param size Endpoint size.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_size(usbd_handle_t *h, usb_endp_t ep, size_t size);

/**
 * @brief Set endpoint buffer.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Endpoint data buffer.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_buf(usbd_handle_t *h, usb_endp_t ep, const void *buf);

/**
 * @brief Set endpoint burst size.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param size Endpoint burst size.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_endp_set_burst_size(usbd_handle_t *h, usb_endp_t ep, size_t size);

/**
 * @brief Register usb device driver event callback.
 *
 * @param h Usb device driver handle object.
 * @param event Usb device driver event type.
 * @param cb Callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_register_event_callback(usbd_handle_t *h, usbd_cb_event_e event, event_cb cb);

/**
 * @brief Register usb device driver control transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param bmRequestType Setup packet bmRequestType field.
 * @param bRequest Setup packet bRequest field.
 * @param setup Setup stage callback function.
 * @param status Status stage callback function.
 * @return usb_rst_e Execution result.
 * @note If the setup callback function does not return USB_RST_OK, the driver will respond with a STALL.
 */
usb_rst_e usbd_register_ctrl_callback(usbd_handle_t *h, uint8_t bmRequestType, uint8_t bRequest, ctrl_setup_cb setup,
                                      ctrl_status_cb status);

/**
 * @brief Register usb device driver interface control transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param itf Interface number.
 * @param setup Setup stage callback function.
 * @param status Status stage callback function.
 * @return usb_rst_e Execution result.
 * @note If the setup callback function does not return USB_RST_OK, the driver will respond with a STALL.
 */
usb_rst_e usbd_register_itf_callback(usbd_handle_t *h, uint16_t itf, ctrl_setup_cb setup, ctrl_status_cb status);

/**
 * @brief Register usb device driver data transfer callback.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param cb Data transfer callback function.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_register_data_callback(usbd_handle_t *h, usb_endp_t ep, data_xfer_cb cb);

/**
 * @brief Usb device driver data upload.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Data buffer.
 * @param size Expected data length to send.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_upload(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size);

/**
 * @brief Usb device driver data download.
 *
 * @param h Usb device driver handle object.
 * @param ep Endpoint number.
 * @param buf Data buffer.
 * @param size Expected data length to receive.
 * @return usb_rst_e Execution result.
 */
usb_rst_e usbd_download(usbd_handle_t *h, usb_endp_t ep, const void *buf, size_t size);

/**
 * @brief Get usb device driver version.
 *
 * @return uint16_t Usb device driver bcd version.
 */
uint16_t usbd_get_version(void);

#ifdef __cplusplus
}
#endif

#endif
