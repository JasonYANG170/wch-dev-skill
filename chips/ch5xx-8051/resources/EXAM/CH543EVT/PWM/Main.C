/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 PWM初始化,占空比设置,PWM默认电平设置
*                      支持中断方式修改PWM占空比
*                      引脚           功能
*                      P15            PWM0
*                      P34            PWM1
*                      P16            PWM2
*                      P14            PWM3
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\PWM\PWM.H"
#pragma  NOAREGS

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void )
{
    CfgFsys( );																	/* CH543时钟选择配置 */
    mDelaymS( 5 );																/* 配置时钟后,建议延时稳定时钟 */
    mInitSTDIO( );                                                             	/* 串口0初始化 */
    printf("PWM Demo start ...\n");
	
    SetPWM8Bit( );																/* PWM数据宽度为8位 */
    
	/* 时钟频率设置 */
    SetPWMClkDiv( 4 );															/* PWM时钟配置,FREQ_SYS/4 */
    SetPWMCycle_H( 1 );
    SetPWMCycle_L( 44 );														/* PWM_CYCLE=FREQ_SYS/4/(cyc_h*256+cyc_l) */
    
	/* PWM0 PWM1输出有效极性设置(可选) */
    PWM0OutPolarLowAct();
    PWM1OutPolarHighAct();
    
	/* 初始占空比配置 */
    SetPWM0Dat( 50 );														                  			         //输出占空比=dat/PWM_CYCLE
    SetPWM1Dat( 100 );
    SetPWM2Dat( 150 );
    SetPWM3Dat( 200 );
	
    /* 启动通道 */
    PWM_SEL_CHANNEL( CH0 | CH1 | CH2 | CH3, Enable );                 			/* 输出使能 */
#ifdef PWM_INTERRUPT
    PWMInterruptEnable( );
#endif

    while( 1 )
    {
		/*********************************************************************/
		/* 主循环处理 */
		
    }
}