/********************************** (C) COPYRIGHT *******************************
* File Name          : MAINSLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 SPI主机例子演示，连接SPI从机进行数据收发
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "CH557X.H"
#include "Debug.H"
#include "SPI.H"

/*硬件接口定义*/
/******************************************************************************
使用CH557 硬件SPI接口
         CH557
         P1.4——SCS
         P1.5——MOSI
         P1.6——MISO
         P1.7——SCK
*******************************************************************************/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8 ret,i=0;
	CfgFsys();
	mDelaymS(5);                                                               //修改系统主频，建议稍加延时等待主频稳定
	mInitSTDIO();                                                             //串口0初始化
	printf("EXAM SPI Master\n");
	SPIMasterModeSet(3);                                                       //SPI主机模式设置，模式3
	SPI_CK_SET(24);                                                            //24分频

	while(1)
	{
		if(i>0xff) i = 0;
			
		SCS = 0;                                                               //SPI主机发送数据
		CH557SPIMasterWrite(i);
		ret = CH557SPIMasterRead();                                            //接收SPI从机返回的数据	
		SCS = 1;
		
		if(ret != (i^0xff))
		{
			printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //如果不等于发送的数据，打印错误信息
		}
		else
		{
			printf("success %02x\n",(UINT16)i);                                //每成功一次打印一次
		}
		i = i+1;

		mDelaymS(80);
	}
}


