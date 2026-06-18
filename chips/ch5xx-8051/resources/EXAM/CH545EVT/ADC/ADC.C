/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH545 ADC采样时钟设置，ADC通道设置函数，电压比较模式设置 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "ADC.H"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : ADCInit
* Description    : ADC采样时钟设置,模块开启，中断开启
* Input          : div：参考时钟设置 
*                  3-64个Fosc
*                  2-128个Fosc
*                  1-256个Fosc                   								
*                  0-512个Fosc									 
* Return         : None
*******************************************************************************/
void ADCInit(UINT8 div)
{
	ADC_CTRL &= ~MASK_ADC_CLK;
	ADC_CTRL |= bADC_EN | (div & MASK_ADC_CLK);                  //ADC电源使能
	#if ADC_INTERRUPT
	ADCIF_CLEAR();                                               //清ADC中断
	IE_ADC = 1;                                                  //使能ADC中断
	
#endif
}

/*******************************************************************************
* Function Name  : ADC_ChannelSelect
* Description    : ADC采样启用
* Input          : ch：采用通道
* Return         : 成功 SUCCESS
*                  失败 FAIL
*******************************************************************************/
UINT8 ADC_ChannelSelect(UINT8 ch)
{
	ADC_CHAN &= 0xf0;

	if(ch >= ADC_Channel0 && ch <= ADC_Channel7){                       //AIN0-AIN7,高阻输入   
		P1_MOD_OC = P1_DIR_PU &= ~(1<<ch);       
	}
	else if(ch>=ADC_Channel8 && ch <=ADC_Channel13){                   //AIN8-AIN13,高阻输入      
		P0_MOD_OC = P0_DIR_PU &= ~(1<<(ch-8));       
	}
	else if(ch == ADC_ChannelV33 || ch == ADC_ChannelV18){             //内部
			
	} 
	else return FAIL;

	ADC_CHAN |= ch;
	 
	return SUCCESS;
}

/*******************************************************************************
* Function Name  :  VoltageCMPModeInit
* Description    :  电压比较器模式初始化
* Input          :  fo：正向端口 
*                    0-VDD 
*                    1-ADC_CHAN
*                   re：反向输入端
*                    000-关闭比较器
*                    001-内部1.2V
*                    010-3.3V
*                    011-5V
*                    100-5.4V
*                    101-5.8V
*                    110-6.2V
*                    111-6.6V
* Return         : 成功 SUCCESS
*                  失败 FAIL
*******************************************************************************/
void VoltageCMPModeInit(UINT8 fo, UINT8 re)
{
	CMP_DCDC &= 0xf0;

	if(fo)  
		CMP_DCDC |= bCMP_PIN;
	if(re)  
		CMP_DCDC |= MASK_CMP_VREF & re;   
    
}