/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : I2C从机FLASH模拟EEPROM使用示例，中断收发数据
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                                
#include "Debug.H"
#include "CH555.H"
#include "Flash.H"

#pragma  NOAREGS

/* Global Variable */ 
#define Data_Size   256									 //模拟EEPROM的大小
UINT8X buf[Data_Size] _at_ 0x0000;

UINT8 I2CS_Flag = 0;                     //传输完成标志位，当接收到STOP信号后，该标志位置1
UINT8	Recv_Flag = 0;	                   //接收数据标志位

void I2CS_Init(){
	P0_MOD_OC |= 0X03;										 //开漏输出，使能上拉电阻
	P0_DIR_PU |= 0X03;	
	I2CS_DEV_A = 0xA0;//从机设备地址
	I2CS_CTRL = 0xE9;
}
/*******************************************************************************
* Function Name  : I2C_Interrupt
* Description    : 硬件I2CS中断函数
* Input          : None
* Return         : None
*******************************************************************************/
void I2C_Interrupt(void) interrupt 11
{
	UINT8 *status;
	UINT8 *bitaddr;
	UINT8 *datas;		
	
#ifdef I2CS_DEBUG
    UINT8 i;
#endif
    if(I2CX_INT & bI2CS_INT_ACT)                   //I2CS0中断 
		{
			status=&I2CS_STAT;
			bitaddr = &I2CS_ADDR; 
			datas= &I2CS_DATA;
			switch(*status&MASK_I2CS_STAT)
			{
				case 3:        										         //对接收到的数据地址进行应答
					*datas = *(PUINT8C)(DATA_FLASH_ADDR+I2CS_ADDR);	//IIC数据收发寄存器值等于(DATA_FLASH+数据地址)处的值
				break;
				case 5:        										         //对接收数据字节进行应答
					Recv_Flag=1;
					buf[I2CS_ADDR]=*datas;
					mDelayuS(100);
					break;
				case 7:        										         //等待和检查发送数据后的应答
					Recv_Flag=2;
				break;
			}      
			if((I2CS_STAT&MASK_I2CS_STAT) == 0x0C)       //stop 一次传输完成
			{
					I2CS_Flag = 1; 
			}
			I2CS_STAT &= 0xF0;                           //清I2CS0中断
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
	UINT8  i=0;
	mDelaymS(50);
	CfgFsys();                                                                //CH555时钟选择配置       
	mInitSTDIO();                                                             //串口0初始化
	
	printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
	printf("EXAM I2C...\n");

	/* Erase dataflash */
	for(i=0;i<(Data_Size/64);i++)
			FlashErasePage(DATA_FLASH_ADDR+i*64);
	memset(buf,0x00,Data_Size); 
	
	I2CS_Init();//从机初始化
	IE_PWM_I2C=1;
	E_DIS=0;
	EA=1;
  while(1)
	{
			if(I2CS_Flag==1&&Recv_Flag==1)
			{
				for(i=0;i<32;i++){
					if( FlashProgByte(DATA_FLASH_ADDR+i,buf[i])){				//如果将写入FLASH的函数放入中断接收数据中，
						printf("------------ FlashProgByte error\n");			//则模拟IIC主机写入速率需要降低
					}
					else
						printf("buf:%02x\n",(UINT16)buf[i]);
				}
				I2CS_Flag=0;
				Recv_Flag = 0;
			}
	}
}