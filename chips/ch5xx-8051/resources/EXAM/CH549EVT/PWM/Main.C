/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 PWM初始化，占空比设置，PWM默认电平设置
*                      支持中断方式修改PWM占空比
*                      引脚           功能
*                      P25            PWM0
*                      P24            PWM1
*                      P23            PWM2
*                      P22            PWM3
*                      P21            PWM4
*                      P20            PWM5
*                      P26            PWM6
*                      P27            PWM7
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\PWM\PWM.H"
#pragma  NOAREGS
void main( )
{
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(5);                                                               //配置时钟后，建议延时稳定时钟
    mInitSTDIO( );                                                             //串口0初始化
    printf("PWM Demo start ...\n");
    /* 时钟 频率设置 */
    SetPWMClkDiv(4);                                                           //PWM时钟配置,FREQ_SYS/4
    SetPWMCycle256Clk();                                                       //PWM周期 FREQ_SYS/4/256
    /* PWM0 PWM1输出有效极性设置(可选) */
    PWM0OutPolarLowAct();
    PWM1OutPolarHighAct();
    /* 初始占空比配置 */
    SetPWM0Dat(10);
    SetPWM1Dat(25);
    SetPWM2Dat(50);
    SetPWM3Dat(60);
    SetPWM4Dat(64);
    SetPWM5Dat(175);
    SetPWM6Dat(200);
    SetPWM7Dat(250);
    /* 启动通道 */
    PWM_SEL_CHANNEL( CH0|CH1|CH2|CH3|CH4|CH5|CH6|CH7,Enable );                 //输出使能
#ifdef PWM_INTERRUPT
    PWMInterruptEnable();
#endif
    while(1)
    {
    }
}
