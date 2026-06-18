/********************************** (C) COPYRIGHT *******************************
 * File Name          : M7_Interrupt.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/12/16
 * Description        : Interrupt configuration

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
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_BRK_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void Waveform_Display (void);

extern uint16_t AdcTrig;
/* Private variables --------------------------------------------------------*/
/* Variables ----------------------------------------------------------------*/
/*********************************************************************
 * @fn      Interrupt_Configuration
 *
 * @brief   Interrupt priority configuration
 *
 * @return  none
 */
void Interrupt_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    TIM_ITConfig(TIM1, TIM_IT_Break,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = SysTicK_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   Timer 1 underflow interrupt
 *
 * @return  none
 */
__attribute__((section(".highcode")))
void TIM1_UP_IRQHandler(void)
{
    PD_Seek();

    if( RunningStatus_M ==PRESTART)
    {
        TIM3->CH4CVR = SVPWM_Cal_M.HALF_T-2;
        TIM3->CH3CVR = SVPWM_Cal_M.HALF_T-2;
        AdcTrig=SVPWM_Cal_M.HALF_T-2+2*(SYSTEM_CLOCK_FREQ/1000000);

        Mpid_M.dCurr.Kp=Mpid_M.dCurr.Kp<<1;
        Mpid_M.dCurr.Ki=Mpid_M.dCurr.Ki<<1;
        Mpid_M.qCurr.Kp=Mpid_M.qCurr.Kp<<1;
        Mpid_M.qCurr.Ki=Mpid_M.qCurr.Ki<<1;

        ADC_Start(ENABLE);
        RunningStatus_M = DIRCHECK;
    }

    if((RunningStatus_M == START)||(RunningStatus_M == RUN)||(RunningStatus_M == POSITION)||(RunningStatus_M == DIRCHECK))
    {
        TIM3->CH4CVR = SVPWM_Cal_M.HALF_T-2;
        TIM3->CH3CVR = SVPWM_Cal_M.HALF_T-2;
        AdcTrig=3390;
    }

    if((Flystart_M.Process_mark==TRUE)&&(RunningStatus_M == DIRCHECK))
     {
         Flystart_Switch(&Flystart_M,&Ramp_M);
     }

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    DCBUS_Volt_Cal(&ADC_M,ADC1);

    Waveform_Display();
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
}
/*********************************************************************
 * @fn      TIM1_BRK_IRQHandler
 *
 * @brief   Brake interrupt
 *
 * @return  none
 */
void TIM1_BRK_IRQHandler(void)
{
    Protection_SetFault(DC_OVER_CURR_HARD_M);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
}
/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{

}
/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  while (1)
  {
      //NVIC_SystemReset();
  }
}
