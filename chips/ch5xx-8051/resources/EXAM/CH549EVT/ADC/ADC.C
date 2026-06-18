/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.C
* Author             : WCH
* Version            : V1.4
* Date               : 2021/12/15
* Description        : CH549 ADC采样时钟设置，ADC通道设置函数，温度检测初始化
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "ADC.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : ADC_ExInit
* Description    : ADC 外部通道初始化函数
* Input          : UINT8 AdcClk
                   0 :512 Fosc cycles for each ADC
                   1 :256 Fosc cycles for each ADC
                   2 :128 Fosc cycles for each ADC
                   3 :64  Fosc cycles for each ADC
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ExInit( UINT8 AdcClk )
{
    UINT8 dat;
    ADC_CFG |= bADC_EN;                              //开启ADC模块电源
    ADC_CFG = (ADC_CFG & ~(bADC_CLK0 | bADC_CLK1))|AdcClk;//选择ADC参考时钟
    ADC_CFG |= bADC_AIN_EN;                          //开启外部通道
    dat = ADC_DAT;                                   //空读
    ADC_CTRL = bADC_IF;                              //清除ADC转换完成标志，写1清零
#if EN_ADC_INT
     SCON2 &= ~bU2IE;                                  //和UART2中断地址共用，故中断需2选1
    IE_ADC = 1;                                      //开启ADC中断使能
    EA = 1;                                          //开启总中断使能
#endif
}
/*******************************************************************************
* Function Name  : ADC_ChSelect
* Description    : ADC 外部通道选择
* Input          : UINT8 ch
                   ch, 位域 0~15bit分别对应外部16通道
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ChSelect( UINT8 ch )
{
    ADC_CHAN = (ADC_CHAN & ~MASK_ADC_CHAN) | ch;
    if(ch<=7)                                  //P10~P17引脚配置,不用每次都设置
    {
        P1_MOD_OC &= ~( 1 << ch );                       //高阻输入
        P1_DIR_PU &= ~( 1 << ch );
    }
    if(ch>7 && ch<=0x0f)                                  //P00~P07引脚配置，不用每次都设置
    {
        P0_MOD_OC &= ~( 1 << (ch-8) );                  //高阻输入
        P0_DIR_PU &= ~( 1 << (ch-8) );
    }
}
/*******************************************************************************
* Function Name  : ADC_InTSInit
* Description    : ADC 内部通道温度通道初始化
                   开启内部温度检测，会关闭所有外部输入通道
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_InTSInit(void)
{
    ADC_CFG |= bADC_EN;                              //开启ADC模块电源
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);    //选择ADC参考时钟 750KHz
    ADC_CFG &= ~bADC_AIN_EN;                         //关闭外部通道
    ADC_CHAN = ADC_CHAN &~MASK_ADC_I_CH | (3<<4);    //内部通道3为温度检测通道
    ADC_CTRL |= bADC_IF;                             //清除ADC转换完成标志，写1清零
#if EN_ADC_INT
     SCON2 &= ~bU2IE;                                  //和UART2中断地址共用，故中断需2选1
    IE_ADC = 1;                                      //开启ADC中断使能
    EA = 1;                                          //开启总中断使能
#endif
}
#if EN_ADC_INT
/*******************************************************************************
* Function Name  : ADC_ISR( )
* Description    : ADC 中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ISR(void)  interrupt INT_NO_ADC
{
    if(ADC_CTRL&bADC_IF)
    {
        ADC_CTRL = bADC_IF;                          //清除ADC转换完成中断标志
        GetValueFlag = 1;			
    }	
    ADC_ChSelect(CH0);                               //选择下一个需要采集的通道	
}
#endif
