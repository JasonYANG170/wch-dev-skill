/********************************** (C) COPYRIGHT *******************************
* File Name          : msc.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/10/30
* Description        : Msc class define.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __MSC_H_
#define __MSC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @enum */
typedef enum
{
    MSC_SIGNATURE_CBW = 0x43425355,
    MSC_SIGNATURE_CSW = 0x53425355,
} msc_signature_e;

typedef enum
{
    MSC_REQ_GET_MAX_LUN = 0xFE,
    MSC_REQ_RESET       = 0xFF,
} msc_request_e;

typedef enum
{
    CSW_STATUS_PASSED,
    CSW_STATUS_FAILED,
    CSW_STATUS_PHASE_ERROR,
} csw_status_w;

typedef enum
{
    SCSI_CMD_TEST_UNIT_READY                 = 0x00,
    SCSI_CMD_INQUIRY                         = 0x12,
    SCSI_CMD_MODE_SELECT_6                   = 0x15,
    SCSI_CMD_MODE_SENSE_6                    = 0x1A,
    SCSI_CMD_START_STOP_UNIT                 = 0x1B,
    SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL    = 0x1E,
    SCSI_CMD_READ_CAPACITY_10                = 0x25,
    SCSI_CMD_REQUEST_SENSE                   = 0x03,
    SCSI_CMD_READ_FORMAT_CAPACITY            = 0x23,
    SCSI_CMD_READ_10                         = 0x28,
    SCSI_CMD_WRITE_10                        = 0x2A,
    SCSI_CMD_READ_TOC_PMA_ATIP               = 0x43,
    SCSI_CMD_READ_HEADER                     = 0x44,
    SCSI_CMD_GET_CONFIGURATION               = 0x46,
    SCSI_CMD_GET_EVENT_STATUS_NOTIFICATION   = 0x4A,
    SCSI_CMD_READ_DISC_INFORMATION           = 0x51,
    SCSI_CMD_READ_TRACK_INFORMATION          = 0x52,
    SCSI_CMD_MODE_SENSE_10                   = 0x5A,
} scsi_cmd_e;

typedef enum
{
    SCSI_SENSE_NONE            = 0x00,
    SCSI_SENSE_RECOVERED_ERROR = 0x01,
    SCSI_SENSE_NOT_READY       = 0x02,
    SCSI_SENSE_MEDIUM_ERROR    = 0x03,
    SCSI_SENSE_HARDWARE_ERROR  = 0x04,
    SCSI_SENSE_ILLEGAL_REQUEST = 0x05,
    SCSI_SENSE_UNIT_ATTENTION  = 0x06,
    SCSI_SENSE_DATA_PROTECT    = 0x07,
    SCSI_SENSE_FIRMWARE_ERROR  = 0x08,
    SCSI_SENSE_ABORTED_COMMAND = 0x0B,
    SCSI_SENSE_EQUAL           = 0x0C,
    SCSI_SENSE_VOLUME_OVERFLOW = 0x0D,
    SCSI_SENSE_MISCOMPARE      = 0x0E,
} scsi_sense_key_type_e;

typedef enum
{
    PERIPHERAL_DEVICE_TYPE_CODE_U_DISK       = 0x00,
    PERIPHERAL_DEVICE_TYPE_CODE_CD_ROM       = 0x05,
} peripheral_device_type_code_e;

/* @struct */

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
} msc_cbw_t;

typedef struct
{
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
} msc_csw_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t lun;
    uint8_t reserved[3];
    uint8_t control;
} scsi_test_unit_ready_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t reserved1;
    uint8_t page_code;
    uint8_t reserved2;
    uint8_t alloc_length;
    uint8_t control;
} inquiry_command_t;

typedef struct
{
    uint8_t peripheral_device_type : 5;
    uint8_t peripheral_qualifier : 3;

    uint8_t : 7;
    uint8_t is_removable : 1;

    uint8_t version;

    uint8_t response_data_format : 4;
    uint8_t hierarchical_support : 1;
    uint8_t normal_aca : 1;
    uint8_t : 2;

    uint8_t additional_length;

    uint8_t protect : 1;
    uint8_t : 2;
    uint8_t third_party_copy : 1;
    uint8_t target_port_group_support : 2;
    uint8_t access_control_coordinator : 1;
    uint8_t scc_support : 1;

    uint8_t addr16 : 1;
    uint8_t : 3;
    uint8_t multi_port : 1;
    uint8_t : 1;
    uint8_t enclosure_service : 1;
    uint8_t : 1;

    uint8_t : 1;
    uint8_t cmd_que : 1;
    uint8_t : 2;
    uint8_t sync : 1;
    uint8_t wbus16 : 1;
    uint8_t : 2;

    uint8_t vendor_id[8];
    uint8_t product_id[16];
    uint8_t product_rev[4];
} inquiry_response_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t reserved[6];
    uint8_t alloc_length_msb;
    uint8_t alloc_length_lsb;
    uint8_t control;
} read_format_capacity_command_t;

typedef struct
{
    uint8_t reserved[3];
    uint8_t list_length;

    uint32_t block_num;
    uint8_t descriptor_type;

    uint8_t reserved2;
    uint16_t block_size_u16;
} read_format_capacity_response_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t reserved1;
    uint32_t lba;
    uint16_t reserved2;
    uint8_t partial_medium_indicator;
    uint8_t control;
} read_capacity10_command_t;

typedef struct
{
    uint32_t last_lba;
    uint32_t block_size;
} read_capacity10_response_t;

typedef struct
{
    uint8_t cmd_code;

    uint8_t : 3;
    uint8_t disable_block_descriptor : 1;
    uint8_t : 4;

    uint8_t page_code : 6;
    uint8_t page_control : 2;

    uint8_t subpage_code;
    uint8_t alloc_length;
    uint8_t control;
} mode_sense6_command_t;

typedef struct
{
    uint8_t data_len;
    uint8_t medium_type;

    uint8_t reserved : 7;
    uint8_t write_protected : 1;

    uint8_t block_descriptor_len;
} mode_sense6_response_t;

typedef struct
{
    uint8_t response_code : 7;
    uint8_t valid : 1;

    uint8_t reserved;

    uint8_t sense_key : 4;
    uint8_t : 1;
    uint8_t ili : 1;
    uint8_t end_of_medium : 1;
    uint8_t filemark : 1;

    uint32_t information;
    uint8_t add_sense_len;
    uint32_t command_specific_info;
    uint8_t add_sense_code;
    uint8_t add_sense_qualifier;
    uint8_t field_replaceable_unit_code;

    uint8_t sense_key_specific[3];
} sense_fixed_response_t;

typedef struct
{
    uint8_t cmd_code;

    uint8_t immded : 1;
    uint8_t : 7;

    uint8_t TU_RESERVED;

    uint8_t power_condition_mod : 4;
    uint8_t : 4;

    uint8_t start : 1;
    uint8_t load_eject : 1;
    uint8_t no_flush : 1;
    uint8_t : 1;
    uint8_t power_condition : 4;

    uint8_t control;
} start_stop_command_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t reserved;
    uint32_t lba;
    uint8_t reserved2;
    uint16_t block_count;
    uint8_t control;
} read10_t, write10_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
