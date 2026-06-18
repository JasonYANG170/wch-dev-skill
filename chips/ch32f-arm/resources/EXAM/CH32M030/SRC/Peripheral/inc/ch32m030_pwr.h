/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_pwr.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : This file contains all the functions prototypes for the PWR
 *                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32M030_PWR_H
#define __CH32M030_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32m030.h"

/* STOP_mode_entry */
#define PWR_STOPEntry_WFI         ((uint8_t)0x01)
#define PWR_STOPEntry_WFE         ((uint8_t)0x02)

#define PWR_STANDBYEntry_WFI      ((uint8_t)0x01)
#define PWR_STANDBYEntry_WFE      ((uint8_t)0x02)

/* PB4_Mode */
#define PWR_PB4_FLOATING          ((uint32_t)0x00000000)
#define PWR_PB4_GPIO_Mode         ((uint32_t)0x00001000)
#define PWR_PB4_VHV_Monitor       ((uint32_t)0x00002000)
#define PWR_PB4_Overvol_Protect   ((uint32_t)0x00003000)

/* VDD8_SEL */
#define PWR_VDD8_SEL_5V           ((uint32_t)0x00000000)
#define PWR_VDD8_SEL_8V           ((uint32_t)0x00000020)
#define PWR_VDD8_SEL_9V           ((uint32_t)0x00000040)
#define PWR_VDD8_SEL_10V          ((uint32_t)0x00000060)

/* ISOURCE_LEVEL */
#define ISOURCE_LEVEL_LOW         ((uint8_t)0x00)
#define ISOURCE_LEVEL_HIGH        ((uint8_t)0x01)

void       PWR_DeInit(void);
void       PWR_EnterSTOPMode(uint8_t PWR_STOPEntry);
void       PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry);
void       PWR_ISINKCmd(FunctionalState NewState);
void       PWR_PMUPROCmd(FunctionalState NewState);
void       PWR_PB4_Config(uint32_t PWR_PB4);
void       PWR_AutoWakeUpCmd(FunctionalState NewState);
void       PWR_AWU_SetWindowValue(uint8_t WindowValue);
void       PWR_VDD8_Config(uint32_t VDD8_SEL);
void       ISINK1_Config(uint16_t ISINK_data);
void       ISINK2_Config(uint16_t ISINK_data);
uint32_t   ISOURCE1_GetData(uint8_t ISOURCE_LEVEL);
uint32_t   ISOURCE2_GetData(uint8_t ISOURCE_LEVEL);
#ifdef __cplusplus
}
#endif

#endif /* __CH32M030_PWR_H */
