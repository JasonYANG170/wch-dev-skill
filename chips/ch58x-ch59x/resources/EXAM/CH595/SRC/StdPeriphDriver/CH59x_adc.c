/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH59x_adc.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

/*********************************************************************
 * @fn      ADC_DataCalib_Rough
 *
 * @brief   采样数据粗调,获取偏差值,必须先配置ADC后调用此函数获取校准值
 *
 * @param   none
 *
 * @return  偏差
 */
signed short ADC_DataCalib_Rough(void) // 采样数据粗调,获取偏差值
{
    uint16_t i;
    uint32_t sum = 0;
    uint8_t  ch = 0;   // 备份通道
    uint8_t  cfg = 0;   // 备份

    ch = R8_ADC_CHANNEL;
    cfg = R8_ADC_CFG;

    R8_ADC_CFG |= RB_ADC_OFS_TEST; // 进入测试模式
    R8_ADC_CFG &= ~RB_ADC_DIFF_EN; // 关闭差分

    R8_ADC_CONVERT |= RB_ADC_START;
    while(R8_ADC_CONVERT & RB_ADC_START);
    for(i = 0; i < 16; i++)
    {
        R8_ADC_CONVERT |= RB_ADC_START;
        while(R8_ADC_CONVERT & RB_ADC_START);
        sum += (~R16_ADC_DATA) & RB_ADC_DATA;
    }
    sum = (sum + 8) >> 4;

    R8_ADC_CFG = cfg;  // 恢复配置值
    R8_ADC_CHANNEL = ch;

    return (2048 - sum);
}

/*********************************************************************
 * @fn      ADC_ExtSingleChSampInit
 *
 * @brief   外部信号单通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 *
 * @return  none
 */
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga)
{
    R32_TKEY_CFG2 &= ~RB_TKEY_EN;
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (sp << 6) | ((ga&0xF) << 4);
}

/*********************************************************************
 * @fn      ADC_ExtDiffChSampInit
 *
 * @brief   外部信号差分通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 *
 * @return  none
 */
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga)
{
    R32_TKEY_CFG2 &= ~RB_TKEY_EN;
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (sp << 6) | ((ga&0xF) << 4);
}

/*********************************************************************
 * @fn      ADC_InterTSSampInit
 *
 * @brief   内置温度传感器采样初始化
 *
 * @param   none
 *
 * @return  none
 */
void ADC_InterTSSampInit(void)
{
    R32_TKEY_CFG2 &= ~RB_TKEY_EN;
    R8_TEM_SENSOR = RB_TEM_SEN_PWR_ON;
    R8_ADC_CHANNEL = CH_INTE_VTEMP;
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (3 << 4);
}

/*********************************************************************
 * @fn      ADC_InterBATSampInit
 *
 * @brief   内置电池电压采样初始化
 *
 * @param   none
 *
 * @return  none
 */
void ADC_InterBATSampInit(void)
{
    R32_TKEY_CFG2 &= ~RB_TKEY_EN;
    R8_ADC_CHANNEL = CH_INTE_VBAT;
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (0 << 4); // 使用-12dB模式
}

/*********************************************************************
 * @fn      ADC_ExcutSingleConver
 *
 * @brief   ADC执行单次转换
 *
 * @param   none
 *
 * @return  ADC转换后的数据
 */
uint16_t ADC_ExcutSingleConver(void)
{
    R8_ADC_CONVERT |= RB_ADC_START;
    while(R8_ADC_CONVERT & RB_ADC_START);

    return (R16_ADC_DATA & RB_ADC_DATA);
}

/*********************************************************************
 * @fn      ADC_AutoConverCycle
 *
 * @brief   设置连续 ADC的周期
 *
 * @param   cycle   - 采样周期计算方法为(256-cycle)*16*Tsys
 *
 * @return  none
 */
void ADC_AutoConverCycle(uint8_t cycle)
{
    R8_ADC_AUTO_CYCLE = cycle;
}

/*********************************************************************
 * @fn      ADC_DMACfg
 *
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 *
 * @return  none
 */
void ADC_DMACfg(uint8_t s, uint32_t startAddr, uint32_t endAddr, ADC_DMAModeTypeDef m)
{
    if(s == DISABLE)
    {
        R8_ADC_CTRL_DMA &= ~(RB_ADC_DMA_ENABLE | RB_ADC_IE_DMA_END);
    }
    else
    {
        R16_ADC_DMA_BEG = startAddr&0xFFFF;
        R16_ADC_DMA_END = endAddr&0xFFFF;
        if(m)
        {
            R8_ADC_CTRL_DMA |= RB_ADC_DMA_LOOP | RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
        }
        else
        {
            R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_LOOP;
            R8_ADC_CTRL_DMA |= RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
        }
    }
}

/*********************************************************************
 * @fn      Touch_Reg_Cfg
 *
 * @brief   触摸寄存器初始化
 *
 * @param   sp      - ADC时钟频率选择
 * @param   ga      - ADC的输入PGA增益选择
 * @param   sc      - 屏蔽通道选择
 * @param   s       - 驱动屏蔽使能
 *
 * @return  none
 */
void Touch_RegCfg( ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga, uint16_t sc, uint8_t s)
{
    R8_ADC_CFG &= ~RB_ADC_DIFF_EN;

    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | ( (ga&0xF) << 4 ) | ( sp << 6 );

    R32_TKEY_SEL &=~ RB_DRIVEROUT_EN;
    if(s == DISABLE){
        R8_TKEY_CFG &=~ RB_TKEY_DRV_EN;
    }else{
        R8_TKEY_CFG |= RB_TKEY_DRV_EN;                          //使能驱动屏蔽
        R32_TKEY_SEL |= (sc << 16) & RB_DRIVEROUT_EN;           //选择驱动屏蔽通道
    }

    R32_TKEY_CFG2 |= RB_TKEY_EN;                                //使能TouchKey
}

/*********************************************************************
 * @fn      Touch_GetSingleValue
 *
 * @brief   获取单次触摸通道采样值
 *
 * @param   channel      - 触摸通道选择
 * @param   cc           - 触摸按键电容充电时间
 * @param   trans        - 触摸按键电容电荷搬移时间
 * @param   transfer_n   - 搬移周期次数
 * @param   start_sel    - 触摸触摸按钮提前启动ADC时间选择
 *
 * @return  单次触摸通道采样值
 */
uint16_t Touch_GetSingleValue( uint16_t channel, uint8_t cc, uint8_t trans, uint16_t transfer_n, uint8_t start_sel)
{
    R32_TKEY_SEL &=~ RB_SEL_S;
    R32_TKEY_SEL |= ( channel ) & RB_SEL_S;                 //设置搬移通道
    R32_TKEY_CFG2 &=~ RB_TKEY_CC;
    R32_TKEY_CFG2 |= ( cc << 3 ) & RB_TKEY_CC;              //设置触摸按键电容充电时间
    R32_TKEY_CFG2 &=~ RB_TKEY_TRANS;
    R32_TKEY_CFG2 |= trans & RB_TKEY_TRANS;                 //设置搬移时间
    R32_TKEY_CFG2 &=~ RB_TRANSFER_N;
    R32_TKEY_CFG2 |= (transfer_n << 16) & RB_TRANSFER_N;    //设置单通道搬移次数
    R32_TKEY_CFG2 &=~ RB_TKADC_START_SEL;                   //ADC使能信号提前打开的时间
    R32_TKEY_CFG2 |= (start_sel << 5) & RB_TKADC_START_SEL;
    R8_ADC_CONVERT |= RB_ADC_START;                         //ADC转换启动控制及状态
    R32_TKEY_CFG2 |= RB_TKEY_CTRANS_ACT;                    //启动检测
    while(( R8_ADC_CONVERT & RB_ADC_START ) != 0);
    return R16_ADC_DATA & RB_ADC_DATA;
}

/*********************************************************************
 * @fn      adc_to_temperature_celsius
 *
 * @brief   Convert ADC value to temperature(Celsius)
 *
 * @param   adc_val - adc value
 *
 * @return  temperature (Celsius)
 */

int adc_to_temperature_celsius(uint16_t adc_val)
{
    uint32_t C25 = 0;
    int      temp;

    C25 = (*((PUINT32)ROM_CFG_TMP_25C));

    /* current temperature = standard temperature + (adc deviation * adc linearity coefficient) */ 
    temp = (((C25 >> 16) & 0xFFFF) ? ((C25 >> 16) & 0xFFFF) : 25) + \
        (adc_val - ((int)(C25 & 0xFFFF))) * 100 / 283;

    return (temp);
}
