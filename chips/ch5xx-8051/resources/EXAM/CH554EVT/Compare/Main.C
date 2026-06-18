/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.00
* Date               : 2017-11-18
* Description        : The demo of the compare module.If using the query mode, must comment the 
function CMP_Interrupt_Enable(),and disable the Global interrupt bit 'EA = 0'.
						cmp channel as below,
						IN0 --> P11 			
						IN1 --> P14 
						IN2 --> P15 
						IN3 --> P32 	
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\Public\CH554.H"
#include "..\Public\DEBUG.H"
#include <STDIO.H>
#include "COMPARE.H"
#include <ASSERT.H>

#define		NDEBUG
/*******************************************************************************
* Function Name  : main
* Description    : main
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/	
void main ( void )
{
	CfgFsys();
	mDelaymS(3);
	mInitSTDIO();
	printf("DEBUG_DATE:"__DATE__"\nDEBUG_TIME:"__TIME__"\n");

	assert( CMP_Init(IN0, IN1) == TRUE );															/* Compare channels must be different. */

	CMP_Enable();
	CMP_Interrupt_Enable();																			      /* if usign query mode, comment this line */

	printf("ADC_CTRL:%02x\tADC_CFG:%02x\n",(UINT16)ADC_CTRL,(UINT16)ADC_CFG);
	
	EA = 1;																						  	            /* if usign query mode, comment this line */
	
	while(1)
	{
		if(	CMP_Flag == 1)																			        /* COM_Flag: a flag for compare has been changed. if using query mode, comment the flag.*/
		{
			CMP_Flag = 0;
			printf("CMP_OUT = %02x\n",(UINT16)CMP_Out());
		}	
	}
	
}
