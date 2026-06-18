/********************************** (C) COPYRIGHT *******************************
* File Name          : IICHost.h
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : CH555 I2C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __IICHOST_H__
#define __IICHOST_H__

#include "Debug.H"
#include "CH555.H"

sbit SCL = P0^0;  
sbit SDA = P0^1;

void I2C_Init();
void I2C_Start();
void I2C_Stop();
void I2C_Send_Byte(UINT8 txd);
UINT8 I2C_Read_Byte(UINT8 ack);
UINT8 I2C_Wait_Ack(void);
void I2C_Ack(void);
void I2C_NAck(void);


#endif




