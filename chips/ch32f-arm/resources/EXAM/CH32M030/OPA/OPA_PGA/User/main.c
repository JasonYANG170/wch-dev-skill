/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/08
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *OPA PGA routine:
 *OPA1_CHP0--PA10
 *OPA1_OUT--PA15
 *The negative input pin of OPA is an internal pin, and when the positive
 *input voltage is given, the output gain is 4 times.bias voltage is 5.5V
 *Or OPA differential input£º
 * OPA1_CHP0--PA10
 * OPA1_CHN0--PA11
 * OPA1_OUT--PA15
*/

#include "debug.h"


/* OPA_4 Mode Definition*/
#define OPA4_signal       0
#define OPA4_diff         1

/* OPA_4  Mode Selection*/
//#define OPA4_MODE_PGA   OPA4_signal
#define OPA4_MODE_PGA   OPA4_diff

/*********************************************************************
 * @fn      OPA4_Init
 *
 * @brief   Initializes OPA4.
 *
 * @return  none
 */
void OPA4_Init(void)
{
    OPA_ISP_InitTypeDef OPA_ISP_InitStruct = {0};
    GPIO_InitTypeDef     GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_OPCM, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#if  (OPA4_MODE_PGA == OPA4_diff)
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

    OPA_ISP_InitStruct.OPA_ISP_GAIN = OPA_ISP_GAIN_4;
    OPA_ISP_InitStruct.OPA_ISP_VFBIAS = OPA_ISP_VFBIAS_0_55V;
#if  (OPA4_MODE_PGA == OPA4_signal)
    OPA_ISP_InitStruct.OPA_ISP_NESL = OPA_ISP_NESL_VSS;
#else
    OPA_ISP_InitStruct.OPA_ISP_NESL = OPA_ISP_NESL_ISN;
#endif

    OPA_ISP_InitStruct.OPA_ISP_SEL_IO = OPA_ISP_SEL_TO_ADC_ON;
    OPA_ISP_InitStruct.OPA_ISP_QDET = OPA_ISP_QDET_EN_ON;
    OPA_ISP_InitStruct.OPA_ISP_QDET_PD30K = OPA_ISP_QDET_PD30K_OFF;
    OPA_ISP_InitStruct.OPA_ISP_QDET_VBSEL = OPA_ISP_QDET_VBSEL_OFF;
    OPA_ISP_Init(OPA4_ISP2,&OPA_ISP_InitStruct);

    OPA_ISP_Cmd(OPA4_ISP2,ENABLE);

}
/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_HB_Div8);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);    //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

/*********************************************************************
 * @fn      Get_ADC_Val
 *
 * @brief   Returns ADCx conversion result data.
 *
 * @param   ch - ADC channel.
 *            ADC_Channel_0 - ADC Channel0 selected.
 *            ADC_Channel_1 - ADC Channel1 selected.
 *            ADC_Channel_2 - ADC Channel2 selected.
 *            ADC_Channel_3 - ADC Channel3 selected.
 *            ADC_Channel_4 - ADC Channel4 selected.
 *            ADC_Channel_5 - ADC Channel5 selected.
 *            ADC_Channel_6 - ADC Channel6 selected.
 *            ADC_Channel_7 - ADC Channel7 selected.
 *            ADC_Channel_8 - ADC Channel8 selected.
 *            ADC_Channel_9 - ADC Channel9 selected.
 *            ADC_Channel_10 - ADC Channel10 selected.
 *            ADC_Channel_11 - ADC Channel11 selected.
 *            ADC_Channel_12 - ADC Channel12 selected.
 *            ADC_Channel_13 - ADC Channel13 selected.
 *            ADC_Channel_14 - ADC Channel14 selected.
 *            ADC_Channel_15 - ADC Channel15 selected.
 *            ADC_Channel_16 - ADC Channel16 selected.
 *            ADC_Channel_17 - ADC Channel17 selected.
 *            ADC_Channel_18 - ADC Channel18 selected.
 *            ADC_Channel_19 - ADC Channel19 selected.
 *
 * @return  none
 */
u16 Get_ADC_Val(u8 ch)
{
    u16 val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_5Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);

    return val;
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u16 OPA4_val;
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    OPA4_Init();
    ADC_Function_Init();
    while(1)
    {
    OPA4_val=Get_ADC_Val(ADC_Channel_10);
    printf("ADC_val  %04d\r\n",OPA4_val);
    Delay_Ms(500);
    }
}
