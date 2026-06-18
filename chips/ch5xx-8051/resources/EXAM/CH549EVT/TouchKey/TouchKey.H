/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.H
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 TouchKey��������
*                      CH0~CH15 �ֱ��Ӧ���� P1.0~P1.7  P0.0~P0.7
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define   EN_ADC_INT                    0
#define   DOWM_THRESHOLD_VALUE          1000                      //按下阈值定义
#define   UP_THRESHOLD_VALUE            50                        //抬起阈值
#define   KEY_BUF_LEN                  10                       //每个KEY按键对应的滤波缓冲区大小（库修改无效）20
#define   KEY_BUF_LOST                  2                       //排序后前后分别丢弃的字节数（库修改无效） 5

extern UINT16 KeyBuf[16][KEY_BUF_LEN];

extern void TouchKey_Init( void );
extern UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw );
extern UINT16 Default_TouchKey( UINT8 ch,UINT8 cpw );
#endif
