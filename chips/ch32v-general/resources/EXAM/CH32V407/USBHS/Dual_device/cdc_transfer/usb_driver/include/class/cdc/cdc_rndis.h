/********************************** (C) COPYRIGHT *******************************
* File Name          : cdc_rndis.h
* Author             : WCH
* Version            : V1.0
* Date               : 2026/01/29
* Description        : CDC RNDIS class define.
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CDC_RNDIS_H_
#define __CDC_RNDIS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @include */
#include <stdint.h>

/* @define */
#define RNDIS_RESPONSE_AVAILABLE                        0x00000001

#define RNDIS_DF_CONNECTIONLESS                         0x00000001
#define RNDIS_DF_CONNECTION_ORIENTED                    0x00000002

#define RNDIS_MEDIUM_802_3                              0x00000000

#define RNDIS_STATUS_SUCCESS                            0x00000000
#define RNDIS_STATUS_FAILURE                            0xC0000001
#define RNDIS_STATUS_INVALID_DATA                       0xC0010015
#define RNDIS_STATUS_NOT_SUPPORTED                      0xC00000BB
#define RNDIS_STATUS_MEDIA_CONNECT                      0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT                   0x4001000C

#define RNDIS_PACKET_MSG                                0x00000001
#define RNDIS_INITIALIZE_MSG                            0x00000002
#define RNDIS_INITIALIZE_CMPLT                          0x80000002
#define RNDIS_HALT_MSG                                  0x00000003
#define RNDIS_QUERY_MSG                                 0x00000004
#define RNDIS_QUERY_CMPLT                               0x80000004
#define RNDIS_SET_MSG                                   0x00000005
#define RNDIS_SET_CMPLT                                 0x80000005
#define RNDIS_RESET_MSG                                 0x00000006
#define RNDIS_RESET_CMPLT                               0x80000006
#define RNDIS_INDICATE_STATUS_MSG                       0x00000007
#define RNDIS_KEEPALIVE_MSG                             0x00000008
#define RNDIS_KEEPALIVE_CMPLT                           0x80000008

#define RNDIS_RESPONSE_AVAILABLE                        0x00000001

#define NDIS_MEDIA_STATE_CONNECTED                      0x00000000
#define NDIS_MEDIA_STATE_DISCONNECTED                   0x00000001

#define NDIS_STATUS_MULTICAST_FULL                      0xC0010009
#define NDIS_STATUS_MULTICAST_EXISTS                    0xC001000A
#define NDIS_STATUS_MULTICAST_NOT_FOUND                 0xC001000B

#define NDIS_DEVICE_WAKE_UP_ENABLE                      0x00000001
#define NDIS_DEVICE_WAKE_ON_PATTERN_MATCH_ENABLE        0x00000002
#define NDIS_DEVICE_WAKE_ON_MAGIC_PACKET_ENABLE         0x00000004

#define NDIS_MINIPORT_BUS_MASTER                        0x00000001
#define NDIS_MINIPORT_WDM_DRIVER                        0x00000002
#define NDIS_MINIPORT_SG_LIST                           0x00000004
#define NDIS_MINIPORT_SUPPORTS_MEDIA_QUERY              0x00000008
#define NDIS_MINIPORT_INDICATES_PACKETS                 0x00000010
#define NDIS_MINIPORT_IGNORE_PACKET_QUEUE               0x00000020
#define NDIS_MINIPORT_IGNORE_REQUEST_QUEUE              0x00000040
#define NDIS_MINIPORT_IGNORE_TOKEN_RING_ERRORS          0x00000080
#define NDIS_MINIPORT_INTERMEDIATE_DRIVER               0x00000100
#define NDIS_MINIPORT_IS_NDIS_5                         0x00000200
#define NDIS_MINIPORT_IS_CO                             0x00000400
#define NDIS_MINIPORT_DESERIALIZE                       0x00000800
#define NDIS_MINIPORT_REQUIRES_MEDIA_POLLING            0x00001000
#define NDIS_MINIPORT_SUPPORTS_MEDIA_SENSE              0x00002000
#define NDIS_MINIPORT_NETBOOT_CARD                      0x00004000
#define NDIS_MINIPORT_PM_SUPPORTED                      0x00008000
#define NDIS_MINIPORT_SUPPORTS_MAC_ADDRESS_OVERWRITE    0x00010000
#define NDIS_MINIPORT_USES_SAFE_BUFFER_APIS             0x00020000
#define NDIS_MINIPORT_HIDDEN                            0x00040000
#define NDIS_MINIPORT_SWENUM                            0x00080000
#define NDIS_MINIPORT_SURPRISE_REMOVE_OK                0x00100000
#define NDIS_MINIPORT_NO_HALT_ON_SUSPEND                0x00200000
#define NDIS_MINIPORT_HARDWARE_DEVICE                   0x00400000
#define NDIS_MINIPORT_SUPPORTS_CANCEL_SEND_PACKETS      0x00800000
#define NDIS_MINIPORT_64BITS_DMA                        0x01000000

#define NDIS_MEDIUM_802_3                               0x00000000
#define NDIS_MEDIUM_802_5                               0x00000001
#define NDIS_MEDIUM_FDDI                                0x00000002
#define NDIS_MEDIUM_WAN                                 0x00000003
#define NDIS_MEDIUM_LOCAL_TALK                          0x00000004
#define NDIS_MEDIUM_DIX                                 0x00000005
#define NDIS_MEDIUM_ARCENT_RAW                          0x00000006
#define NDIS_MEDIUM_ARCENT_878_2                        0x00000007
#define NDIS_MEDIUM_ATM                                 0x00000008
#define NDIS_MEDIUM_WIRELESS_LAN                        0x00000009
#define NDIS_MEDIUM_IRDA                                0x0000000A
#define NDIS_MEDIUM_BPC                                 0x0000000B
#define NDIS_MEDIUM_CO_WAN                              0x0000000C
#define NDIS_MEDIUM_1394                                0x0000000D

#define NDIS_PACKET_TYPE_DIRECTED                       0x00000001
#define NDIS_PACKET_TYPE_MULTICAST                      0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST                  0x00000004
#define NDIS_PACKET_TYPE_BROADCAST                      0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING                 0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS                    0x00000020
#define NDIS_PACKET_TYPE_SMT                            0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL                      0x00000080
#define NDIS_PACKET_TYPE_GROUP                          0x00000100
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL                 0x00000200
#define NDIS_PACKET_TYPE_FUNCTIONAL                     0x00000400
#define NDIS_PACKET_TYPE_MAC_FRAME                      0x00000800

#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA             0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED              0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND              0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK                     0x00000008
#define NDIS_MAC_OPTION_FULL_DUPLEX                     0x00000010
#define NDIS_MAC_OPTION_EOTX_INDICATION                 0x00000020
#define NDIS_MAC_OPTION_8021P_PRIORITY                  0x00000040
#define NDIS_MAC_OPTION_RESERVED                        0x80000000

#define NDIS_802_3_MAC_OPTION_PRIORITY                  0x00000001

#define OID_PNP_CAPABILITIES                            0xFD010100
#define OID_PNP_SET_POWER                               0xFD010101
#define OID_PNP_QUERY_POWER                             0xFD010102
#define OID_PNP_ADD_WAKE_UP_PATTERN                     0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN                  0xFD010104
#define OID_PNP_ENABLE_WAKE_UP                          0xFD010106

#define OID_GEN_SUPPORTED_LIST                          0x00010101
#define OID_GEN_HARDWARE_STATUS                         0x00010102
#define OID_GEN_MEDIA_SUPPORTED                         0x00010103
#define OID_GEN_MEDIA_IN_USE                            0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD                       0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE                      0x00010106
#define OID_GEN_LINK_SPEED                              0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE                   0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE                    0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE                     0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE                      0x0001010B
#define OID_GEN_VENDOR_ID                               0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION                      0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER                   0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD                       0x0001010F
#define OID_GEN_DRIVER_VERSION                          0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE                      0x00010111
#define OID_GEN_PROTOCOL_OPTIONS                        0x00010112
#define OID_GEN_MAC_OPTIONS                             0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS                    0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS                    0x00010115
#define OID_GEN_VENDOR_DRIVER_VERSION                   0x00010116
#define OID_GEN_SUPPORTED_GUIDS                         0x00010117
#define OID_GEN_NETWORK_LAYER_ADDRESSES                 0x00010118
#define OID_GEN_TRANSPORT_HEADER_OFFSET                 0x00010119
#define OID_GEN_MACHINE_NAME                            0x0001021A
#define OID_GEN_RNDIS_CONFIG_PARAMETER                  0x0001021B
#define OID_GEN_VLAN_ID                                 0x0001021C

#define OID_GEN_MEDIA_CAPABILITIES                      0x00010201
#define OID_GEN_PHYSICAL_MEDIUM                         0x00010202

#define OID_GEN_XMIT_OK                                 0x00020101
#define OID_GEN_RCV_OK                                  0x00020102
#define OID_GEN_XMIT_ERROR                              0x00020103
#define OID_GEN_RCV_ERROR                               0x00020104
#define OID_GEN_RCV_NO_BUFFER                           0x00020105

#define OID_GEN_DIRECTED_BYTES_XMIT                     0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT                    0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT                    0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT                   0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT                    0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT                   0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV                      0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV                     0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV                     0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV                    0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV                     0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV                    0x0002020C
#define OID_GEN_RCV_CRC_ERROR                           0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH                   0x0002020E
#define OID_GEN_GET_TIME_CAPS                           0x0002020F
#define OID_GEN_GET_NETCARD_TIME                        0x00020210
#define OID_GEN_NETCARD_LOAD                            0x00020211
#define OID_GEN_DEVICE_PROFILE                          0x00020212
#define OID_GEN_INIT_TIME_MS                            0x00020213
#define OID_GEN_RESET_COUNTS                            0x00020214
#define OID_GEN_MEDIA_SENSE_COUNTS                      0x00020215
#define OID_GEN_FRIENDLY_NAME                           0x00020216
#define OID_GEN_MINIPORT_INFO                           0x00020217
#define OID_GEN_RESET_VERIFY_PARAMETERS                 0x00020218

#define OID_802_3_PERMANENT_ADDRESS                     0x01010101
#define OID_802_3_CURRENT_ADDRESS                       0x01010102
#define OID_802_3_MULTICAST_LIST                        0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE                     0x01010104
#define OID_802_3_MAC_OPTIONS                           0x01010105
#define OID_802_3_RCV_ERROR_ALIGNMENT                   0x01020101
#define OID_802_3_XMIT_ONE_COLLISION                    0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS                  0x01020103
#define OID_802_3_XMIT_DEFERRED                         0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS                   0x01020202
#define OID_802_3_RCV_OVERRUN                           0x01020203
#define OID_802_3_XMIT_UNDERRUN                         0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE                0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST                   0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS                  0x01020207

#define OID_802_11_BSSID                                0x0D010101
#define OID_802_11_SSID                                 0x0D010102
#define OID_802_11_NETWORK_TYPE_IN_USE                  0x0D010204
#define OID_802_11_RSSI                                 0x0D010206
#define OID_802_11_BSSID_LIST                           0x0D010217
#define OID_802_11_BSSID_LIST_SCAN                      0x0D01011A
#define OID_802_11_INFRASTRUCTURE_MODE                  0x0D010108
#define OID_802_11_SUPPORTED_RATES                      0x0D01020E
#define OID_802_11_CONFIGURATION                        0x0D010211
#define OID_802_11_ADD_WEP                              0x0D010113
#define OID_802_11_WEP_STATUS                           0x0D01011B
#define OID_802_11_REMOVE_WEP                           0x0D010114
#define OID_802_11_DISASSOCIATE                         0x0D010115
#define OID_802_11_AUTHENTICATION_MODE                  0x0D010118
#define OID_802_11_RELOAD_DEFAULTS                      0x0D01011C

/* @enum */
typedef enum
{
    RNDIS_MSG_STAGE_IDLE,
    RNDIS_MSG_STAGE_SETUP,
    RNDIS_MSG_STAGE_NOTIFY,
    RNDIS_MSG_STAGE_STATUS,
} rndis_message_stage_e;

/* @struct */
typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
} rndis_generic_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t MaxTransferSize;
} rndis_initialize_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t DeviceFlags;
    uint32_t Medium;
    uint32_t MaxPacketsPerTransfer;
    uint32_t MaxTransferSize;
    uint32_t PacketAlignmentFactor;
    uint32_t AfListOffset;
    uint32_t AfListSize;
} rndis_initialize_cmplt_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
} rndis_halt_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} rndis_query_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
} rndis_query_cmplt_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} rndis_set_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} rndis_set_cmplt_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Reserved;
} rndis_reset_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Status;
    uint32_t AddressingReset;
} rndis_reset_cmplt_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Status;
    uint32_t StatusBufferLength;
    uint32_t StatusBufferOffset;
} rndis_indicate_status_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
} rndis_keepalive_msg_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} rndis_keepalive_cmplt_t;

typedef struct
{
    uint32_t NameOffset;
    uint32_t NameLength;
    uint32_t Type;
    uint32_t ValueOffset;
    uint32_t ValueLength;
} rndis_config_parameter_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t DataOffset;
    uint32_t DataLength;
    uint32_t OOBDataOffset;
    uint32_t OOBDataLength;
    uint32_t NumOOBDataElements;
    uint32_t PerPacketInfoOffset;
    uint32_t PerPacketInfoLength;
    uint32_t VcHandle;
    uint32_t Reserved;
} rndis_data_packet_t;

#ifdef __cplusplus
}
#endif

#endif
