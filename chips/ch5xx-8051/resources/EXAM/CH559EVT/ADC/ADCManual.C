
/********************************** (C) COPYRIGHT *******************************
* File Name          : ADCManual.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/6/24
* Description        : CH559的手动采样ADC操作，支持通道切换
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

UINT8 ADCChannel[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};               //ADC通道1-8

// #define _INT_  1                                                            //ADC中断方式

#pragma  NOAREGS


/*******************************************************************************
* Function Name  : InitADCManual()
* Description    : ADC手动采用初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitADCManual()
{
    P1_IE = 0x00;                                                              //关闭P1口其他数据功能,如果只用到部分采样通道，其余置1，否则影响IO功能
    ADC_SETUP |= bADC_POWER_EN;                                                //ADC电源使能
    ADC_CK_SE = 0x02;                                                          //设置分频
    ADC_CTRL &= ~MASK_ADC_CYCLE;                                               //选择手动采样
    ADC_CTRL &= ~(bADC_CHANN_MOD1 | bADC_CHANN_MOD0);                          //手动选择通道
    ADC_CHANN = ADCChannel[0];                                                 //选通通道1
    ADC_EX_SW |= bADC_RESOLUTION;                                              //采样位数11bit
//	ADC_EX_SW &= ~bADC_RESOLUTION;                                             //采样位数10bit
    mDelayuS(100);                                                             //确保ADC正常启动
}

/*******************************************************************************
* Function Name  : ADCChanelChange()
* Description    : ADC通道切换
* Input          : UINT8 Chanel
* Output         : None
* Return         : UINT16 ADCValue
*******************************************************************************/
UINT16 ADCChanelChange(UINT8 Chanel)
{
  UINT16 ADCValue = 0;
	ADC_CHANN = Chanel;                                                          //切换ADC通道
// 	ADC_EX_SW |= bADC_RESOLUTION;                                              //ADC采集位数
	mDelayuS(10);                                                                //可选，等待通道切换成功
	ADC_CTRL |= bADC_SAMPLE;                                                     //手动产生采样脉冲
	mDelayuS(5);
	ADC_CTRL &= ~bADC_SAMPLE;	
	while((ADC_STAT & bADC_IF_ACT) == 0);                                        //非中断方式，等待采集完成
	ADC_STAT |= bADC_IF_ACT;
	ADCValue = ADC_FIFO;
  return ADCValue;                                                             //返回采样值
}

#ifdef _INT_
/*******************************************************************************
* Function Name  : InitADCInterrupt()
* Description    : ADC中断初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitADCInterrupt()
{
    ADC_SETUP |= bADC_IE_FIFO_OV;                                              //使能FIFO溢出中断
//	ADC_SETUP |= bADC_IE_AIN7_LOW;                                             //使能AIN7低电平中断
    ADC_SETUP |= bADC_IE_ACT;                                                  //ADC完成中断
    IE_ADC = 1;                                                                //使能ADC中断
}

/*******************************************************************************
* Function Name  : ADCInterrupt(void)
* Description    : ADC 中断服务程序
*******************************************************************************/
void	ADCInterrupt( void ) interrupt INT_NO_ADC using 1                       //ADC中断服务程序,使用寄存器组1
{ 
    UINT16 ADCValue = 0;
    if(ADC_STAT & bADC_IF_ACT)                                                //ADC完成中断
    {  
    	ADC_STAT |= bADC_IF_ACT;                                                //清中断                                         
    }
    ADCValue = ADC_FIFO;
	  printf("FIFOCnt:%02X  ADC_DATA:%04X  \n",(UINT16)(ADC_STAT&3),(UINT16)ADC_FIFO);
/* 如果需要继续采样，则使能下面程序，产生手工采样脉冲 */
#if 1
    ADC_CTRL |= bADC_SAMPLE;                                                  //手动产生采样脉冲
    mDelayuS(2);
    ADC_CTRL &= ~bADC_SAMPLE;	
#endif
}
#endif

main( ) 
{
    UINT16 ADCDat;
    UINT8 i = 0;
//  CfgFsys( );                                                                //CH559时钟选择配置 
    mDelaymS(5);                                                               //等待内部晶振稳定
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n"); 
//ADC采样有8个通道，在P1口，通道n 分别ADC_CHANN对应 (0x01 << n) ,如通道0是0x01，通道1是0x02依次类推		
    InitADCManual();                                                           //ADC手动采样初始化
		ADCDat=ADC_FIFO;                                                           //ADC开启后，FIFO中有一个无效的采样值，取出丢弃
	  printf("FIFOCnt:%02X  InvalidADC_DATA:%04X  \n",(UINT16)(ADC_STAT&3),(UINT16)ADCDat);	
#ifdef _INT_
    InitADCInterrupt();                                                        //ADC中断初始化
    EA = 1;                                                                    //开启全局中断
	
    ADC_CTRL |= bADC_SAMPLE;                                                   //手动产生采样脉冲
    mDelayuS(2);
    ADC_CTRL &= ~bADC_SAMPLE;	
#endif	
    while(1)                                                                   //可以如果不用中断方式，可以查询bADC_IF_ACT位，为1代表手动采样成功
    {
#ifndef _INT_
		    ADC_SETUP|=bADC_POWER_EN;                                              //开启ADC电源
        for(i=0;i<8;i++){
          ADCDat = ADCChanelChange(ADCChannel[i]);                             //通道i采样
	        printf("FIFOCnt:%02X  ADC_DATA%X:%04X  \n",(UINT16)(ADC_STAT&3),(UINT16)i,(UINT16)ADCDat);			
        }
	      ADC_SETUP&=~bADC_POWER_EN;                                             //关闭ADC电源
        printf("-----我是分隔符------\n");
        mDelaymS(1000);				
//如果同时采样多个通道，可以在采样完成后通过bADC_CHANN_ID区分采样值所属的通道
#endif
    }                                                                 
}