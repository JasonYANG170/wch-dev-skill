/********************************** (C) COPYRIGHT *******************************
* File Name          : Compare.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 ADC比较器
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */	
#include ".\Compare\Compare.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : CMP_Init
* Description    : 比较器模块初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Init( void )
{
    ADC_CTRL = bADC_EN;                                                         /* 开启ADC模块电源 */
    ANA_CTRL |= bCMP_EN;  					                                    /* 开启CMP电源 */
    ANA_CTRL |= bCMP_CHAN_N;   		                                            /* CM端选择P13 */
    ANA_CTRL &= ~bCMP_CHAN_P;   		                                        /* CP端选择由ADC_CHAN选择的通道4 */
    
    P1_MOD_OC &= ~0x18;                                                         /* P1.3 P1.4高阻输入 */
    P1_DIR_PU &= ~0x18;
		
    ADC_CHAN = (1<<4) | (1<<11);							                    /* 正、负输入端通道分别为 ch4,ch3 */
    ANA_CTRL |= bCMP_CHAN_X;   		                                            /* CO使能，输出到P12 */
    ADC_CTRL |= bADC_IF;                                                        /* 清除比较标志，写1清零 */
#if EN_ADC_INT
    ANA_CTRL |= bCMP_IE;										                /* CMP结果改变中断使能 */
    PWM_CTRL &=~ bPWM_IE_END;                                                   /* 和PWM中断地址共用，故中断需2选1 */
    IE_CMP = 1;                                                                 /* 开启CMP中断使能 */
    EA = 1;                                                                     /* 开启总中断使能 */
#endif
		
}

#if EN_ADC_INT
/*******************************************************************************
* Function Name  : CMP_ISR( )
* Description    : CMP 中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_ISR( void )  interrupt INT_NO_CMP
{
	printf("ANA_CTRL=%x\n",(UINT16)ANA_CTRL);
	printf("INT CMP! CMP changes\n");
}
#endif
