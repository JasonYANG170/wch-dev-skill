/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32m030_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32m030_it.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    if(FLASH_GetECCFlagStatus(FLASH_FLAG_ECC_CKEFail) != RESET)
    {
        if(FLASH_GetECCFlagStatus(FLASH_FLAG_ECC_CKEFail_RECSuc) != RESET)
        {
            printf("ECC check failed ,but correct succeed !\r\n");
        }
        if(FLASH_GetECCFlagStatus(FLASH_FLAG_ECC_CKEFail_RECFail) != RESET)
        {
            printf("ECC check failed ,and correct failed !\r\n");
        }
    }

    FLASH_ClearECCFlag(FLASH_FLAG_ECC_CKEFail);
    while(1)
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


