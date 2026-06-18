/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch641_debug.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2023/12/25
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay ,SysTick functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH641_DEBUG_H
#define __CH641_DEBUG_H

#include <ch641.h>
#include <stdio.h>
#include "led.h"

 /* USART_Flags */
#define USART_FLAG_TXE                       ((uint16_t)0x0080)


/* DEBUG UATR Definition Note that turning on CH641P serial port debugging requires occupying the LED_R pin, and LED_R is no longer valid. */
#ifndef DE_PRINTF
#define DE_PRINTF   0           //Serial port completely shut down
//#define DE_PRINTF   1         //All printf is turned off, but you can use the serial port send function
//#define DE_PRINTF   2         //printf works fine
#endif

typedef struct
{
     uint8_t tmr_ms_cnt_last;
     uint8_t tmr_ms_dlt;
} TMR_PARAM;

extern TMR_PARAM timer;
extern uint8_t volatile tmr_ms_cnt;
extern uint16_t volatile decode_tick;

extern void mDelayuS(uint16_t n);
extern void mDelaymS(uint16_t n);
extern void Timer_Init( void );
extern void USART_Printf_Init(uint32_t baudrate);
extern void uart_send_dat(uint8_t udat);
extern void uart_send_buf(uint8_t *ubuf,uint8_t ulen);

#endif
