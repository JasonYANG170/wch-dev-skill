/********************************** (C) COPYRIGHT *******************************
* File Name          : I2C.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : VGA切换时EDID操作相关程序
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "I2C.H"	 															/* 主程序定义相关头文件 */

#pragma  NOAREGS
/*******************************************************************************
* Function Name  : I2CM_Init
* Description    : I2C主机初始化
* Input          : None
* Return         : None
*******************************************************************************/
void I2CM_Init(void)
{
	I2CM_CK_SE = 60;															    /* 100KHZ */
//	I2CM_CTRL |= bI2CM_IE;													/* 中断使能 */
	I2CM_CTRL |= bI2CM_EN;														/* 主机使能EN */
}

/*******************************************************************************
* Function Name  : I2CS_Init
* Description    : I2C从机初始化
* Input          : None
* Return         : None
*******************************************************************************/
void I2CS_Init(UINT8 addr, UINT8* pbuf )
{		
	I2CS_DEV_A = addr;															  /* 设置设备地址 */
	
	I2CS_CTRL 	= bI2CS_IE_STASTO | bI2CS_DMA_EN;		  /* 起始中断,开启DMA */

	I2CS_CTRL |= bI2CS_EN;														/* 使能I2C从机 */

	I2CS_DMA_H = (UINT16)pbuf>> 8; 
	I2CS_DMA_L = (UINT16)pbuf;  
	
	IE_PWM_I2C = 1;																     /* 开启中断 */
	E_DIS = 0;
	EA = 1;
}
	
/*******************************************************************************
* Function Name  : I2CM_CMD
* Description    : I2C主机发送命令
* Input          : None
* Return         : None
*******************************************************************************/
void I2CM_CMD(UINT8 cmd)
{
	I2CM_CTRL &= 0xfc;
	I2CM_CTRL |= cmd;
}

/*******************************************************************************
* Function Name  : I2CM_WaitACK
* Description    : I2C主机等待从机应答ACK
* Input          : None
* Return         : None
*******************************************************************************/
void I2CM_WaitACK(void)
{
	UINT16 timecount;
	
	/* 等待主机操作完成 */
	timecount = 0x00;
	while(!(I2CM_STAT & 0X80))
	{
		timecount++;
		if(timecount > 5000)
		{
			break;
		}
		mDelayuS(1);	
	}			
	I2CM_STAT |= 0x80;															/* 清中断标志 */
	
	timecount = 0x00;
	while(I2CM_CTRL & 0X10)													/* 查询设备ACK */
	{
		timecount++;
		if(timecount > 5000)
		{
			break;
		}
		mDelayuS(1);	
	}	
}

/*******************************************************************************
* Function Name  : I2CM_RecvData
* Description    : I2C主机接收1个字节数据
* Input          : None
* Return         : None
*******************************************************************************/
UINT8 I2CM_RecvData(void)
{
	UINT16 timecount;
		
	I2CM_CMD(I2CM_CMD_RX_ACK);  												/* 接收一个数据并回复ACK */
	
	/* 主机操作完成 */
	while(!(I2CM_STAT & 0X80))												
	{
		timecount++;
		if(timecount > 5000)
		{
			return(0xFF);
		}
		mDelayuS(1);	
	}	
	
	I2CM_STAT |= 0x80;  														/* 清中断标志 */
	return((UINT8)I2CM_DATA);
}

/*******************************************************************************
* Function Name  : I2C_Interrupt
* Description    : I2C中断服务函数
* Input          : None
* Return         : None
*******************************************************************************/
void I2C_Interrupt(void) interrupt 11
{
	IE_PWM_I2C = 0;																/* 屏蔽中断 */
	
	if(I2CX_INT & 0x20)
	{
		/* I2C中断 */
		switch(I2CX_INT & 0xc0)
		{
			case 0x00:   
				I2CS_STAT |= 0xF0;
				break;
		}
	}
	
	IE_PWM_I2C = 1;                              /* 开启中断 */
}


