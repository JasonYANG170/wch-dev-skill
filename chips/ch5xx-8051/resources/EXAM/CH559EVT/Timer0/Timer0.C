/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer0.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 Timer0接口函数             				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS


//CH559 Timer0时钟选择   
//bTMR_CLK同时影响Timer0&1&2,使用时要注意                                                       
#define mTimer0ClkFsys( ) (T2MOD |= bTMR_CLK | bT0_CLK)                     //定时器,时钟=Fsys
#define mTimer0Clk4DivFsys( ) (T2MOD &= ~bTMR_CLK;T2MOD |=  bT0_CLK)        //定时器,时钟=Fsys/4
#define mTimer0Clk12DivFsys( ) (T2MOD &= ~(bTMR_CLK | bT0_CLK))             //定时器,时钟=Fsys/12
#define mTimer0CountClk( ) (TMOD |= bT0_CT)                                 //计数器,T0引脚的下降沿有效

//CH559 Timer0 开始(SS=1)/结束(SS=0)
#define mTimer0RunCTL( SS ) (TR0 = SS ? START : STOP)

/*******************************************************************************
* Function Name  : mTimer0ModSetup(UINT8 mode)
* Description    : CH559定时计数器0模式0设置
* Input          : UINT8 mode,Timer0模式选择
                   0：模式0，13位定时器，TL0的高3位无效
                   1：模式1，16位定时器
                   2：模式2，8位自动重装定时器
                   3：模式3，两个8位定时器
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer0ModSetup(UINT8 mode)
{
    TMOD &= 0xf0;
    TMOD |= mode; 
}

/*******************************************************************************
* Function Name  : mTimer0SetData(UINT16 dat)
* Description    : CH559Timer0 TH0和TL0赋值
* Input          : UINT16 dat;定时器赋值
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer0SetData(UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;	
    TL0 = tmp & 0xff;
    TH0 = (tmp>>8) & 0xff;
}

/*******************************************************************************
* Function Name  : mTimer0Interrupt()
* Description    : CH559定时计数器0定时计数器中断处理函数
*******************************************************************************/
void	mTimer0Interrupt( void ) interrupt INT_NO_TMR0 using 1                //timer0中断服务程序,使用寄存器组1
{                                                                             //方式3时，TH0使用Timer1的中断资源
    CAP1 = !CAP1;
//  mTimer0SetData(0x2000)                                                    //非自动重载方式需重新给TH0和TL0赋值      
}

main( ) 
{
//  CfgFsys( );                                                               //CH559时钟选择配置  
    mDelaymS(5);                                                              //等待外部晶振稳定	
    mInitSTDIO();                                                             //串口0,可以用于调试,默认波特率57600bps
    printf("start ...\n");

    mTimer0ModSetup(2);	                                                      //方式2，自动重载8为定时器
    mTimer0ClkFsys( );                                                        //时钟选择Fsys定时器方式
    mTimer0SetData(0x2323);                                                   //定时器赋初值
    mTimer0RunCTL( 1 );				                                                //启动定时器
//  printf("%02X  %02X",(UINT16)TH0,(UINT16)TL0);
    ET0 = 1;                                                                  //使能定时计数器0中断
    EA = 1;                                                                   //使能全局中断
	    
    while(1);
}