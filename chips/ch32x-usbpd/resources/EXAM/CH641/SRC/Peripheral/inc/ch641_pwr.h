/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch641_pwr.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/08/28
* Description        : This file contains all the functions prototypes for the PWR
*                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH641_PWR_H
#define __CH641_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ch641.h>

/* PVD_detection_level  */
#define PWR_PVDLevel_0          ((uint32_t)0x00000000)
#define PWR_PVDLevel_1          ((uint32_t)0x00000020)
#define PWR_PVDLevel_2          ((uint32_t)0x00000040)
#define PWR_PVDLevel_3          ((uint32_t)0x00000060)
#define PWR_PVDLevel_4          ((uint32_t)0x00000080)
#define PWR_PVDLevel_5          ((uint32_t)0x000000A0)
#define PWR_PVDLevel_6          ((uint32_t)0x000000C0)
#define PWR_PVDLevel_7          ((uint32_t)0x000000E0)

/* STOP_mode_entry */
#define PWR_STANDBYEntry_WFI      ((uint8_t)0x01)
#define PWR_STANDBYEntry_WFE      ((uint8_t)0x02)

/* STOP_mode_entry */
#define PWR_STOPEntry_WFI         ((uint8_t)0x01)
#define PWR_STOPEntry_WFE         ((uint8_t)0x02)

/* PWR_Flag */
#define PWR_FLAG_PVDO             ((uint32_t)0x00000004)

void       PWR_DeInit(void);
void       PWR_PVDCmd(FunctionalState NewState);
void       PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void       PWR_AutoWakeUpCmd(FunctionalState NewState);
void       PWR_AWU_SetWindowValue(uint8_t WindowValue);
void       PWR_EnterSTOPMode(uint8_t PWR_STOPEntry);
void       PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);

#ifdef __cplusplus
}
#endif

#endif
