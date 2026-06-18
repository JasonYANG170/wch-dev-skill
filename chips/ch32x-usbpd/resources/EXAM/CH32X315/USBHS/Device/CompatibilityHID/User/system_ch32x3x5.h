/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32x3x5.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : CH32X3x5 Device Peripheral Access Layer System Header File.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __SYSTEM_CH32X3x5_H 
#define __SYSTEM_CH32X3x5_H

#ifdef __cplusplus
 extern "C" {
#endif 

extern uint32_t SystemClock;              /* System Clock Frequency (Core Clock) */
extern uint32_t SystemCoreClock;          /* System Core Clock Frequency (Core Clock) */
extern uint32_t HCLKClock;                /* HCLK Clock Frequency (Core Clock) */

/* System_Exported_Functions */  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__CH32X3x5_SYSTEM_H */



