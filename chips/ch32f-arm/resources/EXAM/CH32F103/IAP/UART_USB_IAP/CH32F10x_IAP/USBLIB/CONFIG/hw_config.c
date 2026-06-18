/********************************** (C) COPYRIGHT *******************************
* File Name          : hw_config.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2019/10/15
* Description        : USB configuration file.
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "usb_pwr.h"  
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB wake up exception.
* Input          : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void) 
{
	EXTI_ClearITPendingBit(EXTI_Line18);
} 

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB exception.
* Input          : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
	USB_Istr();
} 

/*******************************************************************************
* Function Name  : Set_USBConfig
* Description    : Set_USBConfig .
* Input          : None
* Return         : None
*******************************************************************************/
void Set_USBConfig( )
{
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);	 		 
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Enter low power mode.
* Input          : None
* Return         : None
*******************************************************************************/
void Enter_LowPowerMode(void)
{  
	bDeviceState=SUSPENDED;
} 

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Leave low power mode.
* Input          : None
* Return         : None
*******************************************************************************/
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo=&Device_Info;
	
	if (pInfo->Current_Configuration!=0)bDeviceState=CONFIGURED; 
	else bDeviceState = ATTACHED; 
} 

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configrate USB interrupt.
* Input          : None
* Return         : None
*******************************************************************************/
void USB_Interrupts_Config(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 	 

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);   
}	

/*******************************************************************************
* Function Name  : USB_Port_Set
* Description    : Set USB IO port.
* Input          : NewState: DISABLE or ENABLE.
*                  Pin_In_IPU: Enables or Disables intenal pull-up resistance.
* Return         : None
*******************************************************************************/
void USB_Port_Set(FunctionalState NewState, FunctionalState Pin_In_IPU)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	if(NewState) {
		_SetCNTR(_GetCNTR()&(~(1<<1)));
		GPIOA->CFGHR&=0XFFF00FFF;
		GPIOA->OUTDR&=~(3<<11);	//PA11/12=0
		GPIOA->CFGHR|=0X00088000;	// IPD
	}
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  
		GPIOA->CFGHR&=0XFFF00FFF;
		GPIOA->OUTDR&=~(3<<11);	//PA11/12=0
		GPIOA->CFGHR|=0X00033000;	// LOW
	}
	
	if(Pin_In_IPU) (EXTEN->EXTEN_CTR) |= EXTEN_USBD_PU_EN; 
	else (EXTEN->EXTEN_CTR) &= ~EXTEN_USBD_PU_EN;
}  








