/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/01/13
* Description        : CH543 Time 初始化、定时器、计数器赋值，T2捕捉功能等                        
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include "Timer.H"
#include "Debug.H"
#pragma  NOAREGS

/*******************************************************************************/
/* 硬件引脚定义 */ 
sbit SCK_0 = P1^5;                                                             	/* Timer中断监视,引脚P15输出脉冲 */
sbit SCK_1 = P1^6;                                                             	/* 捕捉信号CAP1监视,引脚P16输出脉冲 */
sbit SCK_2 = P1^7;                                                             	/* 捕捉信号CAP2监视,引脚P17输出脉冲 */

/*******************************************************************************/
/* 变量定义 */
UINT16 Cap2[ 2 ] = { 0 };
UINT16 Cap1[ 2 ] = { 0 };

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void ) 
{
    mDelaymS( 50 );
    CfgFsys( );																	/* CH543时钟选择配置 */      
    mInitSTDIO( );                                                              /* 串口0初始化 */
    mDelaymS( 50 );
    printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
    printf("EXAM Time\n");
    
    P1_MOD_OC &=~ 0xE0;                                                   		/* 推挽输出模式 */
    P1_DIR_PU |= 0xE0;
    
    SCK_0 = 0;
    SCK_1 = 0;
    SCK_2 = 0;
    
#if Timer0_INTERRUPT
    printf("T0 Test ...\n");
    mTimer0Clk12DivFsys( );														/* T0定时器时钟设置 FREQ_SYS/12 */
    mTimer_x_ModInit( 0, 1 );                                                   /* T0 定时器模式设置 模式1 16位定时器 */
    mTimer_x_SetData( 0, 2000 );                                                /* T0定时器赋值 24MHZ 1MS中断 */
    TR0 = 1;                                                                    /* T0定时器启动 */
    ET0 = 1;                                                                   	/* T0定时器中断开启 */
    EA  = 1;
    
#endif	
    
#if Timer1_INTERRUPT
    printf("T1 Test ...\n");
    mTimer1Clk12DivFsys( );                                                     /* T1定时器时钟设置 FREQ_SYS/12 */
    mTimer_x_ModInit( 1, 1 );                                                   /* T1 定时器模式设置 模式1 16位定时器 */
    mTimer_x_SetData( 1, 2000 );                                                /* T1定时器赋值 24MHZ 1MS中断 */
    TR1 = 1;                                                                   	/* T1定时器启动 */
    ET1 = 1;                                                                   	/* T1定时器中断开启 */
    EA  = 1;
    
#endif	

#if Timer2_INTERRUPT
    printf("T2 Test ...\n");
    mTimer2Clk12DivFsys( );                                                     /* T2定时器时钟设置 FREQ_SYS/12 */
    mTimer_x_ModInit( 2, 0 );                                                   /* T2 定时器模式设置 */
    mTimer_x_SetData( 2, 4000 );                                                /* T2定时器赋值 FREQ_SYS=24MHz,2ms中断 */
    TR2 = 1;                                                                   	/* T2定时器启动 */
    ET2 = 1;                                                                   	/* T2定时器中断开启 */
    EA  = 1;	
    
#endif	

#if T2_CAP
    printf("T2_CAP Test ...\n");
    P1_MOD_OC &= ~( 1 << 1 );                                                  	/* CAP2 浮空输入 */
    P1_DIR_PU &= ~( 1 << 1 );
    P1_MOD_OC &= ~( 1 << 0 );                                                   /* CAP1 浮空输入 */
    P1_DIR_PU &= ~( 1 << 0 );
//	P1_MOD_OC &= ~( 1 << 6 );                                                   /* CAP2 浮空输入 */
//	P1_DIR_PU &= ~( 1 << 6 );
//	P1_MOD_OC &= ~( 1 << 7 );                                                   /* CAP2 浮空输入 */
//	P1_DIR_PU &= ~( 1 << 7 );
    mTimer2Clk12DivFsys( );                                                     /* T2定时器时钟设置 FREQ_SYS/12 */
    mTimer_x_SetData( 2, 0 );                                                   /* T2 定时器模式设置捕捉模式 */
    CAP2Init( 1 );                                                              /* T2 CAP2(P11)设置,任意沿捕捉 */
    CAP1Init( 1 );                                                              /* T2 CAP1(P10)设置,任意沿捕捉 */
    TR2 = 1;                                                                   	/* T2定时器启动 */
    ET2 = 1;                                                                   	/* T2定时器中断开启 */
    EA  = 1;
        
#endif

    while( 1 )
	{ 
        /*********************************************************************/
		/* 主循环处理 */
		
    }
}

#if Timer0_INTERRUPT
/********************************************************************************
* Function Name  : Timer0Interrupt( void )
* Description    : Timer0中断服务程序,延时1ms
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void Timer0Interrupt( void ) interrupt INT_NO_TMR0                      
{  
    SCK_0 = ~SCK_0;
    mTimer_x_SetData( 0, 2000 );
}

#endif

#if Timer1_INTERRUPT
/********************************************************************************
* Function Name  : Timer1Interrupt
* Description    : Timer1中断服务程序,延时1ms
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void Timer1Interrupt( void ) interrupt INT_NO_TMR1                       
{  
    SCK_0 = ~SCK_0;
    mTimer_x_SetData( 1, 2000 );
}
#endif

/********************************************************************************
* Function Name  : Timer2Interrupt
* Description    : Timer2中断服务程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/	
void Timer2Interrupt( void ) interrupt INT_NO_TMR2                      
{
#if Timer2_INTERRUPT
    static UINT16 tmr2 = 0;
 
    if( TF2 )
    {
        TF2 = 0;																/* 清空定时器2溢出中断,需手动清 */
        tmr2++;
        
        if( tmr2 == 10 )                                                       	/* 延时20ms */                                                   
        {
            tmr2 = 0;
            SCK_0 = ~SCK_0;
        }
    }
	
#endif

#if  T2_CAP
    if( EXF2 )                                                                	/* T2ex电平变化中断中断标志 */
    {
        Cap2[ 0 ] = RCAP2;                                                    	/* T2EX */
        printf("CAP2 %04x\n",Cap2[ 0 ] - Cap2[ 1 ]);
        SCK_2 = ~SCK_2;
        Cap2[ 1 ] = Cap2[ 0 ];
        EXF2 = 0;                                                           	/* 清空T2ex捕捉中断标志 */
    }
	
    if( CAP1F )                                                               	/* T2电平捕捉中断标志 */
    {
        Cap1[ 0 ] = T2CAP1;                                                    	/* T2 */
        printf("CAP1 %04x\n",Cap1[ 0 ] - Cap1[ 1 ]);
        SCK_1 = ~SCK_1;
        Cap1[ 1 ] = Cap1[ 0 ];
        CAP1F = 0;                                                          	/* 清空T2捕捉中断标志 */
    }
	
#endif
}