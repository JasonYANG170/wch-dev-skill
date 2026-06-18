/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v4x7_psram.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : This file contains all the functions prototypes for the PSRAM
*                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32v4x7.h"



/*PSRAM Rst CMD */
#define CMD_MR_RESET          ((uint32_t)(PSRAM->CMD1_CFG>>16)&0xFF)

/*PSRAM MR_ADDR select*/
#define CMD_MR_ADDR_0        ((uint32_t)0x00)
#define CMD_MR_ADDR_1        ((uint32_t)0x01)
#define CMD_MR_ADDR_2        ((uint32_t)0x02)
#define CMD_MR_ADDR_4        ((uint32_t)0x04)
#define CMD_MR_ADDR_6        ((uint32_t)0x06)
#define CMD_MR_ADDR_8        ((uint32_t)0x08)

/*PSRAM CMD select*/
#define CMD_MR_READ          ((uint32_t)((PSRAM->CMD1_CFG)>>0)&0xFF)
#define CMD_MR_WRITE         ((uint32_t)((PSRAM->CMD1_CFG)>>8)&0xFF)


/*PSRAM high speed select*/
#define  Hfreq_En            ((uint32_t)0x01)


/*PSRAM  speed set*/
#define Fre_66M              ((uint32_t)0x42)
#define Fre_109M             ((uint32_t)0x6D)
#define Fre_133M             ((uint32_t)0x85)
#define Fre_166M             ((uint32_t)0xA6)
#define Fre_200M             ((uint32_t)0xC8)
#define Fre_225M             ((uint32_t)0xE1)
#define Fre_250M             ((uint32_t)0xFA)
#define Fre_300M             ((uint32_t)0x12C)
#define Fre_333M             ((uint32_t)0x14D)
#define Fre_400M             ((uint32_t)0x190)
/*PSRAM  speed select*/
#define MR4_Write_66M        ((uint32_t)0x0)
#define MR4_Write_109M       ((uint32_t)0x4)
#define MR4_Write_133M       ((uint32_t)0x2)
#define MR4_Write_166M       ((uint32_t)0x6)
#define MR4_Write_200M       ((uint32_t)0x1)
#define MR4_Write_225M       ((uint32_t)0x5)
#define MR4_Write_250M       ((uint32_t)0x3)
#define MR4_Write_300M       ((uint32_t)0x7)
#define MR4_Write_333M       ((uint32_t)0x0)
#define MR4_Write_400M       ((uint32_t)0x4)

#define MR0_Read_66M         ((uint32_t)0x0)
#define MR0_Read_109M        ((uint32_t)0x1)
#define MR0_Read_133M        ((uint32_t)0x2)
#define MR0_Read_166M        ((uint32_t)0x3)
#define MR0_Read_200M        ((uint32_t)0x4)
#define MR0_Read_225M        ((uint32_t)0x5)
#define MR0_Read_250M        ((uint32_t)0x6)
#define MR0_Read_300M        ((uint32_t)0x7)
#define MR0_Read_333M        ((uint32_t)0x0)
#define MR0_Read_400M        ((uint32_t)0x1)

/*PSRAM  Latency select*/
#define Latency_66M        ((uint32_t)0x3)
#define Latency_109M       ((uint32_t)0x4)
#define Latency_133M       ((uint32_t)0x5)
#define Latency_166M       ((uint32_t)0x6)
#define Latency_200M       ((uint32_t)0x7)
#define Latency_225M       ((uint32_t)0x8)
#define Latency_250M       ((uint32_t)0x9)
#define Latency_300M       ((uint32_t)0xb)
#define Latency_333M       ((uint32_t)0xc)
#define Latency_400M       ((uint32_t)0x10)

/*PSRAM  Read Latency Type select*/
#define Read_Variable        ((uint32_t)0x0)
#define Read_Fixed           ((uint32_t)0x1)



void WriteReg(uint32_t Addr, uint32_t Data);
uint32_t ReadReg(uint32_t Addr);
void SetWrLatency(uint32_t PSRAM_Fre, uint32_t Write_Latency);
void SetRdLatency(uint32_t PSRAM_Fre, uint32_t Read_Latency,uint32_t Read_Latency_Type);
void PSRAMDMAWrite(uint32_t *pBuffer,uint32_t WriteAddr,uint32_t n,uint32_t dataWidth,uint32_t ONEBL,uint32_t TOWBL_TOUT);
void PSRAMDMARead(uint32_t *pBuffer,uint32_t ReadAddr,uint32_t n,uint32_t dataWidth,uint32_t ONEBL,uint32_t TOWBL_TOUT);
void Wait_Busy_0();
void Global_RST_Set();
