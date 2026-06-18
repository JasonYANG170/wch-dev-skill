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
#include "string.h"
#include "core_riscv.h"
#include "CH56xSFR.h"
#include "CH56x_common.h"
#include  "ISPEM569.h"

/******************************************************************************/

#define FLASH_Base   0x00004000
#define USBD_DATA_SIZE               64
iapfun jump2app;
uint32_t Program_addr = FLASH_Base;
uint32_t Verity_addr = FLASH_Base;
uint32_t User_APP_Addr_offset = 0x4000;
uint8_t Verity_Star_flag = 0;
uint8_t Fast_Program_Buf[ 390 ];
uint32_t CodeLen = 0;
uint8_t End_Flag = 0;
uint8_t EP2_Rx_Buffer[ USBD_DATA_SIZE ];
#define  isp_cmd_t   ((isp_cmd  *)EP2_Rx_Buffer)

/*********************************************************************
 * @fn      RecData_Deal
 *
 * @brief   UART-USB deal date
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SCUESS - SCUESS
 *          ERR_End - End
 */

uint8_t RecData_Deal(void)
{
	uint16_t s, Lenth;
	static uint8_t ss = 0;
	Lenth = isp_cmd_t->Len;
	switch ( isp_cmd_t->Cmd )
	{
		case CMD_IAP_ERASE:
			s = ERR_SCUESS;
			break;

		case CMD_IAP_PROM:

			CodeLen += Lenth;
			if ( (CodeLen >= 0x1000) )
			{
				FLASH_ROMA_ERASE( Program_addr+0x1000,0x1000 );
				FLASH_ROMA_WRITE( Program_addr , (uint32_t*) isp_cmd_t->data , Lenth );
				CodeLen -= 0x1000;
			}
			else
			{
				FLASH_ROMA_WRITE( Program_addr , (uint32_t*) isp_cmd_t->data , Lenth );
			}
			Program_addr += Lenth;
			s = ERR_SCUESS;
			break;

		case CMD_IAP_VERIFY:

			s = FLASH_ROMA_VERIFY( Verity_addr , (uint32_t*) isp_cmd_t->data , Lenth );

			Verity_addr += Lenth;

			break;

		case CMD_IAP_END:
			Verity_Star_flag = 0;
			End_Flag = 1;
			s = ERR_End;
			break;

		default:
			s = ERR_ERROR;
			break;
	}
	return s;
}

/*********************************************************************
 * @fn      USART3_CFG
 *
 * @brief   baudrate:UART3 baudrate
 *
 * @return  none
 */
void USART3_CFG(uint32_t baudrate)
{
	GPIOB_ModeCfg( GPIO_Pin_3 , GPIO_ModeIN_Floating );
	GPIOB_ModeCfg( GPIO_Pin_4 , GPIO_ModeOut_OP_8mA );
	UART3_DefInit();
	UART3_BaudRateCfg( 57600 );
}
/*********************************************************************
 * @fn      UART3_SendMultiyData
 *
 * @brief   Deal device Endpoint 1 OUT.
 *
 * @param   l: Data length.
 *
 * @return  none
 */
void UART3_SendMultiyData(uint8_t* pbuf, uint8_t num)
{
	uint8_t i = 0;

	while(i<num)
	{
		while(UART3_GetLinSTA() & RB_LSR_TX_FIFO_EMP == RESET);
		UART3_SendByte(pbuf[i]);
		i++;
	}
}
/*********************************************************************
 * @fn      UART3_SendMultiyData
 *
 * @brief   USART3 send date
 *
 * @param   pbuf - Packet to be sent
 *          num - the number of date
 *
 * @return  none
 */
void UART3_SendData(uint8_t data)
{
	while(UART3_GetLinSTA() & RB_LSR_TX_FIFO_EMP == RESET);
	UART3_SendByte( data );
}

/*********************************************************************
 * @fn      Uart3_Rx
 *
 * @brief   Uart1 receive date
 *
 * @return  none
 */
uint8_t Uart3_Rx(void)
{
	while( (UART3_GetLinSTA() & RB_LSR_DATA_RDY) == RESET);
	return UART3_RecvByte();
}

/*********************************************************************
 * @fn      UART_Rx_Deal
 *
 * @brief   UART Rx data deal
 *
 * @return  none
 */
void UART_Rx_Deal(void)
{
	uint8_t i, s;
	uint8_t Data_add = 0;

	if ( Uart3_Rx() == Uart_Sync_Head1 )
	{
		if ( Uart3_Rx() == Uart_Sync_Head2 )
		{
			isp_cmd_t->Cmd = Uart3_Rx();
			Data_add += isp_cmd_t->Cmd;
			isp_cmd_t->Len = Uart3_Rx();
			Data_add += isp_cmd_t->Len;
			isp_cmd_t->Rev[ 0 ] = Uart3_Rx();
			Data_add += isp_cmd_t->Rev[ 0 ];
			isp_cmd_t->Rev[ 1 ] = Uart3_Rx();
			Data_add += isp_cmd_t->Rev[ 1 ];
			if ( ( isp_cmd_t->Cmd == CMD_IAP_PROM ) || ( isp_cmd_t->Cmd == CMD_IAP_VERIFY ) )
			{
				for ( i = 0; i < isp_cmd_t->Len ; i++ )
				{
					isp_cmd_t->data[ i ] = Uart3_Rx();
					Data_add += isp_cmd_t->data[ i ];
				}
			}
			if ( Uart3_Rx() == Data_add )
			{

				s = RecData_Deal();

				if ( s != ERR_End )
				{

					UART3_SendData( 0x00 );
					if ( s == ERR_ERROR )
					{
						UART3_SendData( 0x01 );
					}
					else
					{
						UART3_SendData( 0x00 );
					}
				}

			}
		}
	}

}

