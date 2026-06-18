/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_keyscan.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description        : head file(ch572/ch570)
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH59x_KEYSCAN_H__
#define __CH59x_KEYSCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Keyscan Pin Configuration
 */
#define   KEYSCAN_PA4     0x100
#define   KEYSCAN_PA5     0x200
#define   KEYSCAN_PA6     0x400
#define   KEYSCAN_PA7     0x800
#define   KEYSCAN_PA8     0x1000
#define   KEYSCAN_ALL     0x1F00

/*
 * @brief Keyscan Clock Division
 */
#define   KEYSCAN_DIV2     0x10
#define   KEYSCAN_DIV4     0x30
#define   KEYSCAN_DIV8     0x70
#define   KEYSCAN_DIV16    0xF0

/*
 * @brief Keyscan repetition times
 */
#define   KEYSCAN_REP1     0x02
#define   KEYSCAN_REP2     0x04
#define   KEYSCAN_REP3     0x06
#define   KEYSCAN_REP4     0x08
#define   KEYSCAN_REP5     0x0A
#define   KEYSCAN_REP6     0x0C
#define   KEYSCAN_REP7     0x0E

/*
 * @brief Keyscan Configuration
 */
void  KeyScan_Cfg(uint8_t s, uint16_t keyScanPin, uint16_t ClkDiv, uint16_t Rep);

/*
 * @brief Keypress Wakeup Enable
 */
void  KeyPress_Wake(uint8_t s);

/*
 * @brief Key Values Return
 */
#define   KeyValue   (R32_KEY_SCAN_NUMB & RB_KEY_SCAN_NUMB)

/*
 * @brief Keyscan Count
 */
#define   KeyScan_Cnt   (R32_KEY_SCAN_NUMB >> 20)

/**
 * @brief   KeyScan中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to ENC interrupt bit define
 */
#define KeyScan_ITCfg(s, f)       ((s) ? (R8_KEY_SCAN_INT_EN |= f) : (R8_KEY_SCAN_INT_EN &= ~f))

/**
 * @brief   清除ENC中断标志
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define KeyScan_ClearITFlag(f)    (R8_KEY_SCAN_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define KeyScan_GetITFlag(f)      (R8_KEY_SCAN_INT_FLAG & f)

/*
 * @brief 调节uldo，默认值+3档，注意调节uldo会影响lsi频率。
 */
__attribute__((always_inline)) RV_STATIC_INLINE void KeyScanPowAdj(void)
{
    if((R8_RESET_STATUS & 0x07) == 0x01 || (R8_RESET_STATUS & 0x07) == 0x05)
    {
        sys_safe_access_enable();
        R16_AUX_POWER_ADJ = ((R16_AUX_POWER_ADJ&0xFFF8) | ((R16_AUX_POWER_ADJ&0x07)+3));
        sys_safe_access_disable();
    }
}

#ifdef __cplusplus
}
#endif

#endif // __CH59x_KEYSCAN_H__
