/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 PWM配置与可选的中断处理(PWM 循环周期结束中断)
*                      引脚           功能
*                      P15            PWM0
*                      P34            PWM1
*                      P16            PWM2
*                      P14            PWM3
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



#ifndef __PWM_H__
#define __PWM_H__

/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************/
/* 相关宏定义 */

/* PWM中断使能 */
#define PWM_INTERRUPT

/* PWM通道(可以组合) */
#define   CH0        				0x01
#define   CH1       				0x02
#define   CH2        				0x04
#define   CH3        				0x08

/* 输出使能状态 */
#define  Disable					0x00										/* 关闭输出 */
#define  Enable      				0x01										/* 使能输出 */

/* 外部调用恒定义 */
#define SetPWMClkDiv( n )			PWM_CK_SE = n                            	/* PWM时钟配置,FREQ_SYS/n */  
#define SetPWM12Bit( )				PWM_CTRL2 |= bPWM_MOD_12BIT              	/* PWM数据宽度为12位,仅PWM0和PWM1可用 */
#define SetPWM8Bit( ) 	      		PWM_CTRL2 &= ~bPWM_MOD_12BIT             	/* PWM数据宽度为8位 */
#define SetPWMCycle_L( cyc_l ) 	   	PWM_CYCLE_L = cyc_l							/* 设置PWM周期值低字节 */
#define SetPWMCycle_H( cyc_h ) 	   	PWM_CYCLE_H = cyc_h          				/* 设置PWM周期值高字节低4位 */
#define PWM0OutPolarHighAct( )    	PWM_CTRL &= ~bPWM0_POLAR                 	/* PWM0输出默认低,高有效(默认) */
#define PWM0OutPolarLowAct( )     	PWM_CTRL |= bPWM0_POLAR                  	/* PWM0输出默认高,低有效 */
#define PWM1OutPolarHighAct( )    	PWM_CTRL &= ~bPWM1_POLAR                 	/* PWM1输出默认低,高有效(默认) */         
#define PWM1OutPolarLowAct( )     	PWM_CTRL |= bPWM1_POLAR                  	/* PWM1输出默认高,低有效 */
#define SetPWM0Dat( dat )          	PWM_DATA0 = dat                          	/* 设置PWM0输出占空比 */
#define SetPWM1Dat( dat )          	PWM_DATA1 = dat                          	/* 设置PWM1输出占空比 */
#define SetPWM2Dat( dat )          	PWM_DATA2 = dat                          	/* 设置PWM2输出占空比 */
#define SetPWM3Dat( dat )          	PWM_DATA3 = dat                          	/* 设置PWM3输出占空比 */

/*******************************************************************************/
/* 函数外扩 */
extern void PWM_SEL_CHANNEL( UINT8 Channel, UINT8 NewState );                	/* PWM通道输出使能,包含IO端口配置 */
#ifdef PWM_INTERRUPT
extern void PWMInterruptEnable( void );                                         /* 使能PWM周期溢出中断 */
#endif

#endif
