/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.H
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH545 ADC 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef	__ADC_H__
#define __ADC_H__

#include "Debug.H"


#define ADC_ENABLE()      ADC_CTRL |= bADC_EN
#define ADC_DISABLE()     ADC_CTRL &= ~bADC_EN

#define ADC_START()       ADC_CTRL |= bADC_START     
#define ADCIF_CLEAR()     ADC_CTRL |= bADC_IF

#define DCDC_FREQ(freq)   CMP_DCDC&=~MASK_DCDC_FREQ;CMP_DCDC|=MASK_DCDC_FREQ&freq
#define DCDC_PIN(b)       CMP_DCDC&=~bDCDC_PIN;CMP_DCDC|=bDCDC_PIN&b
//ADC时钟
#define ADC_FOSC512      0
#define ADC_FOSC256      1
#define ADC_FOSC128      2
#define ADC_FOSC64       3

//ADCͨ通道
#define ADC_Channel0     0
#define ADC_Channel1     1
#define ADC_Channel2     2
#define ADC_Channel3     3
#define ADC_Channel4     4
#define ADC_Channel5     5
#define ADC_Channel6     6
#define ADC_Channel7     7
#define ADC_Channel8     8
#define ADC_Channel9     9
#define ADC_Channel10    10
#define ADC_Channel11    11
#define ADC_Channel12    12
#define ADC_Channel13    13
#define ADC_ChannelV33   14
#define ADC_ChannelV18   15

//ÕýÏàÊäÈë
#define CMP_VDD          0
#define CMP_ADC          1

//·´ÏàÊäÈë
#define CMP_CLOSE        0
#define CMP_V12          1
#define CMP_V33          2
#define CMP_V50          3
#define CMP_V54          4
#define CMP_V58          5
#define CMP_V62          6
#define CMP_V66          7

void ADCInit(UINT8 div);
UINT8 ADC_ChannelSelect(UINT8 ch);
void VoltageCMPModeInit(UINT8 fo, UINT8 re);

#endif