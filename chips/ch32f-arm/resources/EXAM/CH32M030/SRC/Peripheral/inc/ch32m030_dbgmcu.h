/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_dbgmcu.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : This file contains all the functions prototypes for the
 *                      DBGMCU firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32M030_DBGMCU_H
#define __CH32M030_DBGMCU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32m030.h"

/* CR1 Register */
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000002)
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00000010)
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00000020)
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00000040)

/* CR2 Register */
#define DBGMCU_SLEEP                 ((uint32_t)0x00010001)
#define DBGMCU_STOP                  ((uint32_t)0x00010002)
#define DBGMCU_STANDBY               ((uint32_t)0x00010004)


uint32_t DBGMCU_GetREVID(void);
uint32_t DBGMCU_GetDEVID(void);
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);
uint32_t DBGMCU_GetCHIPID( void );

#ifdef __cplusplus
}
#endif

#endif /* __CH32M030_DBGMCU_H */
