/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32m030_pwr.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/09/19
 * Description        : This file provides all the PWR firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32m030_pwr.h"
#include "ch32m030_rcc.h"

/* PWR registers bit mask */
/* CTLR register bit mask */
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFD)
#define AWUWR_MASK       ((uint32_t)0xFFFFFFF0)

volatile uint32_t ISINK_ADJ ;
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
    RCC_PB1PeriphResetCmd(RCC_PB1Periph_PWR, ENABLE);
    RCC_PB1PeriphResetCmd(RCC_PB1Periph_PWR, DISABLE);
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
 * @param   PWR_STANDBYEntry - specifies if STOP mode in entered with WFI or WFE instruction.
 *            PWR_STANDBYEntry_WFI - enter standby mode with WFI instruction
 *            PWR_STANDBYEntry_WFE - enter standby mode with WFE instruction
 */
void PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry)
{

    PWR->CTLR |= PWR_CTLR_PDDS;
    NVIC->SCTLR |= (1 << 2);

    if(PWR_STANDBYEntry == PWR_STOPEntry_WFI)
    {
        __WFI();
    }
    else
    {
        __WFE();
    }
    NVIC->SCTLR &= ~(1 << 2);
    PWR->CTLR &= ~PWR_CTLR_PDDS;
}

/*********************************************************************
 * @fn      PWR_ISINKCmd
 *
 * @brief   Enables or disables the ISINK functionality.
 *
 * @param   NewState - new state of the ISINK functionality
 *        (ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_ISINKCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 10);
    }
    else
    {
        PWR->CTLR &= ~(1 << 10);
    }
}

/*********************************************************************
 * @fn      PWR_PMUPROCmd
 *
 * @brief   Enables or disables the PMU Over Temperature Protection
 *        functionality.
 *
 * @param   NewState - new state of the PMU Over Temperature Protection
 *        functionality (ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_PMUPROCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 14);
    }
    else
    {
        PWR->CTLR &= ~(1 << 14);
    }
}

/*********************************************************************
 * @fn      PWR_PB4_Config
 *
 * @brief   Configures the PB4 GPIO function.
 *
 * @param   PWR_PB4 - the PB4 GPIO function.
 *            PWR_PB4_FLOATING - floating input for PB4 pin.
 *            PWR_PB4_GPIO_Mode - GPIO function for PB4 pin.
 *            PWR_PB4_VHV_Monitor - VHV Voltage divider monitoring for PB4 pin.
 *            PWR_PB4_Overvol_Protect - Over voltage protection for PB4 pin.
 *
 * @return  none
 */
void PWR_PB4_Config(uint32_t PWR_PB4)
{
    PWR->CTLR &= ~(3<<12);
    PWR->CTLR |= PWR_PB4;
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
 * @brief   Sets the AWU window value
 *
 * @param   WindowValue - specifies the window value to be compared to the
 *        down counter,which must be lower than 0x10.
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
 * @fn      PWR_VDD8_Config
 *
 * @brief   Configures VDD8 input voltage.
 *
 * @param   VDD8_SEL - the VDD8 voltage.
 *            PWR_VDD8_SEL_5V - VDD8 input voltage is 5V.
 *            PWR_VDD8_SEL_8V - VDD8 input voltage is 8V.
 *            PWR_VDD8_SEL_9V - VDD8 input voltage is 9V.
 *            PWR_VDD8_SEL_10V - VDD8 input voltage is 10V.
 *
 * @return  none
 */
void PWR_VDD8_Config(uint32_t VDD8_SEL)
{
    FLASH->CTLR2 &= ~(3<<5);
    FLASH->CTLR2 |= VDD8_SEL;
}

/*********************************************************************
 * @fn      ISINK_Config
 *
 * @brief   Configures ISINK1 data.
 *
 * @param   ISINK1_data - ISINK1 programmable data bits,which must be
 *        lower than 0x400.
 *
 * @return  none
 */
void ISINK1_Config(uint16_t ISINK_data)
{
    ISINK_ADJ &= ~(0x3FF<<6);
    ISINK_ADJ |= (uint32_t)((uint32_t)ISINK_data<<6);
    EXTEN->EXTEN_CTLR2 = ISINK_ADJ;
}

/*********************************************************************
 * @fn      ISINK2_Config
 *
 * @brief   Configures ISINK2 data.
 *
 * @param   ISINK2_data - ISINK2 programmable data bits,which must be
 *        lower than 0x400.
 *
 * @return  none
 */
void ISINK2_Config(uint16_t ISINK_data)
{
    ISINK_ADJ &= ~(0x3FF<<22);
    ISINK_ADJ |= (uint32_t)((uint32_t)ISINK_data<<22);
    EXTEN->EXTEN_CTLR2 = ISINK_ADJ;
}

/*********************************************************************
 * @fn      ISOURCE1_GetData
 *
 * @brief   Get ISOURCE1 current data.
 *
 * @param   ISOURCE_LEVEL_HIGH - ISOURCE output high gear level,The ISRC1_SEL
                        bit of the EXTEN_CTLR0 register selects the high bit.
 *          ISOURCE_LEVEL_LOW - ISOURCE output low gear level,The ISRC1_SEL 
                        bit of the EXTEN_CTLR0 register selects the low bit.
 *
 * @return  0 - Error(The value is invalid).
 *          other - The ISOURCE1 output current value (nA).
 */
uint32_t ISOURCE1_GetData(uint8_t ISOURCE_LEVEL)
{
    uint32_t ISourceValue = 0;

    if((*(uint32_t*)CFG1_ISOURCE_DATA) != 0xFFFFFFFF)
    {
        if(ISOURCE_LEVEL != 0)
        {
            ISourceValue = (*(uint32_t*)CFG1_ISOURCE_DATA)&(0xFFFF);
        }
        else
        {
            ISourceValue = ((*(uint32_t*)CFG1_ISOURCE_DATA)&(0xFFFF))>>2;
        }
        ISourceValue = ISourceValue*10;
    }

    return ISourceValue;
}

/*********************************************************************
 * @fn      ISOURCE2_GetData
 *
 * @brief   Get ISOURCE2 current data.
 *
 * @param   ISOURCE_LEVEL_HIGH - ISOURCE output high gear level,The ISRC2_SEL
                        bit of the EXTEN_CTLR0 register selects the high bit.
 *          ISOURCE_LEVEL_LOW - ISOURCE output low gear level,The ISRC2_SEL 
                        bit of the EXTEN_CTLR0 register selects the low bit.
 *
 * @return  0 - Error(The value is invalid).
 *          other - The ISOURCE2 output current value (nA).
 */
uint32_t ISOURCE2_GetData(uint8_t ISOURCE_LEVEL)
{
    uint32_t ISourceValue = 0;

    if((*(uint32_t*)CFG1_ISOURCE_DATA) != 0xFFFFFFFF)
    {
        if(ISOURCE_LEVEL != 0)
        {
            ISourceValue = (uint32_t)((*(uint32_t*)CFG1_ISOURCE_DATA)>>16);
        }
        else
        {
            ISourceValue = (uint32_t)((*(uint32_t*)CFG1_ISOURCE_DATA)>>18);
        }
        ISourceValue = ISourceValue*10;
    }

    return ISourceValue;
}
