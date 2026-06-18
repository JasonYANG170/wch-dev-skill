/********************************** (C) COPYRIGHT ******************************
* File Name          : CH559_IAP.H
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __CH559_IAP_H__
#define __CH559_IAP_H__
#define        Uart_Sync_Head1      0x57
#define        Uart_Sync_Head2      0xab
/*以下为IAP下载命令定义*/
#define        CMD_IAP_PROM         0x80        // ISP编程命令
#define        CMD_IAP_ERASE        0x81        // ISP擦除命令
#define        CMD_IAP_VERIFY       0x82        // ISP校验命令
#define        CMD_IAP_END          0x83        // ISP结束标志
/********************************************************************************/
/*
  命令格式说明:
 1, CMD_IAP_END(IAP结束标志):
    此命令格式如下:
        CMD_IAP_END+1字节数据长度+2字节状态（保留）.
    返回数据:
        无
 2, CMD_IAP_VERIFY(IAP校验命令):
     此命令格式如下:
         CMD_IAP_VERIFY+1字节数据长度+2字节数据地址+后续数据.
    返回数据:
        正确:0x0000
        错误:0x0001
 3, CMD_IAP_PROM(IAP编程命令):
     此命令格式如下:
         CMD_IAP_PROM+1字节数据长度+2字节数据地址+后续数据.注意此命令发下来的数据个数必须为4字节的整数倍
     返回数据:
        正确:0x0000
        错误:0x0001
 4, CMD_IAP_ERASE(IAP擦除命令):
     此命令格式如下:
        CMD_IAP_ERASE+1字节数据长度+2字节开始地址（一般为0x0000），此命令只需要发送一次
    返回数据:
        正确:0x0000
        错误:0x0001
*/
#define     IAP_LEN            64
/* IAP下载程序结构体 */
typedef union _IAP_CMD
{
    struct
    {
        UINT8    cmd;                   /* 命令码 0x81 */
        UINT8    len;                   /* 后续数据长度 */
        UINT8    addr[2];               /* 擦除地址 */
    } erase;                            /* 擦除命令 */
    struct
    {
        UINT8    cmd;                   /* 命令码 0x83 */
        UINT8    len;                   /* 后续数据长度 */
        UINT8    status[2];             /* 两字节状态，保留 */
    } end;                              /* 结束命令 */
    struct
    {
        UINT8    cmd;                   /* 命令码 0x82 */
        UINT8    len;                   /* 后续数据长度 */
        UINT8    addr[2];               /* 校验地址 */
        UINT8    buf[IAP_LEN-4];        /* 校验数据 */
    } verify;                           /* 校验命令 */
    struct
    {
        UINT8    cmd;                   /* 命令码 0x80 */
        UINT8    len;                   /* 后续数据长度 */
        UINT8    addr[2];               /* 地址 */
        UINT8    buf[IAP_LEN-4];        /* 后续数据 */
    } program;                          /* 编程命令 */
    struct
    {
        UINT8    buf[IAP_LEN];          /* 接收数据包*/
    } other;
} iap_cmd1;
#endif

