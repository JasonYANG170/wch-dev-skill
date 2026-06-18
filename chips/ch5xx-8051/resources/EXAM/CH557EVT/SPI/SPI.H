/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 SPI
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

#include "CH557.H"
#include "Debug.H"


//#define  SPI_INTERRUPT                                                      //SPI中断
#define  SPI_CK_SET(n) (SPI0_CK_SE = n)                                     //SPI时钟设置 


//Master
extern void  SPIMasterModeSet(UINT8 mode);                                    
extern void  CH557SPIMasterWrite(UINT8 dat);                                  
extern UINT8 CH557SPIMasterRead(void);                                            

//Slave��غ���
extern void  SPISlvModeSet(void);                                               
extern void  CH557SPISlvWrite(UINT8 dat);                                     
extern UINT8 CH557SPISlvRead(void);                                            

#endif
