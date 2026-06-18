/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 TouchKey触摸按键
*                      CH0~CH2,分别对应P10 P11 P32
*                      CH4~CH11,分别对应P14~P17 P36 P37 P12 P13
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/*******************************************************************************/
/* 头文件包含 */
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS

/*******************************************************************************/
/* 变量定义 */
UINT16 IntCurValue = 0;                              							/* 中断采集到的当前值 */
UINT16I KeyBuf[ 12 ][ KEY_BUF_LEN ];
UINT16I Temp_Buf[ KEY_BUF_LEN ];                                      			/* 专用缓冲区 */

/********************************************************************************
* Function Name  : TouchKey_Init
* Description    : 触摸按键初始化
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void TouchKey_Init( void )
{
	/* Touch采样通道设置为高阻输入 */
    P1_MOD_OC &= 0x0F;                               							/* P14~P17高阻输入 */
    P1_DIR_PU &= 0x0F;
    ADC_CTRL |= bADC_EN;                              							/* 开启ADC模块电源,选择外部通道 */
    ADC_CTRL = ADC_CTRL & ~( bADC_CLK0 | bADC_CLK1 );							/* 选择ADC参考时钟 */
    ADC_CHAN = 0x04;					           				      			/* 默认为CH4 */
    ADC_CTRL |= bADC_IF;                              							/* 清除ADC转换完成标志,写1清零 */

#if EN_ADC_INT
    IE_ADC = 1;                                       							/* 开启ADC中断使能 */
    EA = 1;                                           							/* 开启总中断使能 */
#endif
}

/********************************************************************************
* Function Name  : Buf_UpData_Filter
* Description    : 去尖峰平均值滑动滤波,更新窗口数组,首移出,把newdat插入数组尾,
*				   数组大小 KEY_BUF_LEN排序,去值,取平均返回                
* Input          : buf
*                  newdat: 更新值
* Output         : None
* Return         : None
********************************************************************************/
UINT16 Buf_UpData_Filter( UINT16 *buf, UINT16 newdat )
{
	UINT8  i,j,k;
	UINT16 temp;
	UINT32 sum;

	/* 更新队列 */
	for( i = 1; i != KEY_BUF_LEN; i++ )
	{
		buf[ i - 1 ] = buf[ i ];
	}
	buf[ i - 1 ] = newdat;

	/* 排序 */
	memcpy( Temp_Buf, buf, sizeof( Temp_Buf ) );
	k = KEY_BUF_LEN - 1;
	for( i = 0; i != k; i++ )
	{
		for( j = i + 1; j != KEY_BUF_LEN; j++ )
		{
			if( Temp_Buf[ i ] < Temp_Buf[ j ] )
			{
				temp = Temp_Buf[ i ];
				Temp_Buf[ i ] = Temp_Buf[ j ];
				Temp_Buf[ j ] = temp;
			}
		}
	}
	
	/* 丢值,取平均 */
	sum = 0;
	k = KEY_BUF_LEN - KEY_BUF_LOST;
	for( i = KEY_BUF_LOST; i != k; i++ )
	{
		sum += Temp_Buf[ i ];
	}
	return ( sum / ( KEY_BUF_LEN - 2 * KEY_BUF_LOST ) );
}

/********************************************************************************
* Function Name  : TouchKeySelect
* Description    : 触摸按键通道选择和充电脉冲宽度配置获取TouchKey 电压滤波平均值
* Input          : ch: 通道号选择
*                      0~2,分别对应P10 P11 P32
*                      4~11,分别对应P14~P17 P36 P37 P12 P13
*                  cpw: 充电脉冲宽度 0~127                   
*                  cpw由外部触摸按键电容、VDD电压、主频三者决定;
*                  计算公式: count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
*                  TKEY_CTRL=count > 127 ? 127 : count (其中Cint为15pF,ITKEY为50u)
*                  简化公式: cpw = (Ckey+15)*0.35*VDD*Fsys/50
*                  cpw = cpw>127?127:cpw
* Output         : None
* Return         : 返回触摸检测电压
********************************************************************************/
UINT16 TouchKeySelect( UINT8 ch, UINT8 cpw )
{
	ADC_CHAN = ADC_CHAN & ( ~MASK_ADC_CHAN ) | ch;								/* 外部通道选择 */
	
	/* 电容较大时可以先设置IO低,然后恢复浮空输入实现手工放电,≤0.2us */
	TKEY_CTRL = cpw;                                 							/* 充电脉冲宽度配置,仅低7位有效(同时清除bADC_IF,启动一次TouchKey) */
	while( ADC_CTRL & bTKEY_ACT );
	IntCurValue = ( ADC_DAT & 0xFFF );
	TKEY_CTRL = 0;
	IntCurValue = Buf_UpData_Filter( &KeyBuf[ ch ][ 0 ], IntCurValue );
	return ( IntCurValue );
}

/********************************************************************************
* Function Name  : Default_TouchKey
* Description    : 获取TouchKey 电压初值
* Input          : ch: 通道号选择
*                      0~2,分别对应P10 P11 P32
*                      4~11,分别对应P14~P17 P36 P37 P12 P13
*                  充电脉冲宽度 cpw:0~127
*                  cpw由外部触摸按键电容、VDD电压、主频三者决定;
*                  计算公式: count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
*                  TKEY_CTRL=count > 127 ? 127 : count (其中Cint为15pF,ITKEY为50u)
*                  简化公式: cpw = (Ckey+15)*0.35*VDD*Fsys/50
*                  cpw = cpw>127?127:cpw
* Output         : None
* Return         : 返回触摸检测电压
********************************************************************************/
UINT16 Default_TouchKey( UINT8 ch, UINT8 cpw )
{
	ADC_CHAN = ADC_CHAN & ( ~MASK_ADC_CHAN ) | ch;								/* 外部通道选择 */
	
	/* 电容较大时可以先设置IO低,然后恢复浮空输入实现手工放电,≤0.2us */
	TKEY_CTRL = cpw;                                 							/* 充电脉冲宽度配置,仅低7位有效(同时清除bADC_IF,启动一次TouchKey) */
	while( ADC_CTRL & bTKEY_ACT );
	IntCurValue = ( ADC_DAT & 0xFFF );
	TKEY_CTRL = 0;
	return ( IntCurValue );
}