/********************************** (C) COPYRIGHT *******************************
* File Name          : adc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : Adc-related configuration
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include <ch641_debug.h>

/* ADC_channels */
#define ADC_Channel_NTC                                 ((uint8_t)ADC_Channel_0)
#define ADC_Channel_OSC                                 ((uint8_t)ADC_Channel_1)
#define ADC_Channel_ISP                                 ((uint8_t)ADC_Channel_8)

/* ADC RSTCAL mask */
#define CTLR2_RSTCAL_Set                                 ((uint32_t)0x00000008)

/* ADC CAL mask */
#define CTLR2_CAL_Set                                    ((uint32_t)0x00000004)

/* Function extensibility */
extern void ADC_User_Init( void );
extern void ADC_Set_VHV( void );
extern void ADC_Set_NTC( void );
extern void ADC_NTC_Release( void );
extern void ADC_Set_OSC( void );
extern void ADC_Set_ISEN( void );
extern uint16_t ADC_Start( void );
extern uint16_t ADC_Sample_X4(void);
extern void ISEN_Calibration( void );


#endif /* USER_ADC_H_ */
