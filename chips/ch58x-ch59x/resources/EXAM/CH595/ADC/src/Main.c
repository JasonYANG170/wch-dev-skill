/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/03/09
 * Description        : adc采样示例，包括温度检测、单通道检测、差分通道检测、TouchKey检测、中断方式采样。
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

uint16_t adcBuff[40];

volatile uint8_t adclen;
volatile uint8_t DMA_end = 0;
#define CHARGE_TIMES       46       //搬移次数，取值范围1~1023

/* 在应用上需要测量高精度的绝对值时，建议使用差分模式， 一端接地  */


/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   调试初始化
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main()
{

    uint8_t      i;
    signed short RoughCalib_Value = 0; // ADC粗调偏差值

    SetSysClock(SYSCLK_FREQ);

    /* 配置串口调试 */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);
    /* 温度采样并输出 */
    PRINT("\n1.Temperature sampling...\n");
    ADC_InterTSSampInit();
    for(i = 0; i < 20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver(); // 连续采样20次
    }
    for(i = 0; i < 20; i++)
    {
        uint32_t C25 = 0;
        C25 = (*((PUINT32)ROM_CFG_TMP_25C));
        PRINT("%d %d %d \n", adc_to_temperature_celsius(adcBuff[i]),adcBuff[i],C25);
    }


    /* 单通道采样：选择adc通道0做采样，对应 PA4引脚， 带数据校准功能 */
    PRINT("\n2.Single channel sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_Floating);
    ADC_ChannelCfg(0);      //默认ADC通道为内部温度传感器，切换到其他通道使用时，推荐至少间隔25个时钟周期后再进行ADC采样
    ADC_ExtSingleChSampInit(SampleFreq_16, ADC_PGA_0);

    RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    PRINT("RoughCalib_Value =%d \n", RoughCalib_Value);

    for(i = 0; i < 20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value; // 连续采样20次
    }
    for(i = 0; i < 20; i++)
    {
        PRINT("%d \n", adcBuff[i]); // 注意：由于ADC内部偏差的存在，当采样电压在所选增益范围极限附近的时候，可能会出现数据溢出的现象
    }


    /* DMA单通道采样：选择adc通道0做采样，对应 PA4引脚 */
    PRINT("\n3.Single channel DMA sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit(SampleFreq_16, ADC_PGA_0);
    ADC_ChannelCfg(0);
    ADC_AutoConverCycle(192); // 采样周期为 (256-192)*16个系统时钟
    DMA_end = 0;
    ADC_DMACfg(ENABLE, (uint32_t)&adcBuff[0], (uint32_t)&adcBuff[40], ADC_Mode_Single);
    PFIC_EnableIRQ(ADC_IRQn);
    ADC_StartAutoDMA();
    while(!DMA_end);
    ADC_DMACfg(DISABLE, 0, 0, 0);

    for(i = 0; i < 40; i++)
    {
        PRINT("%d \n", adcBuff[i]);
    }


    /* 差分通道采样：选择adc通道0做采样，对应 PA4(AIN0)、PA12(AIN2) */
    PRINT("\n4.Diff channel sampling...\n");
    GPIOA_ModeCfg(GPIO_Pin_4 | GPIO_Pin_12, GPIO_ModeIN_Floating);
    ADC_ExtDiffChSampInit(SampleFreq_16, ADC_PGA_0);
    ADC_ChannelCfg(0);
    for(i = 0; i < 20; i++)
    {
        adcBuff[i] = ADC_ExcutSingleConver(); // 连续采样20次
    }
    for(i = 0; i < 20; i++)
    {
        PRINT("%d \n", adcBuff[i]);
    }


    /* 单通道采样：中断方式,选择adc通道1做采样，对应 PA5引脚， 不带数据校准功能 */
    PRINT("\n6.Single channel sampling in interrupt mode...\n");
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit(SampleFreq_16, ADC_PGA_0);
    ADC_ChannelCfg(1);
    adclen = 0;
    ADC_ClearITFlag();
    PFIC_EnableIRQ(ADC_IRQn);

    ADC_StartUp();
    while(adclen < 20);
    PFIC_DisableIRQ(ADC_IRQn);
    for(i = 0; i < 20; i++)
    {
        PRINT("%d \n", adcBuff[i]);
    }

    /* TouchKey采样：选择通道  10 做采样，对应 PA6引脚，需要在PB0外接1nF电容
     * notice:如果想要更换触摸通道，需要遵循以下原则：
     * 1.本例程在通道8接入外部参考电容Cx，如果需要更换此通道，硬件上需要将1nF外部参考电容也同步更换。
     * 2.选择触摸通道时，如果要同时使用多个触摸通道，可以参考"TOUCH_CH_10 | TOUCH_CH_11"此类格式输入参数，但接入了外部参考电容的Cx的通道不可选择
     */
    PRINT("\n7.TouchKey sampling...\n");
    {
        uint32_t touch_val = 0;
        GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_Floating);
        Touch_RegCfg(SampleFreq_8, ADC_PGA_0, TOUCH_CH_10, ENABLE);
        TOUCH_CX_ChannelCfg( 8 );
        for(i = 0; i < 20; i++)
        {
            adcBuff[i] = Touch_GetSingleValue( TOUCH_CH_10, TKEY_CC_4Tsys, TKEY_TRANS_16Tsys, CHARGE_TIMES, TKEY_START_SEL_2);
        }
        for(i = 0; i < 20; i++)
        {
            PRINT("%d \n", adcBuff[i]);
        }
    }

    while(1);
}

/*********************************************************************
 * @fn      ADC_IRQHandler
 *
 * @brief   ADC中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void ADC_IRQHandler(void) //adc中断服务程序
{
    if(ADC_GetDMAStatus())
    {
        ADC_StopAutoDMA();
        R16_ADC_DMA_BEG = ((uint32_t)adcBuff) & 0xffff;
        ADC_ClearDMAFlag();
        DMA_end = 1;
    }
    if(ADC_GetITStatus())
    {
        ADC_ClearITFlag();
        if(adclen < 20)
        {
            adcBuff[adclen] = ADC_ReadConverValue();
            ADC_StartUp(); // 作用清除中断标志并开启新一轮采样
        }
        adclen++;
    }
}
