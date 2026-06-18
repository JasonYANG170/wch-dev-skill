/********************************** (C) COPYRIGHT *******************************
 * File Name          : M2_PID_Parameter.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/11/04
 * Description        : Definition of PID control parameters.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __M2_MOTOR_PID_H
#define __M2_MOTOR_PID_H

/* Includes -----------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
    _iq24 AngPower_Inst;
    _iq24 AngPower_Aver1;
    PID_Structure_Type Power;
    LPF_First_Order_Type PowerLPF1st;
}Motor_Power_Type;
/* Exported constants --------------------------------------------------------*/

#define POWER_REGULATOR_FREQ_M    SPEED_LOOP_CAL_FREQ
#define POWER_REGULATOR_PERIOD_M  (_IQ(1.0)/SPEED_REGULATOR_FREQ_M)
#define POWER_REGULATOR_KP_M      _IQ(0.128)
#define POWER_REGULATOR_KI_M      _IQ(0.6)
#define POWER_REGULATOR_ULIM_M    _IQ(0.8)
#define POWER_REGULATOR_LLIM_M    _IQ(0.05)
#define POWER_REGULATOR_DLIM_M    _IQ(0.2)

#define POWERLPF_CF_M        50
#define POWERLPF_PERIOD_M    (_IQ(1.0)/SPEED_LOOP_CAL_FREQ)
#define POWERLPF_WcT_M       _IQmpyI32(_IQmpy(POWERLPF_PERIOD_M,PI_2_Q24),POWERLPF_CF_M)

#define SPEED_REGULATOR_FREQ_M    SPEED_LOOP_CAL_FREQ               // Speed regulator execution frequency (unit-Hz)
#define SPEED_REGULATOR_PERIOD_M  (_IQ(1.0)/SPEED_REGULATOR_FREQ_M) // Speed regulator execution period (unit-s)
#define SPEED_REGULATOR_KP_M      _IQ(0.00128)                        // Speed regulator proportional coefficient(actual value)
#define SPEED_REGULATOR_KI_M      _IQ(0.002)                       // Speed regulator integral coefficient(actual value)
#define SPEED_REGULATOR_KD_M      _IQ(0.0)                          // Speed regulator differential coefficient(actual value)
#define SPEED_REGULATOR_KPADJ_M   _IQ(1.0)                          // Speed regulator Proportional coefficient adjustment
#define SPEED_REGULATOR_KIADJ_M   _IQ(1.0)                          // Speed regulator integral coefficient adjustment
#define SPEED_REGULATOR_KDADJ_M   _IQ(1.0)                          // Speed regulator differential coefficient adjustment
#define SPEED_REGULATOR_ULIM_M    _IQ(0.80)                          // Speed regulator upper limit (unit-A per)
#define SPEED_REGULATOR_LLIM_M    _IQ(0.00)                          // Speed regulator lower limiting limit (unit-A per)
#define SPEED_REGULATOR_DLIM_M    _IQ(0.2)                          // Speed regulator single regulation limit (unit-A per)


// Voltage limiting value definition
#ifdef MAG100_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(1.0000)                     // Max modulation ratio 1.0(unit-1)
#define MAX_MODULATION              _IQ(1.0000)                     // Max modulation ratio 1.0(unit-1)
#elif  defined MAG098_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.9604)                     // Max modulation ratio 0.98(unit-1)
#define MAX_MODULATION              _IQ(0.9800)                     // Max modulation ratio 0.98(unit-1)
#elif  defined MAG096_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.9216)                     // Max modulation ratio 0.96(unit-1)
#define MAX_MODULATION              _IQ(0.9600)                     // Max modulation ratio 0.96(unit-1)
#elif  defined MAG094_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.8836)                     // Max modulation ratio 0.94(unit-1)
#define MAX_MODULATION              _IQ(0.9400)                     // Max modulation ratio 0.94(unit-1)
#elif  defined MAG092_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.8464)                     // Max modulation ratio 0.92(unit-1)
#define MAX_MODULATION              _IQ(0.9200)                     // Max modulation ratio 0.92(unit-1)
#elif  defined MAG090_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.8100)                     // Max modulation ratio 0.90(unit-1)
#define MAX_MODULATION              _IQ(0.9000)                     // Max modulation ratio 0.90(unit-1)
#elif  defined MAG088_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.7744)                     // Max modulation ratio 0.88(unit-1)
#define MAX_MODULATION              _IQ(0.8800)                     // Max modulation ratio 0.88(unit-1)
#elif  defined MAG086_TABLE_USED
#define MAX_MODULATIONSQUARE        _IQ(0.7396)                     // Max modulation ratio 0.86(unit-1)
#define MAX_MODULATION              _IQ(0.8600)                     // Max modulation ratio 0.86(unit-1)
#endif

// Current regulator parameters
#define CURR_REGULATOR_FREQ_M   FREQ_SVPWM                          // Current regulator execution frequency (unit-Hz)
#define CURR_REGULATOR_BW_M     (uint16_t)((500*PI_2_Q16)>>16)      // Current regulator bandwidth (unit-rad/s)

#define DAXIS_CURR_REGULATOR_KD_M       _IQ(0.0)                    //D axis current regulator differential coefficient
#define DAXIS_CURR_REGULATOR_KPADJ_M    _IQ(1.0)                    //D axis current regulator proportional coefficient adjustment
#define DAXIS_CURR_REGULATOR_KIADJ_M    _IQ(1.0)                    //D axis current regulator integral coefficient adjustment
#define DAXIS_CURR_REGULATOR_KDADJ_M    _IQ(1.0)                    //D axis current regulator differential coefficient adjustment
#define DAXIS_CURR_REGULATOR_ULIM_M     _IQ(0.5)                    //D-axis current regulator upper limit (unit-V per)
#define DAXIS_CURR_REGULATOR_LLIM_M     _IQ(-0.5)                   //D-axis current regulator lower limit (unit-V per)
#define DAXIS_CURR_REGULATOR_DLIM_M     _IQ(1.0)                    //D-axis current regulator single regulation limit (unit-V per)

#define QAXIS_CURR_REGULATOR_KD_M       _IQ(0.0)                    //Q axis current regulator differential coefficient
#define QAXIS_CURR_REGULATOR_KPADJ_M    _IQ(1.0)                    //Q axis current regulator proportional coefficient adjustment
#define QAXIS_CURR_REGULATOR_KIADJ_M    _IQ(1.0)                    //Q axis current regulator integral coefficient adjustment
#define QAXIS_CURR_REGULATOR_KDADJ_M    _IQ(1.0)                    //Q axis current regulator differential coefficient adjustment
#define QAXIS_CURR_REGULATOR_ULIM_M     MAX_MODULATION              //Q-axis current regulator upper limit (unit-V per)
#define QAXIS_CURR_REGULATOR_LLIM_M     (-MAX_MODULATION)           //Q-axis current regulator lower limit (unit-V per)
#define QAXIS_CURR_REGULATOR_DLIM_M     _IQ(1.0)                    //Q-axis current regulator single regulation limit (unit-V per)


#define DAXIS_CURR_REF_INIT1_M          _IQ(0.05)                   //D axis current reference value 1(unit-A per)
#define DAXIS_CURR_REF_INIT2_M          _IQ(0.05)                   //D axis current reference value 2(unit-A per)
#define DAXIS_CURR_REF_STEP1_M          _IQ(0.0001)                 //D axis current reference adjustment step 1(unit-A per/s)
#define DAXIS_CURR_REF_STEP2_M          _IQ(0.0001)                 //D axis current reference adjustment step 2(unit-A per/s)
#define DAXIS_CURR_WEAK_SPEED_M         _IQ(1.5)                    //Magnetic weaken over speed threshold(unit-rad/s_per)

#define QAXIS_CURR_REF_INIT1_M          _IQ(0.1)
/* Exported variables -------------------------------------------------------*/
extern Motor_PID_Type  Mpid_M;
extern DCurrProcess_Type    DCurrProcess_M;
extern Motor_Power_Type Power_Cal;
extern const _iq24 Magnitude_table[256];


/* Exported macro -----------------------------------------------------------*/
/* Exported functions -------------------------------------------------------*/

void PID_Prameter_Define (Motor_PID_Type * Mpid);

void PID_Output_Init(Motor_PID_Type * Mpid, int8_t Direction);

void PID_ParaBase_Cal_M(Motor_Base_Type *Basestruc,Motor_PID_Type * Mpid);

void DCurrProcess_Parameter_Define(DCurrProcess_Type * DCurrProcess);

void PowerCal_Prameter_Define(Motor_Power_Type *MPower );

void PowerCal_Init(Motor_Power_Type *MPower);

#endif
