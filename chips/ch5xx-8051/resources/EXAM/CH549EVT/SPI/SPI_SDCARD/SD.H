/********************************** (C) COPYRIGHT *******************************
* File Name          : SD.H
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/28
* Description        : CH549 SD
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __SD_H__
#define __SD_H__
#include ".\SPI\SPI.H"                                                 /* 包含SPI操作基本函数 */
#define SD_CMD_TIMEOUT  0x3FFFF                                        /* 超时时间配置 */
/* SD命令可能返回的错误码 */
#define SD_ERR_CMD_OK                   0x00                           /* 卡命令执行成功 */
#define SD_ERR_CMD_TIMEOUT              0x6A                           /* 卡命令响应超时 */
#define SD_ERR_INIT                     0xFE                           /* 卡初始化失败 */
#define SD_CMD_UNSURPORT                0x6B                           /* 不支持的命令 */
#define SD_ERR_CMD_PARAM                0x6C                           /* 命令参数错误 */
#define SD_ERR_CMD_CRC                  0x6D                           /* 命令CRC错误 */
#define SD_ERR_VOL_NOTSUSP              0x6E                           /* CSD数据出错 */
#define SD_BLOCK_ERROR                  0x6F                           /* 块大小出错 */
#define SD_ERR_INSERT                   0x70                           /* 卡未插入 */
#define CMD0                            0                              /* 卡复位命令 */
#define CMD1                            1                              /* sd卡一般使用acmd41初始化，但是mmc卡必须用CMD1 */
#define CMD8                            8                              /* 接口操作条件,大致区分卡类型 */
#define CMD9                            9                              /* 获取CSD */
#define CMD12                           12                             /* 终止传输 R1b */
#define CMD16                           16                             /* 设置块长度 */
#define CMD17                           17                             /* 单块读 */
#define CMD18                           18                             /* 多块读 */
#define CMD24                           24                             /* 单块写 */
#define CMD25                           25                             /* 多块写 */
#define CMD55                           55                             /* ACMD之前需要 */
#define CMD58                           58                             /* 获取OCR */
#define ACMD41                          41                             /* 初始化命令 */
typedef struct SD_INFO_
{
    UINT8  insert;                                                     /* 插拔标志 0：拔出  1：插入 */
    UINT8  type;                                                       /* 0:mmc v3   1:sd v1   2:sd v2标准卡   3:sd v2高容量卡  4:others(未识别) */
    UINT8  init_success;                                               /* 初始化完成置1 */
    UINT16 block_len;                                                  /* 单个block大小 */
    UINT32 block_num;                                                  /* 块数 */
    UINT32 capability;                                                 /* 单位:K字节 */
} SD_INFO;
//外部调用恒定义
#define  SD_INSERT_PIN_IN( )            P3_4
//外部调用变量
extern SD_INFO xdata sd_info;                                          /* SD卡信息 */
//外部调用子函数
extern UINT8 SD_Check_Insert( void );
extern UINT8 SD_Init(void);
extern UINT8 SD_READ_SECTOR(UINT8X *datbuf,UINT32 sector_num);
extern UINT8 SD_READ_MULT_SECTOR(UINT8X *datbuf,UINT32 sector_num,UINT16 blocks );
extern UINT8 SD_WRITE_SECTOR(UINT8X *datbuf,UINT32 sector_num);
extern UINT8 SD_WRITE_MULT_SECTOR( UINT8X *datbuf,UINT32 sector_num,UINT16 blocks );
#endif
