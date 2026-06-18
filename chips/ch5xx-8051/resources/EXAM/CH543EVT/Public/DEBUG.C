/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/1/5
* Description        : CH5XX DEBUG Interface
                     (1)、主频设置;
                     (2)、us\ms基本延时函数;										 
                     (3)、串口0输出打印信息,波特率可调;            				   
                     (4)、看门狗初始化和赋值函数;    										 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"

/********************************************************************************
* Function Name  : CfgFsys( void )
* Description    : CH5XX时钟选择和配置函数,默认使用内部晶振48MHz,如果定义了FREQ_SYS可以
                   根据CLOCK_CFG配置得到
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/ 
void CfgFsys( void )  
{
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
	
#if FREQ_SYS == 48000000
	POWER_CFG |= bLDO_CORE_VOL;	
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x07;							/* 48MHz	*/
#endif	
#if FREQ_SYS == 24000000
#if	WAKEUP_FAST
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x05;							/* 24MHz,fast wakeup */
#else
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x06;							/* 24MHz,slow wakeup */
#endif
#endif
#if FREQ_SYS == 16000000		
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x04;							/* 16MHz */
#endif	
#if FREQ_SYS == 12000000		
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x03;  							/* 12MHz */	
#endif	
#if FREQ_SYS == 3000000	
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x02;  							/* 3MHz	*/	
#endif
#if FREQ_SYS == 375000	
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x01;  							/* 375KHz */		
#endif
#if FREQ_SYS == 5000	
    CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x00;  							/* ~5KHz */	
#endif
    SAFE_MOD = 0x00;
}

/********************************************************************************
* Function Name  : mDelayus( UNIT16 n )
* Description    : us延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
********************************************************************************/ 
void mDelayuS( UINT16 n )
{
	while( n ) 
	{  
		/* total = 12~13 Fsys cycles, 1uS @Fsys=12MHz */
		
		/* 2 Fsys cycles, for higher Fsys, add operation here */
		++ SAFE_MOD;  			
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 34000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 36000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 38000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 40000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 42000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 44000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 46000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 48000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 50000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 52000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 54000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 56000000
		++ SAFE_MOD;
#endif
#endif
		-- n;
	}
}

/********************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
********************************************************************************/
void mDelaymS( UINT16 n )                                                  
{
	while ( n ) 
	{
		mDelayuS( 1000 );
		-- n;
	}
}    

/********************************************************************************
* Function Name  : CH543UART0Alter( void )
* Description    : CH543串口0引脚映射，串口映射到P3.0(R)和P3.1(T)
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void CH543UART0Alter( void )
{
	P3_MOD_OC |= 0xC0;															/* 准双向模式 */
	P3_DIR_PU |= 0xC0;
	PIN_FUNC &=~ 0x18;                                 			             	/* 开启引脚复用功能 */
}

/********************************************************************************
* Function Name  : mInitSTDIO( void )
* Description    : CH543串口0初始化,默认使用T1作UART0的波特率发生器,也可以使用T2
                   作为波特率发生器
* Input          : None
* Output         : None+
* Return         : None
********************************************************************************/
void mInitSTDIO( void )
{
    UINT32 x;
    UINT8  x2; 

    SM0 = 0;
    SM1 = 1;
    SM2 = 0;																	/* 串口0使用模式1 */
	
    RCLK = 0;																	/* UART0接收时钟 */
    TCLK = 0;                                                                  	/* UART0发送时钟 */
    PCON |= SMOD;																/* UART0模式 1,2,3: 快速模式 */
    x = 10 * FREQ_SYS / UART0BUAD / 16;                                        	/* 如果更改主频,注意x的值不要溢出 */                            
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                       	/* 四舍五入 */

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;				/* 0X20,Timer1作为8位自动重载定时器 */
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;											/* Timer1时钟选择 */
    TH1 = 0 - x;                                                                 
    TR1 = 1;                                                                   	/* 启动定时器1 */
    TI = 1;
    REN = 1;                                                                   	/* 串口0接收使能 */
}

/********************************************************************************
 * Function Name  : CH543SoftReset( void )
 * Description    : CH543软复位
 * Input          : None
 * Output         : None
 * Return         : None
********************************************************************************/
void CH543SoftReset( void )
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bSW_RESET;
}

/********************************************************************************
* Function Name  : CH543WDTModeSelect( UINT8 mode )
* Description    : CH543看门狗模式选择
*                  8位计数器,溢出周期(秒): (131072/FREQ_SYS) * (256-WDOG_COUNT)
* Input          : UINT8 mode 
                   0  timer
                   1  watchDog
* Output         : None
* Return         : None
********************************************************************************/
void CH543WDTModeSelect( UINT8 mode )
{
	SAFE_MOD = 0x55;															/* 进入安全模式 */
	SAFE_MOD = 0xaa; 
	
	if( mode )
	{
		GLOBAL_CFG |= bWDOG_EN;													/* 启动看门狗复位 */
	}
	else
	{
		GLOBAL_CFG &= ~bWDOG_EN;	                                            /* 启动看门狗仅仅作为定时器 */
	}		
		
	SAFE_MOD = 0x00;                                                            /* 退出安全模式 */
	WDOG_COUNT = 0;																/* 看门狗赋初值 */
}

/********************************************************************************
* Function Name  : CH543WDTFeed( UINT8 tim )
* Description    : CH543看门狗定时时间设置
* Input          : UINT8 tim 看门狗复位时间设置
                   00H(12MHz)=2.8s     00H(24MHz)=1.4s
                   80H(12MHz)=1.4s     80H(24MHz)=0.7s
* Output         : None
* Return         : None
********************************************************************************/
void CH543WDTFeed( UINT8 tim )
{
   WDOG_COUNT = tim;															/* 看门狗计数器赋值 */
}

