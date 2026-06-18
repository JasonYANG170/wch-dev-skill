/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer1.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 TIME1接口函数                				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

//CH559 Timer1时钟选择   
//bTMR_CLK同时影响Timer0&1&2,使用时要注意                                                       
#define mTimer1ClkFsys( ) (T2MOD |= bTMR_CLK | bT1_CLK)                     //定时器,时钟=Fsys
#define mTimer1Clk4DivFsys( ) (T2MOD &= ~bTMR_CLK;T2MOD |=  bT1_CLK)        //定时器,时钟=Fsys/4
#define mTimer1Clk12DivFsys( ) (T2MOD &= ~(bTMR_CLK | bT1_CLK))             //定时器,时钟=Fsys/12
#define mTimer1CountClk( ) (TMOD |= bT1_CT)                                 //计数器,T1引脚的下降沿有效

//CH559 Timer1 开始(SS=1)/结束(SS=0)
#define mTimer1RunCTL( SS ) (TR1 = SS ? START : STOP)

/*******************************************************************************
* Function Name  : mTimer1ModSetup(UINT8 mode)
* Description    : CH559定时计数器1模式设置
* Input          : UINT8 mode,Timer1模式选择
                   0：模式0，13位定时器，TL1的高3位无效
                   1：模式1，16位定时器
                   2：模式2，8位自动重装定时器
                   3：模式3，停止Timer1
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer1ModSetup(UINT8 mode)
{
    TMOD &= 0x0f;
    TMOD |= mode << 4; 
}

/*******************************************************************************
* Function Name  : mTimer1SetData(UINT16 dat)
* Description    : CH559Timer1 TH1和TL1赋值
* Input          : UINT16 dat;定时器赋值
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer1SetData(UINT16 dat)
{
  UINT16 tmp;
  tmp = 65536 - dat;
	TL1 = tmp & 0xff;
	TH1 = (tmp>>8) & 0xff;
}

/*******************************************************************************
* Function Name  : mTimer1Interrupt()
* Description    : CH559定时计数器1定时计数器中断处理函数
*******************************************************************************/
void	mTimer1Interrupt( void ) interrupt INT_NO_TMR1 using 1                   //timer1中断服务程序,使用寄存器组1
{                                                                              //方式3时，停止Timer1
    RXD_ = !RXD_;
//	mTimer1SetData(0x3737);                                                    //非自动重载方式需重新给TH1和TL1赋值        
}

main( ) 
{
//  CfgFsys( );                                                                //CH559时钟选择配置 
    mDelaymS(5);                                                               //等待外部晶振稳定
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");

    mTimer1Clk12DivFsys( );                                                    //时钟选择Fsys定时器方式
    mTimer1ModSetup(2);	                                                       //方式2，自动重载8为定时器                                                      
    mTimer1SetData(0x8080);                                                    //定时器赋初值
    mTimer1RunCTL(1);                                                          //启动定时器
    ET1 = 1;                                                                   //使能定时计数器1中断
    EA = 1;                                                                    //使能全局中断
	    
    while(1);
}

