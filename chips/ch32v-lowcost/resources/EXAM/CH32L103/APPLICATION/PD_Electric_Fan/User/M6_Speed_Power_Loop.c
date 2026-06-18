/* Includes -----------------------------------------------------------------*/
#include "M0_Control_Library.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private functions --------------------------------------------------------*/
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* Private variables --------------------------------------------------------*/
static uint8_t PD_CNT = PD_PERIOD;  //PD处理计数
/* Variables ----------------------------------------------------------------*/
/******************************************************************************
* Function Name  : SysTick_Handler
* Description    : 转速环处理中断.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void SysTick_Handler(void)
{
    _iq CRef2R_cq=0;
    PD_CNT--;
    //程序用于以中断时间(0.5ms)为基准的变量递减处理
    Time_Delay_Process();

    if(PD_CNT == 0)
    {
        //PD处理
        PD_Process();
        PD_CNT = PD_PERIOD;  //PD处理计数
    }
    //上下电控制处理逻辑
    Poweroperation_Process(&Ramp_M,&ADC_M,&Powerctr_M,&RunningStatus_M,MLBG_M.Cal_Struc.Angspeed_Aver1);

    //蜂鸣器控制处理
    BUZZ_PROCESS(&BUZZ_M);

    //上电有效后按键才可以按
    if(Powerctr_M.Poweron==TRUE)
    {
        //按键检测
        KeyCheck_Process(&Key_M,&Ramp_M);
    }
    switch(RunningStatus_M)
    {
    case RUN:
               //LBG观测转速数组更新
               Buffer_Update(&(LBG_SpeedBuffer_M[0]), &(MLBG_M.SpeedBuffPara), MLBG_M.Cal_Struc.Angspeed_Inst);

               //LBG观测平均转速计算
               MLBG_M.Cal_Struc.Angspeed_Aver1 = LPF_1st_Process(&(MLBG_M.Cal_Struc.SpeedLPF1st_M),\
                       MLBG_M.Cal_Struc.Angspeed_Inst);//观测转速低通滤波

               LBG_Status_M.Reliable =  Speed_Reliable_Verify(&(MLBG_M.SpeedBuffPara),&LBG_SpeedBuffer_M[0],\
                       MLBG_M.Cal_Struc.Angspeed_Aver1,LBG_Status_M.VarianceThresh);

   //            //转速稳定性判定
               Speed_Stable_Verify(&LBG_Status_M, MLBG_M.Cal_Struc.Angspeed_Aver1);
               if(LBG_Status_M.Stable==FALSE)
                   Protection_SetFault(SPEED_ABNORMAL_M);
   //
   //             //堵转判定
               if((Ramp_M.Decemark==FALSE))
               {
                   Motor_Stall_Verify(Ramp_M.Aftramp, MLBG_M.Cal_Struc.EalphaNew , MLBG_M.Cal_Struc.EbetaNew, MStruc_M.CRef2R.cq,&LBG_Status_M,MLBG_M.Cal_Struc.Angspeed_Aver1);
                   if(LBG_Status_M.Stall==TRUE)
                    Protection_SetFault(MOTOR_STALL_M);

                   Speed_Outrange_Verify(_IQabs(MLBG_M.Cal_Struc.Angspeed_Aver2),&LBG_Status_M);
                   //欠速保护触发堵转保护
                       if(LBG_Status_M.Underspeed==TRUE)
                           Protection_SetFault(MOTOR_STALL_M);
               }


               //转速环调节
   #if     (CLOSE_LOOP==SPEED_LOOP)
                   Speed_Ramp_Process(&Ramp_M);
                   CRef2R_cq = PI_Calculaion_Delta (Ramp_M.Aftramp, MLBG_M.Cal_Struc.Angspeed_Aver2, &(Mpid_M.Speed));
   #elif   (CLOSE_LOOP==POWER_LOOP)
                   Power_Cal.AngPower_Inst= _IQabs(_IQMPY_MACRO(MStruc_M.VRef2RC.cd,MStruc_M.C2R.cd))+_IQabs(_IQMPY_MACRO(MStruc_M.VRef2RC.cq,MStruc_M.C2R.cq));
                   Power_Cal.AngPower_Aver1 = LPF_1st_Process(&(Power_Cal.PowerLPF1st),Power_Cal.AngPower_Inst);//功率低通滤波
                   Power_Ramp_Process(&Ramp_M);
                   CRef2R_cq = PI_Calculaion_Delta (Ramp_M.Aftramp, Power_Cal.AngPower_Aver1, &(Power_Cal.Power));
                   CRef2R_cq=CRef2R_cq*Ramp_M.Direction;
   #endif

   //            //设定转速和欠速一致 停留太久触发欠速保护
               if((_IQabs(MLBG_M.Cal_Struc.Angspeed_Aver2)<LBG_UNDERSPEED_THRESH_M)&&(Ramp_M.Decemark==FALSE))
               {
                   //这里固定IQ 客户用的电机加大电流 输出角度压不住
                   MStruc_M.CRef2R.cq=_IQ(0.35)*Ramp_M.Direction;

   #if     (CLOSE_LOOP==SPEED_LOOP)
                   Mpid_M.Speed.Output_New=_IQ(0.35)*Ramp_M.Direction;
                   Mpid_M.Speed.Output_Prev=_IQ(0.35)*Ramp_M.Direction;
   #elif   (CLOSE_LOOP==POWER_LOOP)
                   Power_Cal.Power.Output_New=_IQ(0.25);
                   Power_Cal.Power.Output_Prev=_IQ(0.25);
   #endif
               }
               else
                   MStruc_M.CRef2R.cq=CRef2R_cq;

               //d轴电流切换处理
               if((DCurrProcess_M.Stage == 1)&&(_IQabs(MStruc_M.CRef2R.cd) <= DCurrProcess_M.Target1)&&(_IQabs(MLBG_M.Cal_Struc.Angspeed_Aver1)>=(_IQ(0.1))))
                   Dcurr_Process_Init(&DCurrProcess_M,2);

               MStruc_M.CRef2R.cd = Dcurr_Process(&DCurrProcess_M, MLBG_M.Cal_Struc.Angspeed_Aver1, MStruc_M.CRef2R.cd);

               break;
           default:
                   break;
       }

       //母线电压过压
       DCOvervolt_Protection(ADC_M.DCVoltAver, &DCBusProtecion_M);

       //母线电压欠压
//       DCUndervolt_Protection(ADC_M.DCVoltAver, &DCBusProtecion_M);

       SysTick->SR &= ~(1 << 0);
   }

/************************************************************************************************************
* Function Name  : Systick_Init
* Description    : SYS定时器初始化
* Input          : NewState - ENABLE or DISABLE.
* Output         : None
* Return         : None
*************************************************************************************************************/
void Systick_Init(void)
{
    SysTick->CTLR  =0;
    SysTick->CMP   =0;
    SysTick->SR   &= ~(1 << 0);
    SysTick->CNT   = 0;
    SysTick->CMP   = TIMER_PERIDO_SYSTICK;  //比较值
    SysTick->CTLR |=(1 << 1)|(1 << 3)|(1 << 5);//使能中断 自动重装载使能 向上计数 初始值更新
}
/************************************************************************************************************
* Function Name  : SYS_TIM_Cmd
* Description    : SYS定时器使能
* Input          : NewState - ENABLE or DISABLE.
* Output         : None
* Return         : None
*************************************************************************************************************/
void SYS_TIM_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SysTick->CTLR |= TIM_CEN;
    }
    else
    {
        SysTick->CTLR &= (uint16_t)(~((uint16_t)TIM_CEN));
    }
}
