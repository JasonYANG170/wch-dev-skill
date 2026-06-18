/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : GPIO设置与使用和GPIO中断使用示例   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                                 
#include "Debug.H"
#include "GPIO.H"

#pragma  NOAREGS

/* Global Variable */ 
sbit LED = P2^2;

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void) 
{
	mDelaymS(50);
	CfgFsys();                                                                //CH557时钟选择配置       
	mInitSTDIO();                                                             //串口0初始化
	printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
  printf("EXAM GPIO...\n"); 
	
	/* 配置GPIO */
	GPIO_Init(PORT2,PIN2,MODE1);                                              //P2.2推挽输出                                        
  GPIO_Init(PORT0,PIN3,MODE3);                                              //P03上拉输入
	GPIO_Init(PORT1,PIN5,MODE3);                                              //P15上拉输入
  GPIO_Init(PORT3,PIN2,MODE3);                                              //P32(INT0)上拉输入
  GPIO_Init(PORT3,PIN3,MODE3);                                              //P33(INT1)上拉输入
	GPIO_Init(PORT4,PIN0,MODE3);                                              //P40上拉输入
	GPIO_Init(PORT4,PIN1,MODE3);                                              //P41上拉输入
	
  /* 配置外部中断 */	
	GPIO_INT_Init((INT_P03_L|INT_P15_L|INT_P4x_L|INT_INT0_L|INT_INT1_L),INT_EDGE,Enable); //外部中断配置03\15\INT0\INT1\40\41
	
	while(1){
		LED = ~LED;
		mDelaymS(100);
	}
}