/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_adc.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2024/12/25
 * Description        : This file contains all the functions prototypes for the
 *                      ADC firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32M030_ADC_H
#define __CH32M030_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32m030.h"

/* ADC Init structure definition */
typedef struct
{
    uint32_t ADC_Mode; /* Configures the ADC to operate in independent or
                          dual mode.
                          This parameter can be a value of @ref ADC_mode */

    FunctionalState ADC_ScanConvMode; /* Specifies whether the conversion is performed in
                                         Scan (multichannels) or Single (one channel) mode.
                                         This parameter can be set to ENABLE or DISABLE */

    FunctionalState ADC_ContinuousConvMode; /* Specifies whether the conversion is performed in
                                               Continuous or Single mode.
                                               This parameter can be set to ENABLE or DISABLE. */

    uint32_t ADC_ExternalTrigConv; /* Defines the external trigger used to start the analog
                                      to digital conversion of regular channels. This parameter
                                      can be a value of @ref ADC_external_trigger_sources_for_regular_channels_conversion */

    uint32_t ADC_DataAlign; /* Specifies whether the ADC data alignment is left or right.
                               This parameter can be a value of @ref ADC_data_align */

    uint8_t ADC_NbrOfChannel; /* Specifies the number of ADC channels that will be converted
                                   using the sequencer for regular channel group.
                                   This parameter must range from 1 to 16. */
} ADC_InitTypeDef;

/* ADC_mode */
#define ADC_Mode_Independent                           ((uint32_t)0x00000000)

/* ADC_external_trigger_sources_for_regular_channels_conversion */
#define ADC_ExternalTrigConv_None                      ((uint32_t)0x00000000)
#define ADC_ExternalTrigConv_T1_CC4                    ((uint32_t)0x00010000)
#define ADC_ExternalTrigConv_T1_CC5                    ((uint32_t)0x00020000)
#define ADC_ExternalTrigConv_T2_CC1                    ((uint32_t)0x00030000)
#define ADC_ExternalTrigConv_T2_CC2                    ((uint32_t)0x00040000)
#define ADC_ExternalTrigConv_T2_CC3                    ((uint32_t)0x00050000)
#define ADC_ExternalTrigConv_T2_CC4                    ((uint32_t)0x00060000)
#define ADC_ExternalTrigConv_T3_CC1                    ((uint32_t)0x00070000)
#define ADC_ExternalTrigConv_T3_CC2                    ((uint32_t)0x00080000)
#define ADC_ExternalTrigConv_Ext_PA14_PB6              ((uint32_t)0x00090000)
#define ADC_ExternalTrigConv_T1_CC4_CC5                ((uint32_t)0x000A0000)

/* ADC_data_align */
#define ADC_DataAlign_Right                            ((uint32_t)0x00000000)
#define ADC_DataAlign_Left                             ((uint32_t)0x00000400)

/* ADC_channels */
#define ADC_Channel_0                                  ((uint8_t)0x00)
#define ADC_Channel_1                                  ((uint8_t)0x01)
#define ADC_Channel_2                                  ((uint8_t)0x02)
#define ADC_Channel_3                                  ((uint8_t)0x03)
#define ADC_Channel_4                                  ((uint8_t)0x04)
#define ADC_Channel_5                                  ((uint8_t)0x05)
#define ADC_Channel_6                                  ((uint8_t)0x06)
#define ADC_Channel_7                                  ((uint8_t)0x07)
#define ADC_Channel_8                                  ((uint8_t)0x08)
#define ADC_Channel_9                                  ((uint8_t)0x09)
#define ADC_Channel_10                                 ((uint8_t)0x0A)
#define ADC_Channel_11                                 ((uint8_t)0x0B)
#define ADC_Channel_12                                 ((uint8_t)0x0C)
#define ADC_Channel_13                                 ((uint8_t)0x0D)
#define ADC_Channel_14                                 ((uint8_t)0x0E)
#define ADC_Channel_15                                 ((uint8_t)0x0F)
#define ADC_Channel_16                                 ((uint8_t)0x10)
#define ADC_Channel_17                                 ((uint8_t)0x11)
#define ADC_Channel_18                                 ((uint8_t)0x12)
#define ADC_Channel_19                                 ((uint8_t)0x13)

/* ADC_sampling_time */
#define ADC_SampleTime_5Cycles5                        ((uint8_t)0x00)
#define ADC_SampleTime_11Cycles5                       ((uint8_t)0x01)
#define ADC_SampleTime_23Cycles5                       ((uint8_t)0x02)
#define ADC_SampleTime_59Cycles5                       ((uint8_t)0x03)

/* ADC_external_trigger_sources_for_injected_channels_conversion */
#define ADC_ExternalTrigInjecConv_None                 ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjecConv_T1_CC4               ((uint32_t)0x00000800)
#define ADC_ExternalTrigInjecConv_T1_CC5               ((uint32_t)0x00001000)
#define ADC_ExternalTrigInjecConv_T2_CC1               ((uint32_t)0x00001800)
#define ADC_ExternalTrigInjecConv_T2_CC2               ((uint32_t)0x00002000)
#define ADC_ExternalTrigInjecConv_T2_CC3               ((uint32_t)0x00002800)
#define ADC_ExternalTrigInjecConv_T2_CC4               ((uint32_t)0x00003000)
#define ADC_ExternalTrigInjecConv_T3_CC1               ((uint32_t)0x00003800)
#define ADC_ExternalTrigInjecConv_T3_CC2               ((uint32_t)0x00004000)
#define ADC_ExternalTrigInjecConv_T1_CC4_CC5           ((uint32_t)0x00004800)
#define ADC_ExternalTrigInjecConv_Ext_PA14_PB6         ((uint32_t)0x00005000)

/* ADC_injected_channel_selection */
#define ADC_InjectedChannel_1                          ((uint8_t)0x14)
#define ADC_InjectedChannel_2                          ((uint8_t)0x18)
#define ADC_InjectedChannel_3                          ((uint8_t)0x1C)
#define ADC_InjectedChannel_4                          ((uint8_t)0x20)

/* ADC_analog_watchdog_selection */
#define ADC_AnalogWatchdog_SingleRegEnable             ((uint32_t)0x00800200)
#define ADC_AnalogWatchdog_SingleInjecEnable           ((uint32_t)0x00400200)
#define ADC_AnalogWatchdog_SingleRegOrInjecEnable      ((uint32_t)0x00C00200)
#define ADC_AnalogWatchdog_AllRegEnable                ((uint32_t)0x00800000)
#define ADC_AnalogWatchdog_AllInjecEnable              ((uint32_t)0x00400000)
#define ADC_AnalogWatchdog_AllRegAllInjecEnable        ((uint32_t)0x00C00000)
#define ADC_AnalogWatchdog_None                        ((uint32_t)0x00000000)

/* ADC_interrupts_definition */
#define ADC_IT_EOC                                     ((uint32_t)0x00400020)
#define ADC_IT_AWD                                     ((uint32_t)0x00600040)
#define ADC_IT_JEOC                                    ((uint32_t)0x00500080)
#define ADC_IT_AWD_OV                                  ((uint32_t)0x00812000)
#define ADC_IT_AWD_DN                                  ((uint32_t)0x00814000)
#define ADC_IT_AVG_OV                                  ((uint32_t)0x10810100)
#define ADC_IT_AVG_DN                                  ((uint32_t)0x10810200)

/* ADC_flags_definition */
#define ADC_FLAG_AWD                                   ((uint32_t)0x00000001)
#define ADC_FLAG_EOC                                   ((uint32_t)0x00000002)
#define ADC_FLAG_JEOC                                  ((uint32_t)0x00000004)
#define ADC_FLAG_JSTRT                                 ((uint32_t)0x00000008)
#define ADC_FLAG_STRT                                  ((uint32_t)0x00000010)
#define ADC_FLAG_AWD_OV                                ((uint32_t)0x00000020)
#define ADC_FLAG_AWD_DN                                ((uint32_t)0x00000040)
#define ADC_FLAG_MULT_CMP1                             ((uint32_t)0x00000080)
#define ADC_FLAG_MULT_CMP2                             ((uint32_t)0x00000100)
#define ADC_FLAG_MULT_CMP3                             ((uint32_t)0x00000200)
#define ADC_FLAG_AVG_OVI                               ((uint32_t)0x10010000)
#define ADC_FLAG_AVG_DNI                               ((uint32_t)0x10020000)
#define ADC_FLAG_AVG_OV                                ((uint32_t)0x10040000)
#define ADC_FLAG_AVG_DN                                ((uint32_t)0x10080000)

/* ADC_analog_watchdog_flags_definition */
#define ADC_AnalogWatchdog_0_FLAG                      ((uint32_t)0x00000100)
#define ADC_AnalogWatchdog_1_FLAG                      ((uint32_t)0x00000200)
#define ADC_AnalogWatchdog_2_FLAG                      ((uint32_t)0x00000400)

/* ADC_analog_watchdog_reset_enable_selection */
#define ADC_AnalogWatchdog_0_RST_EN                    ((uint32_t)0x00000010)
#define ADC_AnalogWatchdog_1_RST_EN                    ((uint32_t)0x00000020)
#define ADC_AnalogWatchdog_2_RST_EN                    ((uint32_t)0x00000040)

/* ADC_moving_average_window_value_definition*/
#define ADC_MovingWindow_Size_32                       ((uint32_t)0x00000000)
#define ADC_MovingWindow_Size_64                       ((uint32_t)0x00100000)
#define ADC_MovingWindow_Size_128                      ((uint32_t)0x00200000)
#define ADC_MovingWindow_Size_256                      ((uint32_t)0x00300000)
#define ADC_MovingWindow_Size_512                      ((uint32_t)0x00400000)
#define ADC_MovingWindow_Size_1024                     ((uint32_t)0x00500000)

/* ADC_external_trigger_sources_delay_channels_definition */
#define ADC_ExternalTrigRegul_DLY                      ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjec_DLY                      ((uint32_t)0x00020000)

void       ADC_DeInit(ADC_TypeDef *ADCx);
void       ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct);
void       ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct);
void       ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ITConfig(ADC_TypeDef *ADCx, uint32_t ADC_IT, FunctionalState NewState);
void       ADC_ResetCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_StartCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *ADCx);
void       ADC_DiscModeChannelCountConfig(ADC_TypeDef *ADCx, uint8_t Number);
void       ADC_DiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_ExternalTrigConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
uint16_t   ADC_GetConversionValue(ADC_TypeDef *ADCx);
uint8_t    ADC_GetConversionChannelSequence(ADC_TypeDef *ADCx);
void       ADC_AutoInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_InjectedDiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef *ADCx, uint32_t ADC_ExternalTrigInjecConv);
void       ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef *ADCx);
void       ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_InjectedSequencerLengthConfig(ADC_TypeDef *ADCx, uint8_t Length);
void       ADC_SetInjectedOffset(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset);
uint16_t   ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel);
void       ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void       ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void       ADC_AnalogWatchdog1ThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void       ADC_AnalogWatchdog2ThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void       ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint32_t ADC_FLAG);
void       ADC_ClearFlag(ADC_TypeDef *ADCx, uint32_t ADC_FLAG);
ITStatus   ADC_GetITStatus(ADC_TypeDef *ADCx, uint32_t ADC_IT);
void       ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint32_t ADC_IT);
void       ADC_BufferCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_MULTCmpCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
FlagStatus ADC_GetAnalogWatchdogFlagStatus(ADC_TypeDef *ADCx, uint32_t AnalogWatchdog_FLAG);
void       ADC_ClearAnalogWatchdogFlag(ADC_TypeDef *ADCx, uint32_t AnalogWatchdog_FLAG);
void       ADC_AnalogWatchdogResetCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog_x, FunctionalState NewState);
void       ADC_AnalogWatchdogScanCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelIndicationCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ChannelAdvanceSwCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelPollSaveCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_CmpareResultXORCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_MovingAverageCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_MovingAverageWindowSizeConfig(ADC_TypeDef *ADCx, uint32_t MoveSize);
void       ADC_AverageValueThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold,uint16_t LowThreshold);
void       ADC_ExternalTrig_DLY(ADC_TypeDef *ADCx, uint32_t channel, uint16_t DelayTim);
uint16_t   ADC_GetAverageValue(ADC_TypeDef *ADCx);
uint16_t   ADC_GetConversionValue2(ADC_TypeDef *ADCx);
uint16_t   ADC_GetConversionValue3(ADC_TypeDef *ADCx);
uint16_t   ADC_GetConversionValue4(ADC_TypeDef *ADCx);

#ifdef __cplusplus
}
#endif

#endif /*__CH32M030_ADC_H */
