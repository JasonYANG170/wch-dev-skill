/********************************** (C) COPYRIGHT *******************************
* File Name          : user_proc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : User profile
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_USER_PROC_H_
#define USER_USER_PROC_H_

#include <ch641_debug.h>


/* Q value of the detected object */
#define Q_VALUE_LTHRD      18                       /* Threshold value to be judged under 5V */ /* If the standby reports an error, you can check the current ADC value to adapt. */
#define Q_VALUE_HTHRD      38                       /* Judgment threshold at 9V */
#define Q_VALUE_HHTHRD     50                       /* 12V Lower Judgment Threshold */


/* Current value, internal amplification 74.3 times. For example, 1.6A, 0.01R, Vres=0.016V, the voltage is 1.1888V after 74.3 times of internal amplification, and it is converted into ADC value 1.1888/3.3*1023=368. */
/* Note that the defined currents are all after subtracting the bias */
#define PING_0_3A_ISEN     87                       /* 0.35A */ /* If the normal standby reports an error, it is likely that the value is set too small */
#define PING_0_8A_ISEN     200                      /* 0.8A */
#define PING_1_6A_ISEN     400                      /* 1.6A */

/* The lowest current value set after triggering an overtemperature */
#define NTC_MIN_ISEN        150                     /* Minimum current for over-temperature regulation 600mA */

/* 8.2K pull-up resistor  */
/* 100K 3950, 80 degrees trigger protection, 70 degrees recovery, 73 degrees reduced output power */
#define NTC_TRIG_VALUE    894         //80 degrees Celsius
#define NTC_ADJ_VALUE     976         //73 degrees Celsius
#define NTC_RESUM_VALUE   1010        //70 degrees Celsius


/* VHV overvoltage and undervoltage protection value 20%. */
/* Configuration bit bit_vhv12_prefer=1, then automatically prioritize the request for 12V voltage, otherwise prioritize the request for 9V.*/
#define VHV_TRIG_HIGH    824                        /* 13.3v */
#define VHV_TRIG_LOW     787                        /* 12.7V */
//#define VHV_TRIG_HIGH    651                      /* 10.5v */
//#define VHV_TRIG_LOW     601                      /* 9.7V */

#define VHV_LTRIG_HIGH    260                       /* 4.2V  recovery value */
#define VHV_LTRIG_LOW     236                       /* 3.8V  trigger value */

/* OSC coil limiting voltage */
#define OSC_BLIND_RMV_VOLT   496                    /* 1.6V Blind Removal Voltage */
#define OSC_WORK_TRIG_LVOLT  558                    /* 1.8V  Limit Lock Voltage */
#define OSC_WORK_TRIG_HVOLT  620                    /* 2V  Direct stop voltage */


typedef struct
{
    uint8_t var_batpercent;                     /* Battery percentage */
    uint8_t pwm_dead_time;                      /* Dead time = pwm_dead_time/48 us, e.g. pwm_dead_time=12, i.e. 250ns, internal limit 4~15 */
    uint8_t pwm_dt_forword;                     /* Dead time, in order to minimize the dead time, there is a switching independent dead time on one side of the half-bridge. */

    uint8_t bit_ap_pop_win_en;                  /* Pop-up flag 0: No 1: Yes If on, only high voltage input will pop-up by default, if you need low voltage 5V also pop-up, you need to set bit_lv_ap_pop_win=1 at the same time.*/
    uint8_t bit_lv_ap_pop_win;                  /* 0:Low-voltage no pop-up 1:Allow low-voltage 5V pop-up Note the prerequisite is to enable the high-voltage pop-up enable bit_ap_pop_win_en=1*/
    uint8_t bit_magsafe_pop_mode;               /* 0:generic popup 1: magsafe popup (shows current battery level)*/

    uint8_t bit_low_power;                      /* Standby low power consumption 0:No 1:Yes Low power consumption mode is mainly used for rechargeable treasure, other are recommended to set 0 */
    uint8_t bit_fod_notry;                      /* Whether to set timeout for static FOD foreign object 0: with timeout 2min exit function 1: dead wait for removal, will not timeout retries */
    uint8_t bit_vhv12_prefer;                   /* If it is set, the request will be prioritized to 12V, otherwise, the request will be prioritized to 9V. */
    uint8_t bit_epp20_en;                       /* 0:epp default 15W protocol 1: enable epp 20W (currently some test stands support this mode) */
} USER_CFG;

typedef struct
{
    uint8_t bit_vhv_trig;                       /* VHV trigger flag */
    uint8_t bit_ntc_trig;                       /* NTC trigger flag */
    uint8_t bit_ntc_lock;                       /* NTC Over Temperature Power Reduction Flag */
    uint8_t bit_det_wait;                       /* Exception wait flag */
    uint8_t bit_det_wait_mode;                  /* waiting mode 0:2min  1:30min */
    uint8_t bit_q_thing;                        //q to the device flag
    uint8_t bit_ot_rmv_proc;                    //Over-temperature removal treatment for special devices (not really removal) Huawei Watch
    uint8_t bit_wait_remove;                    //Flag to be removed
    uint8_t bit_is_thing;                       //In abnormal mode 0, if it is a static FOD, set . If it is a normal device, it is cleared.
    uint8_t bit_10w_power;                      //The current power level is 10w, which involves power calculation.
    uint8_t bit_15w_power;                      //The current power level is 15w, which involves power calculation.

    uint8_t ping_det_cnt;                       //Exception count cumulative value
    uint8_t fod_cnt;                            //Ping phase current counting and fod anomaly counting
    uint16_t q_value_ref;                       //The value is different for different voltages
} PROTECT_PARAM;

typedef struct
{
    uint8_t Com_Buf[26];
    uint8_t Com_Buf_Len;

} MSG_PROC;


typedef struct
{
    uint8_t bit_fastcharge;
    uint8_t bit_highvol;
} _VOLT_STATE;


extern USER_CFG usercfg;
extern PROTECT_PARAM Expt;
extern MSG_PROC Msg_Proc;
extern _VOLT_STATE volt_state;
extern uint8_t Blind_Cont_Cnt;                                                  /* Number of adjustments in a blind succession */

/* Function extensibility */
extern void Recv_QI_Data_Callback( void );                                      /* qi receives data, which is mainly used for outputting messages. */
extern void Get_Cur_Q_Ref(uint16_t meas);                                       /* Static foreign matter threshold setting (may need to be adjusted depending on external device selection)*/
extern void Get_ISEN_State( void );                                             /* Get current status */
extern void OSC_Check( uint16_t meas );                                         /* OSC Overvoltage Handling */
extern void Over_ISEN_Check( void );                                            /* Overcurrent handling (just modify the MAX_ISEN_LIMIT macro directly, the function doesn't need to be modified) */
extern void NTC_Check( void );                                                  /* Temperature Detection */
extern void VHV_Check( void );                                                  /* Voltage Detection */
extern uint16_t Get_Real_ISEN( void );                                          /* Get the current real current value (return value in mA)*/
extern void Get_Cur_Batpercent( void );                                         /* Get battery percentage  */
extern void App_Cfg_Init( void );                                               /* Initialization of the configuration of common user parameters */
extern void Blind_Callback( void );                                             /* For printing only */
extern uint16_t Get_Pingrmv_ISEN( void );                                       /* Setting the blocking current during PING */
extern void User_Loop_Proc( void );                                             /* Program segments executed in the main program cycle */                                           /* 主程序周期执行的程序段 */

extern void Remove_Device( void );                                              /* Device Removal */

#endif /* USER_USER_PROC_H_ */
