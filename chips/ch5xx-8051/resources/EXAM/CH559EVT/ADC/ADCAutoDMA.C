
/********************************** (C) COPYRIGHT *******************************
* File Name          : ADCAutoDMA.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/6/24
* Description        : CH559的ADC自动DMA方式采样操作,设置采样通道为0和1轮测，采样位数设置为11位               				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#define ADCCount 30
UINT16X	ADCbuf[ ADCCount ] _at_ 0x0040 ;                                       //存储ADC采样数据
UINT8 Flag;

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : InitADCInterrupt()
* Description    : ADC中断初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitADCInterrupt()
{
//  ADC_SETUP |= bADC_IE_FIFO_OV;                                             //使能FIFO溢出中断
//  ADC_SETUP |= bADC_IE_AIN7_LOW;                                            //使能AIN7低电平中断
//  ADC_SETUP |= bADC_IE_ACT;                                                 //ADC完成中断
    IE_ADC = 1;                                                               //使能ADC中断
}

/*******************************************************************************
* Function Name  : InitADCAuto()
* Description    : ADC自动采样初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitADCAuto()
{
    P1_IE = 0x00;                                                              //关闭P1口其他数据功能,如果只用部分通道，可根据需要设定，否则影响IO正常使用
    ADC_SETUP |= bADC_POWER_EN;                                                //ADC电源使能
    ADC_CK_SE |= (MASK_ADC_CK_SE & 0x0C);                                      //设置12分频
    ADC_CTRL &= ~MASK_ADC_CYCLE;
    ADC_CTRL |= 0x0C;                                                          //设置ADC自动采样周期
    ADC_CTRL &= ~bADC_CHANN_MOD1;                                     
    ADC_CTRL |= bADC_CHANN_MOD0;                                               //自动通道选择通道0和通道1
    ADC_EX_SW |= bADC_RESOLUTION;                                              //采样位数11bit
//	ADC_EX_SW &= ~bADC_RESOLUTION;                                             //采样位数10bit
    mDelayuS(100);                                                             //确保ADC正常启动	
}

/*******************************************************************************
* Function Name  : InitADCDMA(UINT16 addr,UINT8 num)
* Description    : ADC的DMA初始化
* Input          : UINT16 addr,DMA起始地址
                   UINT8 num,DMA剩余计数
* Output         : None
* Return         : None
*******************************************************************************/
void InitADCDMA( )
{
    ADC_DMA = ADCbuf;                                                         //设置DMA起始地址
    ADC_DMA_CN = ADCCount;                                                    //设置DMA剩余计数
    ADC_SETUP |= bADC_DMA_EN;                                                 //使能DMA和中断
}

/*******************************************************************************
* Function Name  : ADCInterrupt(void)
* Description    : ADC 中断服务程序
*******************************************************************************/
void ADCInterrupt( void ) interrupt INT_NO_ADC using 1                        //ADC中断服务程序,使用寄存器组1
{
  	printf("%02X  ",(UINT16)ADC_STAT);  
    if(ADC_STAT & bADC_IF_DMA_END)                                            //DMA完成中断
    {
    	Flag = 1;
    	ADC_STAT |= bADC_IF_DMA_END;                                          //清中断
    }
    if(ADC_STAT & bADC_IF_FIFO_OV)                                            //ADC的FIFO溢出中断
    {
    	ADC_STAT |= bADC_IF_FIFO_OV;                                          //清中断
    }
    if(ADC_STAT & bADC_IF_AIN7_LOW)                                           //AIN7低电平中断
    {
    	ADC_STAT |= bADC_IF_AIN7_LOW;                                         //清中断
    }
    if(ADC_STAT & bADC_IF_ACT)                                                //ADC完成中断
    {
    	ADC_STAT |= bADC_IF_ACT;                                              //清中断
    }
}

main( ) 
{
    UINT8 i;
    mDelaymS(30);                                                              //上电延时,等待内部晶振稳定,必加 
//  CfgFsys( );                                                                //CH559时钟选择配置
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n"); 
    Flag = 0;
//ADC采样有8个通道，在P1口，通道n 分别ADC_CHANN对应 (0x01 << n) ,如通道0是0x01，通道1是0x02依次类推	
    InitADCInterrupt();                                                        //ADC自动采样初始化
    InitADCAuto();                                                             //ADC中断初始化
    InitADCDMA();

    EA = 1;                                                                    //开启全局中断
    while(1)
    { 
      if(Flag)
	    {
	    	for(i=0;i<30;i++)
	    	{
	    	    printf("%04X  ",ADCbuf[i]);                                    //初次采样值建议丢弃
	    	}
	    	Flag = 0;
	    }
    }
}