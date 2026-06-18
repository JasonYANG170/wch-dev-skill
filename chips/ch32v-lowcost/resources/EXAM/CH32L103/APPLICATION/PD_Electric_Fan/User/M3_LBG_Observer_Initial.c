/* Includes ------------------------------------------------------------------*/
#include "M0_Control_Library.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private functions --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Variables ----------------------------------------------------------------*/
Motor_LBG_Type  MLBG_M;
Observer_Status_Type    LBG_Status_M;
_iq24 LBG_SpeedBuffer_M[LBG_SPEED_BUFFERSIZE_M];
/*********************************************************************
 * @fn      LBG_Parameter_Define
 *
 * @brief   LBG observer parameter definition
 *
 * @para    MLBG: observer structure pointer
 *          MStatus: observer state structure pointer
 *
 * @return  none
 */
void LBG_Parameter_Define (Motor_LBG_Type *MLBG,Observer_Status_Type *LBGStatus)
{
    MLBG->Para.CalFreq = LBG_CAL_FREQ_M;
    MLBG->Para.CalT = LBG_CAL_PERIOD_M;
    MLBG->Para.K1CerrC = LBG_K1_CERROR_CURR_M;
    MLBG->Para.K2CerrE = LBG_K2_CERROR_EMF_M;

    MLBG->Cal_Struc.EUlim = LBG_MAX_EMF_M;
    MLBG->Cal_Struc.ELlim = LBG_MIN_EMF_M;
    MLBG->Cal_Struc.CUlim = LBG_MAX_CURR_M;
    MLBG->Cal_Struc.CLlim = LBG_MIN_CURR_M;

    MLBG->Cal_Struc.SpeedLPF1st_M.Input_Coef = _IQdiv(LBG_SPEEDLPF_WcT_M,(LBG_SPEEDLPF_WcT_M + UNIT_Q24));
    MLBG->Cal_Struc.SpeedLPF1st_M.Output_Coef = _IQdiv(UNIT_Q24,(LBG_SPEEDLPF_WcT_M + UNIT_Q24));

    MLBG->Speed_PLL.Zeta=PLL_ZETA;
    MLBG->Speed_PLL.Bandwidth=PLL_BAND;
    MLBG->Speed_PLL.SpeedLim =PLL_SPEED_DLIM;
    MLBG->Speed_PLL.Upper_Lim = LBG_SPEED_MAX_M * STARTUP_DIRECTION;
    MLBG->Speed_PLL.Lower_Lim = LBG_SPEED_MIN_M * STARTUP_DIRECTION;

    MLBG->SpeedBuffPara.Length = LBG_SPEED_BUFFERSIZE_M;

    LBGStatus->OverspeedThresh = LBG_OVERSPEED_THRESH_M;
    LBGStatus->OverspeedcntThresh = LBG_OVERSPEED_CNT_THRESH_M;
    LBGStatus->UnderspeedThresh = LBG_UNDERSPEED_THRESH_M;
    LBGStatus->UnderspeedcntThresh = LBG_UNDERSPEED_CNT_THRESH_M;
    LBGStatus->VarianceThresh = LBG_SPEED_VARIANCE_THRESH_M;
    LBGStatus->UnstableCntThresh = LBG_UNSTABLE_CNT_THRESH_M;
    LBGStatus->UnstableSpeedThresh = LBG_UNSTABLE_SPEED_THRESH_M;

    LBGStatus->StallSpeedBand = LBG_STALLBAND_M;
    LBGStatus->StallSpeed = LBG_STALLSPEED_M;
    LBGStatus->StallCurr =  LBG_STALLCURR_M;
    LBGStatus->StallCntThresh = LBG_STALL_CNT_THRESH_M;
}

/*********************************************************************
 * @fn      LBG_Status_Init
 *
 * @brief   observer state initialization
 *
 * @para    MLBG:observer structure pointer
 *          MStatus:observer state structure pointer
 *          Direction:direction
 * @return  none
 */
void LBG_Status_Init (Motor_LBG_Type *MLBG,Observer_Status_Type *LBGStatus, int8_t  Direction)
{
    MLBG->Cal_Struc.Inimark = FALSE;

    MLBG->Cal_Struc.EalphaNew = _IQ(0.0);
    MLBG->Cal_Struc.EalphaPrev = _IQ(0.0);

    MLBG->Cal_Struc.EbetaNew = _IQ(0.0);
    MLBG->Cal_Struc.EbetaPrev  = _IQ(0.0);

    MLBG->Cal_Struc.CalphaNew = _IQ(0.0);
    MLBG->Cal_Struc.CalphaPrev = _IQ(0.0);
    MLBG->Cal_Struc.CbetaNew = _IQ(0.0);
    MLBG->Cal_Struc.CbetaPrev = _IQ(0.0);

    MLBG->Cal_Struc.ThetaErr = _IQ(0.0);
    MLBG->Cal_Struc.PosiElecPU = _IQ(0.0);

    MLBG->Cal_Struc.Angspeed_Inst = _IQ(0.0);
    MLBG->Cal_Struc.Angspeed_Aver1 = _IQ(0.0);
    MLBG->Cal_Struc.Angspeed_Aver2 = _IQ(0.0);
    TrigonoMetric_Function(&(MLBG->Cal_Struc.Triangle),MLBG->Cal_Struc.PosiElecPU);

    MLBG->Cal_Struc.SpeedLPF1st_M.Out_New = _IQ(0.0);
    MLBG->Cal_Struc.SpeedLPF1st_M.Out_Pre = _IQ(0.0);

    LBGStatus->Overspeed = FALSE;
    LBGStatus->Underspeed = FALSE;
    LBGStatus->Reliable = FALSE;
    LBGStatus->Stable = TRUE;
    LBGStatus->Stall = FALSE;

    LBGStatus->Overspeed_CNT =0;
    LBGStatus->Underspeed_CNT = 0;
    LBGStatus->Unstable_CNT = 0;
    LBGStatus->Stall_CNT = 0;

    MLBG->Speed_PLL.Error_New = _IQ(0.0);
    MLBG->Speed_PLL.Error_Prev = _IQ(0.0);
    MLBG->Speed_PLL.Output_New = _IQ(0.0);
    MLBG->Speed_PLL.Output_Prev = _IQ(0.0);
    MLBG->Speed_PLL.Upper_Lim = LBG_SPEED_MAX_M * Direction;
    MLBG->Speed_PLL.Lower_Lim = LBG_SPEED_MIN_M * Direction;

}

