/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v4x7_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v4x7_it.h"
#include "eth_driver.h"
#include "app_iochub.h"
extern volatile uint32_t print_flag;
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));
extern void uart1_isr (void);
/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
  while (1)
  {
  }
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
  NVIC_SystemReset();
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      ETH_IRQHandler
 *
 * @brief   This function handles ETH exception.
 *
 * @return  none
 */
void ETH_IRQHandler(void)
{
    WCHNET_ETHIsr();
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    static uint8_t counter = 0;
    if(counter>=WCHNETTIMERPERIOD)
    {
      counter = 0;
      WCHNET_TimeIsr(WCHNETTIMERPERIOD);
    }
    counter++;
    WCHIOCHUB_TimeIsr();
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/*********************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   This function handles USART1 exception.
 *
 * @return  none
 */
void USART1_IRQHandler (void) {
    uart1_isr();
}