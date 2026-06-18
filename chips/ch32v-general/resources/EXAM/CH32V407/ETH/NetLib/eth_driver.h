/********************************** (C) COPYRIGHT ************* ******************
* File Name          : eth_driver.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/05
* Description        : This file contains the headers of the ETH Driver.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __ETH_DRIVER__
#define __ETH_DRIVER__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "debug.h"
#include "wchnet.h"

#define PHY_ADDRESS                             1

#define ROM_CFG_USERADR_ID                      0x1FFFF7E8

#ifndef WCHNETTIMERPERIOD
#define WCHNETTIMERPERIOD                       10   /* Timer period, in Ms. */
#endif

extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern SOCK_INF SocketInf[ ];

void WCHNET_ETHIsr(void);
void ETH_PHYLink( void );
void WCHNET_MainTask(void);
void ETH_Init( uint8_t *macAddr );
void WCHNET_GetMacAddr( uint8_t *p );
void WCHNET_TimeIsr( uint16_t timperiod );
void ETH_Configuration( uint8_t *macAddr );
uint8_t ETH_LibInit( uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *macaddr);

#ifdef __cplusplus
}
#endif

#endif
