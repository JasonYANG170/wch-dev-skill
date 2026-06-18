/********************************** (C) COPYRIGHT *******************************
* File Name          : led.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : led
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_LED_H_
#define USER_LED_H_

#include <ch641_debug.h>
#include "user_proc.h"

//LED Pin Definition
#define     RED_LED         GPIO_Pin_1      //PB1
#define     BLUE_LED        GPIO_Pin_6      //PA6

typedef struct
{
    uint8_t led_init_stage;             /* LED power-up light display control sign */
    uint8_t led_breath_trig;            /* User program modification control duty cycle [0-20], 0 corresponds to full extinction, 20 corresponds to full illumination */
    uint8_t led_breath_trig_x;          /* Used to calculate led_breath_trig */
    uint8_t led_breath_tick;            /* Incremented automatically by timer interrupt [0-19], looped automatically by interrupt */
    uint8_t tmr_led_breath_cycle;       /* Breathing light brightness adjustment cycle */

    uint16_t led_init_tmr_cnt;          /* Initialize LED control variables on power-up */
    uint16_t tmr_LED_Tog_cycle;         /* Abnormal indicator blinking period control variable */

    uint8_t bit_led_initok;             /* LED power-up light display control sign */
    uint8_t soft_breath_en;             /* breath enable bit */
    uint8_t led_trig_inc;               /* Controlling the led_breath_trig incremental flag */

} LED_PARAM;

extern LED_PARAM led;

#define LED_ON_RED()       {GPIOB->BCR  = RED_LED;}
#define LED_OFF_RED()      {GPIOB->BSHR = RED_LED;}
#define LED_ON_BLUE()      {GPIOA->BCR  = BLUE_LED;}
#define LED_OFF_BLUE()     {GPIOA->BSHR = BLUE_LED;}

/* Function extensibility */
extern void Tmr_ISR_Callback( void );
extern void LED_Breath_Ctl( void );
extern void Pre_Init_Callback( void );

extern void LED_Init( void );
extern void LED_Standby( void );
extern void LED_Rdy2work( void );
extern void LED_Rdy2full( void );
extern void LED_Full( void );
extern void LED_Tog( void );



#endif /* USER_LED_H_ */
