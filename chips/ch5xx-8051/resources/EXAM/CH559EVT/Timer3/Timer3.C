/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer3.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 TIME3接口函数                				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\DEBUG.C"                                                          //调试信息打印
#include ".\DEBUG.H"

#pragma  NOAREGS

UINT8V FLAG;
UINT16 Cap[8];

#define TIMER3    0
#define T3PWM3    0
#define T3CAP3    1

#define mTimer3Stop( )     {T3_CTRL &= ~bT3_CNT_EN;}                               //关闭定时器3
#define mTimer3Start( )    {T3_CTRL |= bT3_CNT_EN;}                                //开始定时器/计数器3
#define mTimer3Init( dat ) {T3_END_L = dat & 0xff;T3_END_H = (dat >> 8) & 0xff;}   //T3 定时器赋初值

/*******************************************************************************
* Function Name  : mSetTimer3Clk(UINT16 DIV)
* Description    : CH559 Timer3时钟设置
* Input          : UINT16 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void mSetTimer3Clk(UINT16 DIV)
{
    T3_SETUP |= bT3_EN_CK_SE; 
    T3_CK_SE_L = DIV & 0xff;
    T3_CK_SE_H = (DIV >> 8) & 0xff;
    T3_SETUP &= ~bT3_EN_CK_SE;  
}

/*******************************************************************************
* Function Name  : void mTimer3Setup( )
* Description    : CH559定时计数器3初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer3Setup( )
{
    T3_CTRL |= bT3_CLR_ALL;                                                    //清空FIFO和计数
    T3_CTRL &= ~bT3_CLR_ALL;
    T3_SETUP |= bT3_IE_END;                                                    //使能定时器/计数器3FIFO溢出中断
    T3_CTRL &= ~bT3_MOD_CAP;                                                   //设置为工作在定时器模式
    T3_STAT |= bT3_IF_DMA_END | bT3_IF_FIFO_OV | bT3_IF_FIFO_REQ | bT3_IF_ACT | bT3_IF_END;//清空寄存器所有中断
}

/*******************************************************************************
* Function Name  : mTimer3PWMSetup( )
* Description    : CH559定时计数器PWM3初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer3PWMSetup( )
{
    PORT_CFG &= ~bP1_DRV;
    P1_DIR |= bPWM3;                                                           //设置PWM3引脚为推挽模式
    T3_CTRL |= bT3_CLR_ALL;                                                    //清空FIFO和计数
    T3_CTRL &= ~bT3_CLR_ALL;
    T3_SETUP |= bT3_IE_END | bT3_IE_ACT;
    T3_CTRL |= bT3_OUT_EN;
}

/*******************************************************************************
* Function Name  : mTimer3PWMPolarSel(UINT8 mode)
* Description    : 
* Input          : UINT8 mode,PWM有效电平输出选择
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer3PWMPolarSel(UINT8 mode)
{
    if(mode)
    {
        T3_CTRL |= bT3_PWM_POLAR;                                             //设置PWM 输出极性，低电平
    }
    else
    {
        T3_CTRL &= ~bT3_PWM_POLAR;                                            //设置PWM 输出极性，高电平
    }	
}
/*******************************************************************************
* Function Name  : mTimer3PWMDatInit(UINT16 dat0,UINT16 dat1)
* Description    : 
* Input          : UINT16 dat0,PWM周期时间
                   UINT16 dat1,PWM有效电平输出时间
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer3PWMDatInit(UINT16 dat0,UINT16 dat1)
{
    T3_END_L = dat0 & 0xff;		                                                 //设置占空比
    T3_END_H = (dat0 >> 8) & 0xff;
    T3_FIFO_L = dat1 & 0xff;
    T3_FIFO_H = (dat1 >> 8) & 0xff;
}

/*******************************************************************************
* Function Name  : mTimer3CaptureSetup(UINT16 dat0,UINT8 mode)
* Description    : CH559定时计数器3捕捉功能初始化CAP3
* Input          : UINT16 dat0,捕获超时时间设定
				           UINT8 mode,捕获模式选择
				           0：关闭捕获
				           1：从任意沿到任意沿之间的捕获
				           2：从下降沿到下降沿之间的捕获
				           3：从上升沿到上升沿之间的捕获
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer3CaptureSetup(UINT16 dat0,UINT8 mode)
{
    T3_CTRL = 0;
    T3_CTRL |= bT3_CLR_ALL;                                                   //清空T3相关寄存器
    T3_CTRL &= ~bT3_CLR_ALL;

    T3_SETUP |=  bT3_IE_ACT;
    T3_CTRL |= bT3_MOD_CAP | bT3_CAP_WIDTH;
    T3_CTRL |= mode << 6;
    T3_END_L = dat0 & 0xff;                                                    //捕获的最大超时时间，超时时间time = Pclk*max_time, Pclk为系统时钟
    T3_END_H = (dat0 >> 8) & 0xff;
    T3_FIFO_L = 0;                                                             //清空
    T3_FIFO_H = 0;
    T3_STAT = 0xF0;                                                            //清中断
}

/*******************************************************************************
* Function Name  : mTimer3Interrupt()
* Description    : CH559定时计数器3定时计数器中断处理函数
*******************************************************************************/
void	mTimer3Interrupt( void ) interrupt INT_NO_TMR3 using 2                 //timer3中断服务程序,使用寄存器组1
{
  mTimer3Stop( );
#if TIMER3
    if(T3_STAT & bT3_IF_END)                                                   //计数中断
    { 
        T3_STAT |= bT3_IF_END;
    }
#endif

#if T3CAP3	
    if(T3_STAT & bT3_IF_ACT)                                                   //捕捉中断
    {
        printf("Cap:%04X  ",(UINT16)T3_FIFO);        
//         Cap[FLAG++] = T3_FIFO;
        T3_STAT |= bT3_IF_ACT;
    }
#endif

//	 if(T3_STAT & bT3_IF_DMA_END)
//   	{
//	    T3_STAT |= bT3_IF_DMA_END;
//	  }
mTimer3Start();
}

main( ) 
{
    UINT8 i,tmp;
//  CfgFsys( );   
    mDelaymS(5);                                                               //等待外部晶振稳定		
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");
	
    mSetTimer3Clk(0x02);                                                       //T3时钟配置	
/* 计数定时器 */
#if TIMER3          
    mTimer3Setup( );                                                           //定时器基本设置  
    mTimer3Init( 0x2000 );                                                     //T3 定时器赋初值
    mTimer3Start( );                                                           //开始定时器/计数器3
    IE_TMR3 = 1;                                              	
    EA = 1; 
    while(1);
#endif
	
/* PWM3演示 */
#if T3PWM3			
    PORT_CFG &= ~bP1_OC;                                                        //P1.2设置PWM口为推挽输出模式
    P1_DIR |= bPWM3;
    P1_PU |= bPWM3;
    mTimer3PWMSetup( );                                                         //PWM3基本配置 
    mTimer3PWMPolarSel(1);                                                      //PWM3输出高低极性选择
    mTimer3PWMDatInit(12,3);                                                    //PWM输出占空比设置
    mTimer3Start( );
    IE_TMR3 = 1;                                       
    EA = 1;
    while(1);
#endif
 
 /* CAP3演示 */
#if T3CAP3	
    PIN_FUNC |= bTMR3_PIN_X;
    PORT_CFG |= bP1_OC;                                                         //CAP3口为准双向模式模式
    P1_DIR |= bCAP3;
    P1_PU |= bCAP3;
	
    FLAG = 0;
    mTimer3CaptureSetup(0x4000,1);				                                      //CAP3捕获功能
    mTimer3Start( );                                                            //开始定时器/计数器3
    EA = 1;
    IE_TMR3 = 1;	
		while(1);
	
    for(i=0; i<6; i++)
    {
        mDelayuS(300);
        CAP3 = !CAP3;													                                   //模拟CAP3引脚电平变化	
    }
	
    mTimer3Stop();
    IE_TMR3 = 0;
    EA = 1;
	
    printf("FLAG:%02X  \n",(UINT16)FLAG);
    for(i = 0;i < FLAG;i++)
    {
        printf("Cap[%02X]:%04X  ",(UINT16)i,(UINT16)Cap[i]);
    }
    printf("\n");
    tmp = T3_STAT&MASK_T3_FIFO_CNT;	
    for(i = 0;i < tmp;i++)
    {
        printf("%04X  ",T3_FIFO);
    }
    printf("T3_STAT:%02X  \n",(UINT16)T3_STAT);	
#endif
    while(1);
}

/* 
1.当前TIME3定时/计数功能是从0计数到T3_END的时间，然后进入中断；
2.PWM模式下，占空比=T3_FIFO/T3_END
3.捕捉模式，当前FIFO中的数值就是所要捕捉边沿时间；
*/