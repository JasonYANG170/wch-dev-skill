/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __M3_LBG_OBSERVER_H
#define __M3_LBG_OBSERVER_H

/* Includes -----------------------------------------------------------------*/
/* Exported types -----------------------------------------------------------*/
/* Exported constants -------------------------------------------------------*/
#define LBG_CAL_FREQ_M          FREQ_SVPWM
#define LBG_CAL_PERIOD_M        _IQ(1.0)/LBG_CAL_FREQ_M
#define LBG_K1_CERROR_CURR_M    (-10000)
#define LBG_K2_CERROR_EMF_M     20000

#define LBG_MAX_EMF_M           _IQ(2.0)
#define LBG_MIN_EMF_M           _IQ(-2.0)

#define LBG_MAX_CURR_M          _IQ(2.0)
#define LBG_MIN_CURR_M          _IQ(-2.0)

#define PLL_ZETA                _IQ(1)
#define PLL_BAND                _IQ(0.25)
#define PLL_SPEED_DLIM          _IQ(0.2)
#define LBG_SPEED_MAX_M         _IQ(1.5)
#define LBG_SPEED_MIN_M         _IQ(0.01)

#define LBG_SPEED_BUFFERSIZE_M    32

#define LBG_SPEEDLPF_CF_M        50
#define LBG_SPEEDLPF_PERIOD_M    (_IQ(1.0)/SPEED_LOOP_CAL_FREQ)
#define LBG_SPEEDLPF_WcT_M       _IQmpyI32(_IQmpy(LBG_SPEEDLPF_PERIOD_M,PI_2_Q24),LBG_SPEEDLPF_CF_M)

#define LBG_OVERSPEED_THRESH_M          _IQ(1.5)
#define LBG_UNDERSPEED_THRESH_M         _IQ(0.1)
#define LBG_OVERSPEED_CNT_THRESH_M      7000
#define LBG_UNDERSPEED_CNT_THRESH_M     7000

#define LBG_SPEED_VARIANCE_THRESH_M     _IQ(0.2)

#define LBG_UNSTABLE_CNT_THRESH_M       300
#define LBG_UNSTABLE_SPEED_THRESH_M     _IQ(0.02)

#define LBG_STALLBAND_M         _IQ(0.8)
#define LBG_STALLSPEED_M        _IQ(0.1)
#define LBG_STALLCURR_M         _IQMPY_MACRO(_IQ(0.8),SPEED_REGULATOR_ULIM_M)
#define LBG_STALL_CNT_THRESH_M  6000

/* Exported variables -------------------------------------------------------*/
extern Motor_LBG_Type  MLBG_M;
extern Observer_Status_Type    LBG_Status_M;
extern _iq24 LBG_SpeedBuffer_M[LBG_SPEED_BUFFERSIZE_M];

/* Exported macro -----------------------------------------------------------*/
/* Exported functions -------------------------------------------------------*/
void LBG_Parameter_Define (Motor_LBG_Type *MLBG,Observer_Status_Type *LBGStatus);

void LBG_Paramter_Cal (Motor_Base_Type *Basestruc,Motor_LBG_Type *MLBG);

void LBG_Status_Init (Motor_LBG_Type *MLBG,Observer_Status_Type *LBGStatus,int8_t  Direction);
#endif
