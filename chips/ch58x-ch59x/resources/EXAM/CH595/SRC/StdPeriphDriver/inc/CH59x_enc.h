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

#ifndef __CH59x_ENC_H__
#define __CH59x_ENC_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  Configuration ENC mode
 */
typedef enum
{
    Mode_IDLE = 0,   // IDLE模式
    Mode_T2 ,        // T2边沿计数模式
    Mode_T1 ,        // T1边沿计数模式
    Mode_T1T2 ,      // T1和T2边沿计数模式
} ENCModeTypeDef;


/**
 * @brief   配置ENC功能
 *
 * @param   s           - 是否开启编码器功能
 * @param   encReg      - 编码器模式终值(最大值0xFFFF)
 * @param   m           - 配置ENC模式
 *
 * @return  none
 */
void ENC_Config(uint8_t s, uint32_t encReg, ENCModeTypeDef m);

/**
 * @brief   配置ENC唤醒功能
 *
 * @param   s           - 是否开启功能
 *
 * @return  none
 */
void ENC_Wake(uint8_t s);

/**
 * @brief   获取编码器当前方向
 *
 * @return  方向值  0:前进  1:后退
 */
#define ENC_GetCurrentDir       (R8_ENC_REG_CTRL>>5 & 0x01)

/**
 * @brief   获取编码器当前计数值
 */
#define ENC_GetCurrentCount      R32_ENC_REG_CCNT

/**
 * @brief   编码器模式读计数并清0
 */
#define ENC_GetCountandReset()  R8_ENC_REG_CTRL |= RB_RD_CLR_EN

/**
 * @brief   ENC中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_ITCfg(s, f)         ((s) ? (R8_ENC_INTER_EN |= f) : (R8_ENC_INTER_EN &= ~f))

/**
 * @brief   清除ENC中断标志
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_ClearITFlag(f)      (R8_ENC_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to ENC interrupt bit define
 */
#define ENC_GetITFlag(f)        (R8_ENC_INT_FLAG & f)

#ifdef __cplusplus
}
#endif

#endif // __CH59x_ENC_H__
