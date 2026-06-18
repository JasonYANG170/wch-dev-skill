/********************************** (C) COPYRIGHT ************* ******************
* File Name          : eth_driver.h
* Author             : WCH
* Version            : V1.3.0
* Date               : 2022/06/02
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

 /* 1: interrupt 0: polling in RMII or RGMII mode */
#define LINK_STAT_ACQUISITION_METHOD            0

#define PHY_ADDRESS                             1

#define ETH_DMARxDesc_FrameLengthShift          16

#define ROM_CFG_USERADR_ID                      0x1FFFF7E8

#ifndef WCHNETTIMERPERIOD
#define WCHNETTIMERPERIOD                       10   /* Timer period, in Ms. */
#endif


#define ETH_TXBUFNB                             16
#define ETH_RXBUFNB                             16
#define ETH_RX_BUF_SZE                          ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SZE                          ETH_MAX_PACKET_SIZE

extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern __attribute__ ((aligned(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_TX_BUF_SZE];


#define R32_ETH_CFG0    (*((volatile uint32_t *)0x40030000))
#define R32_ETH_CFG1    (*((volatile uint32_t *)0x40030004))
#define R32_ETH_CFG2    (*((volatile uint32_t *)0x40030008))
#define R32_ETH_CFG3    (*((volatile uint32_t *)0x4003000C))
#define R32_ETH_CFG4    (*((volatile uint32_t *)0x40030010))
#define R32_ETH_CFG5    (*((volatile uint32_t *)0x40030014))
#define R32_ETH_CFG6    (*((volatile uint32_t *)0x40030018))
#define R32_ETH_CFG7    (*((volatile uint32_t *)0x4003001C))
#define R32_ETH_CFG8    (*((volatile uint32_t *)0x40030020))
#define R32_ETH_CFG9    (*((volatile uint32_t *)0x40030024))
#define R32_ETH_CFG10   (*((volatile uint32_t *)0x40030028))
#define R32_ETH_CFG11   (*((volatile uint32_t *)0x4003002C))
#define R32_ETH_ANA1    (*((volatile uint32_t *)0x40030030))
#define R32_ETH_ANA2    (*((volatile uint32_t *)0x40030034))
#define R32_ETH_OTHER   (*((volatile uint32_t *)0x40030038))

#define PHY_PAG_SEL 0x1F
void WCHNET_ETHIsr(void);
void ETH_PHYLink( void );
void WCHNET_MainTask(void);
void ETH_Init( uint8_t *macAddr );
void WCHNET_GetMacAddr( uint8_t *p );
void WCHNET_TimeIsr( uint16_t timperiod );
void ETH_Configuration( uint8_t *macAddr );
uint32_t MACRAW_Tx(uint8_t *buff, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
