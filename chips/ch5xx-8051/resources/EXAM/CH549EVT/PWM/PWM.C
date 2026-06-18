/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/29
* Description        : CH549 PWM配置与可选的中断处理（PWM 循环周期结束中断）
*                      引脚           功能
*                      P25            PWM0
*                      P24            PWM1
*                      P23            PWM2
*                      P22            PWM3
*                      P21            PWM4
*                      P20            PWM5
*                      P26            PWM6
*                      P27            PWM7
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\PWM\PWM.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : PWM_CFG_CHANNEL()
* Description    : PWM通道输出使能,包含IO端口配置
* Input          : Channel：通道号，位域表示
*                  NewState：0:关闭通道  1:开启通道
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_SEL_CHANNEL(UINT8 Channel,UINT8 NewState)
{
    UINT8 i;
    /* 通道配置 */
    if(NewState == Enable)                    //输出开启
    {
        PWM_CTRL &= ~bPWM_CLR_ALL;
        if(Channel&CH0)
        {
            PWM_CTRL |= bPWM0_OUT_EN;
        }
        if(Channel&CH1)
        {
            PWM_CTRL |= bPWM1_OUT_EN;
        }
        PWM_CTRL2 = (Channel>>2);
        /* 对应的GPIO口配置成推挽模式 */
        for(i=0; i!=6; i++)
        {
            if(Channel & (1<<i))
            {
                P2_MOD_OC &= ~(1<<(5-i));
                P2_DIR_PU |= (1<<(5-i));
            }
        }
        if(Channel&CH6)
        {
            P2_MOD_OC &= ~CH6;
            P2_DIR_PU |= CH6;
        }
        if(Channel&CH7)
        {
            P2_MOD_OC &= ~CH7;
            P2_DIR_PU |= CH7;
        }
    }
    else                                      //输出关闭
    {
        if(Channel&CH0)
        {
            PWM_CTRL &= ~bPWM0_OUT_EN;
        }
        if(Channel&CH1)
        {
            PWM_CTRL &= ~bPWM1_OUT_EN;
        }
        PWM_CTRL2 &= ~(Channel>>2);
    }
}
#ifdef PWM_INTERRUPT
/*******************************************************************************
* Function Name  : PWMInterruptEnable()
* Description    : PWM中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWMInterruptEnable()
{
    PWM_CTRL |= bPWM_IF_END;                                                  //清除PWM中断
    IE_PWMX = 1;
    EA = 1;
}
/*******************************************************************************
* Function Name  : PWMInterrupt(void)
* Description    : PWM中断服务程序 FREQ_SYS/n/64或FREQ_SYS/n/256 进中断
*******************************************************************************/
void PWMInterrupt( void ) interrupt INT_NO_PWMX using 1                       //PWM0~8中断服务程序,使用寄存器组1
{
    static UINT8 duty = 0;
    PWM_CTRL |= bPWM_IF_END;                                                  //清除PWM中断

    /* 可选的,重新配置占空比 */
    SetPWM0Dat(duty);
    SetPWM1Dat(duty);
    SetPWM2Dat(duty);
    duty++;

}
#endif