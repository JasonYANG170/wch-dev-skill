/********************************** (C) COPYRIGHT *******************************
* File Name          : ch58x_drv_ledc.c
* Author             : WCH
* Version            : V1.0
* Date               : 2024/11/20
* Description        : LED驱动相关
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

/*********************************************************************
 * @fn      ch59x_led_controller_init
 *
 * @brief   LED初始化
 *
 * @return  none
 */
void ch59x_led_controller_init(ch59x_led_out_mode_t mode, uint8_t led_clk_div)
{
    R8_LED_CLOCK_DIV  = led_clk_div;
    R8_LED_CTRL_MOD = 0;
    R8_LED_CTRL_MOD |= (mode<<6);
    R8_LED_CTRL_MOD |= RB_LED_BIT_ORDER;
}

/*********************************************************************
 * @fn      ch59x_led_controller_send
 *
 * @brief   LED数据使用DMA发送
 *
 * @return  none
 */
void ch59x_led_controller_send(uint32_t startAddrMain, uint32_t startAddrAux, uint16_t length, LEDModeTypeDef m)
{
    if(m)
    {
        R32_LED_DMA_MAIN = ((uint32_t)startAddrMain);
        R32_LED_DMA_AUX = ((uint32_t)startAddrAux);
        R16_LED_DMA_CNT = length;
        R8_LED_CTRL_MOD |= RB_LED_DMA_EN;
    }
    else
    {
        R32_LED_DMA_MAIN = ((uint32_t)startAddrMain);
        R16_LED_DMA_CNT = length;
        R8_LED_CTRL_MOD |= RB_LED_DMA_EN;
    }
}

/*********************************************************************
 * @fn      LED_DMACfg
 *
 * @brief   配置DMA功能
 *
 * @param   s               - 是否打开DMA功能
 * @param   startAddrMain   - DMA 主起始地址
 * @param   startAddrAux    - DMA 辅助起始地址（单通道模式不生效）
 * @param   len             - DMA 发送长度
 * @param   m               - 配置LED模式
 *
 * @return  none
 */
void LED_DMACfg(uint8_t s, uint32_t startAddrMain, uint32_t startAddrAux, uint16_t len, LEDModeTypeDef m)
{
    if(s == DISABLE)
    {
        R8_LED_CTRL_MOD &= ~RB_LED_DMA_EN;
    }
    else
    {
        if(m)
        {
            R32_LED_DMA_MAIN = ((uint32_t)startAddrMain);
            R32_LED_DMA_AUX = ((uint32_t)startAddrAux);
            R16_LED_DMA_CNT = len;
            R8_LED_CTRL_MOD |= RB_LED_DMA_EN;
        }
        else
        {
            R32_LED_DMA_MAIN = ((uint32_t)startAddrMain);
            R16_LED_DMA_CNT = len;
            R8_LED_CTRL_MOD |= RB_LED_DMA_EN;
        }
    }
}

