/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : 模拟I2C主机收发数据使用示例 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "CH555.H"
#include "DEBUG.H"	
#include "IICHost.h"

UINT8 IICdata[32]={
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x18,0x19,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x31,0x32,0x33,0x34,0x35};

/*******************************************************************************
* Function Name  : AT24CXX_WRITE_BYTE
* Description    : 在AT24CXX的指定地址写一字节
* Input          : Addr：指定地址
* 		             Data：写入数据
* Return         : None
*******************************************************************************/
void AT24CXX_WRITE_BYTE(UINT8 Addr,UINT8 Data){
	
	I2C_Start();
	I2C_Send_Byte(0XA0);
	I2C_Wait_Ack();
	I2C_Send_Byte(Addr);
	I2C_Wait_Ack();
	I2C_Send_Byte(Data);
	I2C_Wait_Ack();
	I2C_Stop();
}

/*******************************************************************************
* Function Name  : AT24CXX_READ_BYTE
* Description    : 在AT24CXX的指定地址读一字节
* Input          : Addr：指定地址
* Return         : temp：读出字节
*******************************************************************************/
UINT8 AT24CXX_READ_BYTE(UINT8 Addr){
	UINT8 temp = 0;
	
	I2C_Start();
	I2C_Send_Byte(0XA0);
	I2C_Wait_Ack();
	I2C_Send_Byte(Addr);
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte(0XA1);
	I2C_Wait_Ack();
	temp = I2C_Read_Byte(0);
	I2C_Stop();
	return temp;
}

/*******************************************************************************
* Function Name  : AT24CXX_WriteLenByte
* Description    : 在AT24CXX的指定地址开始写入指定个数的数据
* Input          : WriteAddr： 开始写入的地址
* 		             pBuffer：  数据数组首地址
* 		             NumToRead：要写入数据的个数,最大数据量=32
* Return         : None
*******************************************************************************/
void AT24CXX_WriteLenByte(UINT8 WriteAddr, UINT8 *pBuffer, UINT8 NumToWrite)
{
    while (NumToWrite--)
    {
        AT24CXX_WRITE_BYTE(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

/*******************************************************************************
* Function Name  : AT24CXX_ReadLenByte
* Description    : 在AT24CXX的指定地址开始读出指定个数的数据
* Input          : ReadAddr： 指定地址
* 		             pBuffer：  数据数组首地址
* 		             NumToRead：要读出数据的个数
* Return         : None
*******************************************************************************/
void AT24CXX_ReadLenByte(UINT8 ReadAddr, UINT8 *pBuffer, UINT8 NumToRead)
{
    while (NumToRead)
    {
        *pBuffer++ = AT24CXX_READ_BYTE(ReadAddr++);
        NumToRead--;
    }
}

void main(void)
{
	UINT8 i;
	UINT8 ReadNum;
	ReadNum=20;
	
	CfgFsys();   
  mInitSTDIO();
	printf("AA\r\n");	
	P0_MOD_OC = 0X03;				//开漏输出，使能上拉电阻
	P0_DIR_PU = 0X03;
	printf("dd:%x\n",(UINT16)CHIP_ID);
	
	AT24CXX_WriteLenByte(0,&IICdata,16);
	mDelaymS(500);
	AT24CXX_ReadLenByte(0,&IICdata,ReadNum);
	for(i=0;i<ReadNum;i++)
	{
		printf("%02x ",(UINT16)IICdata[i]);
	}
	
	while(1);
}

