/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 ADC采样时钟设置,ADC通道设置函数,外部通道检测初始化;
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __ADC_H__
#define __ADC_H__

/******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/******************************************************************************/
/* 相关宏定义 */

/* ADC中断使能定义 */
#define  EN_ADC_INT                     0                                       /* ADC中断使能开关 */

/* ADC通道定义 */
#define  CH0                            0		                                /* P1.0 */
#define  CH1                            1		                                /* P1.1 */
#define  CH2                            2		                                /* P3.2 */
#define  CH3                            3		                                /* RST*25% */
#define  CH4                            4		                                /* P1.4 */
#define  CH5                            5		                                /* P1.5 */
#define  CH6                            6		                                /* P1.6 */
#define  CH7                            7		                                /* P1.7 */
#define  CH8                            8		                                /* P3.6 */
#define  CH9                            9		                                /* P3.7 */
#define  CH10                           0x0A                                    /* P1.2 */
#define  CH11                           0x0B                                    /* P1.3 */
#define  CH12                           0x0C                                    /* VDD12*25% */
#define  CH13                           0x0D                                    /* VREF(约1.2V; 可能有噪声) */
#define  CHALL                          0xFFFF

/* 快速调用宏定义 */
#define  ADC_ExChannelSelect( ch )      ADC_CHAN = ADC_CHAN & ~0x0F | ch        /* 选择ADC通道: 0~13 */
#define  ADC_StartSample( )             ADC_CTRL |= bADC_START                  /* 启动一次ADC采样 */

/******************************************************************************/
/* 函数外扩 */
extern void ADC_Init( UINT8 AdcClk );                                           /* ADC通道初始化 */
extern void ADC_ChSelect( UINT8 ch );                                           /* ADC通道选择 */

#endif

/*********************************END OF FILE**********************************/
