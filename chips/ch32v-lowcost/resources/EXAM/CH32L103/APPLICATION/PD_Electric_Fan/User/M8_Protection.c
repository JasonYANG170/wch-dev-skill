/********************************** (C) COPYRIGHT *******************************
 * File Name          : M8_Protection.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/11/04
 * Description        : Protection parameter initialization, protection state setting

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
InstPhaseOC_Type  InstPhaseOC_M;        // Instantaneous over current protection structure
DCbusProtection_Type DCBusProtecion_M;  // Bus voltage protection structure
Phaseloss_Type Phaseloss_M;             // Phase loss protection structure
/*********************************************************************
 * @fn      Protection_and_Emergency_Parameter_Define
 *
 * @brief   Protection parameters are initialization
 *
 * @return  none
 */
void Protection_and_Emergency_Define (void)
{
    InstPhaseOC_M.Cnt = 0;
    InstPhaseOC_M.CurrThreshold = INST_PHASEOC_THRE_M;
    InstPhaseOC_M.CntThreshold = INST_PHASEOC_CNT_THRE_M;

    DCBusProtecion_M.OverThresh = DCBUS_OVER_VOLT_THRE;
    DCBusProtecion_M.UnderThresh = DCBUS_UNDER_VOLT_THRE;
    DCBusProtecion_M.OverCntThresh = DCBUS_OVER_VOLT_CNT;
    DCBusProtecion_M.UnderCntThresh = DCBUS_UNDER_VOLT_CNT;
    DCBusProtecion_M.OverCnt = 0;
    DCBusProtecion_M.UnderCnt = 0;
    DCBusProtecion_M.Overvolt = FALSE;
    DCBusProtecion_M.Undervolt = FALSE;

    Phaseloss_M.CurrThresh = PHASELOSS_CURR_THRE;
    Phaseloss_M.CntThresh = (uint16_t)(PHASELOSS_TIME_THRE * PHASELOSS_CAL_FREQ);
    Phaseloss_M.MultiphaseCnt = 0;
    Phaseloss_M.SiglephaseCntA = 0;
    Phaseloss_M.SiglephaseCntB = 0;
    Phaseloss_M.SiglephaseCntC = 0;
    Phaseloss_M.CurrBuffPara.Length = PHASELOSS_CAL_BUFFSIZE;
    Phaseloss_M.CurrBuffPara.Inv1 = _IQ(1.0)/PHASELOSS_CAL_BUFFSIZE;
    Phaseloss_M.CurrBuffPara.Inv2 = _IQ(1.0) - Phaseloss_M.CurrBuffPara.Inv1;

    Phaseloss_M.DetectphaseSet = (uint16_t)(PHASELOSS_CAL_FREQ*PHASELOSS_DETECT_TIME);
}
/******************************************************************************
* Function Name  : Protection_and_Emergency_Init
* Description    : 故障保护参数初始化
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Protection_and_Emergency_Init (void)
{
    InstPhaseOC_M.Cnt = 0;
    DCBusProtecion_M.OverCnt = 0;
    DCBusProtecion_M.UnderCnt = 0;
    DCBusProtecion_M.Overvolt = FALSE;
    DCBusProtecion_M.Undervolt = FALSE;

    Phaseloss_M.MultiphaseCnt = 0;
    Phaseloss_M.SiglephaseCntA = 0;
    Phaseloss_M.SiglephaseCntB = 0;
    Phaseloss_M.SiglephaseCntC = 0;
    Phaseloss_M.DetectphaseSet = (uint16_t)(PHASELOSS_CAL_FREQ*PHASELOSS_DETECT_TIME);
    Phaseloss_M.Detectphase=FALSE;


}
/******************************************************************************
* Function Name  : Protection_SetFault
* Description    : 故障设定
* Input          : 错误类型
* Output         : None
* Return         : None
******************************************************************************/
void Protection_SetFault(uint16_t Fault_type)
{
    //设定故障存留后时间 ，故障清除前判定(清除过快可能无法读到)
    Time_Delay_Set(Operation_M.FaultHoldingSet,&(Operation_M.FaultHoldingCnt));

    TIM_CtrlPWMOutputs(TIM1, DISABLE);  //PWM停止输出

    ADC_Start(DISABLE);//ADC启动

    Operation_M.Checkmode =0;

    RunningStatus_M = FAULT;            //电机运行状态设置为故障

    System_Status_Global |= Fault_type; //设置全局变量的故障位
}

/******************************************************************************
* Function Name  : Protction_Fault_Reset_M
* Description    : 故障复位
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Protction_Fault_Reset_M(void)
{
    if (Time_Delay_Elapse(Operation_M.FaultHoldingCnt))         //设定延迟时间到
    {
        System_Status_Global &= ~STARTUP_FAILURE_M;         //启动失败故障清除

        System_Status_Global &= ~SPEED_ABNORMAL_M;          //转速检测异常故障清除
        LBG_Status_M.Unstable_CNT = 0;

        System_Status_Global &= ~MOTOR_OVER_CURR_SOFT_M;    //软件过流故障清除
        InstPhaseOC_M.Cnt = 0;                              //软件过流判定次数清零

        if (GPIO_ReadInputDataBit(BRK_GPIO_M, BRK_PIN_M))   //不存在硬件过流故障
          System_Status_Global &= ~DC_OVER_CURR_HARD_M;     //清除故障标志

        System_Status_Global &= ~MOTOR_STALL_M;             //堵转异常故障清除
        LBG_Status_M.Stall_CNT = 0;

        System_Status_Global &= ~PHASELOSS_FALUT;           //缺相异常故障清除
        Phaseloss_M.MultiphaseCnt = 0;
        Phaseloss_M.SiglephaseCntA = 0;
        Phaseloss_M.SiglephaseCntB = 0;
        Phaseloss_M.SiglephaseCntC = 0;

        if((System_Status_Global&DC_OVER_VOLT)==DC_OVER_VOLT)       //对于母线过压故障
        {
            if(ADC_M.DCVoltAver < DCBusProtecion_M.OverThresh)   //判断此时是否小于母线过压阈值
            {
                System_Status_Global&= ~DC_OVER_VOLT;               //清除母线过压故障标志
                DCBusProtecion_M.OverCnt = 0;
            }
        }
        if((System_Status_Global&DC_UNDER_VOLT)==DC_UNDER_VOLT)      //对于母线欠压故障
        {
            if(ADC_M.DCVoltAver > DCBusProtecion_M.UnderThresh)  //判断此时是否大于母线欠压阈值
            {
                System_Status_Global&= ~DC_UNDER_VOLT;              //清除母线欠压故障标志
                DCBusProtecion_M.UnderCnt = 0;
            }
        }
    }

    //重新判定故障
    if ( (System_Status_Global & (STARTUP_FAILURE_M |SPEED_ABNORMAL_M | MOTOR_OVER_CURR_SOFT_M | MOTOR_OVERLOAD_M\
             | DC_OVER_CURR_HARD_M | MOTOR_STALL_M |PHASELOSS_FALUT | DC_OVER_VOLT | DC_UNDER_VOLT)) == 0 )
    {
      RunningStatus_M = IDLE;                   //置位IDLE状态
    }
}

/******************************************************************************
* Function Name  : DCOvervolt_Protection
* Description    : 母线过压保护
* Input          : 检测母线电压，电压保护结构体指针
* Output         : None
* Return         : None
******************************************************************************/
void DCOvervolt_Protection(_iq24 DCVolt, DCbusProtection_Type *DCbusProtec)
{
    if (DCVolt >= DCbusProtec->OverThresh)
        DCbusProtec->OverCnt++;
    else
        DCbusProtec->OverCnt = 0;

    if(DCbusProtec->OverCnt > DCbusProtec->OverCntThresh)
    {
        Protection_SetFault(DC_OVER_VOLT);//母线过压
        DCBusProtecion_M.Overvolt = TRUE;
    }
}
/*********************************************************************
 * @fn      DCUndervolt_Protection
 *
 * @brief   DC bus under voltage protection
 *
 *  @para   DCVolt: DC bus voltage
 *          DCbusProtec: Bus voltage protection structure pointer
 *
 * @return  none
 */
void DCUndervolt_Protection(_iq24 DCVolt, DCbusProtection_Type *DCbusProtec)
{
    if (DCVolt <= DCbusProtec->UnderThresh)
         DCbusProtec->UnderCnt++;
    else
        DCbusProtec->UnderCnt = 0;

    if(DCbusProtec->UnderCnt > DCbusProtec->UnderCntThresh)
    {
        Protection_SetFault(DC_UNDER_VOLT);//母线欠压
        DCBusProtecion_M.Overvolt = TRUE;
    }
}
