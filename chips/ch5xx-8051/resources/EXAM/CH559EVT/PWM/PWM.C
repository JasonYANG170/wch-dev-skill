/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/6/24
* Description        : CH559 PWM接口函数             				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                         //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

#define SetPWMClk(CK_SE) (PWM_CK_SE = CK_SE)                                  //分频,默认时钟Fsys            
#define SetPWMCycle(Cycle) (PWM_CYCLE = Cycle)                                //设置循环周期
#define SetPWM1Dat(dat) (PWM_DATA = dat)                                      //设置PWM输出占空比
#define SetPWM2Dat(dat) (PWM_DATA2 = dat)
#define PWMPINAlter( )  { P4_DIR |= bPWM2_ | bPWM1_;PIN_FUNC |= bPWM1_PIN_X;} //设置PWM引脚映射

/*******************************************************************************
* Function Name  : InitPWM1(UINT8 polar)
* Description    : PWM1初始化函数
* Input          : polar=0选择默认低电平，高电平输出有效;
                   polar=1选择默认高电平，低电平输出有效;
* Output         : None
* Return         : None
*******************************************************************************/
void  InitPWM1(UINT8 polar)
{
    PWM_CTRL &= ~bPWM_CLR_ALL;                                                //清空FIFO和计数                                                      
    PWM_CTRL &= ~bPWM_MOD_MFM;
    PWM_CTRL |=	bPWM_IE_END;                                                  //使能PWM计数周期完成中断
    PWM_CTRL |=	bPWM_OUT_EN;                                                  //PWM1输出使能
    PWM_CTRL  |= bPWM_IF_END;                                                 //清除所有的中断标志
    if(polar){
        PWM_CTRL |= bPWM_POLAR;                                               //低电平有效
    }
    else{
        PWM_CTRL &= ~bPWM_POLAR;                                              //高电平有效  
    }			
}

/*******************************************************************************
* Function Name  : InitPWM2(UINT8 polar)
* Description    : PWM初始化函数
* Input          : polar=0选择默认低电平，高电平输出有效;
                   polar=1选择默认高电平，低电平输出有效;
* Output         : None
* Return         : None
*******************************************************************************/
void  InitPWM2(UINT8 polar)
{
    PWM_CTRL &= ~bPWM_CLR_ALL;                                                //清空FIFO和计数                                                      
    PWM_CTRL &= ~bPWM_MOD_MFM;
    PWM_CTRL |=	bPWM_IE_END;                                                  //使能PWM计数周期完成中断
    PWM_CTRL |= bPWM2_OUT_EN;                                                 //PWM2输出使能	
    PWM_CTRL  |= bPWM_IF_END;                                                 //清除所有的中断标志
    if(polar){
        PWM_CTRL |= bPWM2_POLAR;                                              //低电平有效
    }
    else{
        PWM_CTRL &= ~bPWM2_POLAR;                                             //高电平有效  
    }			
}

/*******************************************************************************
* Function Name  : PWMInterrupt(void)
* Description    : PWM中断服务程序   
*******************************************************************************/
void	PWMInterrupt( void ) interrupt INT_NO_PWM1 using 1                     //PWM1&2中断服务程序,使用寄存器组1
{
    if(PWM_CTRL & bPWM_IF_END)                                                 
    {
        PWM_CTRL |= bPWM_IF_END;
        printf("PWM_DATA  %02X\n",(UINT16)PWM_DATA);
    }                                                                
}

main( ) 
{
    mDelaymS(30);                                                              //上电延时,等待内部晶振稳定,必加 
//  CfgFsys( );  
    PORT_CFG &= ~bP2_OC;
    P2_DIR |= bPWM1 | bPWM2;                                                   //开启PWM建议设置引脚为推挽输出	
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");
#if 0
    PWMPINAlter( );                                                            //引脚映射	
#endif
    SetPWMClk(12);                                                             //设置PWM1&2的时钟分频系数为12
    InitPWM1(1);                                                               //PWM1初始化，低电平有效
    InitPWM2(0);                                                               //PWM2初始化，高电平有效
    SetPWMCycle(100);                                                          //设置循环周期100
    IE_PWM1 = 1;                                                               //使能PWM1中断
    SetPWM1Dat(50);                                                            //PWM1占空比设置50/100
    SetPWM2Dat(50);                                                            //PWM1占空比设置50/100
    EA = 1;                                                                    //中断总开关
    while(1);
}


