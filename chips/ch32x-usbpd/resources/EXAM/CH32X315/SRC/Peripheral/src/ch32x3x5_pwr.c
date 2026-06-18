/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32x3x5_pwr.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file provides all the PWR firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32x3x5_pwr.h"
#include "ch32x3x5_rcc.h"

/* PWR registers bit mask */
/* CTLR register bit mask */
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFE)
#define CTLR_PLS_MASK    ((uint32_t)0xFFFFFF9F)

/*********************************************************************
 * @fn      PWR_DeInit
 *
 * @brief   Deinitializes the PWR peripheral registers to their default
 *        reset values.
 *
 * @return  none
 */
void PWR_DeInit(void)
{
    RCC_HB1PeriphResetCmd(RCC_HB1Periph_PWR, ENABLE);
    RCC_HB1PeriphResetCmd(RCC_HB1Periph_PWR, DISABLE);
}

/*********************************************************************
 * @fn      PWR_PVDCmd
 *
 * @brief   Enables or disables the Power Voltage Detector(PVD).
 *
 * @param   NewState - new state of the PVD(ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_PVDCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 4);
    }
    else
    {
        PWR->CTLR &= ~(1 << 4);
    }
}

/*********************************************************************
 * @fn      PWR_PLANCmd
 *
 * @brief   Enables or disables the Power adjustment position preset function.
 *
 * @param   NewState - new state of the PLAN(ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_PLANCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 7);
    }
    else
    {
        PWR->CTLR &= ~(1 << 7);
    }
}

/*********************************************************************
 * @fn      PWR_PVDLevelConfig
 *
 * @brief   Configures the voltage threshold detected by the Power Voltage
 *        Detector(PVD).
 *
 * @param   PWR_PVDLevel - specifies the PVD detection level
 *            PWR_PVDLevel_MODE0 - PVD detection level set to mode 0.
 *            PWR_PVDLevel_MODE1 - PVD detection level set to mode 1.
 *            PWR_PVDLevel_MODE2 - PVD detection level set to mode 2.
 *            PWR_PVDLevel_MODE3 - PVD detection level set to mode 3.
 *
 * @return  none
 */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;
    tmpreg &= CTLR_PLS_MASK;
    tmpreg |= PWR_PVDLevel;
    PWR->CTLR = tmpreg;
}

/*********************************************************************
 * @fn      PWR_EnterSTOPMode
 *
 * @brief   Enters STOP mode.
 *
 * @param   PWR_Regulator - specifies the regulator state in STOP mode.
 *            PWR_Regulator_ON - STOP mode with regulator ON
 *            PWR_Regulator_LowPower - STOP mode with regulator in low power mode
 *          PWR_STOPEntry - specifies if STOP mode in entered with WFI or WFE instruction.
 *            PWR_STOPEntry_WFI - enter STOP mode with WFI instruction
 *            PWR_STOPEntry_WFE - enter STOP mode with WFE instruction
 *
 * @return  none
 */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;
    tmpreg &= CTLR_DS_MASK;
    tmpreg |= PWR_Regulator;
    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    if(PWR_STOPEntry == PWR_STOPEntry_WFI)
    {
        __WFI();
    }
    else
    {
        __WFE();
    }

    NVIC->SCTLR &= ~(1 << 2);
}

/*********************************************************************
 * @fn      PWR_GetFlagStatus
 *
 * @brief   Checks whether the specified PWR flag is set or not.
 *
 * @param   PWR_FLAG - specifies the flag to check.
 *            PWR_FLAG_PVDO - PVD Output
 *
 * @return  The new state of PWR_FLAG (SET or RESET).
 */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}


