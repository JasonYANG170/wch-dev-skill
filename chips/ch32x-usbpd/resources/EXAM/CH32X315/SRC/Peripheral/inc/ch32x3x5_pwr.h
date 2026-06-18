/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32x3x5_pwr.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file contains all the functions prototypes for the PWR  
*                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32X3x5_PWR_H
#define __CH32X3x5_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32x3x5.h"

/* PVD_detection_level  */
#define PWR_PVDLevel_MODE0          ((uint32_t)0x00000000)
#define PWR_PVDLevel_MODE1          ((uint32_t)0x00000020)
#define PWR_PVDLevel_MODE2          ((uint32_t)0x00000040)
#define PWR_PVDLevel_MODE3          ((uint32_t)0x00000060)

/* Regulator_state_is_STOP_mode */
#define PWR_Regulator_ON            ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower      ((uint32_t)0x00000001)

/* PWR_Flag */
#define PWR_FLAG_PVDO               ((uint32_t)0x00000001)

/* STOP_mode_entry */
#define PWR_STOPEntry_WFI           ((uint8_t)0x01)
#define PWR_STOPEntry_WFE           ((uint8_t)0x02)

void PWR_DeInit(void);
void PWR_PVDCmd(FunctionalState NewState);
void PWR_PLANCmd(FunctionalState NewState);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);

#ifdef __cplusplus
}
#endif

#endif 

