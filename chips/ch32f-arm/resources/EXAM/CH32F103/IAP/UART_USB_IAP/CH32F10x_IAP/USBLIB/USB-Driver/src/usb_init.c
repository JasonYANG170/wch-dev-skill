/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_init.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2019/10/15
 * Description        : Initialization routines & global variables
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/ 
#include "usb_lib.h"

uint8_t	EPindex;
DEVICE_INFO *pInformation;
DEVICE_PROP *pProperty;
uint16_t	SaveState ;
uint16_t  wInterrupt_Mask;
DEVICE_INFO	Device_Info;
USER_STANDARD_REQUESTS  *pUser_Standard_Requests;

/*********************************************************************
 * @fn      USB_Init
 *
 * @brief   USB system initialization
 *
 * @return  none
 */
void USB_Init(PWR_VDD VDD_Voltage)
{
  if( VDD_Voltage == PWR_VDD_5V)
  {
	EXTEN->EXTEN_CTR |= EXTEN_USB_5V_SEL;
  }
  else
  {
	EXTEN->EXTEN_CTR &= ~EXTEN_USB_5V_SEL;
  }
	
  pInformation = &Device_Info;
  pInformation->ControlState = 2;
  pProperty = &Device_Property;
  pUser_Standard_Requests = &User_Standard_Requests;
  pProperty->Init();
}





