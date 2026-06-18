/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 TouchKey触摸按键
*                      CH0~CH2,分别对应P10 P11 P32
*                      CH4~CH11,分别对应P14~P17 P36 P37 P12 P13
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************/
/* 相关宏定义 */
#define   EN_ADC_INT                    0
#define   DOWM_THRESHOLD_VALUE          1000                     				/* 按下阈值定义 */
#define   UP_THRESHOLD_VALUE            50                       				/* 抬起阈值 */
#define   KEY_BUF_LEN                   8                       				/* 每个KEY按键对应的滤波缓冲区大小(库修改无效) 20 */
#define   KEY_BUF_LOST                  1                        				/* 排序后前后分别丢弃的字节数(库修改无效) 5 */
#define   MASK_ADC_CHAN                 0x0F

/*******************************************************************************/
/* 变量外扩 */
extern UINT16I KeyBuf[ 12 ][ KEY_BUF_LEN ];

/*******************************************************************************/
/* 函数外扩 */
extern void TouchKey_Init( void );
extern UINT16 TouchKeySelect( UINT8 ch, UINT8 cpw );
extern UINT16 Default_TouchKey( UINT8 ch, UINT8 cpw );

#endif
