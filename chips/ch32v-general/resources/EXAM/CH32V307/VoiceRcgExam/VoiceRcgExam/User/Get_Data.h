/********************************** (C) COPYRIGHT *******************************
* File Name          : Get_Data.h
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/03/24
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef _GET_DATA_H
#define _GET_DATA_H
#include "debug.h"

void voice_init(void);
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize );
#endif

