/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_dbgmcu.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/09/05
 * Description        : This file provides all the DBGMCU firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32m030_dbgmcu.h"

#define IDCODE_DEVID_MASK    ((uint32_t)0x0000FFFF)

/*********************************************************************
 * @fn      DBGMCU_GetREVID
 *
 * @brief   Returns the device revision identifier.
 *
 * @return  Revision identifier.
 */
uint32_t DBGMCU_GetREVID(void)
{
	return ((*(uint32_t *)0x1FFFF384) & IDCODE_DEVID_MASK);
}

/*********************************************************************
 * @fn      DBGMCU_GetDEVID
 *
 * @brief   Returns the device identifier.
 *
 * @return  Device identifier.
 */
uint32_t DBGMCU_GetDEVID(void)
{
    return ((*(uint32_t *)0x1FFFF384) >> 16);
}

/*********************************************************************
 * @fn      DBGMCU_Config
 *
 * @brief   Configures the specified peripheral and low power mode behavior
 *        when the MCU under Debug mode.
 *
 * @param   DBGMCU_Periph - specifies the peripheral and low power mode.
 *            DBGMCU_WWDG_STOP - Debug WWDG stopped when Core is halted.
 *            DBGMCU_TIM1_STOP - TIM1 counter stopped when Core is halted
 *            DBGMCU_TIM2_STOP - TIM2 counter stopped when Core is halted
 *            DBGMCU_TIM3_STOP - TIM3 counter stopped when Core is halted
 *            DBGMCU_SLEEP - Keep debugger connection during SLEEP mode
 *            DBGMCU_STOP - Keep debugger connection during STOP mode
 *            DBGMCU_STANDBY - Keep debugger connection during STANDBY mode
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
    if((DBGMCU_Periph == DBGMCU_SLEEP) || (DBGMCU_Periph == DBGMCU_STOP) || (DBGMCU_Periph == DBGMCU_STANDBY))
    {
        if(NewState != DISABLE)
        {
            DBGMCU->CR2 |= (uint16_t)DBGMCU_Periph;
        }
        else
        {
            DBGMCU->CR2 &= ~(uint16_t)DBGMCU_Periph;
        }
    }
    else
    {
        if(NewState != DISABLE)
        {
            DBGMCU->CR1 |= DBGMCU_Periph;
        }
        else
        {
            DBGMCU->CR1 &= ~DBGMCU_Periph;
        }
    }
}

/*********************************************************************
 * @fn      DBGMCU_GetCHIPID
 *
 * @brief   Returns the CHIP identifier.
 *
 * @return Device identifier.
 *          ChipID List-
 *            CH32M030C8U7-0x03B00800
 *	          CH32M030C8T -0x03B10800
 *            CH32M030K8U -0x03B20800
 *            CH32M030C8U3-0x03B30800
 *            CH32M030G8R -0x03BB0800
 *            CH32M030C9U7-0x03B90800
 */
uint32_t DBGMCU_GetCHIPID( void )
{
    return( *( uint32_t * )0x1FFFF384 );
}
