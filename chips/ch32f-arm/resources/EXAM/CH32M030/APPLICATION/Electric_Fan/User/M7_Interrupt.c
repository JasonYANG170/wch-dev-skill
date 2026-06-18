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
    PFIC->IPRIOR[SysTick_IRQn]|=((0XE0));
    NVIC_EnableIRQ(SysTick_IRQn);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
    TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
    NVIC_EnableIRQ(TIM1_BRK_IRQn);

    NVIC_EnableIRQ(ADC_IRQn);
}

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   Timer 1 underflow interrupt
 *
 * @return  none
 */
//__attribute__((section(".highcode")))
void TIM1_UP_IRQHandler(void)
{
    if( RunningStatus_M ==PRESTART)
    {
        TIM1->CH4CVR = 10;
        ADC_Start(ENABLE);
        RunningStatus_M=DIRCHECK;
    }

    if((RunningStatus_M!=RUN)&&(RunningStatus_M!=DIRCHECK))
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    DCBUS_Volt_Cal(&ADC_M,ADC1);

    if((Flystart_M.Process_mark==TRUE)&&(RunningStatus_M == DIRCHECK))
     {
         Flystart_Switch(&Flystart_M,&SpeedRamp_M);
     }
    TIM1->INTFR = (uint16_t)~TIM_FLAG_Update;
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
