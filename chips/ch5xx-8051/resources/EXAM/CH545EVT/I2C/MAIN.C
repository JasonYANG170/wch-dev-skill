/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : I2C设置与使用和I2C中断使用示例 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                              
#include "Debug.H"
#include "I2C.H"

#pragma  NOAREGS

/* Global Variable */ 

UINT8XV  EDID_Succ_Flag = 0x00;													/* EDID数据成功获取标志 */
UINT16X  EDID_Read_TiemCount = 0x00;											/* EDID数据读取数据计时 */

/* VGA显示器EDID数据缓冲区 */
UINT8 EDID_Data[128] =
{     
	/* DELL E2016H */
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x10,0xAC,0xC7,0xA0,0x49,0x48,0x37,0x34,
	0x2A,0x1B,0x01,0x04,0xA5,0x2B,0x18,0x78,0xE2,0xE8,0xF5,0xA2,0x56,0x4F,0xA1,0x28,
	0x10,0x50,0x54,0xA5,0x4B,0x00,0x71,0x4F,0x81,0x80,0xA9,0xC0,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x30,0x2A,0x40,0xC8,0x60,0x84,0x64,0x30,0x18,0x50,
	0x13,0x00,0xB0,0xEC,0x10,0x00,0x00,0x1E,0x00,0x00,0x00,0xFF,0x00,0x30,0x48,0x47,
	0x4A,0x34,0x37,0x41,0x47,0x34,0x37,0x48,0x49,0x0A,0x00,0x00,0x00,0xFC,0x00,0x44,
	0x45,0x4C,0x4C,0x20,0x45,0x32,0x30,0x31,0x36,0x48,0x0A,0x20,0x00,0x00,0x00,0xFD,
	0x00,0x32,0x4C,0x1E,0x53,0x11,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0xF2
};


/*******************************************************************************
* Function Name  : I2CM_Get_EDID
* Description    : I2C主机获取VGA显示器EDID数据
* Input          : None
* Return         : 0: 表示获取成功 
*                  1: 表示获取失败
*******************************************************************************/
UINT8 I2CM_Get_EDID(void)
{
	UINT8  i;
	UINT16 timecount;
		
	timecount = 0;
	
	while(1)
	{
		I2CM_START = 0XA0; 														/* 伪写操作 */

		/* 等待主机操作完成 */
		while(!(I2CM_STAT & 0x80))		
		{
			timecount++;
			if(timecount > 2000)
			{
				return(0x01);
			}
			mDelayuS(1);	
		}	

		I2CM_STAT |= 0x80;														/* 清中断标志 */
		if(I2CM_CTRL & 0x10)
		{
			/* 没有回复ACK */
			timecount++;
			if(timecount > 60000) 
			{
				return(0x01);													/* 超时退出 */
			}
		}
		else	
		{
			/* 返回ACK */
			break;					
		}
	}
	
	/* 读取128个字节的EDID数据 */
	I2CM_DATA = 0X00; 															/* 0地址 */
	I2CM_WaitACK();
	I2CM_START = 0XA1; 															/* 读操作 */
	I2CM_WaitACK();	
	for(i=0; i<128; i++)
	{
		EDID_Data[i] = I2CM_RecvData();
	}
	I2CM_CMD(I2CM_CMD_STOP);	
	return(0x00);	
}


/*******************************************************************************
* Function Name  : EDID_Deal
* Description    : EDID数据读取及发送处理
* Input          : time：重试次数
* Return         : None
*******************************************************************************/
void EDID_Deal(UINT8 time)
{
	UINT8  retry;
	UINT8  status;
	UINT8  i;

	I2CM_Init();																/* I2C主机初始化 */
	
  for(retry=0; retry<time; retry++)
	{
		status = I2CM_Get_EDID();	   					    /* I2C主机获取EDID数据 */
		
		if(status == 0x00)
		{
			EDID_Succ_Flag = 0x01;
			I2CM_CTRL &= ~bI2CM_EN;									/* 关闭I2C主机,似乎主从不能一起用 */
			I2CS_Init(0xA0, EDID_Data);						  /* I2C从机初始化 */

			printf("EDID_Data: \n");			
			for(i=0; i<128; i++)
			{
				printf("%02x ",(UINT16)EDID_Data[i]);
			}
			printf("\n");

			break;
		}
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void) 
{
	mDelaymS(50);
	CfgFsys();                                                                //CH545时钟选择配置       
	mInitSTDIO();                                                             //串口0初始化
	printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
	printf("EXAM I2C...\n");

	EDID_Deal(1000);
	
  while(1);
}



