/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
* Author             : WCH
* Version            : V1.1
* Date               : 2018/02/27
* Description        : CH554 PWM中断使能和中断处理               				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\Public\CH554.H"                                                 
#include "..\Public\Debug.H"
#include "PWM.H"
#include "stdio.h"

#pragma  NOAREGS

#if PWM_INTERRUPT
/*******************************************************************************
* Function Name  : PWMInterruptEnable()
* Description    : PWM中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  PWMInterruptEnable()
{
    PWM_CTRL |= bPWM_IF_END | bPWM_IE_END;                                      //清除PWM中断，使能PWM中断
    IE_PWMX = 1;	
}

/*******************************************************************************
* Function Name  : PWMInterrupt(void)
* Description    : PWM中断服务程序   
*******************************************************************************/
void	PWMInterrupt( void ) interrupt INT_NO_PWMX using 1                      //PWM1&2中断服务程序,使用寄存器组1
{
    PWM_CTRL |= bPWM_IF_END;                                                  //清除PWM中断
    SetPWM1Dat(0x10);
    SetPWM2Dat(0x40);	
    printf("PWM_CTRL  INT\n");                                                //开启可以用于查看是否进入中断	
}
#endif

