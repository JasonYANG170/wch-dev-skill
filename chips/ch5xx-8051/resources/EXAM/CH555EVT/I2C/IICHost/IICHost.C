/********************************** (C) COPYRIGHT *******************************
* File Name          : IICHost.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : 模拟IIC主机
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "IICHost.h"														

#pragma  NOAREGS



/*******************************************************************************
* Function Name  : I2C_Start
* Description    : 模拟I2C起始信号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Start()
{
  SDA=1;
	mDelayuS(10);
	SCL=1;
  mDelayuS(10);
 	SDA=0;
	mDelayuS(10);
	SCL=0;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : 模拟I2C停止信号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Stop()
{
    SCL=0;
		mDelayuS(10);
    SDA=0;
    mDelayuS(10);
    SCL=1;
		mDelayuS(10);
    SDA=1;
    mDelayuS(10);
		SCL=0;
}

//产生ACK应答
void I2C_Ack(void)
{
	SCL=0;
	mDelayuS(10);
	SDA=0;
	mDelayuS(10);
	SCL=1;
	mDelayuS(10);
	SCL=0;
}

//不产生ACK应答		    
void I2C_NAck(void)
{
	SCL=0;
	mDelayuS(10);
	SDA=1;
	mDelayuS(10);
	SCL=1;
	mDelayuS(10);
	SCL=0;
}	

//等待从机ACK应答
UINT8 I2C_Wait_Ack(void)
{
	UINT8 ucErrTime=0;
	SDA=1;
	mDelayuS(2);
	SCL=1;
  mDelayuS(2);	 
	while(SDA)
	{
		ucErrTime++;
		if(ucErrTime>200)
		{
			I2C_Stop();
			return 1;
		}
	}
	SCL=0;//时钟输出0 
	mDelayuS(1);
	return 0;  
} 

//IIC发送一个字节	  
void I2C_Send_Byte(UINT8 txd)
{                        
    UINT8 t;  
    SCL=0;//拉低时钟开始数据传输
		mDelayuS(10);
    for(t=0;t<8;t++)
    {              
        SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		mDelayuS(10);   //对TEA5767这三个延时都是必须的
		SCL=1;
		mDelayuS(10); 
		SCL=0;	
		mDelayuS(10);
    }	 
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
UINT8 I2C_Read_Byte(UINT8 ack)
{
	unsigned char i,receive=0;
    for(i=0;i<8;i++ )
	{
		SCL=0; 
		mDelayuS(10);
		SCL=1;
		mDelayuS(10);
		receive<<=1;
		if(SDA)receive++;   
		mDelayuS(1); 
  }	
	SCL=0;
	if (!ack)
			I2C_NAck();//发送nACK
	else
			I2C_Ack(); //发送ACK   
	mDelayuS(10); 
	return receive;
}