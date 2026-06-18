/********************************** (C) COPYRIGHT *******************************
* File Name          : COMPARE.C
* Author             : WCH
* Version            : V1.00
* Date               : 2017-11-18
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\Public\CH554.H"
#include "COMPARE.H"

UINT8 CMP_Flag = 0;

/*******************************************************************************
* Function Name  : CMP_Enable
* Description    : Enable the compare module
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Enable(void)
{
	ADC_CFG |= bCMP_EN;
	
}

/*******************************************************************************
* Function Name  : CMP_Disable
* Description    : Disable the compare module
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Disable(void)
{
	ADC_CFG &= ~bCMP_EN;
}

/*******************************************************************************
* Function Name  : CMP_Init
* Description    : channel init 
* Input          :  _cmp_channel pos_chann,	invert channel
					_cmp_channel neg_chann,   no-invert channel
* Output         : None
* Return         : if pos_chann equal to neg_chann, return FALSE.
*******************************************************************************/
UINT8 CMP_Init(_cmp_channel pos_chann, _cmp_channel neg_chann)
{
	ADC_CTRL = 0;						/* Deinit the ADC/CMP module. */

	if (pos_chann == neg_chann)
	{
		return FALSE;
	}
	
	if(( neg_chann == IN0 )||( neg_chann == IN2 ))									/* neg channel only support channel 1 and channel 3 */
	{
		return FALSE;
	}

	switch (neg_chann)
	{
	case IN1:
		ADC_CTRL &= ~INVERT_IN;
		break;

	case IN3:
		ADC_CTRL |= INVERT_IN;
		break;

	default:
		return FALSE;
	}

	switch (pos_chann)
	{
	case IN0:
		ADC_CTRL |= NO_INVERT_CHAN0;
		break;

	case IN1:
		ADC_CTRL |= NO_INVERT_CHAN1;
		break;

	case IN2:
		ADC_CTRL |= NO_INVERT_CHAN2;
		break;

	case IN3:
		ADC_CTRL |= NO_INVERT_CHAN3;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
* Function Name  : CMP_Interrupt_Disable
* Description    : Disable compare interrupt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Interrupt_Disable(void)
{
  ADC_CFG &= ~bADC_EN;                                                        //shut down ADC
	IE_CMP = 0;
}

/*******************************************************************************
* Function Name  : CMP_Interrupt_Enable
* Description    : ebable compare interrupt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_Interrupt_Enable(void)
{
  ADC_CFG |= bADC_EN;                                                        //enable power for ADC
	CMP_START = 1;

	CMP_IF = 0;
	IE_CMP = 1;
}

/*******************************************************************************
* Function Name  : CMP_Out
* Description    : output the compare result 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CMP_Out(void)
{
	return CMPO;
}

/*******************************************************************************
* Function Name  : CMP_interrupt_ISR
* Description    : interrupt of the compare module
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMP_interrupt_ISR(void) interrupt INT_NO_ADC using 1
{
	if (CMP_IF)
	{
		T2^=1;
		CMP_IF = 0;
		CMP_Flag = 1;					/* Return the CMPO flag. */
		CMP_START = 1;					

	}
	
}

/* END OF FILE */
