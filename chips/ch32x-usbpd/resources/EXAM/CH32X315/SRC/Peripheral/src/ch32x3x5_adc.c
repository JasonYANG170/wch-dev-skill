/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32x3x5_adc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file provides all the ADC firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32x3x5_adc.h"
#include "ch32x3x5_rcc.h"

/* ADC DISCNUM mask */
#define CTLR1_DISCNUM_Reset              ((uint32_t)0xFFFF1FFF)

/* ADC DISCEN mask */
#define CTLR1_DISCEN_Set                 ((uint32_t)0x00000800)
#define CTLR1_DISCEN_Reset               ((uint32_t)0xFFFFF7FF)

/* ADC JAUTO mask */
#define CTLR1_JAUTO_Set                  ((uint32_t)0x00000400)
#define CTLR1_JAUTO_Reset                ((uint32_t)0xFFFFFBFF)

/* ADC JDISCEN mask */
#define CTLR1_JDISCEN_Set                ((uint32_t)0x00001000)
#define CTLR1_JDISCEN_Reset              ((uint32_t)0xFFFFEFFF)

/* ADC AWDCH mask */
#define CTLR1_AWDCH_Reset                ((uint32_t)0xFFFFFFE0)

/* ADC Analog watchdog enable mode mask */
#define CTLR1_AWDMode_Reset              ((uint32_t)0xFF3FFDFF)
/* ADC Cascade enable mode mask */
#define CTLR1_Cascade_Reset              ((uint32_t)0xF3FFFFFF)
/* ADC Cascade CONTS mask */
#define CTLR1_CONTS_Reset              ((uint32_t)0xCFFFFFFF)
/* CTLR1 register Mask */
#define CTLR1_CLEAR_Mask                 ((uint32_t)0xFBF0FEFF)

/* ADC ADON mask */
#define CTLR2_ADON_Set                   ((uint32_t)0x00000001)
#define CTLR2_ADON_Reset                 ((uint32_t)0xFFFFFFFE)

/* ADC DMA mask */
#define CTLR2_DMA_Set                    ((uint32_t)0x00000100)
#define CTLR2_DMA_Reset                  ((uint32_t)0xFFFFFEFF)

/* ADC RSTCAL mask */
#define CTLR2_RSTCAL_Set                 ((uint32_t)0x00000008)

/* ADC CAL mask */
#define CTLR2_CAL_Set                    ((uint32_t)0x00000004)

/* ADC SWSTART mask */
#define CTLR2_SWSTART_Set                ((uint32_t)0x00400000)

/* ADC EXTTRIG mask */
#define CTLR2_EXTTRIG_Set                ((uint32_t)0x00100000)
#define CTLR2_EXTTRIG_Reset              ((uint32_t)0xFFEFFFFF)

/* ADC Software start mask */
#define CTLR2_EXTTRIG_SWSTART_Set        ((uint32_t)0x00500000)
#define CTLR2_EXTTRIG_SWSTART_Reset      ((uint32_t)0xFFAFFFFF)

/* ADC JEXTSEL mask */
#define CTLR2_JEXTSEL_Reset              ((uint32_t)0xFFFF8FFF)

/* ADC JEXTTRIG mask */
#define CTLR2_JEXTTRIG_Set               ((uint32_t)0x00008000)
#define CTLR2_JEXTTRIG_Reset             ((uint32_t)0xFFFF7FFF)

/* ADC JSWSTART mask */
#define CTLR2_JSWSTART_Set               ((uint32_t)0x00200000)

/* ADC injected software start mask */
#define CTLR2_JEXTTRIG_JSWSTART_Set      ((uint32_t)0x00208000)
#define CTLR2_JEXTTRIG_JSWSTART_Reset    ((uint32_t)0xFFDF7FFF)

/* ADC TSPD mask */
#define CTLR2_TSVREFE_Set                ((uint32_t)0x00800000)
#define CTLR2_TSVREFE_Reset              ((uint32_t)0xFF7FFFFF)

/* CTLR2 register Mask */
#define CTLR2_CLEAR_Mask                 ((uint32_t)0xFCF1F7FD)

/* ADC SQx mask */
#define RSQR3_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR2_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR1_SQ_Set                     ((uint32_t)0x0000001F)

/* RSQR1 register Mask */
#define RSQR1_CLEAR_Mask                 ((uint32_t)0xFF0FFFFF)

/* ADC JSQx mask */
#define ISQR_JSQ_Set                     ((uint32_t)0x0000001F)

/* ADC JL mask */
#define ISQR_JL_Set                      ((uint32_t)0x00300000)
#define ISQR_JL_Reset                    ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SAMPTR1_SMP_Set                  ((uint32_t)0x00000007)
#define SAMPTR2_SMP_Set                  ((uint32_t)0x00000007)

/* ADC IDATARx registers offset */
#define IDATAR_Offset                    ((uint8_t)0x28)

/* ADC1 RDATAR register base address */
#define RDATAR_ADDRESS                   ((uint32_t)0x4001244C)

#define ADC_INJEDLY_Mask                 ((uint32_t)0xFFFFC0FF)

#define ADC_REGUDLY_Mask                 ((uint32_t)0xFFFFFFC0)

#define ADC_INJE_Mask                    ((uint32_t)0xFFF0FFFF)

#define ADC_SACAN_Mask                   ((uint32_t)0xFFFFFFF1)

/*********************************************************************
 * @fn      ADC_DeInit
 *
 * @brief   Deinitializes the ADCx peripheral registers to their default
 *        reset values.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  none
 */
void ADC_DeInit(ADC_TypeDef *ADCx)
{
    if(ADCx == ADC1)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC1, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC1, DISABLE);
    }
    else if(ADCx == ADC2)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC2, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC2, DISABLE);
    }
    else if(ADCx == ADC3)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC3, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC3, DISABLE);
    }
    else if(ADCx == ADC4)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC4, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_ADC4, DISABLE);
    }
}

/*********************************************************************
 * @fn      ADC_Init
 *
 * @brief   Initializes the ADCx peripheral according to the specified
 *        parameters in the ADC_InitStruct.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_InitStruct - pointer to an ADC_InitTypeDef structure that
 *        contains the configuration information for the specified ADC
 *        peripheral.
 *
 * @return  none
 */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct)
{
    uint32_t tmpreg1 = 0;
    uint8_t  tmpreg2 = 0;

    tmpreg1 = ADCx->CTLR1;
    tmpreg1 &= CTLR1_CLEAR_Mask;
    tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_Mode | (uint32_t)ADC_InitStruct->ADC_OutputBuffer |
                            ((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8));
    ADCx->CTLR1 = tmpreg1;

    tmpreg1 = ADCx->CTLR2;
    tmpreg1 &= CTLR2_CLEAR_Mask;
    tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_DataAlign | ADC_InitStruct->ADC_ExternalTrigConv | 
                         ADC_InitStruct->ADC_DataMode |((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));
    ADCx->CTLR2 = tmpreg1;

    tmpreg1 = ADCx->CONCFG;
    tmpreg1 &= ~(ADC_INJE_NUM4);
    tmpreg1 |= (uint32_t)(ADC_InitStruct->INJENUM);
    ADCx->CONCFG = tmpreg1;
   
    tmpreg1 = ADCx->RSQR1;
    tmpreg1 &= RSQR1_CLEAR_Mask;
    tmpreg2 |= (uint8_t)(ADC_InitStruct->ADC_NbrOfChannel - (uint8_t)1);
    tmpreg1 |= (uint32_t)tmpreg2 << 20;
    ADCx->RSQR1 = tmpreg1;

}

/*********************************************************************
 * @fn      ADC_StructInit
 *
 * @brief   Fills each ADC_InitStruct member with its default value.
 *
 * @param   ADC_InitStruct - pointer to an ADC_InitTypeDef structure that
 *        contains the configuration information for the specified ADC
 *        peripheral.
 *
 * @return  none
 */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
    ADC_InitStruct->ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct->ADC_ScanConvMode = DISABLE;
    ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct->ADC_NbrOfChannel = 1;
    ADC_InitStruct->ADC_DataMode = ADC_DataMode_12B;
    ADC_InitStruct->INJENUM = ADC_INJE_NUM1;
}

/*********************************************************************
 * @fn      ADC_Cmd
 *
 * @brief   Enables or disables the specified ADC peripheral.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_ADON_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_ADON_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_DMACmd
 *
 * @brief   Enables or disables the specified ADC DMA request.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_DMA_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_DMA_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_ITConfig
 *
 * @brief   Enables or disables the specified ADC interrupts.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_IT - specifies the ADC interrupt sources to be enabled or disabled.
 *            ADC_IT_EOC - End of conversion interrupt mask.
 *            ADC_IT_AWD - Analog watchdog interrupt mask.
 *            ADC_IT_JEOC - End of injected conversion interrupt mask.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_ITConfig(ADC_TypeDef *ADCx, uint16_t ADC_IT, FunctionalState NewState)
{
    uint8_t itmask = 0;

    itmask = (uint8_t)ADC_IT;

    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= itmask;
    }
    else
    {
        ADCx->CTLR1 &= (~(uint32_t)itmask);
    }
}

/*********************************************************************
 * @fn      ADC_ResetCalibration
 *
 * @brief   Resets the selected ADC calibration registers.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  none
 */
void ADC_ResetCalibration(ADC_TypeDef *ADCx)
{
    ADCx->CTLR2 |= CTLR2_RSTCAL_Set;
}

/*********************************************************************
 * @fn      ADC_GetResetCalibrationStatus
 *
 * @brief   Gets the selected ADC reset calibration registers status.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_RSTCAL_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_StartCalibration
 *
 * @brief   Starts the selected ADC calibration process.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  None
 */
void ADC_StartCalibration(ADC_TypeDef *ADCx)
{
    ADCx->CTLR2 |= CTLR2_CAL_Set;
}

/*********************************************************************
 * @fn      ADC_GetCalibrationStatus
 *
 * @brief   Gets the selected ADC calibration status.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_CAL_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_SoftwareStartConvCmd
 *
 * @brief   Enables or disables the selected ADC software start conversion.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_EXTTRIG_SWSTART_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_EXTTRIG_SWSTART_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetSoftwareStartConvStatus
 *
 * @brief   Gets the selected ADC Software start conversion Status.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  FlagStatus - SET or RESET.
 */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_SWSTART_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_DiscModeChannelCountConfig
 *
 * @brief   Configures the discontinuous mode for the selected ADC regular
 *        group channel.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          Number - specifies the discontinuous mode regular channel
 *            count value(1-8).
 *
 * @return  None
 */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef *ADCx, uint8_t Number)
{
    uint32_t tmpreg1 = 0;
    uint32_t tmpreg2 = 0;

    tmpreg1 = ADCx->CTLR1;
    tmpreg1 &= CTLR1_DISCNUM_Reset;
    tmpreg2 = Number - 1;
    tmpreg1 |= tmpreg2 << 13;
    ADCx->CTLR1 = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_DiscModeCmd
 *
 * @brief   Enables or disables the discontinuous mode on regular group
 *        channel for the specified ADC.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_DiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_DISCEN_Set;
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_DISCEN_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_RegularChannelConfig
 *
 * @brief   Configures for the selected ADC regular channel its corresponding
 *        rank in the sequencer.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_Channel - the ADC channel to configure.
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
 *          Rank - The rank in the regular group sequencer.
 *            This parameter must be between 1 to 16.
 *
 * @return  None
 */
void ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0;

    if(Rank < 7)
    {
        tmpreg1 = ADCx->RSQR3;
        tmpreg2 = RSQR3_SQ_Set << (5 * (Rank - 1));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR3 = tmpreg1;
    }
    else if(Rank < 13)
    {
        tmpreg1 = ADCx->RSQR2;
        tmpreg2 = RSQR2_SQ_Set << (5 * (Rank - 7));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR2 = tmpreg1;
    }
    else
    {
        tmpreg1 = ADCx->RSQR1;
        tmpreg2 = RSQR1_SQ_Set << (5 * (Rank - 13));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR1 = tmpreg1;
    }
}

/*********************************************************************
 * @fn      ADC_ExternalTrigConvCmd
 *
 * @brief   Enables or disables the ADCx conversion through external trigger.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_ExternalTrigConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_EXTTRIG_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_EXTTRIG_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetConversionValue
 *
 * @brief   Returns the last ADCx conversion result data for regular channel.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  ADCx->RDATAR - The Data conversion value.
 */
uint16_t ADC_GetConversionValue(ADC_TypeDef *ADCx)
{
    return (uint16_t)ADCx->RDATAR;
}

/*********************************************************************
 * @fn      ADC_GetDualModeConversionValue
 *
 * @brief   Returns the last ADC1 and ADC2  or ADC3 and ADC4 conversion result data in dual mode.
 *
 * @return  RDATAR_ADDRESS - The Data conversion value.
 */
uint32_t ADC_GetDualModeConversionValue(void)
{
    return (*(__IO uint32_t *)RDATAR_ADDRESS);
}

/*********************************************************************
 * @fn      ADC_AutoInjectedConvCmd
 *
 * @brief   Enables or disables the selected ADC automatic injected group
 *        conversion after regular one.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_AutoInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_JAUTO_Set;
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_JAUTO_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_InjectedDiscModeCmd
 *
 * @brief   Enables or disables the discontinuous mode for injected group
 *        channel for the specified ADC.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_InjectedDiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_JDISCEN_Set;
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_JDISCEN_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_ExternalTrigInjectedConvConfig
 *
 * @brief   Configures the ADCx external trigger for injected channels conversion.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_ExternalTrigInjecConv - specifies the ADC trigger to start
 *        injected conversion.
 *            ADC_ExternalTrigInjecConv_T1_TRGO - Timer1 TRGO event selected.
 *            ADC_ExternalTrigInjecConv_T1_CC4 - Timer1 capture compare4 selected.
 *            ADC_ExternalTrigInjecConv_T2_TRGO - Timer2 TRGO event selected.
 *            ADC_ExternalTrigInjecConv_T2_CC1 - Timer2 capture compare1 selected.
 *            ADC_ExternalTrigInjecConv_T3_CC4 - Timer3 capture compare4 selected.
 *            ADC_ExternalTrigInjecConv_T4_TRGO - Timer4 TRGO event selected.
 *            ADC_ExternalTrigInjecConv_Ext_IT15- External interrupt
 *        line 15 event selected.
 *            ADC_ExternalTrigInjecConv_None: Injected conversion started
 *        by software and not by external trigger.
 *
 * @return  None
 */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef *ADCx, uint32_t ADC_ExternalTrigInjecConv)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR2;
    tmpreg &= CTLR2_JEXTSEL_Reset;
    tmpreg |= ADC_ExternalTrigInjecConv;
    ADCx->CTLR2 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_ExternalTrigInjectedConvCmd
 *
 * @brief   Enables or disables the ADCx injected channels conversion through
 *        external trigger.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_JEXTTRIG_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_JEXTTRIG_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_SoftwareStartInjectedConvCmd
 *
 * @brief   Enables or disables the selected ADC start of the injected
 *        channels conversion.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  None
 */
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_JEXTTRIG_JSWSTART_Set;
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_JEXTTRIG_JSWSTART_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetSoftwareStartInjectedConvCmdStatus
 *
 * @brief   Gets the selected ADC Software start injected conversion Status.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_JSWSTART_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_InjectedChannelConfig
 *
 * @brief   Configures for the selected ADC injected channel its sample time.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_Channel - the ADC channel to configure.
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
 *            ADC_Channel_12 - ADC Channel12 selected..
 *          Rank - The rank in the regular group sequencer.
 *            This parameter must be between 1 to 4.
 *
 * @return  None
 */
void ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;

    tmpreg1 = ADCx->ISQR;
    tmpreg3 = (tmpreg1 & ISQR_JL_Set) >> 20;
    tmpreg2 = ISQR_JSQ_Set << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 &= ~tmpreg2;
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 |= tmpreg2;
    ADCx->ISQR = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_InjectedSequencerLengthConfig
 *
 * @brief   Configures the sequencer length for injected channels.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          Length - The sequencer length.
 *            This parameter must be a number between 1 to 4.
 *
 * @return  None
 */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef *ADCx, uint8_t Length)
{
    uint32_t tmpreg1 = 0;
    uint32_t tmpreg2 = 0;

    tmpreg1 = ADCx->ISQR;
    tmpreg1 &= ISQR_JL_Reset;
    tmpreg2 = Length - 1;
    tmpreg1 |= tmpreg2 << 20;
    ADCx->ISQR = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_SetInjectedOffset
 *
 * @brief   Set the injected channels conversion value offset.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_InjectedChannel: the ADC injected channel to set its offset.
 *            ADC_InjectedChannel_1 - Injected Channel1 selected.
 *            ADC_InjectedChannel_2 - Injected Channel2 selected.
 *            ADC_InjectedChannel_3 - Injected Channel3 selected.
 *            ADC_InjectedChannel_4 - Injected Channel4 selected.
 *          Offset - the offset value for the selected ADC injected channel.
 *            This parameter must be a 15bit value.
 *
 * @return  None
 */
void ADC_SetInjectedOffset(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)ADCx;
    tmp += ADC_InjectedChannel;

    *(__IO uint32_t *)tmp = (uint32_t)Offset;
}

/*********************************************************************
 * @fn      ADC_GetInjectedConversionValue
 *
 * @brief   Returns the ADC injected channel conversion result.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_InjectedChannel - the ADC injected channel to set its offset.
 *            ADC_InjectedChannel_1 - Injected Channel1 selected.
 *            ADC_InjectedChannel_2 - Injected Channel2 selected.
 *            ADC_InjectedChannel_3 - Injected Channel3 selected.
 *            ADC_InjectedChannel_4 - Injected Channel4 selected.
 *
 * @return  tmp - The Data conversion value.
 */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)ADCx;
    tmp += ADC_InjectedChannel + IDATAR_Offset;

    return (uint16_t)(*(__IO uint32_t *)tmp);
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogCmd
 *
 * @brief   Enables or disables the analog watchdog on single/all regular
 *        or injected channels.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_AnalogWatchdog - the ADC analog watchdog configuration.
 *            ADC_AnalogWatchdog_SingleRegEnable - Analog watchdog on a
 *        single regular channel.
 *            ADC_AnalogWatchdog_SingleInjecEnable - Analog watchdog on a
 *        single injected channel.
 *            ADC_AnalogWatchdog_SingleRegOrInjecEnable - Analog watchdog
 *        on a single regular or injected channel.
 *            ADC_AnalogWatchdog_AllRegEnable - Analog watchdog on  all
 *        regular channel.
 *            ADC_AnalogWatchdog_AllInjecEnable - Analog watchdog on  all
 *        injected channel.
 *            ADC_AnalogWatchdog_AllRegAllInjecEnable - Analog watchdog on
 *        all regular and injected channels.
 *            ADC_AnalogWatchdog_None - No channel guarded by the analog
 *        watchdog.
 *           
 * @return  none
 */
void ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR1;
    tmpreg &= CTLR1_AWDMode_Reset;
    tmpreg |= ADC_AnalogWatchdog;
    ADCx->CTLR1 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogThresholdsConfig
 *
 * @brief   Configures the high and low thresholds of the analog watchdog.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          HighThreshold - the ADC analog watchdog High threshold value.
 *            This parameter must be a 15bit value.
 *          LowThreshold - the ADC analog watchdog Low threshold value.
 *            This parameter must be a 15bit value.
 *
 * @return  none
 */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold,
                                        uint16_t LowThreshold)
{
    ADCx->WDHTR = HighThreshold;
    ADCx->WDLTR = LowThreshold;
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogSingleChannelConfig
 *
 * @brief   Configures the analog watchdog guarded single channel.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_Channel - the ADC channel to configure.
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
 *
 * @return  None
 */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR1;
    tmpreg &= CTLR1_AWDCH_Reset;
    tmpreg |= ADC_Channel;
    ADCx->CTLR1 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_TempSensorVrefintCmd
 *
 * @brief   Enables or disables the temperature sensor and Vrefint channel.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_TempSensorVrefintCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADC1->CTLR2 |= CTLR2_TSVREFE_Set;
    }
    else
    {
        ADC1->CTLR2 &= CTLR2_TSVREFE_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetFlagStatus
 *
 * @brief   Checks whether the specified ADC flag is set or not.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_FLAG - specifies the flag to check.
 *            ADC_FLAG_AWD - Analog watchdog flag.
 *            ADC_FLAG_EOC - End of conversion flag.
 *            ADC_FLAG_JEOC - End of injected group conversion flag.
 *            ADC_FLAG_JSTRT - Start of injected group conversion flag.
 *            ADC_FLAG_STRT - Start of regular group conversion flag.
 *            ADC_FLAG_RST - RST of ADC flag.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint16_t ADC_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->STATR & ADC_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_ClearFlag
 *
 * @brief   Clears the ADCx's pending flags.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_FLAG - specifies the flag to clear.
 *            ADC_FLAG_AWD - Analog watchdog flag.
 *            ADC_FLAG_EOC - End of conversion flag.
 *            ADC_FLAG_JEOC - End of injected group conversion flag.
 *            ADC_FLAG_JSTRT - Start of injected group conversion flag.
 *            ADC_FLAG_STRT - Start of regular group conversion flag.
 *
 * @return  none
 */
void ADC_ClearFlag(ADC_TypeDef *ADCx, uint8_t ADC_FLAG)
{
    ADCx->STATR = ~(uint32_t)ADC_FLAG;
}

/*********************************************************************
 * @fn      ADC_GetITStatus
 *
 * @brief   Checks whether the specified ADC interrupt has occurred or not.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_IT - specifies the ADC interrupt source to check.
 *            ADC_IT_EOC - End of conversion interrupt mask.
 *            ADC_IT_AWD - Analog watchdog interrupt mask.
 *            ADC_IT_JEOC - End of injected conversion interrupt mask.
 *
 * @return  FlagStatus: SET or RESET.
 */
ITStatus ADC_GetITStatus(ADC_TypeDef *ADCx, uint16_t ADC_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t itmask = 0, enablestatus = 0;

    itmask = ADC_IT >> 8;
    enablestatus = (ADCx->CTLR1 & (uint8_t)ADC_IT);

    if(((ADCx->STATR & itmask) != (uint32_t)RESET) && enablestatus)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_ClearITPendingBit
 *
 * @brief   Clears the ADCx's interrupt pending bits.
 *
 * @param   ADCx - where x can be 1 to 4to select the ADC peripheral.
 *          ADC_IT - specifies the ADC interrupt pending bit to clear.
 *            ADC_IT_EOC - End of conversion interrupt mask.
 *            ADC_IT_AWD - Analog watchdog interrupt mask.
 *            ADC_IT_JEOC - End of injected conversion interrupt mask.
 *
 * @return  none
 */
void ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint16_t ADC_IT)
{
    uint8_t itmask = 0;

    itmask = (uint8_t)(ADC_IT >> 8);
    ADCx->STATR = ~(uint32_t)itmask;
}

/*********************************************************************
 * @fn      ADC_BufferCmd
 *
 * @brief   Enables or disables the ADCx buffer.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_BufferCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= (1 << 26);
    }
    else
    {
        ADCx->CTLR1 &= ~(1 << 26);
    }
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogResetCmd
 *
 * @brief   Enables or disables the analog watchdog reset
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_AnalogWatchdogResetCmd(ADC_TypeDef *ADCx,  FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
      ADCx->CTLR1 |= (1<<29);
  }
  else
  {
      ADCx->CTLR1 &= ~(1<<29);
  }
}

/*********************************************************************
 * @fn      ADC_RegularChannelCascadeCmd
 *
 * @brief   Enables or disables the ADC Regular Channel Cascade
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *         
 * @return  none
 */
void ADC_RegularChannelCascadeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
      ADCx->CTLR2 |= (1<<26);
  }
  else
  {
      ADCx->CTLR2 &= ~(1<<26);
  }
}

/*********************************************************************
 * @fn      ADC_InjectedChannelCascadeCmd
 *
 * @brief   Enables or disables the ADC Injected Channel Cascade
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *         
 * @return  none
 */
void ADC_InjectedChannelCascadeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
      ADCx->CTLR2 |= (1<<27);
  }
  else
  {
      ADCx->CTLR2 &= ~(1<<27);
  }
}

/*********************************************************************
 * @fn      ADC_CascadeContsFrontConfig
 *
 * @brief   the ADC Front Cascade conts select
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_CONTS_Front -  the ADC Cascademode configuration.
 *            ADC_CONTS_Front_ADC1 -  select Front Cascade of ADC1.
 *            ADC_CONTS_Front_ADC2 -  select Front Cascade of ADC2.
 *            ADC_CONTS_Front_ADC3 -  select Front Cascade of ADC3.
 *            ADC_CONTS_Front_ADC4 -  select Front Cascade of ADC4.
 *         
 * @return  none
 */
void ADC_CascadeContsFrontConfig(ADC_TypeDef *ADCx,  uint32_t ADC_CONTS_Front)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR2;
    tmpreg &= CTLR1_CONTS_Reset;
    tmpreg |= ADC_CONTS_Front;
    ADCx->CTLR2 = tmpreg;
}
/*********************************************************************
 * @fn      ADC_InjectedChannelCascadeDLYConfig
 *
 * @brief   the Cascade of ADC injected Channel delay
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_INJE_DLY - This parameter must range from 0 to 0x3F.
 *         
 * @return  none
 */
void ADC_InjectedChannelCascadeDLYConfig(ADC_TypeDef *ADCx,  uint8_t ADC_INJE_DLY)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CONCFG;
    tmpreg &= ADC_INJEDLY_Mask;
    ADC_INJE_DLY &= 0x3F;
    tmpreg |= (ADC_INJE_DLY << 8);
    ADCx->CONCFG = tmpreg;
}

/*********************************************************************
 * @fn      ADC_RegularChannelCascadeDLYConfig
 *
 * @brief   the Cascade of ADC regular Channel delay
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_REGU_DLY - This parameter must range from 0 to 0x3F.
 *         
 * @return  none
 */
void ADC_RegularChannelCascadeDLYConfig(ADC_TypeDef *ADCx,  uint8_t ADC_REGU_DLY)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CONCFG;
    tmpreg &= ADC_REGUDLY_Mask;
    ADC_REGU_DLY &= 0x3F;
    tmpreg |= ADC_REGU_DLY;
    ADCx->CONCFG = tmpreg;
}

/*********************************************************************
 * @fn      ADC_SampleTimeModeConfig
 *
 * @brief   Configures the sample time mode of ADC.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_Channel - the ADC channel to configure.
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
 *          ADC_ST_Mode - Sample Time Mode.
 *            ADC_Sample_Time_Mode0 - the sample time depend on SMPx[2:0].
 *            ADC_Sample_Time_Mode1 - the sample time depend on SMPUDP[7:0].
 * @return  none
 */
void ADC_SampleTimeModeConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t ADC_ST_Mode)
{
    if(ADC_ST_Mode == ADC_Sample_Time_Mode0)
    {
        ADCx->SMPMD &= ~(1 << ADC_Channel);
    }
    else if(ADC_ST_Mode == ADC_Sample_Time_Mode1)
    {
        ADCx->SMPMD |= (1 << ADC_Channel);
    }
}

/*********************************************************************
 * @fn      ADC_RegularChannelConfig
 *
 * @brief   Configures for the selected ADC regular channel its sample time.
 *        (@ADC_Sample_Time_Mode0).
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_Channel - the ADC channel to configure.
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
 *          ADC_SampleTime - The sample time value to be set for the selected channel.
 *            ADC_SampleTime_CyclesMode0 - Sample time mode 0.
 *            ADC_SampleTime_CyclesMode1 - Sample time mode 1.
 *            ADC_SampleTime_CyclesMode2 - Sample time mode 2.
 *            ADC_SampleTime_CyclesMode3 - Sample time mode 3.
 *            ADC_SampleTime_CyclesMode4 - Sample time mode 4.
 *            ADC_SampleTime_CyclesMode5 - Sample time mode 5.
 *            ADC_SampleTime_CyclesMode6 - Sample time mode 6.
 *            ADC_SampleTime_CyclesMode7 - Sample time mode 7.
 *
 * @return  None
 */
void ADC_SetMode0SampleTimeConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t ADC_SampleTime)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0;

    if(ADC_Channel > ADC_Channel_9)
    {
        tmpreg1 = ADCx->SAMPTR1;
        tmpreg2 = SAMPTR1_SMP_Set << (3 * (ADC_Channel - 10));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR1 = tmpreg1;
    }
    else
    {
        tmpreg1 = ADCx->SAMPTR2;
        tmpreg2 = SAMPTR2_SMP_Set << (3 * ADC_Channel);
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR2 = tmpreg1;
    }
}

/*********************************************************************
 * @fn      ADC_SetMode1SampleTimeConfig
 *
 * @brief   Configures the sample time(@ADC_Sample_Time_Mode1).
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_UDP_SMPCLK - the sample time for ADC normal channel.
 *            This parameter must range from 0 to 0x3F.
 *
 * @return   none
 */
void ADC_SetMode1SampleTimeConfig(ADC_TypeDef *ADCx, uint8_t ADC_UDP_SMPCLK) 
{
    ADCx->UDP &= ~(ADC_SMPUDP );
    ADCx->UDP |= (ADC_UDP_SMPCLK );
}
/*********************************************************************
 * @fn      ADC_CAL_SampleTimeConfig
 *
 * @brief   Configures the ADC_CAL CHALE sample time.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_CAL_SMPCLK - the sample time for ADC calibration channel.
 *            This parameter must range from 0 to 0x3F.
 *
 * @return   none
 */
void ADC_CAL_SampleTimeConfig(ADC_TypeDef *ADCx, uint8_t ADC_CAL_SMPCLK) 
{
    ADCx->UDP &= ~(ADC_SMPCAL);
    ADCx->UDP |= (ADC_CAL_SMPCLK << 8);
}
/*********************************************************************
 * @fn      ADC_ScanCountOutputConfig
 *
 * @brief   Configures the Scan Count Output.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_ScanCntOutMode - the Scan Count Output mode.
 *            ADC_ScanCntOutMode0 - SCANOE0 = 0, SCANOE1 = 0, SCANOE2 = 0;
 *            ADC_ScanCntOutMode1 - SCANOE0 = 1, SCANOE1 = 0, SCANOE2 = 0;
 *            ADC_ScanCntOutMode2 - SCANOE0 = 0, SCANOE1 = 1, SCANOE2 = 0;
 *            ADC_ScanCntOutMode3 - SCANOE0 = 1, SCANOE1 = 1, SCANOE2 = 0;
 *            ADC_ScanCntOutMode4 - SCANOE0 = 0, SCANOE1 = 0, SCANOE2 = 1;
 *            ADC_ScanCntOutMode5 - SCANOE0 = 1, SCANOE1 = 0, SCANOE2 = 1;
 *            ADC_ScanCntOutMode6 - SCANOE0 = 0, SCANOE1 = 1, SCANOE2 = 1;
 *            ADC_ScanCntOutMode7 - SCANOE0 = 1, SCANOE1 = 1, SCANOE2 = 1;
 *             
 * @return   none
 */
void ADC_ScanCountOutputConfig(ADC_TypeDef *ADCx,  uint8_t ADC_ScanCntOutMode )
{
    ADCx->SCANCNT &= ADC_SACAN_Mask;
    ADCx->SCANCNT |= ADC_ScanCntOutMode;
}

/*********************************************************************
 * @fn      ADC_ScanChannelSourecConfig
 *
 * @brief   Configures the Scan channel source.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_ScanCH_SRC - the Scan channel source.
 *            ADC_ScanCH_SRC_Regular - Regular as the Scan channel source.
 *            ADC_ScanCH_SRC_Injected - Injected as the Scan channel source.
 *             
 * @return   none
 */
void ADC_ScanModeConfig(ADC_TypeDef *ADCx,  uint8_t ADC_ScanCH_SRC)
{
    if(ADC_ScanCH_SRC == ADC_ScanCH_SRC_Regular)
    {
        ADCx->SCANCNT &= ~ADC_ScanCH_SRC_Injected;
    }
    else if(ADC_ScanCH_SRC == ADC_ScanCH_SRC_Injected)
    {
        ADCx->SCANCNT |= ADC_ScanCH_SRC_Injected;
    }
}

/*********************************************************************
 * @fn      ADC_INJESEQConfig
 *
 * @brief   Configures the Injected trigger..
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_INJE_SEQ - the ADC  injected trigger SEG.
 *            ADC_INJE_SEQ1 - the ADC injected SEQ 1 
 *            ADC_INJE_SEQ2 - the ADC injected SEQ 2 
 *            ADC_INJE_SEQ3 - the ADC injected SEQ 3 
 *            ADC_INJE_SEQ4 - the ADC injected SEQ 4
 *          
 * @return   none
 */
void ADC_INJESEQConfig(ADC_TypeDef *ADCx,  uint32_t ADC_INJE_SEQ )
{
    ADCx->CONCFG &= ~ADC_INJE_SEQ4;
    ADCx->CONCFG |= ADC_INJE_SEQ;
}

/*********************************************************************
 * @fn      ADC_INJESEQConfig
 *
 * @brief   Configures the Injected trigger number.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          ADC_INJE_NUM - the ADC  injected trigger number.
 *            ADC_INJE_NUM1 - the ADC injected SEQ 1 
 *            ADC_INJE_NUM2 - the ADC injected SEQ 2 
 *            ADC_INJE_NUM3 - the ADC injected SEQ 3 
 *            ADC_INJE_NUM4 - the ADC injected SEQ 4
 *          
 * @return   none
 */
void ADC_INJENUMConfig(ADC_TypeDef *ADCx, uint32_t ADC_INJE_NUM)
{
    ADCx->CONCFG &= ~ADC_INJE_NUM4;
    ADCx->CONCFG |= ADC_INJE_NUM;
}

/*********************************************************************
 * @fn      ADC_ANA_RST_Cmd
 *
 * @brief   Enables or disables the ADCx ANA RST.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_ANA_RST_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= ADC_ANARST;
    }
    else
    {
        ADCx->CTLR1 &= ~ADC_ANARST;
    }
}

/*********************************************************************
 * @fn      ADC_SW_PRE_Cmd
 *
 * @brief   Enables or disables the ADCx SW_PRE.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_SW_PRE_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= ADC_SWPRE;
    }
    else
    {
        ADCx->CTLR1 &= ~ADC_SWPRE;
    }
}

/*********************************************************************
 * @fn      ADC_HD_CalibrationCmd
 *
 * @brief   Enables or disables the specified ADC hardware automatic Calibration.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ADC_HD_CalibrationCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= (1 << 6);
    }
    else
    {
        ADCx->CTLR2 &= ~(1 << 6);
    }
}

/*********************************************************************
 * @fn      ADC_CalibrationVoltageConfig
 *
 * @brief   Enables or disables the ADCx TO SMP SEL.
 *
 * @param   ADCx - where x can be 1 to 4 to select the ADC peripheral.
 *          CalibrationVoltage_Mode - configer SMPx mode.
 *            ADC_CalibrationVoltage_Mode0 - Calibration voltage 0.
 *            ADC_CalibrationVoltage_Mode1 - Calibration voltage 1.
 *            ADC_CalibrationVoltage_Mode2 - Calibration voltage 2.
 *            ADC_CalibrationVoltage_Mode3 - Calibration voltage 3.
 *
 * @return  none
 */
void ADC_CalibrationVoltageConfig(ADC_TypeDef *ADCx, uint8_t CalibrationVoltage_Mode)
{
    ADCx->CTLR2 &= ~ADC_CalibrationVoltage_Mode3;
    ADCx->CTLR2 |= CalibrationVoltage_Mode;
}

/*********************************************************************
 * @fn      ADC_GetCalibrationOffset
 *
 * @brief   Get Calibration Offset value.
 *
 * @param   ADCx - where x can be 1 t0 4 to select the ADC peripheral.
 *
 * @return  Calibration Offset value.
 */
uint16_t ADC_GetCalibrationOffset(ADC_TypeDef *ADCx)
{
    return ((uint16_t)((ADCx->STATR & ADC_DELTA) >> 16));
}

/*********************************************************************
 * @fn      ADC_ScanSelConfig
 *
 * @brief   ADC Scan Out Select.
 *
 * @param   ADC_SCAN_SEL - ADC Scan Out Select.
 *            ADC1_SCAN_OUT - the ADC1 Select Out 
 *            ADC2_SCAN_OUT - the ADC2 Select Out 
 *            ADC3_SCAN_OUT - the ADC3 Select Out 
 *            ADC4_SCAN_OUT - the ADC4 Select Out 
 *
 * @return  none.
 */
void ADC_ScanSelConfig(uint32_t ADC_SCAN_SEL)
{
    EXTEN->EXTEN_CTR &= ~(ADC4_SCAN_OUT);
    EXTEN->EXTEN_CTR |= (ADC_SCAN_SEL);
}