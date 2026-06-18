/********************************** (C) COPYRIGHT *******************************
* File Name          : TIMER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 Time 初始化、定时器、计数器赋值、T2捕捉功能开启函数等
                       定时器中断函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include "Timer.H"
#pragma  NOAREGS

/********************************************************************************
* Function Name  : mTimer_x_ModInit
* Description    : CH543定时计数器选择和模式设置
* Input          : x: 选择定时计数器
*                  	0-TIM0
*                   1-TIM1
*                   2-TIM2
*                  mode: Timer模式选择
*                   0-模式0,13位定时器,TLn的高3位无效
*                   1-模式1,16位定时器
*                   2-模式2,8位自动重装定时器
*                   3-模式3,Timer0分成两个8位定时器,Timer1停止
* Output         : None
* Return         : 成功  SUCCESS
*                  失败  FAIL
********************************************************************************/
UINT8 mTimer_x_ModInit( UINT8 x, UINT8 mode )
{
    if( x == 0 )
    {
        TMOD = TMOD & 0xf0 | mode;
    }
    else if( x == 1 )
    {
        TMOD = TMOD & 0x0f | ( mode << 4 );
    }
    else if( x == 2 )
    {
        RCLK = 0;    															/* 16位自动重载定时器 */
        TCLK = 0;
        CP_RL2 = 0;
    }
    else
    {
        return FAIL;
    }
    
    return SUCCESS;
}

/********************************************************************************
* Function Name  : mTimer_x_SetData
* Description    : CH543Timer 计数器赋值
* Input          : x: 选择定时计数器
*                   0-TIM0
*                   1-TIM1
*                   2-TIM2
*                  dat: 计数器赋值
* Output         : None
* Return         : None
********************************************************************************/
void mTimer_x_SetData( UINT8 x, UINT16 dat )
{
    UINT16 tmp;
    
    tmp = 65536 - dat;
    if( x == 0 )
    {
        TL0 = tmp & 0xff;
        TH0 = ( tmp >> 8 ) & 0xff;
    }
    else if( x == 1 )
    {
        TL1 = tmp & 0xff;
        TH1 = ( tmp >> 8 ) & 0xff;
    }
    else if( x == 2 )
    {
        RCAP2L = TL2 = tmp & 0xff;												/* 16位自动重载定时器 */
        RCAP2H = TH2 = ( tmp >> 8 ) & 0xff;
    }
}

/********************************************************************************
* Function Name  : CAP2Init
* Description    : CH543定时计数器2 T2EX引脚捕捉功能初始化(CAP2 P11)
* Input          : mode: 边沿捕捉模式选择
*                   0或2: T2ex从下降沿到下一个下降沿
*                   1: T2ex任意边沿之间
*                   3: T2ex从上升沿到下一个上升沿
* Output         : None
* Return         : None
********************************************************************************/
void CAP2Init( UINT8 mode )
{
    RCLK = 0;
    TCLK = 0;
    C_T2  = 0;
    EXEN2 = 1;
    CP_RL2 = 1;                                                                	/* 启动T2ex的捕捉功能 */
    T2MOD |= mode << 2;                                                        	/* 边沿捕捉模式选择 */
}

/********************************************************************************
* Function Name  : CAP1Init
* Description    : CH543定时计数器2 T2引脚捕捉功能初始化T2(CAP1 P10)
* Input          : mode,边沿捕捉模式选择
*                   0或2: T2ex从下降沿到下一个下降沿
*                   1: T2ex任意边沿之间
*                   3: T2ex从上升沿到下一个上升沿
* Output         : None
* Return         : None
********************************************************************************/
void CAP1Init( UINT8 mode )
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    EXEN2 = 1;
    C_T2 = 0;
    T2MOD = T2MOD & ~T2OE | ( mode << 2 ) | bT2_CAP1_EN;						/* 使能T2引脚捕捉功能,边沿捕捉模式选择 */
}

