/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : ADC初始化，ADC中断和查询方式采集演示示例
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                              
#include "Debug.H"
#include "ADC.H"

#pragma  NOAREGS

/* Global Variable */ 
UINT8 i;

#define ADC_INTERRUPT   0           //0:查询方式,1:中断方式

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void) 
{
	mDelaymS(50);
	CfgFsys();                                                                //CH555时钟选择配置       
	mInitSTDIO();                                                             //串口0初始化
	printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
	printf("EXAM ADC...\n");

	i = 1;
	ADCInit(2);

#if ADC_INTERRUPT    
	ADC_ChannelSelect(i);
	EA = 1;
	ADC_START();
	
#endif  
  
	while(1){
#if ADC_INTERRUPT == 0
		for(i=0; i<16;i++){
			ADC_ChannelSelect(i); 
			ADC_START();   
			while(ADC_CTRL & bADC_START);
			printf("Channel:%02d %04d\n",(UINT16)i,ADC_DAT);
			mDelaymS(500);
		}
		
#endif		
	}
}

#if ADC_INTERRUPT
/*******************************************************************************
* Function Name  : ADCInterrupt
* Description    : ADC 中断服务程序
* Input          : None
* Return         : None
*******************************************************************************/
void ADCInterrupt(void) interrupt INT_NO_ADC using 1                       //ADC中断服务程序,使用寄存器组1
{  
	ADCIF_CLEAR();                                                               //清空ADC中断标志

	printf("Channel:%02d %04d\n",(UINT16)i++,ADC_DAT);
    if(i==16) i=0;
	ADC_ChannelSelect(i); 
	ADC_START();
}

#endif
