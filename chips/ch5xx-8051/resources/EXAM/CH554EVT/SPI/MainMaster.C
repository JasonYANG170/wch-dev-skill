
/********************************** (C) COPYRIGHT *******************************
* File Name          : MainMaster.C
* Author             : WCH
* Version            : V1.1
* Date               : 2019/07/22
* Description        : CH554 SPI主机和CH554 SPI从机通讯，主机发送数据，从机取反返回
                       从机参考MainSlave.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\Public\CH554.H"                                                  
#include "..\Public\Debug.H"
#include "SPI.H"
#include "stdio.h"
#include <string.h>

/*硬件接口定义*/
/******************************************************************************
使用CH554 硬件SPI接口 
         CH554               
         P1.4        =       SCS
         P1.5        =       MOSI
         P1.6        =       MISO
         P1.7        =       SCK
*******************************************************************************/


void main()
{ 
    UINT8 ret,i=0;
    CfgFsys( ); 
    mDelaymS(5);                                                               //调整主频，建议稍加延时等待内部时钟稳定   
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n");  
	
    SPIMasterModeSet(3);                                                       //SPI主机模式设置，模式3
	
    SPI_CK_SET(4);                                                             //4分频
#ifdef SPI0Interrupt
    CH554SPIInterruptInit();                                                   //SPI中断初始化
    EA  = 1;                                                                   //使能全局中断
#endif
    mDelaymS(100);
    while(1)
    {   
	      SCS = 0;                                                               //SPI主机发送数据
        CH554SPIMasterWrite(i);                                                      
        mDelaymS(5);
        ret = CH554SPIMasterRead();                                            //接收SPI从机返回的数据，取反返回
        SCS = 1;
        if(ret != (i^0xff))
        {
		        printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //如果不等于发送数据的取反，打印错误信息					
        }
		    mDelaymS(50);
	     	i = i+1;
        if((i%40)==0)
        {
            printf("success %02x\n",(UINT16)i);                                //每成功40次打印一次										
        }
    }
}