/********************************** (C) COPYRIGHT *******************************
 * File Name		  : main.c
 * Author			  : WCH
 * Version			  : V1.0.0
 * Date			      : 2025/12/06
 * Description		  : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *The routine is used to demonstrate: Port0 handshake SNK, open Vconn power supply to obtain Emk information and then send 5, 9 SrcCap;
 *1. Port1 for power supply use, or can support PD;
 *2. Port0 supports SNK SRC DRP roles, SNK can be powered by Port0 alone, and the rest need to be powered by Port separately.
 */
 

#include "string.h"
#include "debug.h"
#include "PD_User.h"
#include "libPD_CH32M030.h"



/*********************************************************************
 * @fn      RCC_Init
 *
 * @brief   Init RCC
 *
 * @return  none
 */
void RCC_Init(void)
{
	RCC_PB1PeriphClockCmd( RCC_PB1Periph_TIM2 | RCC_PB1Periph_PWR, ENABLE );
	RCC_HBPeriphClockCmd( RCC_HBPeriph_USBPD0 | RCC_HBPeriph_USBPD1 | RCC_HBPeriph_DMA1 | RCC_HBPeriph_OPCM, ENABLE );  
    RCC_PB2PeriphClockCmd( RCC_PB2Periph_TIM1 | RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC | RCC_PB2Periph_ADC1 | RCC_PB2Periph_AFIO, ENABLE );

    GPIO_PinRemapConfig( GPIO_Remap_SDI_Disable, ENABLE );
	PWR_ISINKCmd( ENABLE );
}

/*********************************************************************
 * @fn      ADCs_Init
 *
 * @brief   Init ADC
 *
 * @return  none
 */
void ADCs_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	ADC_InitTypeDef ADC_InitStructure = {0};

	RCC_ADCCLKConfig( RCC_HB_Div6 );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	/*  iSRC PIN */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	EXTEN->EXTEN_KEYR = EXTEN_KEY1;
    EXTEN->EXTEN_KEYR = EXTEN_KEY2;    // code
    EXTEN->EXTEN_CTLR0 |= (EXTEN_ISR_C1_SEL | EXTEN_ISR_C1_EN);	//ISRC1 ENABLE 32uA

	ADC_DeInit( ADC1 );
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init( ADC1, &ADC_InitStructure );

	/*  VIN VDC ISP1 ISP2 */
 	ADC_RegularChannelConfig( ADC1, ADC_Channel_18, 1, ADC_SampleTime_23Cycles5 );  
  	ADC_RegularChannelConfig( ADC1, ADC_Channel_19, 2, ADC_SampleTime_23Cycles5 ); 

  	ADC_RegularChannelConfig( ADC1, ADC_Channel_4, 3, ADC_SampleTime_23Cycles5 ); 

    ADC_Cmd( ADC1, ENABLE );

	// ADC_BufferCmd( ADC1, DISABLE );    //disable buffer
    ADC_ResetCalibration( ADC1 );
    while( ADC_GetResetCalibrationStatus(ADC1) );
    ADC_StartCalibration( ADC1 );
    while( ADC_GetCalibrationStatus(ADC1) );
 	ADC_SoftwareStartConvCmd( ADC1, ENABLE );
}

/*********************************************************************
 * @fn      PWM_Init
 *
 * @brief   HVCP boost initialization
 *
 * @return  none
 */
void PWM_Init(void)   
{
	GPIO_InitTypeDef GPIO_InitStructure={0};

	/*  GATE1-4 HVIO */
	GPIO_SetBits( GPIOB, GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	/*  BOOST */
	AFIO->PCFR1 |= (1<<11);    /* reuse:PB14: TIM2 CH4 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM2->CTLR1 |= (1<<12);    /* PC5 PWM EN */
	TIM2->PSC = 1-1;
	TIM2->ATRLR = 48;    	   /* 500K */

	TIM2->CHCTLR1 |= (6<<12)|(6<<4);
	TIM2->CH1CVR = 24;
	TIM2->CH2CVR = 24;
	TIM2->CCER |= (1<<0)|(1<<4);

	TIM2->TIM2_DTCR |= (1<<4);            /* DT2_P = 1 */
	TIM2->TIM2_DTCR |= (2<<12);           /* 104ns dead zone */
	TIM2->TIM2_DTCR |= (1<<1);     		  /* OC2N_EN = 1 */
	TIM2->TIM2_DTCR |= (1<<0)|(1<<3);     /* OC1N_EN, DT1N_P = 1 */

	TIM_CtrlPWMOutputs( TIM2, ENABLE );
	TIM_ARRPreloadConfig( TIM2, ENABLE );
	TIM_Cmd( TIM2, ENABLE );
}

/*********************************************************************
 * @fn      PD_Init
 *
 * @brief   Init PD
 *
 * @return  none
 */
void PD_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure = {0};

	/*  PDIO */
    GPIO_InitStructure.GPIO_Pin = C0_PIN_CC1 | C0_PIN_CC2 | C1_PIN_CC1 | C1_PIN_CC2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    
	/*  PDIO high threshold, CC analog reference */
    EXTEN->EXTEN_KEYR = EXTEN_KEY1;
    EXTEN->EXTEN_KEYR = EXTEN_KEY2;    // code
    EXTEN->EXTEN_CTLR0 |= (USBPD0_CC_HVT | USBPD1_CC_HVT);	//High threshold		
    // EXTEN->EXTEN_CTLR1 &= ~(1<<14) | (1<<30);			//UDP DUM
       
	USBPD0->CONFIG |= (IE_TX_END|IE_RX_RESET|IE_RX_ACT|PD_DMA_EN|PD_FILT_ED);
	USBPD1->CONFIG |= (IE_TX_END|IE_RX_RESET|IE_RX_ACT|PD_DMA_EN|PD_FILT_ED);
    USBPD0->CONTROL |= BMC_START;             
    USBPD1->CONTROL |= BMC_START;

	/*  CC Vconn IO */
	GPIO_ResetBits( GPIOC, GPIO_Pin_8 ); 
	GPIO_ResetBits( GPIOC, GPIO_Pin_10 );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

	/*  LED DCDC_EN */
	GPIO_SetBits( GPIOC, GPIO_Pin_2 ); 		/* Default open DCDC EN */
	GPIO_SetBits( GPIOC, GPIO_Pin_3 ); 
	GPIO_ResetBits( GPIOC, GPIO_Pin_4 );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    PORT[0].DEVICE.pDevChk = PORT[1].DEVICE.pDevChk = Prot_NULL;
    Port_Sel( PD0 );
    Port_Sel( PD1 ); 

	PD_TMR_Init();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
 int main(void)
 {
 	SystemCoreClockUpdate();
    Delay_Init();
	Delay_Ms(5);
	USART_Printf_Init(921600);
	printf("M030 PD Test Sysclk:%d\r\n", SystemCoreClock);

	RCC_Init();
	ADCs_Init();
	PWM_Init();    
	PD_Init();

	Set_DevChk( Port1, DevRole_Sink );	/* For power use only */
	Set_DevChk( Port0, DevRole_Sink );
	while (1) {
		/* PD connect/disconnect */
		for ( u8 ch=0; ch<2; ch++ )
		{

			if ( PORT[ch].DEVICE.Attaching )
			{
				PORT[ch].DEVICE.Attaching = 0;
				pDevice_Attached( ch );
			}
			if ( PORT[ch].DEVICE.Unattaching )
			{
				PORT[ch].DEVICE.Unattaching = 0;
				pDevice_Unattached( ch );
			}
			if( (PORT[PHYch].PHY.ChangeVoltType == PD_VC_DCDCISink) && PORT[ch].PHY.VoltChanging )
			{
				PORT[PHYch].PHY.VoltChanging = 0;
				pVolt_DCDCChangeIsink();
			}
		}
	}
}












