/********************************** (C) COPYRIGHT *******************************
* File Name          : I2C.H
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH545 I2C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#include "Debug.H"

/****************** �ⲿ�����Ӻ��� ****************************/
extern void I2CM_Init( void );													/* I2C������ʼ�� */
extern void I2CS_Init(UINT8 addr, PUINT8 pbuf );													/* I2C�ӻ���ʼ�� */
extern void I2CM_CMD( UINT8 cmd );												/* I2C������������ */
extern void I2CM_WaitACK( void );												/* I2C�����ȴ��ӻ�Ӧ��ACK */
extern UINT8 I2CM_RecvData( void );												/* I2C��������1���ֽ����� */


#endif




