/********************************** (C) COPYRIGHT *******************************
* File Name          : CH59x_led.h
* Author             : WCH
* Version            : V1.0
* Date               : 2024/11/20
* Description        : LED驱动头文件
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH59x_LED_H__
#define __CH59x_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

//start of led controller
typedef enum {
    CH59X_LED_OUT_MODE_SINGLE = 0,  //data map to LED0
    CH59X_LED_OUT_MODE_DOUBLE,      //data map to LED0,LED1
    CH59X_LED_OUT_MODE_FOUR,        //data map to LED0,LED1,LED2,LED3
    CH59X_LED_OUT_MODE_FOUR_EXT,    //data map to LED0,LED1, data_aux map to LED2,LED3
}ch59x_led_out_mode_t;

/**
 * @brief  Configuration LED mode
 */
typedef enum
{
    Mode_Els = 0,       // 其它通道模式
    Mode_FourExtCHn,    // 4通道模式
} LEDModeTypeDef;

/**
 * @brief   LED输出使能
 */
#define  LED_ENABLE()   (R8_LED_CTRL_MOD |= RB_LED_OUT_EN)

/**
 * @brief   LED输出失能
 */
#define  LED_DISABLE()  (R8_LED_CTRL_MOD &= ~(RB_LED_DMA_EN))

/**
 * @brief   清除LED中断标志
 *
 * @param   f       - refer to LED interrupt bit define
 */
#define LED_ClearITFlag(f)    (R8_LED_STATUS = f)

/**
 * @brief   查询LED中断标志状态
 *
 * @param   f       - refer to LED interrupt bit define
 */
#define LED_GetITFlag(f)      (R8_LED_STATUS & f)

void ch59x_led_controller_init(ch59x_led_out_mode_t mode, uint8_t led_clk_div);

void ch59x_led_controller_send(uint32_t startAddrMain, uint32_t startAddrAux, uint16_t length, LEDModeTypeDef m);

/**
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 */
void LED_DMACfg(uint8_t s, uint32_t startAddrMain, uint32_t startAddrAux, uint16_t len, LEDModeTypeDef m);


#ifdef __cplusplus
}
#endif

#endif  // __CH59x_LED_H__
