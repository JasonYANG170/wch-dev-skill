/********************************** (C) COPYRIGHT *******************************
* File Name          : PD.H
* Author             : WCH
* Version            : V1.00
* Date               : 018/09/15
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __PD_H__
#define __PD_H__
#include "CH549.H"
#include "DEBUG.H"
/*******************************************************************************
 *     GPIO            Function
 *     P14               CC1
 *     P15               CC2
 *     P11               PD 通讯连接状态输出
 *******************************************************************************/

/* 电源控制开关/输出状态指示
    关断输出为0，打开输出为1
*/
sbit VBUSG = P1^1;
/* 电源角色宏定义 */
#define     SINK                    0
#define     SOURCE                  1
/* 根据需求选择SOURCE 与SINK 只能选择1个 */
#define     POWER_ROLE              SINK
#define     CC_CHAN_1               1
#define     CC_CHAN_2               2
#define     CC_AD_CHAN_1            4
#define     CC_AD_CHAN_2            5
/* CC脚检测到的ADC值判断供电能力 */
/*  实际电压        供电电流        ADC实际值(ADC满偏电压5V，12位)
    考虑到CC线上有5V供电3.3V供电横流供电等情况，CC线检测电压要留组余量
 * +---------+----------+-------+
 * | 0.4 V   | Default  | 342   |
 * | 0.9 V   | 1.5A     | 771   |
 * | 1.6 V   | 3.0A     | 1383  |
 * +---------+----------+-------+

*/
#define     DEFAULT_POWER_MIN       (342-137)
#define     DEFAULT_POWER_MAX       (342+158)
#define     POWER_1A5_MIN           (771-198)
#define     POWER_1A5_MAX           (771+180)
#define     POWER_3A0_MIN           (1383-310)
#define     POWER_3A0_MAX           (1383+288)
/* PD 消息命令宏定义 */
#define     SOURCE_SEND_CAP         0x01                                            /* 5B:00001 */
#define     GOOD_CRC                0x01                                            /* 5B:00001 */
#define     SINK_SEND_REQUEST       0x02                                            /* 5B:00010 */
#define     SOURCE_SEND_ACCEPT      0x03                                            /* 5B:00011 */
#define     REJECT                  0x04                                            /* 5B:00100 */
#define     PS_RDY                  0x06                                            /* 5B:00110 */
/* DFP连接状态 */
#define     DFP_DEFAULT_CURRENT     0X01
#define     DFP_1_5_A               0X02
#define     DFP_3_0_A               0X03
#define     DFP_DISCONNECT          0XFF
#define     DFP_STAT_DISCONNECT     0X00
#define     DFP_STAT_CONNECT        0X01
/*********************************************************************************************************/
/* SOURCE 端宏定义*/
/* UFP连接状态 */
#define     UCC_DISCONNECT          0x00                                            //设备未连接
#define     UCC1_CONNECT            0x01                                            //正向连接
#define     UCC2_CONNECT            0x02                                            //反向连接
/*
 * 设置5个电压、电流档位用于配置Source Capability
 * 不需要的档位必须设置成0,其中5V档必须存在，且必须为VOLT1,5V档电流可调
*/
/* 电压档，单位1mV */
#define     VOLT1                   5000
#define     VOLT2                   9000
#define     VOLT3                   15000
#define     VOLT4                   20000
#define     VOLT5                   25000
/* 电流档，单位1mA */
#define     AMP1                    3000
#define     AMP2                    1800
#define     AMP3                    1500
#define     AMP4                    1200
#define     AMP5                    1000
/* 外部调用变量 */
extern UINT8X CCSel;                                                                /* PD通讯CC连接管脚: 1：CC1  2：CC2 */
/* 外部子函数 */
/*******************************************************************************
* Function Name  : Timer0Init()
* Description    : 发送使用Timer0定时模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void  Timer0Init( );
/*******************************************************************************
* Function Name  : PD_Init()
* Description    : Type-C UPF模式配置,引脚ADC配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void  PD_Sink_Init( );
/*******************************************************************************
* Function Name  : Connect_Check(void)
* Description    : UPF检测DPF供电能力
* Input          : None
* Output         : None
                   1 默认电流
                   2 1.5A
                   3 3.0A
                   0xff 悬空
*******************************************************************************/
extern UINT8 Connect_Check( void );
/*******************************************************************************
* Function Name  : ResetSndHeader
* Description    : 初始化发送头默认值
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
extern void  ResetSndHeader (void);
/*******************************************************************************
* Function Name  : SendHandle
* Description    : 底层发送函数,包含GOODCRC的接收
*                  填充PRE、SOF、CRC32、EOP,然后进行4B5B编码、BMC编码
* Input          : CC_Sel,表示所用的CC脚
* Output         : None
* Return         : 操作状态
*                  0x00: 发送成功
*                  0x01: 发送失败
*******************************************************************************/
extern UINT8 SendHandle ( UINT8 CC_Sel );
/*******************************************************************************
* Function Name  : ReceiveHandle
* Description    : 底层接收函数,包含GOODCRC的发送
*                  填充PRE、SOF、CRC32、EOP
* Input          : CC_Sel,表示所用的CC脚
* Output         : None
* Return         : 0x00:接收到数据包
*                  0x01:没有收到
*******************************************************************************/
extern UINT8 ReceiveHandle ( UINT8 CC_Sel ) ;
/*******************************************************************************
* Function Name  : SearchVoltage
* Description    : 搜索Volt电压以下最接近的
* Input          : Volt,电压值，单位 mV
* Output         : None
* Return         : 序号值
*                  如果返回0xFF,最小的也比请求的大，找不到所需电压值
*******************************************************************************/
extern UINT8 SearchVoltage ( UINT16 Volt );
/*******************************************************************************
* Function Name  : PrepareReqData
* Description    : 准备Request请求数据包
* Input          : Index : SearchVoltage()函数返回的索引值
* Output         : NONE
* Return         : NONE
*******************************************************************************/
extern void PrepareReqData(UINT8 index);
/*******************************************************************************
* Function Name  : GetMsgType
* Description    : 获取当前消息状态标识
* Input          : NONE
* Output         : NONE
* Return         : 消息类型
*******************************************************************************/
extern UINT8 GetMsgType(void);
/* PD SOURCE 操作函数 */
/*******************************************************************************
* Function Name  : PD_Source_Init()
* Description    : Type-C UPF检测初始化，初始为3A电流输出能力
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void PD_Source_Init( void );
/*******************************************************************************
* Function Name  : DFP_Insert(void)
* Description    : DPF检测UFP正插，反插以及未插入和已插入
* Input          : None
* Output         : None
* Return         : 0   未连接
                   1   正向连接
                   2   反向连接
*******************************************************************************/
extern UINT8 DFP_Insert( void );
/*******************************************************************************
* Function Name  : Config_SourceCap
* Description    : 配置 Source Capability
* Input          : None
* Return         : None
*******************************************************************************/
extern UINT8 Config_SourceCap(void);
/*******************************************************************************
* Function Name  : SetMsgType
* Description    : 配置消息类型
* Input          : None
* Return         : None
*******************************************************************************/
extern void SetMsgType(UINT8 t_msgType);
#endif
