/********************************** (C) COPYRIGHT *******************************
* File Name          : ch641_pwr.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/06/14
* Description        : This file provides all the PWR firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <ch641_pwr.h>
#include <ch641_rcc.h>

/* CTLR register bit mask */
#define CTLR_PLS_MASK    ((uint32_t)0xFFFFFF1F)
#define AWUWR_MASK       ((uint32_t)0xFFFFFFF0)

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
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
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
 * @fn      PWR_PVDLevelConfig
 *
 * @brief   Configures the voltage threshold detected by the Power Voltage
 *        Detector(PVD).
 *
 * @param   PWR_PVDLevel - specifies the PVD detection level
 *            PWR_PVDLevel_0 - PVD detection level set to mode 0 
 *            PWR_PVDLevel_1 - PVD detection level set to mode 1
 *            PWR_PVDLevel_2 - PVD detection level set to mode 2
 *            PWR_PVDLevel_3 - PVD detection level set to mode 3
 *            PWR_PVDLevel_4 - PVD detection level set to mode 4
 *            PWR_PVDLevel_5 - PVD detection level set to mode 5
 *            PWR_PVDLevel_6 - PVD detection level set to mode 6
 *            PWR_PVDLevel_7 - PVD detection level set to mode 7
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
 * @fn      PWR_AutoWakeUpCmd
 *
 * @brief   Enables or disables the Auto WakeUp functionality.
 *
 * @param   NewState - new state of the Auto WakeUp functionality
 *        (ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_AutoWakeUpCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->AWUCSR |= (1 << 1);
    }
    else
    {
        PWR->AWUCSR &= ~(1 << 1);
    }
}

/*********************************************************************
 * @fn      PWR_AWU_SetWindowValue
 *
 * @brief   Sets the WWDG window value
 *
 * @param   WindowValue - specifies the window value to be compared to the
 *        downcounter,which must be lower than 0xF
 *
 * @return  none
 */
void PWR_AWU_SetWindowValue(uint8_t WindowValue)
{
    __IO uint32_t tmpreg = 0;

    tmpreg = PWR->AWUWR & AWUWR_MASK;

    tmpreg |= WindowValue;

    PWR->AWUWR  = tmpreg;
}

/*********************************************************************
 * @fn      PWR_EnterSTOPMode
 *
 * @brief   Enters STOP mode.
 *
 * @param   PWR_STOPEntry - specifies if STOP mode in entered with WFI or WFE instruction.
 *            PWR_STOPEntry_WFI - enter STOP mode with WFI instruction
 *            PWR_STOPEntry_WFE - enter STOP mode with WFE instruction
 *
 * @return  none
 */
void PWR_EnterSTOPMode(uint8_t PWR_STOPEntry)
{
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
 * @fn      PWR_EnterSTANDBYMode
 *
 * @brief   Enters STANDBY mode.
 *
 * @param   PWR_STANDBYEntry - specifies if STANDBY mode in entered with WFI or WFE instruction.
 *            PWR_STANDBYEntry_WFI - enter STANDBY mode with WFI instruction
 *            PWR_STANDBYEntry_WFE - enter STANDBY mode with WFE instruction
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry)
{
    PWR->CTLR |= PWR_CTLR_PDDS;

    NVIC->SCTLR |= (1 << 2);

    if(PWR_STANDBYEntry == PWR_STANDBYEntry_WFI)
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

