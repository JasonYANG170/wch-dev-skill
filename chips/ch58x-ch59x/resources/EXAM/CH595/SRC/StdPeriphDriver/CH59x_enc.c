/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_keyscan.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2024/12/17
 * Description        : source file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

/*********************************************************************
 * @fn      ENC_Config
 *
 * @brief   配置编码器功能
 *
 * @param   s           - 是否开启编码器功能
 * @param   encReg      - 编码器模式终值(最大值0xFFFF)
 * @param   m           - 配置ENC模式
 *
 * @return  none
 */
void ENC_Config(uint8_t s, uint32_t encReg, ENCModeTypeDef m)
{
    if(s == DISABLE)
    {
        R8_ENC_REG_CTRL &= ~(RB_START_ENC_EN);
    }
    else
    {
        R8_ENC_REG_CTRL |= (m << 1) | RB_START_ENC_EN;
        R32_ENC_REG_CEND |= encReg;
    }
}


/*********************************************************************
 * @fn      ENC_Wake
 *
 * @brief   ENC唤醒睡眠使能
 *
 * @param   s            -  设置是否开启唤醒功能
 *
 * @return  none
 */
void ENC_Wake(uint8_t s)
{
    if(s == DISABLE)
    {
        sys_safe_access_enable();
        R8_SLP_CLK_OFF2 &= ~(RB_SLP_ENC_WAKE);
        sys_safe_access_disable();
    }
    else
    {
        sys_safe_access_enable();
        R8_SLP_CLK_OFF2 = RB_SLP_ENC_WAKE;
        sys_safe_access_disable();
    }
}
