/********************************** (C) COPYRIGHT *******************************
 * File Name          : M5_MotorStart.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/11/04
 * Description        : Initialization of the motor before starting
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/* Includes -----------------------------------------------------------------*/
#include "M0_Control_Library.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private functions --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Variables ----------------------------------------------------------------*/
/*********************************************************************
 * @fn      Motor_Start_Init_M
 *
 * @brief   Initialization of the motor before starting
 *
 * @return  none
 */
void Motor_Start_Init_M(void)
{
    rampctr_Initial(&Ramp_M);//斜坡初始化，放在最前面，含有转向的定义

    KeyandLED_Status_Init(&Key_M);//按键和指示灯状态初始化

    Operation_Status_Init(&Operation_M);//操作状态初始化

    Protection_and_Emergency_Init();//保护参数和状态初始化

    PID_ParaBase_Cal_M(&MBase_M,&Mpid_M);//重新计算PI参数
    PID_Output_Init(&Mpid_M,Ramp_M.Direction);//PID调节器输出初始化

#if (CLOSE_LOOP==POWER_LOOP)
    PowerCal_Init(&Power_Cal);//功率环初始化
#endif

    Motor_Status_Initial_M(&MStruc_M);//电机状态初始化

    Dcurr_Process_Init(&DCurrProcess_M,1);//D轴电流处理初始化

    SVPWM_2R_Status_Init_M(&SVPWM_Cal_M);//SVPWM计算状态初始化

    LBG_Status_Init(&MLBG_M,&LBG_Status_M,Ramp_M.Direction);//LBG观测器初始化

    Buffer_Clear(&(LBG_SpeedBuffer_M[0]),&(MLBG_M.SpeedBuffPara));//LBG观测转速数组初始化

    ADC_Offset_Reading(ADC1,&ADC_M,1);//ADC1采样偏置读取

    Flystart_Status_Init(&Flystart_M,Ramp_M.Direction); //反电动势检测

    SVPWM_Zerovolt_Output(&SVPWM_Cal_M);

    TIM_CtrlPWMOutputs(TIM1,ENABLE);//使能PWM输出
}
