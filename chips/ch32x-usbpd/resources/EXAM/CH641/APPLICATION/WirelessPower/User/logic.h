/********************************** (C) COPYRIGHT *******************************
* File Name          : logic.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : Power regulation and dynamic FOD
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_LOGIC_H_
#define USER_LOGIC_H_

#include <ch641.h>

/*******************************************************************************/
#define AP_ISEN_LIMIT      325                                                      /* Apple device current limit point 1.3A */
#define MAX_ISEN_LIMIT     575                                                      /* Other equipment current limit points 2.3A */
#define RT_ISEN_LIMIT      600                                                      /* Real-time current limit point 2.4A */

#define MAX_ISEN_ERR        67                                                      /* Near current threshold point 270ma slow adjustment */

/* Undervoltage lockout limit 20% */
#define MIN_VOTL_ERR        12                                                      /* Slow tuning within 200mv of the threshold point */

/* Dynamic FOD related */
#define DYN_FOD_ENABLE       1                                                      /* Enable Dynamic FOD 0:Disable 1:Enable */
#define MIN_FOD_ISEN        150                                                     /* FOD Uncalculated Current 600mA */
#define FOD_CHECK_MAXCNT     3                                                      /* Maximum number of consecutive power anomaly detections */


typedef struct
{
    uint8_t bit_pwm_out_en;         //pwm output enable flag bit
    uint8_t bit_half_bridge;        //0:Full bridge 1:Half bridge
    uint8_t bit_local_half_bridge;  //Temporary bridge status
    uint8_t bit_fix_freq;           //fixed frequency
    uint8_t bit_127k_mode;          //127K mode
    uint8_t bit_power_dir;          //Power increase or decrease flag 0: power increase 1: power decrease
    uint8_t bit_br_sw_mode;         //Bridge switching mode
    uint8_t bit_current_volt;       //Power increase marking
    uint8_t bit_adj_dir;            //Free adjustment of direction

} _PWM_CTRL;
extern _PWM_CTRL pwm_ctrl;

extern uint16_t max_isen;           //Maximum current limit
extern uint16_t Uvlo_Thrdvalue;     //Undervoltage lockout threshold (subvoltage)

/* Function extensibility */
extern void Power_Proc( uint8_t var_pwr_chg );      //Power adjustment, bit_power_dir determines whether to increase or decrease power.
extern void PID_Process( uint8_t dat );             //PID Handling Functions

extern void QI_CE_Handle( uint8_t dat );            //error handling function
extern uint8_t QI_RP_Handle( uint8_t dat );         //Dynamic FOD Handling Functions

#endif /* USER_LOGIC_H_ */
