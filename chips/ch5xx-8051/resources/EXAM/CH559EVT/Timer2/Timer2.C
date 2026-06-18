/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer2.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 TIME2接口函数                				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#ifndef TIMER
#define TIMER    0                                                             //T2作为定时器
#define T2EX_CAP 0                                                             //T2ex捕捉引脚电平
#define T2_CAP   1                                                             //T2捕捉引脚电平
#endif

#pragma  NOAREGS

//CH559 Timer2时钟选择   
//bTMR_CLK同时影响Timer0&1&2,使用时要注意                                                       
#define mTimer2ClkFsys( )      {T2MOD |= (bTMR_CLK | bT2_CLK);C_T2=0;}         //定时器,时钟=Fsys
#define mTimer2Clk4DivFsys( )  {T2MOD &= ~bTMR_CLK;T2MOD |=  bT2_CLK;C_T2 = 0;}//定时器,时钟=Fsys/4
#define mTimer2Clk12DivFsys( ) {T2MOD &= ~(bTMR_CLK | bT2_CLK);C_T2 = 0;}      //定时器,时钟=Fsys/12
#define mTimer2CountClk( )     {C_T2 = 1;}                                     //计数器,T2引脚的下降沿有效

//CH559 Timer2 开始(SS=1)/结束(SS=0)
#define mTimer2RunCTL( SS )    {TR2 = SS ? START : STOP;}

UINT8 FLAG;
UINT16 Cap[8] = {0};

/*******************************************************************************
* Function Name  : mTimer2Setup(UINT8 T2Out)
* Description    : CH559定时2初始化
* Input          : UINT8 T2Out,是否允许T2输出时钟
                   0：不允许输出
                   1：允许输出  
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer2Setup(UINT8 T2Out)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 0;                                                                //启动自动重载定时器功能
    if(T2Out)
    {
	      T2MOD |= T2OE;                                                        //是否允许T2输出时钟,如果允许时钟=1/2定时器2溢出率
    }
    else
    {
	      T2MOD &= ~T2OE;
    }
}

/*******************************************************************************
* Function Name  : mTimer2Init(UINT16 Tim)
* Description    : CH559 T2定时器赋初值                   
* Input          : UINT16 Tim,定时器初值
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer2Init(UINT16 Tim)
{
    UINT16 tmp;
    tmp = 65536 - Tim;
    RCAP2L = TL2 = tmp & 0xff;
    RCAP2H = TH2 = (tmp >> 8) & 0xff;
}

/*******************************************************************************
* Function Name  : T2exCaptureSetup(UINT8 mode)
* Description    : CH559定时计数器2 T2EX引脚捕捉功能初始化
                   UINT8 mode,边沿捕捉模式选择
                   0:T2ex从下降沿到下一个下降沿
                   1:T2ex任意边沿之间
                   3:T2ex从上升沿到下一个上升沿
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T2exCaptureSetup(UINT8 mode)
{
    C_T2  = 0;
    EXEN2 = 1; 
    CP_RL2 = 1;                                                                //启动T2ex的捕捉功能
    T2MOD |= mode << 2;                                                        //边沿捕捉模式选择
}

/*******************************************************************************
* Function Name  : T2CaptureSetup(UINT8 mode)
* Description    : CH559定时计数器2 T2引脚捕捉功能初始化T2
                   UINT8 mode,边沿捕捉模式选择
                   0:T2ex从下降沿到下一个下降沿
                   1:T2ex任意边沿之间
                   3:T2ex从上升沿到下一个上升沿
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void T2CaptureSetup(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD &= ~T2OE;                                                            //使能T2引脚捕捉功能
    T2MOD |= (mode << 2) | bT2_CAP1_EN;                                        //边沿捕捉模式选择
}

/*******************************************************************************
* Function Name  : mTimer2Interrupt()
* Description    : CH559定时计数器2定时计数器中断处理函数
*******************************************************************************/
void	mTimer2Interrupt( void ) interrupt INT_NO_TMR2 using 2                //timer2中断服务程序,使用寄存器组1
{
    mTimer2RunCTL( 0 );                                                       //关定时器
#if T2EX_CAP
    if(EXF2)                                                                  //T2ex电平变化中断中断标志
    {
        MOSI1 = !MOSI1;                                                       //P2.1电平指示监控
        Cap[FLAG++] = RCAP2;                                                  //T2EX
        EXF2 = 0;                                                             //清空T2ex捕捉中断标志		
    }
#endif

#if T2_CAP
    if(CAP1F)                                                                  //T2电平捕捉中断标志
    {
        Cap[FLAG++] = T2CAP1;                                                  //T2;	  	
        CAP1F = 0;                                                             //清空T2捕捉中断标志
    }
#endif
	
#if TIMER
    if(TF2)
    {
        TF2 = 0;                                                               //清空定时器2溢出中断	                                                      
        UDTR = !UDTR;                                                          //P0.2电平指示监控
    }
#endif
    mTimer2RunCTL( 1 );                                                        //开定时器
}

main( ) 
{
    UINT8 i;
//  CfgFsys( );                                                                //CH559时钟选择配置   
    mDelaymS(5);                                                               //等待外部晶振稳定	

    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n"); 

    FLAG = 0;
    PORT_CFG |= bP0_OC;                                                        //P0.2设置为弱准双向IO
    P0_DIR |= bUDTR;
    P0_PU |= bUDTR;


    mTimer2ClkFsys( );                                                         //时钟选择Fsys定时器方式
#if TIMER
    mTimer2Setup(0);                                                           //定时器功能演示
    mTimer2Init(0x2000);                                                       //定时器赋初值
    ET2 = 1;                                                                   //使能定时计数器2中断
    EA = 1;                                                                    //使能全局中断
    mTimer2RunCTL( 1 );                                                        //启动定时器
#endif

#if T2EX_CAP
    T2exCaptureSetup(1);                                                       //T2ex引脚捕捉演示
    ET2 = 1;                                                                   //使能定时计数器2中断
    EA = 1;                                                                    //使能全局中断
    mTimer2RunCTL( 1 );                                                        //启动定时器
    T2EX = 0;                                                                  //模拟T2ex引脚电平变化
    mDelayuS(500);
    T2EX = 1;
    mDelayuS(500);
    T2EX = 0;
    mDelayuS(500);
    T2EX = 1;
    mDelaymS(1);                                                                //确保最后一次采到数据
    mTimer2RunCTL( 0 );                                                         //关闭定时器
#endif

#if T2_CAP
    T2CaptureSetup(1);                                                         //T2引脚捕捉演示
    ET2 = 1;                                                                   //使能定时计数器2中断
    EA = 1;                                                                    //使能全局中断
    mTimer2RunCTL( 1 );                                                        //启动定时器
    T2 = 0;	                                                                   //模拟T2引脚电平变化
    mDelayuS(90);
    T2 = 1;
    mDelayuS(200);
    T2 = 0;
    mDelaymS(1);                                                                //确保最后一次采到数据
    mTimer2RunCTL( 0 );                                                         //关闭定时器
#endif

#if T2EX_CAP|T2_CAP                                                             //捕捉数据打印
    EA = 0;                                                                     //使能全局中断
    ET2 = 0;                                                                    //使能定时计数器2中断
    printf("FLAG %02X\n",(UINT16)FLAG);
    for(i=0;i<FLAG;i++)
    {
        printf("%04X  ",(UINT16)Cap[i]);
    }
#endif
    while(1);
}