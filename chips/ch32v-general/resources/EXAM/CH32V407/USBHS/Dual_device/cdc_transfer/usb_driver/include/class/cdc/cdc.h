/********************************** (C) COPYRIGHT *******************************
* File Name          : cdc.h
* Author             : WCH
* Version            : V1.0
* Date               : 2025/08/18
* Description        : CDC class define.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CDC_H_
#define __CDC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @define */
#define CDC_REQ_SEND_ENCAPSULATED_COMMAND                               0x00
#define CDC_REQ_GET_ENCAPSULATED_RESPONSE                               0x01
#define CDC_REQ_SET_COMM_FEATURE                                        0x02
#define CDC_REQ_GET_COMM_FEATURE                                        0x03
#define CDC_REQ_CLEAR_COMM_FEATURE                                      0x04

#define CDC_REQ_SET_AUX_LINE_STATE                                      0x10
#define CDC_REQ_SET_HOOK_STATE                                          0x11
#define CDC_REQ_PULSE_SETUP                                             0x12
#define CDC_REQ_SEND_PULSE                                              0x13
#define CDC_REQ_SET_PULSE_TIME                                          0x14
#define CDC_REQ_RING_AUX_JACK                                           0x15

#define CDC_REQ_SET_LINE_CODING                                         0x20
#define CDC_REQ_GET_LINE_CODING                                         0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE                                  0x22
#define CDC_REQ_SEND_BREAK                                              0x23

#define CDC_REQ_SET_RINGER_PARMS                                        0x30
#define CDC_REQ_GET_RINGER_PARMS                                        0x31
#define CDC_REQ_SET_OPERATION_PARMS                                     0x32
#define CDC_REQ_GET_OPERATION_PARMS                                     0x33
#define CDC_REQ_SET_LINE_PARMS                                          0x34
#define CDC_REQ_GET_LINE_PARMS                                          0x35
#define CDC_REQ_DIAL_DIGITS                                             0x36
#define CDC_REQ_SET_UNIT_PARAMETER                                      0x37
#define CDC_REQ_GET_UNIT_PARAMETER                                      0x38
#define CDC_REQ_CLEAR_UNIT_PARAMETER                                    0x39
#define CDC_REQ_GET_PROFILE                                             0x3A

#define CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS                          0x40
#define CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER            0x41
#define CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER            0x42
#define CDC_REQ_SET_ETHERNET_PACKET_FILTER                              0x43
#define CDC_REQ_GET_ETHERNET_STATISTIC                                  0x44

#define CDC_REQ_SET_ATM_DATA_FORMAT                                     0x50
#define CDC_REQ_GET_ATM_DEVICE_STATISTICS                               0x51
#define CDC_REQ_SET_ATM_DEFAULT_VC                                      0x52
#define CDC_REQ_GET_ATM_VC_STATISTICS                                   0x53

#define CDC_REQ_GET_NTB_PARAMETERS                                      0x80
#define CDC_REQ_GET_NET_ADDRESS                                         0x81
#define CDC_REQ_SET_NET_ADDRESS                                         0x82
#define CDC_REQ_GET_NTB_FORMAT                                          0x83
#define CDC_REQ_SET_NTB_FORMAT                                          0x84
#define CDC_REQ_GET_NTB_INPUT_SIZE                                      0x85
#define CDC_REQ_SET_NTB_INPUT_SIZE                                      0x86
#define CDC_REQ_GET_MAX_DATAGRAM_SIZE                                   0x87
#define CDC_REQ_SET_MAX_DATAGRAM_SIZE                                   0x88
#define CDC_REQ_GET_CRC_MODE                                            0x89
#define CDC_REQ_SET_CRC_MODE                                            0x8A

#define CDC_REQ_MDLM_SEMANTIC_MODEL                                     0x60

#define CDC_ETH_NOTIFY_CODE_NETWORK_CONNECTION                          0x00
#define CDC_ETH_NOTIFY_CODE_RESPONSE_AVAILABLE                          0x01
#define CDC_ETH_NOTIFY_CODE_CONNECTION_SPEED_CHANGE                     0x2A

#define CDC_ECM_PACKET_FILTER_PROMISCUOUS                               0x01
#define CDC_ECM_PACKET_FILTER_ALL_MULTICAST                             0x02
#define CDC_ECM_PACKET_FILTER_DIRECTED                                  0x04
#define CDC_ECM_PACKET_FILTER_BROADCAST                                 0x08
#define CDC_ECM_PACKET_FILTER_MULTICAST                                 0x10
#define CDC_ECM_PACKET_FILTER_MASK                                      0x1F

#define CDC_NTH16_SIGNATURE                                             0x484D434E
#define CDC_NTH32_SIGNATURE                                             0x686D636E
#define CDC_NDP16_SIGNATURE0                                            0x304D434E
#define CDC_NDP16_SIGNATURE1                                            0x314D434E
#define CDC_NDP32_SIGNATURE0                                            0x306D636E
#define CDC_NDP32_SIGNATURE1                                            0x316D636E

/* @enum */
typedef enum
{
    CDC_CONTROL_LINE_STATE_DTR = 0x01,
    CDC_CONTROL_LINE_STATE_RTS = 0x02,
} cdc_control_line_state_t;

typedef enum
{
    CDC_LINE_CODING_STOP_BITS_1,
    CDC_LINE_CODING_STOP_BITS_1_5,
    CDC_LINE_CODING_STOP_BITS_2,
} cdc_line_coding_stopbits_t;

typedef enum
{
    CDC_LINE_CODING_PARITY_NONE,
    CDC_LINE_CODING_PARITY_ODD,
    CDC_LINE_CODING_PARITY_EVEN,
    CDC_LINE_CODING_PARITY_MARK,
    CDC_LINE_CODING_PARITY_SPACE,
} cdc_line_coding_parity_t;

/* @struct */

/* Start single-byte alignment */
#pragma pack(1)

typedef struct
{
    uint32_t bit_rate;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t data_bits;
} cdc_line_coding_t;

typedef struct
{
    uint8_t bmRequestType;
    uint8_t bNotificationType;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint8_t data[8];
} cdc_eth_notif_t;

typedef struct
{
    uint16_t wLength;
    uint16_t bmNtbFormatsSupported;
    uint32_t dwNtbInMaxSize;
    uint16_t wNdbInDivisor;
    uint16_t wNdbInPayloadRemainder;
    uint16_t wNdbInAlignment;
    uint16_t wReserved;
    uint32_t dwNtbOutMaxSize;
    uint16_t wNdbOutDivisor;
    uint16_t wNdbOutPayloadRemainder;
    uint16_t wNdbOutAlignment;
    uint16_t wNtbOutMaxDatagrams;
} cdc_ntb_parameters_t;

typedef struct
{
    uint32_t dwSignature;
    uint16_t wHeaderLength;
    uint16_t wSequence;
    uint16_t wBlockLength;
    uint16_t wNdpIndex;
} cdc_nth16_t;

typedef struct
{
    uint32_t dwSignature;
    uint16_t wHeaderLength;
    uint16_t wSequence;
    uint32_t dwBlockLength;
    uint32_t dwNdpIndex;
} cdc_nth32_t;

typedef struct
{
    uint16_t wDatagramIndex;
    uint16_t wDatagramLength;
} cdc_ndp16_datagram_t;

typedef struct
{
    uint32_t dwSignature;
    uint16_t wLength;
    uint16_t wNextNdpIndex;
    cdc_ndp16_datagram_t datagram[];
} cdc_ndp16_t;

typedef struct
{
    uint32_t dwDatagramIndex;
    uint32_t dwDatagramLength;
} cdc_ndp32_datagram_t;

typedef struct
{
    uint32_t dwSignature;
    uint16_t wLength;
    uint16_t wReserved6;
    uint32_t dwNextNdpIndex;
    uint32_t dwReserved12;
    cdc_ndp32_datagram_t datagram[];
} cdc_ndp32_t;

/* End single-byte alignment */
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
