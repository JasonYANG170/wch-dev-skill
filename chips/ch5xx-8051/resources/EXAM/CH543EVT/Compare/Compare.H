/********************************** (C) COPYRIGHT *******************************
* File Name          : Compare.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 ADC比较器
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


#ifndef __COMPARE_H__
#define __COMPARE_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************/
/* 相关宏定义 */

/* CMP中断使能定义 */
#define   EN_ADC_INT                 1                                          /* CMP中断使能开关 */

/*******************************************************************************/
/* 函数外扩 */
extern void CMP_Init( void );                                                   /* 比较器模块初始化 */

#endif

/*********************************END OF FILE**********************************/
