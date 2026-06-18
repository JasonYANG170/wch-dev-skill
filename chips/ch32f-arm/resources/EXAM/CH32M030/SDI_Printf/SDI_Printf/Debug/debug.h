/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "ch32m030.h"

/* UART Printf Definition */
#define DEBUG_UART1_NoRemap   1  //Tx-PC1
#define DEBUG_UART1_Remap1    2  //Tx-PC1
#define DEBUG_UART1_Remap2    3  //Tx-PB6
#define DEBUG_UART1_Remap3    4  //Tx-PB1
#define DEBUG_UART1_Remap4    5  //Tx-PA3
#define DEBUG_UART1_Remap5    6  //Tx-PA2

/* DEBUG UATR Definition */
#ifndef DEBUG
#define DEBUG   DEBUG_UART1_NoRemap
#endif

/* SDI Printf Definition */
#define SDI_PR_CLOSE   0
#define SDI_PR_OPEN    1

#ifndef SDI_PRINT
#define SDI_PRINT   SDI_PR_CLOSE
#endif


extern volatile uint32_t ISINK_ADJ;

void Delay_Init(void);
void Delay_Us(uint32_t n);
void Delay_Ms(uint32_t n);
void USART_Printf_Init(uint32_t baudrate);
void SDI_Printf_Enable(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H */
