/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH59x_adc.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH59x_ADC_H__
#define __CH59x_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_CFG_TMP_25C    0x40230

#define TOUCH_CX_ChannelCfg(d)      (R8_ADC_CHANNEL = d)
#define TOUCH_CH_0                  (1 << CH_EXTIN_0)
#define TOUCH_CH_1                  (1 << CH_EXTIN_1)
#define TOUCH_CH_2                  (1 << CH_EXTIN_2)
#define TOUCH_CH_3                  (1 << CH_EXTIN_3)
#define TOUCH_CH_4                  (1 << CH_EXTIN_4)
#define TOUCH_CH_5                  (1 << CH_EXTIN_5)
#define TOUCH_CH_6                  (1 << CH_EXTIN_6)
#define TOUCH_CH_7                  (1 << CH_EXTIN_7)
#define TOUCH_CH_8                  (1 << CH_EXTIN_8)
#define TOUCH_CH_9                  (1 << CH_EXTIN_9)
#define TOUCH_CH_10                 (1 << CH_EXTIN_10)
#define TOUCH_CH_11                 (1 << CH_EXTIN_11)
#define TOUCH_CH_12                 (1 << CH_EXTIN_12)
#define TOUCH_CH_13                 (1 << CH_EXTIN_13)
/**
 * @brief  Touch capacitor charging cycle
 */
typedef enum
{
    TKEY_CC_1Tsys = 0,   // 1Tsys
    TKEY_CC_2Tsys,       // 2Tsys
    TKEY_CC_3Tsys,       // 3Tsys
    TKEY_CC_4Tsys,       // 4Tsys
} TKEY_CCTypeDef;

/**
 * @brief  TouchKey capacitor charge transfer time
 */
typedef enum
{
    TKEY_TRANS_1Tsys = 0,   // 1Tsys
    TKEY_TRANS_4Tsys,       // 4Tsys
    TKEY_TRANS_8Tsys,       // 8Tsys
    TKEY_TRANS_12Tsys,      // 12Tsys
    TKEY_TRANS_16Tsys,      // 16Tsys
    TKEY_TRANS_24Tsys,      // 24Tsys
    TKEY_TRANS_32Tsys,      // 32Tsys
    TKEY_TRANS_48Tsys,      // 48Tsys
} TKEY_TRANSTypeDef;

/**
 * @brief  TouchKey start ADC time select in advance
 */
typedef enum
{
    TKEY_START_SEL_2 = 0,   // 2个搬移周期
    TKEY_START_SEL_4,       // 4个搬移周期
    TKEY_START_SEL_8,       // 8个搬移周期
    TKEY_START_SEL_16,      // 16个搬移周期
} TKEY_StartSelectTypeDef;

/**
 * @brief  adc single channel define
 */
typedef enum
{
    CH_EXTIN_0 = 0,   // ADC 外部模拟通道 0
    CH_EXTIN_1,       // ADC 外部模拟通道 1
    CH_EXTIN_2,       // ADC 外部模拟通道 2
    CH_EXTIN_3,       // ADC 外部模拟通道 3
    CH_EXTIN_4,       // ADC 外部模拟通道 4
    CH_EXTIN_5,       // ADC 外部模拟通道 5
    CH_EXTIN_6,       // ADC 外部模拟通道 6
    CH_EXTIN_7,       // ADC 外部模拟通道 7
    CH_EXTIN_8,       // ADC 外部模拟通道 8
    CH_EXTIN_9,       // ADC 外部模拟通道 9
    CH_EXTIN_10,      // ADC 外部模拟通道 10
    CH_EXTIN_11,      // ADC 外部模拟通道 11
    CH_EXTIN_12,      // ADC 外部模拟通道 12
    CH_EXTIN_13,      // ADC 外部模拟通道 13

    CH_INTE_VBAT = 14,  // ADC 内部电池检测通道
    CH_INTE_VTEMP = 15, // ADC 内部温度传感器检测通道

} ADC_SingleChannelTypeDef;

/**
 * @brief  adc differential channel define
 */
typedef enum
{
    CH_DIFF_0_2 = 0, // ADC 差分通道 #0-#2
    CH_DIFF_1_3,     // ADC 差分通道 #1-#3

} ADC_DiffChannelTypeDef;

/**
 * @brief  adc sampling clock
 */
typedef enum
{
    SampleFreq_16 = 0,  // 16M 采样频率
    SampleFreq_8,       // 8M  采样频率
} ADC_SampClkTypeDef;

/**
 * @brief  adc signal PGA
 */
typedef enum
{
    ADC_PGA_1_4 = 0,    // -12dB, 1/4倍
    ADC_PGA_1_2,        // -6dB, 1/2倍
    ADC_PGA_0,          // 0dB, 1倍，无增益
    ADC_PGA_2,          // 6dB, 2倍
} ADC_SignalPGATypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum
{
    ADC_Mode_Single = 0, // 单次模式
    ADC_Mode_LOOP,       // 循环模式
} ADC_DMAModeTypeDef;


/**
 * @brief   设置 ADC 采样通道
 *
 * @param   d   - refer to ADC_SingleChannelTypeDef
 */
#define ADC_ChannelCfg(d)      (R8_ADC_CHANNEL = d)

/**
 * @brief   设置 ADC 采样时钟
 *
 * @param   d   - refer to ADC_SampClkTypeDef
 */
#define ADC_SampClkCfg(d)      (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_CLK_DIV) | (d << 6))

/**
 * @brief   设置 ADC 信号增益
 *
 * @param   d   - refer to ADC_SignalPGATypeDef
 */
#define ADC_PGACfg(d)          (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_PGA_GAIN) | (d << 4))

/**
 * @brief   设置内部温度传感器校准值
 *
 * @param   d   - 校准值
 */
#define ADC_TempCalibCfg(d)    (R8_TEM_SENSOR = R8_TEM_SENSOR & (~RB_TEM_SEN_CALIB) | d)

/**
 * @brief   外部信号单通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 */
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

/**
 * @brief   外部信号差分通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 */
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

/**
 * @brief   触摸按键通道采样初始化
 */
void TouchKey_ChSampInit(void);

/**
 * @brief   关闭TouchKey电源
 */
#define TouchKey_DisableTSPower()    (R8_TKEY_CFG &= ~RB_TKEY_PWR_ON)

/**
 * @brief   内置温度传感器采样初始化
 */
void ADC_InterTSSampInit(void);

/**
 * @brief   关闭温度传感器电源
 */
#define ADC_DisableTSPower()    (R8_TEM_SENSOR = 0)

/**
 * @brief   内置电池电压采样初始化
 */
void ADC_InterBATSampInit(void);

/**
 * @brief   ADC执行单次转换
 *
 * @return  ADC转换后的数据
 */
uint16_t ADC_ExcutSingleConver(void);

/**
 * @brief   采样数据粗调,获取偏差值,必须先配置ADC后调用此函数获取校准值
 *
 * @return  偏差
 */
signed short ADC_DataCalib_Rough(void);

/**
 * @brief   TouchKey转换后数据
 *
 * @param   charg   - Touchkey充电时间,5bits有效, t=charg*Tadc
 * @param   disch   - Touchkey放电时间,3bits有效, t=disch*Tadc
 *
 * @return  当前TouchKey等效数据
 */
uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch);

/**
 * @brief   设置连续 ADC的周期
 *
 * @param   cycle   - 单位为 16个系统时钟
 */
void ADC_AutoConverCycle(uint8_t cycle);

/**
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 */
void ADC_DMACfg(uint8_t s, uint32_t startAddr, uint32_t endAddr, ADC_DMAModeTypeDef m);

/**
 * @brief   触摸寄存器初始化
 *
 * @param   sp      - ADC时钟频率选择
 * @param   ga      - ADC的输入PGA增益选择
 * @param   sc      - 屏蔽通道选择
 * @param   s       - 驱动屏蔽使能
 *
 * @return  none
 */
void Touch_RegCfg( ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga, uint16_t sc, uint8_t s);

/**
 * @brief   获取单次触摸通道采样值
 *
 * @param   channel      - 触摸通道选择
 * @param   cc           - 触摸按键电容充电时间
 * @param   trans        - 触摸按键电容电荷搬移时间
 * @param   transfer_n   - 搬移次数
 * @param   start_sel    - 触摸触摸按钮提前启动ADC时间选择
 *
 * @return  单次触摸通道采样值
 */
uint16_t Touch_GetSingleValue( uint16_t channel, uint8_t cc, uint8_t trans, uint16_t transfer_n, uint8_t start_sel);

/**
 * @brief   Convert ADC value to temperature(Celsius)
 *
 * @param   adc_val - adc value
 *
 * @return  temperature (Celsius)
 */
int adc_to_temperature_celsius(uint16_t adc_val);

/**
 * @brief   获取ADC转换值
 *
 * @return  ADC转换值
 */
#define ADC_ReadConverValue()     (R16_ADC_DATA)

/**
 * @brief   ADC执行单次转换
 */
#define ADC_StartUp()             (R8_ADC_CONVERT |= RB_ADC_START)

/**
 * @brief   获取ADC中断状态
 */
#define ADC_GetITStatus()         (R8_ADC_INT_FLAG & RB_ADC_IF_EOC)

/**
 * @brief   清除ADC中断标志
 */
#define ADC_ClearITFlag()         (R8_ADC_CONVERT = R8_ADC_CONVERT)

/**
 * @brief   获取ADC DMA完成状态
 */
#define ADC_GetDMAStatus()        (R8_ADC_DMA_IF & RB_ADC_IF_DMA_END)

/**
 * @brief   清除ADC DMA完成标志
 */
#define ADC_ClearDMAFlag()        (R8_ADC_DMA_IF |= RB_ADC_IF_DMA_END)

/**
 * @brief   获取ADC DMA ADC转换完成完成状态
 */
#define ADC_GetDMA_EOCStatus()    (R8_ADC_DMA_IF & RB_ADC_IF_END_ADC)

/**
 * @brief   清除ADC DMA ADC转换完成完成标志
 */
#define ADC_ClearDMA_EOCFlag()    (R8_ADC_DMA_IF |= RB_ADC_IF_END_ADC)

/**
 * @brief   开启定时间隔自动连续 ADC
 */
#define ADC_StartAutoDMA()        (R8_ADC_CTRL_DMA |= RB_ADC_AUTO_EN)

/**
 * @brief   停止定时间隔自动连续 ADC
 */
#define ADC_StopAutoDMA()         (R8_ADC_CTRL_DMA &= ~RB_ADC_AUTO_EN)

/**
 * @brief   开启连续转换 ADC
 */
#define ADC_StartContDMA()        (R8_ADC_CTRL_DMA |= RB_ADC_CONT_EN)

/**
 * @brief   停止连续转换 ADC
 */
#define ADC_StopContDMA()         (R8_ADC_CTRL_DMA &= ~RB_ADC_CONT_EN)

/**
 * @brief   关闭ADC电源
 */
#define ADC_DisablePower()        (R8_ADC_CFG &= ~RB_ADC_POWER_ON)

#ifdef __cplusplus
}
#endif

#endif // __CH59x_ADC_H__
