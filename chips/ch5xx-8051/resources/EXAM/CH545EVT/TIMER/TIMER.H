/********************************** (C) COPYRIGHT *******************************
* File Name          : TIMER.H
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH545 Time
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __TIMER_H__
#define __TIMER_H__

#include "Debug.H"


#define Timer0_INTERRUPT        0                                              //选择Timer0中断方式
#define Timer1_INTERRUPT        0                                              //选择Timer1中断方式
#define Timer2_INTERRUPT        0                                              //选择Timer2中断方式
#define T2_CAP                  1                                              //选择Timer2捕捉模式

/******************* CH545 Timer0相关 ************************/
//bTMR_CLK同时影响Timer0&1&2,使用时要注意 (除定时使用标准时钟)
#define mTimer0Clk12DivFsys() (T2MOD &= ~bT0_CLK)                             //定时器,时钟=Fsys/12 T0标准时钟
#define mTimer0ClkFsys()      (T2MOD |= bTMR_CLK | bT0_CLK)                   //定时器,时钟=Fsys
#define mTimer0Clk4DivFsys()  (T2MOD &= ~bTMR_CLK;T2MOD |= bT0_CLK)          //定时器,时钟=Fsys/4（此时需同步调整DEBUG.C的mInitSTDIO）
#define mTimer0CountClk()     (TMOD |= bT0_CT)                                //选择工作在计数器模式（T0引脚（P34）的下降沿有效）
/***** CH555 Timer1相关(DEBUG.C的mInitSTDIO使用T1模式2) *****/
#define mTimer1Clk12DivFsys() (T2MOD &= ~bT1_CLK)                             //定时器,时钟=Fsys/12  T1标准时钟
#define mTimer1ClkFsys()      (T2MOD |= bTMR_CLK | bT1_CLK)                   //定时器,时钟=Fsys
#define mTimer1Clk4DivFsys()  (T2MOD &= ~bTMR_CLK;T2MOD |= bT1_CLK)          //定时器,时钟=Fsys/4
#define mTimer1CountClk()     (TMOD |= bT1_CT)                                //选择工作在计数器模式（T1引脚（P35）的下降沿有效）
/******************* CH555 Timer2相关 ************************/
#define mTimer2Clk12DivFsys() {T2MOD &= ~ bT2_CLK;C_T2 = 0;}                  //定时器,时钟=Fsys/12 T2标准时钟
#define mTimer2ClkFsys()      {T2MOD |= (bTMR_CLK | bT2_CLK);C_T2=0;}         //定时器,时钟=Fsys
#define mTimer2Clk4DivFsys()  {T2MOD &= ~bTMR_CLK;T2MOD |= bT2_CLK;C_T2 = 0;}//定时器,时钟=Fsys/4（此时需同步调整DEBUG.C的mInitSTDIO）
#define mTimer2CountClk()     {C_T2 = 1;}                                     //选择工作在计数器模式（T2引脚（P10）的下降沿有效）
#define mTimer2OutCTL()       (T2MOD |= T2OE)                                 //T2输出  频率TF2/2   



UINT8 mTimer_x_ModInit(UINT8 x, UINT8 mode);
void mTimer_x_SetData(UINT8 x, UINT16 dat);
void CAP2Init(UINT8 mode);
void CAP1Init(UINT8 mode);

#endif

