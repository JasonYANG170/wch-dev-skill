/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH555 SPI
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

#include "CH555.H"
#include "Debug.H"


//#define  SPI_INTERRUPT                                                      //ÖĐ¶ÏÊ¹ÄÜ£¨½öSlaveÄ£Ê½£©
#define  SPI_CK_SET(n) (SPI0_CK_SE = n)                                     //SPIÊ±ÖÓÉèÖĂº¯Êư 


//MasterÏà¹Øº¯Êư
extern void  SPIMasterModeSet(UINT8 mode);                                    //SPIÖ÷»úÄ£Ê½³ơÊ¼»¯
extern void  CH555SPIMasterWrite(UINT8 dat);                                  //CH555Ó²¼₫SPIĐ´Êư¾Ư£¬Ö÷»úÄ£Ê½
extern UINT8 CH555SPIMasterRead(void);                                            //CH555Ó²¼₫SPI0¶ÁÊư¾Ư£¬Ö÷»úÄ£Ê½

//SlaveÏà¹Øº¯Êư
extern void  SPISlvModeSet(void);                                                //SPI´Ó»úÄ£Ê½³ơÊ¼»¯
extern void  CH555SPISlvWrite(UINT8 dat);                                     //CH555Ó²¼₫SPIĐ´Êư¾Ư£¬´Ó»úÄ£Ê½
extern UINT8 CH555SPISlvRead(void);                                               //CH555Ó²¼₫SPI0¶ÁÊư¾Ư£¬´Ó»úÄ£Ê½

#endif
