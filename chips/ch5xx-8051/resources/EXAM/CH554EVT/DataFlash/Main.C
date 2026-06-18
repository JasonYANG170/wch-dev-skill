
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.7
* Date               : 2023/05/31
* Description        : CH554 DataFlash字节读写演示示例
                       写 DataFlash 数据区域，向目标地址写入单字节数据
                       读 DataFlash 数据区域，从目标地址读取单字节数据		   									
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "DataFlash.H"
#include "stdio.h"
#include <string.h>

/*
 * @Note
 * Whenever programming on board or downloading program via UART or USB, 5V supply  
 * voltage must be used temporarily. 
 */
 
void main( ) 
{
    UINT8 i,m,len;
    UINT16 j = 0;
    CfgFsys( );                                                                //CH554时钟选择配置   
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n"); 
    
    while(1){
      Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
      Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
      for(i=0;i<128;i++){	                                                     //循环写入128字节		
        len = WriteDataFlash(i,&i,1);                                          //向DataFlash区域偏移地址i写入i
        if(len != 1){
          printf("Write Err 次 = %02x,m = %02x\n",j,(UINT16)m);                //写出错打印				
        }
      }
      Flash_Op_Check_Byte1 = 0x00;
      Flash_Op_Check_Byte2 = 0x00;
      for(i=0;i<128;i++){                                                      //读DataFlash区域偏移地址i并校验
        len = ReadDataFlash(i,1,&m);
        if((len != 1) ||(m != i)){
          printf("Read Err 次 = %02x, = %02x,addr =%02x ,值= %02x\n",j,(UINT16)(i*2),(UINT16)ROM_DATA_L,(UINT16)m);				
        }                                                                      //读校验出错打印
      }  
      printf("$$OK %02x \n",j);			
      j++;
      mDelaymS(100);			
    }
}