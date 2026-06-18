/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
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



/*******************************************************************************/
/* 头文件包含 */
#include ".\PWM\PWM.H"
#pragma  NOAREGS

/********************************************************************************
* Function Name  : PWM_SEL_CHANNEL( UINT8 Channel,UINT8 NewState )
* Description    : PWM通道输出使能,包含IO端口配置
* Input          : Channel：通道号,位域表示
*                  NewState：0:关闭通道  1:开启通道
* Output         : None
* Return         : None
********************************************************************************/
void PWM_SEL_CHANNEL( UINT8 Channel, UINT8 NewState )
{
    UINT8 i;
	
    /* 通道配置 */
    if( NewState == Enable )													/* 输出开启 */
    {
        PWM_CTRL &= ~bPWM_CLR_ALL;
        
		if( Channel & CH0 )
        {
            PWM_CTRL |= bPWM0_OUT_EN;
            P1_MOD_OC &= ~( 1 << 5 );             								/* 对应的GPIO口配置成推挽模式 */
            P1_DIR_PU |= ( 1 << 5 );
        }
		
        if( Channel & CH1 )
        {
            PWM_CTRL |= bPWM1_OUT_EN;
            P3_MOD_OC &= ~( 1 << 4 );
            P3_DIR_PU |= ( 1 << 4 );
        }
		
        if( Channel & CH2 )
        {
            PWM_CTRL2 |= bPWM2_OUT_EN;
            P1_MOD_OC &= ~( 1 << 6 );
            P1_DIR_PU |= ( 1 << 6 );
        }
		
        if( Channel & CH3 )
        {
            PWM_CTRL2 |= bPWM3_OUT_EN;
            P1_MOD_OC &= ~( 1 << 4 );
            P1_DIR_PU |= ( 1 << 4 );
        }
    }
    else                                      									/* 输出关闭 */
    {
        if( Channel & CH0 )
        {
            PWM_CTRL &= ~bPWM0_OUT_EN;
        }
		
        if(Channel & CH1 )
        {
            PWM_CTRL &= ~bPWM1_OUT_EN;
        }
		
        if( Channel & CH2 )
        {
            PWM_CTRL2 &= ~bPWM2_OUT_EN;
        }
		
        if( Channel & CH3 )
        {
            PWM_CTRL2 &= ~bPWM3_OUT_EN;
        }
    }
}

#ifdef PWM_INTERRUPT
/********************************************************************************
* Function Name  : PWMInterruptEnable( void )
* Description    : PWM中断使能
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void PWMInterruptEnable( void )
{
    PWM_CTRL |= bPWM_IE_END;
    PWM_CTRL |= bPWM_IF_END;													/* 清除PWM中断 */
    IE_PWMX = 1;
    EA = 1;
}

/********************************************************************************
* Function Name  : PWMInterrupt( void )
* Description    : PWM0~3中断服务程序,使用寄存器组1
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void PWMInterrupt( void ) interrupt INT_NO_PWMX using 1                       	
{
    static UINT8 duty = 0;
	
    PWM_CTRL |= bPWM_IF_END;                                                  	/* 清除PWM中断 */

    /* 可选的,重新配置占空比 */
    SetPWM0Dat( duty );
    SetPWM1Dat( duty );
    SetPWM2Dat( duty );
    SetPWM3Dat( duty );
    duty += 3;
}
#endif
