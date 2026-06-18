/********************************** (C) COPYRIGHT  *******************************
* File Name          : iap.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/12/16
* Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "iap.h"
#include "usb_istr.h"
#include "usb_lib.h"
#include "string.h"
#include "flash.h"


/******************************************************************************/
#define FLASH_Base   0x08005000   

iapfun jump2app; 
u32 Program_addr = FLASH_Base; 
u32 Verity_addr = FLASH_Base; 
u32 User_APP_Addr_offset = 0x5000;  
u8 Verity_Star_flag = 0;   
u8 Fast_Program_Buf[256];
u8 CodeLen = 0;
u8 End_Flag = 0;   

#define  isp_cmd_t   ((isp_cmd  *)EP2_Rx_Buffer)


/*******************************************************************************
* Function Name  : RecData_Deal
* Description    : UART-USB 
* Input          : None
* Return         : ERR_ERROR: 
*                  ERR_SCUESS:
*                  ERR_End:
*******************************************************************************/ 
u8 RecData_Deal(void)
{
	u8 i,s,Lenth;
	
	Lenth = isp_cmd_t->Len;
	
	switch( isp_cmd_t->Cmd){				
		case CMD_IAP_ERASE:  
      FLASH_Unlock_Fast();
			s = ERR_SCUESS;
			break;
				
		case CMD_IAP_PROM:
			for(i=0; i<Lenth; i++){
				Fast_Program_Buf[CodeLen + i] = isp_cmd_t->data[i];
			}
			
		  CodeLen += Lenth;
					
			if(CodeLen >= 128){	
				FLASH_ErasePage_Fast(Program_addr);	
				CH32_IAP_Program(Program_addr, (u32*)Fast_Program_Buf);

				CodeLen -= 128;
				for(i=0; i<CodeLen; i++){
					Fast_Program_Buf[i] = Fast_Program_Buf[128+i];
				}		
				
				Program_addr += 0x80;
				
			}
			s = ERR_SCUESS;
			break;

		case CMD_IAP_VERIFY:
			if(Verity_Star_flag==0){
				Verity_Star_flag = 1;

				for(i=0; i<(128-CodeLen); i++){
					Fast_Program_Buf[CodeLen + i] = 0xFF;
				}
				
				FLASH_ErasePage_Fast(Program_addr);		
				CH32_IAP_Program(Program_addr, (u32*)Fast_Program_Buf);
				CodeLen = 0;	
			}			
			
			s=ERR_SCUESS;  // verity suc
			for(i=0; i<Lenth; i++){				
				if(isp_cmd_t->data[i] != *(u8*)(Verity_addr + i)){	
					s=ERR_ERROR;  // verity fail
					break;	
				}
			}
			
			Verity_addr += Lenth;
		
			break;

		case CMD_IAP_END:
			Verity_Star_flag = 0; 
			End_Flag = 1;
			Program_addr = FLASH_Base;
			Verity_addr = FLASH_Base;	
			s = ERR_End;  
			break;

		default:
			s = ERR_ERROR;  
			break	;			
	}				
	
	return s;
}


/*******************************************************************************
* Function Name  : EP2_IN_Tx_Deal
* Description    : �˵� 1��IN�����ݷ���
* Input          : None
* Return         : None
*******************************************************************************/ 
void EP2_IN_Tx_Deal(void)
{
	UserToPMABufferCopy(EP2_Tx_Buffer, ENDP2_TXADDR, EP2_Tx_Cnt);
	SetEPTxCount(ENDP2, EP2_Tx_Cnt);
	
	SetEPTxValid(ENDP2); 
}


/*******************************************************************************
* Function Name  : EP2_RecData_Deal
* Description    : �˵� 2��OUT�����ݴ���
* Input          : None
* Return         : None
*******************************************************************************/ 
void EP2_RecData_Deal(void)  
{
	u8 s;

	if(EP2_OUT_Flag){
		EP2_OUT_Flag = 0;
		EP2_Rx_Cnt = USB_SIL_Read(EP2_OUT, EP2_Rx_Buffer);
			
		s = RecData_Deal();
		
		if(s!=ERR_End){		
				EP2_Tx_Buffer[0] = 0x00;
				if(s==ERR_ERROR) EP2_Tx_Buffer[1] = 0x01;    //err
				else EP2_Tx_Buffer[1] = 0x00; 
				EP2_Tx_Cnt = 2;
				EP2_IN_Tx_Deal();
		}
		
		SetEPRxStatus(ENDP2, EP_RX_VALID);
	}
	
}


/*******************************************************************************
* Function Name  : GPIO_Cfg_init
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/ 
void GPIO_Cfg_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
	
}


/*******************************************************************************
* Function Name  : PA0_Check
* Description    :  
* Input          : None
* Return         : 1: IAP
*                  0：APP
*******************************************************************************/ 
u8 PA0_Check(void)
{
    u8 i;
    GPIO_Cfg_init();
    i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if (i == 0) {
        Delay_Ms(100);
        if (i == 0) {
            return 0;
        }
    }

    return 1;
}


__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*******************************************************************************
* Function Name  : iap_load_app
* Description    : jump APP
* Input          : appxaddr
* Output         : None
* Return         : None
*******************************************************************************/ 
void iap_load_app(u32 appxaddr)
{
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		
		MSR_MSP(*(vu32*)appxaddr);					
		jump2app();								
}	

/*******************************************************************************
* Function Name  : USBD_CFG
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
void USBD_CFG(void)
{
	Set_USBConfig(); 
	USB_Init(PWR_VDD_SupplyVoltage());	    
   
	USB_Port_Set(DISABLE, DISABLE);	
	Delay_Ms(200);
	USB_Port_Set(ENABLE, ENABLE);
	
 	USB_Interrupts_Config();   	
}

/*******************************************************************************
* Function Name  : USART3_CFG
* Description    : 
* Input          : baudrate:UART3
* Output         : None
* Return         : None
*******************************************************************************/ 
void USART3_CFG(u32 baudrate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
  USART_Init(USART3, &USART_InitStructure); 
	
	USART_Cmd(USART3, ENABLE); 
}

/*******************************************************************************
* Function Name  : UART3_SendMultiyData
* Description    : USART3
* Input          : pbuf
*                  num
* Output         : None
* Return         : None
*******************************************************************************/ 
void UART3_SendMultiyData(u8* pbuf, u8 num)
{
	u8 i=0;
	
	while(i<num){
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  
		USART_SendData(USART3, pbuf[i]);
		i++;
	}
}

/*******************************************************************************
* Function Name  : UART3_SendMultiyData
* Description    : USART3
* Input          : pbufbuff
*                  num
* Output         : None
* Return         : None
*******************************************************************************/ 
void UART3_SendData(u8 data)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  
	USART_SendData(USART3, data);
			
}

/*******************************************************************************
* Function Name  : Uart3_Rx
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
u8 Uart3_Rx( void )
{
	while( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData( USART3 );
}

/*******************************************************************************
* Function Name  : UART_Rx_Deal
* Description    : UART Rx data deal
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
void UART_Rx_Deal(void)
{
	u8 i,s;
	u8 Data_add=0;
	
	if(Uart3_Rx() == Uart_Sync_Head1){
		if(Uart3_Rx() == Uart_Sync_Head2){	
			isp_cmd_t->Cmd = Uart3_Rx();Data_add += isp_cmd_t->Cmd;
			isp_cmd_t->Len = Uart3_Rx();Data_add += isp_cmd_t->Len;
			isp_cmd_t->Rev[0] = Uart3_Rx();Data_add += isp_cmd_t->Rev[0];
			isp_cmd_t->Rev[1] = Uart3_Rx();Data_add += isp_cmd_t->Rev[1];
			
			if((isp_cmd_t->Cmd == CMD_IAP_PROM) || (isp_cmd_t->Cmd == CMD_IAP_VERIFY)){ 
				for(i=0; i<isp_cmd_t->Len; i++){
					isp_cmd_t->data[i] = Uart3_Rx();Data_add += isp_cmd_t->data[i];		
				}
			}
							
			if(Uart3_Rx() == Data_add){  
				s = RecData_Deal();			
							
				if(s!=ERR_End){		
						UART3_SendData(0x00);     					
						if(s==ERR_ERROR){
							UART3_SendData(0x01);    
						}
						else{
							UART3_SendData(0x00);    
						}							
				}		
			}
		}			
	}
}



