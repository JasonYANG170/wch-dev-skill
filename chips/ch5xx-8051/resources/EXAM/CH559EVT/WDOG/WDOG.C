/********************************** (C) COPYRIGHT *******************************
* File Name          : WDOG.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 WatchDOG DEMO
                     (1)、提供看门狗初始化接口函数     
                     (2)、看门定时周期计算公式 262144*(0x100 - WDOG_COUNT)/Fsys          				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : WDOGInit(UINT8 n,UINT8 value)
* Description    : 看门狗中断初始化
* Input          : UINT8 n,计时时长
                   UINT8 value,选择看门狗计时完成后操作
                   value=1芯片复位;
                   value=0产生看门狗中断;
* Output         : None
* Return         : None   
*******************************************************************************/
void WDOGInit(UINT8 n,UINT8 value)
{	
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;                                                          //开启安全模式
    GLOBAL_CFG |= value;    
    SAFE_MOD = 0xFF;                                                          //关闭安全模式
    
    WDOG_COUNT = n;                                                           //看门狗超时时间
    if(!value)
    {
        IE_WDOG = 1;                                                          //使能看门狗中断
        EA = 1;                                                               //开总中断
    }
}

void CheckResetValue()
{
    printf("PCON %02X\n",(UINT16)PCON);
    printf("PLL_CFG %02X\n",(UINT16)PLL_CFG);
    printf("CLOCK_CFG %02X\n",(UINT16)CLOCK_CFG);
    printf("SLEEP_CTRL %02X\n",(UINT16)SLEEP_CTRL);
    printf("WAKE_CTRL %02X\n",(UINT16)WAKE_CTRL);	
    printf("RESET_KEEP %02X\n",(UINT16)RESET_KEEP);
    printf("WDOG_COUNT %02X\n",(UINT16)WDOG_COUNT);
    printf("PLL_CFG %02X\n",(UINT16)PLL_CFG);
    printf("CLOCK_CFG %02X\n",(UINT16)CLOCK_CFG);
}

/*******************************************************************************
* Function Name  : WDogInterrupt(void)
* Description    : 看门狗中断服务程序 
*******************************************************************************/
void	WDogInterrupt( void ) interrupt INT_NO_WDOG using 1                    //看门狗中断服务程序,使用寄存器组1
{  	
    printf("PCON  %02X\n",(UINT16)PCON);
    WDOG_COUNT = 0x20;
    P4_OUT = ~P4_OUT;
}

main( ) 
{
//  CfgFsys( );     
    mDelaymS(5);                                                               //等待外部晶振稳定			
    P4_DIR |= 0x0f;                                                            //使能P40-P44输出           
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n"); 
	
    CheckResetValue( ); 
    WDOGInit(0x23,0);
    while(1);
}