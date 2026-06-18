/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 ADC采样时钟设置,ADC通道设置函数,外部通道检测初始化
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */	
#include "ADC.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : ADC_Init
* Description    : ADC 初始化函数
* Input          : UINT8 AdcClk
                   0 :1024 Fosc cycles for each ADC
                   1 :512  Fosc cycles for each ADC
                   2 :256  Fosc cycles for each ADC
                   3 :128  Fosc cycles for each ADC
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Init( UINT8 AdcClk )
{
    UINT8 dat;
    ADC_CTRL = bADC_EN;                                                         /* 开启ADC模块电源 */
    ADC_CTRL = (ADC_CTRL & ~(bADC_CLK0 | bADC_CLK1)) | AdcClk;                  /* 选择ADC参考时钟 */
    dat = ADC_DAT;                                                              /* 空读 */
    ADC_CTRL |= bADC_IF;                                                        /* 清除ADC转换完成标志，写1清零 */
#if EN_ADC_INT
    IE_ADC = 1;                                                                 /* 开启ADC中断使能 */
    EA = 1;                                                                     /* 开启总中断使能 */
#endif
}

/*******************************************************************************
* Function Name  : ADC_ChSelect
* Description    : ADC 通道选择
* Input          : UINT8 ch
                   ch, 位域 0~13bit为外部12通道和内部电源电压、内部参考电压
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ChSelect( UINT8 ch )
{
    ADC_CHAN = (ADC_CHAN & ~0x0F) | ch;
    switch(ch)													                /* 引脚配置，不用每次都设置 */
    {
        case 0:
        case 1:
        case 4:
        case 5:
        case 6:
        case 7:
             P1_MOD_OC &= ~( 1 << ch );							                /* 高阻输入 */
             P1_DIR_PU &= ~( 1 << ch );
             break;
        case 2:
             P3_MOD_OC &= ~( 1 << ch ); 						                /* 高阻输入 */
             P3_DIR_PU &= ~( 1 << ch );
             break;
        case 8:
        case 9:
             P3_MOD_OC &= ~( 1 << ( ch - 2 ));							        /* 高阻输入 */
             P3_DIR_PU &= ~( 1 << ( ch - 2 ));
             break;
        case 0x0A:
        case 0X0B:
             P1_MOD_OC &= ~( 1 << ( ch - 8 ));							        /* 高阻输入 */
             P1_DIR_PU &= ~( 1 << ( ch - 8 ));
             break;
    }
}

#if EN_ADC_INT

extern UINT8 GetValueFlag;

/*******************************************************************************
* Function Name  : ADC_ISR( )
* Description    : ADC 中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ISR( void )  interrupt INT_NO_ADC
{
    if( ADC_CTRL & bADC_IF )
    {
        ADC_CTRL |= bADC_IF;							                        /* 清除ADC转换完成中断标志 */
        GetValueFlag = 1;			
    }	
    ADC_ChSelect( CH0 );							                            /* 选择下一个需要采集的通道	*/
}
#endif