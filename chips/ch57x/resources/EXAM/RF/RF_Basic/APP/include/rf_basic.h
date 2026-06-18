/********************************** (C) COPYRIGHT *******************************
* File Name          : RF_basic.h
* Author             : WCH
* Version            : V1.0
* Date               : 2022/03/10
* Description        : 
*******************************************************************************/

#ifndef __RF_BASIC_H
#define __RF_BASIC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <CH572rf.h>
#include "CH57x_common.h"

#define   ACCESS_ADR   0x71762345
#define   CRC_INIT     0X555555
#define   CRC_POLY     0x80032d

#define  RSSI_OFFSET          4           // RSSI数据偏移量，不可修改

#define PKT_DET_CFG4( var )     { (*((PUINT32V)0x4000C120))= var; } // Demodulation parameter

typedef struct
{
    uint32_t errContinue;
    uint32_t errCount;
    uint32_t txCount;
    uint32_t rxCount;
    uint8_t testCount;
    uint8_t testData;
    int8_t  rssi;
    int8_t  rssiMax;
    int8_t  rssiMin;
    uint8_t boundEst;
    uint8_t boundConnect;
} testBound_t;

void RFRole_Init(void);
void RF_LowPower( uint32_t time);
void RF_ProcessRx( void );

#ifdef __cplusplus
}
#endif

#endif
