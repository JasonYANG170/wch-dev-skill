/********************************** (C) COPYRIGHT *******************************
* File Name          : Compare.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ADC�Ƚ���
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "Compare.H"
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
    ADC_CFG |= bCMP_EN;                     //开启比较器模块电源
    /* 比较通道选择,列出三种情况,实际使用根据具体需求配置 */
#if 1
    /* 外部通道0和内部1/8 VDD电压比较 */
    P1_MOD_OC &= ~0x01;                     //P1.0高阻输入
    P1_DIR_PU &= ~0x01;
    ADC_CFG |= (bADC_AIN_EN | bVDD_REF_EN); //开启外部通道,开启参考电压
    ADC_CHAN = (0<<6)|(3<<4)|(0<<0);        //比较器-通道、内部通道、外部通道分别 0,3,0
#endif
#if 0
    /* 外部通道0和外部通道1电压比较 */
    P1_MOD_OC &= ~0x03;                     //P1.0 P1.1高阻输入
    P1_DIR_PU &= ~0x03;
    ADC_CFG |= bADC_AIN_EN;                 //开启外部通道
    ADC_CHAN = (2<<6)|(3<<4)|(0<<0);        //比较器-通道、内部通道、外部通道分别 2,3,0
#endif
#if 0
    /* 内部54.5%的V33 和外部通道1比较 */
    P1_MOD_OC &= ~0x02;                     //P1.1高阻输入
    P1_DIR_PU &= ~0x02;
    ADC_CFG &= ~bADC_AIN_EN;                //关闭外部通道
    ADC_CHAN = (2<<6)|(2<<4)|(0<<0);        //比较器-通道、内部通道、外部通道分别 2,2,0
#endif
    ADC_CTRL = bCMP_IF;                     //清除比较标志，写1清零
#if EN_ADC_INT
    SCON2 &= bU2IE;                         //和UART2中断地址共用，故中断需2选1
    IE_ADC = 1;                             //开启ADC中断使能
    EA = 1;                                 //开启总中断使能
#endif
}
/*******************************************************************************
* Function Name  : CMP_IN_Minus_Signal_SELT
* Description    : 比较器IN-反向通道信号选择
                   UINT8 mode
                   0:ADC/comparator IN-连接VDD/8
                   1:ADC/comparator IN-连接VDD
                   2:ADC/comparator IN-连接VDD/4
                   3:断开内部连接,浮空
                   4:ADC/comparator IN-连接P11
                   5:ADC/comparator IN-连接P12
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_IN_Minus_Signal_SELT(UINT8 mode)
{
  switch(mode)
  {
    case 0:
      ADC_CFG = ADC_CFG | bVDD_REF_EN | bCMP_EN; //ADC/comparator IN-连接VDD/8
      ADC_CHAN &= ~MASK_CMP_CHAN;	
    break;	
    case 1:
      ADC_CFG = ADC_CFG | bVDD_REF_EN & ~bCMP_EN; //ADC/comparator IN-连接VDD
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x40;	      			
    break;
    case 2:
      ADC_CFG = ADC_CFG | bVDD_REF_EN | bCMP_EN; //ADC/comparator IN-连接VDD/4
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x40;				
    break;
    case 3:
      ADC_CFG |= bCMP_EN;                        //ADC/comparator IN-连接P11
      ADC_CHAN = ADC_CHAN & ~MASK_CMP_CHAN | 0x80;			
    break;
    case 4:
      ADC_CFG |= bCMP_EN;                        //ADC/comparator IN-连接P12
      ADC_CHAN |= MASK_CMP_CHAN;				
    break;
    case 5:
    break;
    default:
    break;			
  }
}
/*******************************************************************************
* Function Name  : CMP_Inter_IN_Plus_Signal_SELT
* Description    : 比较器IN+正向内部通道信号选择
                   UINT8 mode
                   0:ADC/comparator IN+连接VDD/2
                   1:ADC/comparator IN+连接V33
                   2:ADC/comparator IN+连接V33/1.83
                   3:断开内部连接
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Inter_IN_Plus_Signal_SELT(UINT8 mode)
{
    ADC_CHAN = ADC_CHAN & ~MASK_ADC_I_CH | (mode<<4);
    if(mode == 0)
    {
        ADC_CFG |= bVDD_REF_EN;
    }
    if(mode == 3)
    {
        ADC_CFG |= bADC_AIN_EN;
    }
}
/*******************************************************************************
* Function Name  : CMP_Ext_IN_Plus_SELT
* Description    : 比较器IN+正向外部信号通道选择
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Ext_IN_Plus_SELT(UINT8 ch)
{
    ADC_CFG |= bADC_AIN_EN;
    ADC_CHAN = ADC_CHAN & ~MASK_ADC_CHAN | ch;
}
/*******************************************************************************
* Function Name  : CMP_Ext_IN_Plus_Float
* Description    : 比较器IN+正向外部信号通道设置浮空
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Ext_IN_Plus_Float( )
{
    ADC_CFG &= ~bADC_AIN_EN;
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
    if(ADC_CTRL&bCMP_IF)
    {
        ADC_CTRL = bCMP_IF;                 //清除比较器变化中断标志
        if(ADC_CTRL&bCMPDO)
        {
            printf("+ > -\n");
        }
        else
        {
            printf("+ < -\n");
        }
    }
}
#endif