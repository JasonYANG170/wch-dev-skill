/********************************** (C) COPYRIGHT *******************************
 * File Name          : RF_basic.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2025/06/24
 * Description        : 2.4G库基本模式收发测试例程
 *
 *                      1. 功率设置
 *                      txPowerVal，支持-25dBm ~ +7dBm 动态调整
 *
 *                      2.发送状态切换稳定时间
 *                      waitTime，如果需要切换通道发送，稳定时间不低于80us
 *
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "rf_basic.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
#define  ALIGNED4(x)       ((x+3)/4*4)

rfipTx_t gTxParam;
rfipRx_t gRxParam;

__attribute__((__aligned__(4))) uint8_t TxBuf[64];
__attribute__((__aligned__(4))) uint8_t RxBuf[264]; // 接收DMA buf不能小于264字节

#define  MODE_RX     0
#define  MODE_TX     1

#define  WAIT_ACK         0               // 是否使能ACK
#define  TEST_DATA_LEN    4               // 数据长度
#define  TEST_FREQUENCY   16              // 通信频点

#define  TEST_MODE     MODE_TX            // 发送模式
//#define  TEST_MODE     MODE_RX            // 接收模式

#define  RF_DEVICE_PERIDOC    1000

uint32_t volatile gTxCount;
uint32_t volatile gRxCount;
int32_t  volatile Rssi_sum=0;

int8_t gRssi=0;

/******************************** 发送相关函数 ********************************/
/**
 * @brief   配置发送的频点
 *
 * @param   f - 需要配置的频点
 *
 * @return  None.
 */
__HIGH_CODE
void rf_tx_set_frequency( uint32_t f )
{
    gTxParam.frequency = f;
}

/*******************************************************************************
 * @brief   配置发送的地址
 *
 * @param   sync_word - 需要配置的接入地址
 *
 * @return  None.
 */
__HIGH_CODE
void rf_tx_set_sync_word( uint32_t sync_word )
{
    gTxParam.accessAddress = sync_word;
}

/*******************************************************************************
 * @brief   rf发送数据子程序
 *
 * @param   pBuf - 发送的DMA地址
 *
 * @return  None.
 */
__HIGH_CODE
void rf_tx_start( uint8_t *pBuf )
{
    //    RFRole_Stop();
    // 配置发送的频点
    gTxParam.frequency = TEST_FREQUENCY;
    // 发送的DMA地址
    gTxParam.txDMA = (uint32_t)pBuf;
    gTxParam.waitTime = 40*2; // 如果需要切换通道发送，稳定时间不低于80us
    RFIP_StartTx( &gTxParam );
}

/******************************** 接收相关函数 ********************************/
/**
 * @brief   配置接收的地址
 *
 * @param   sync_word - 需要配置的接入地址
 *
 * @return  None.
 */
__HIGH_CODE
void rf_rx_set_sync_word( uint32_t sync_word )
{
    gRxParam.accessAddress = sync_word;
}

/*******************************************************************************
 * @fn      rf_rx_set_frequency
 *
 * @param   f - 需要配置的频点
 *
 * @return  None.
 */
__HIGH_CODE
void rf_rx_set_frequency( uint32_t f )
{
    gRxParam.frequency = f;
}

/*******************************************************************************
 * @fn      rf_rx_start
 *
 * @brief   rf接收数据子程序
 *
 * @return  None.
 */
__HIGH_CODE
void rf_rx_start( void )
{
    // 配置发送的频点
    gRxParam.frequency = TEST_FREQUENCY;
    // 配置接收的超时时间，0则无超时
    gRxParam.timeOut = 0;
    RFIP_SetRx( &gRxParam );
}

/*******************************************************************************
 * @fn      rf_rx_process_data
 *
 * @brief   rf接收数据处理
 *
 * @return  None.
 */
__HIGH_CODE
void rf_rx_process_data( void )
{
    gRxCount ++;
    {
        uint8_t *pData = (uint8_t *)gRxParam.rxDMA;
#if(TEST_MODE == MODE_RX)               // rx
        Rssi_sum += (int8_t)pData[TEST_DATA_LEN+RSSI_OFFSET];
#else
        #if( WAIT_ACK )                 // tx ack
            uint8_t AckLen = pData[1];
            gRssi = (int8_t)pData[AckLen+RSSI_OFFSET];
        #endif
#endif
//     for( int i=0;i<TEST_DATA_LEN;i++ )
//     {
//         PRINT("%x\n",pData[i]);
//     }
//     PRINT("\n");

    }
}

/*******************************************************************************
 * @fn      LLE_IRQHandler
 *
 * @brief   LLE_IRQHandler
 *
 * @return  None.
 */
__INTERRUPT
__HIGH_CODE
void LLE_IRQHandler( void )
{
    LLE_LibIRQHandler( );
}

/*******************************************************************************
 * @fn      BB_IRQHandler
 *
 * @brief   BB_IRQHandler
 *
 * @return  None.
 */
__INTERRUPT
__HIGH_CODE
void BB_IRQHandler( void )
{
    BB_LibIRQHandler( );
}

/*******************************************************************************
 * @fn      RF_ProcessCallBack
 *
 * @brief   rf中断处理程序
 *
 * @param   sta - 中断状态.
 *          id - 保留
 *
 * @return  None.
 */
__HIGH_CODE
void RF_ProcessCallBack( rfRole_States_t sta,uint8_t id )
{
    if( sta&RF_STATE_RX )
    {
        rf_rx_process_data();
#if( TEST_MODE == MODE_RX )
#if( WAIT_ACK )
        TxBuf[0] = 0x0d;
        TxBuf[1] = 0;
        rf_tx_start( TxBuf );
#else
        rf_rx_start( );
#endif
#endif
    }
    if( sta&RF_STATE_RX_CRCERR )
    {
#if( TEST_MODE == MODE_RX )
        rf_rx_start( );
#endif
    }
    if( sta&RF_STATE_TX_FINISH )
    {
#if( WAIT_ACK )
        rf_rx_start( );
#endif
        gTxCount ++;
    }
    if( sta&RF_STATE_TIMEOUT )
    {
        PRINT("error.\n");   // rx地址匹配但接收超时，如果是发送则是发送失败
#if( TEST_MODE ==  MODE_RX )
        rf_rx_start( );
#endif
    }
}

/*********************************************************************
 * @fn      TMR0_IRQHandler
 *
 * @brief   TMR0中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR_IRQHandler(void) // TMR 定时中断
{
    if(TMR_GetITFlag(TMR_IT_CYC_END))
    {
        TMR_ClearITFlag(TMR_IT_CYC_END); // 清除中断标志
#if( TEST_MODE == MODE_RX )
        PRINT("rx %d, rssi %d\n",gRxCount, (Rssi_sum/(int)gRxCount));
        gRxCount = 0;
        Rssi_sum = 0;
#else
        // 初始化发送的数据
        TxBuf[0] = 0x55;
        TxBuf[1] = TEST_DATA_LEN;        // 此字节固定为数据长度，从此往后有TEST_DATA_LEN字节数据
        TxBuf[2] ++;
        TxBuf[3] = 3;
        TxBuf[4] = 4;
        TxBuf[5] = gRssi;
        rf_tx_start( TxBuf );
#endif
    }
}

/*******************************************************************************
 * @fn      RFRole_Init
 *
 * @brief   RF应用层初始化
 *
 * @param   None.
 *
 * @return  None.
 */
void RFRole_Init(void)
{
    sys_safe_access_enable( );
    R32_MISC_CTRL = (R32_MISC_CTRL&(~(0x3f<<24)))|(0xe<<24);
    sys_safe_access_disable( );
    PKT_DET_CFG4(0x3f);
    {
        rfRoleConfig_t conf ={0};
        conf.rfProcessCB = RF_ProcessCallBack;
        conf.processMask = RF_STATE_RX|RF_STATE_RX_CRCERR|RF_STATE_TX_FINISH|RF_STATE_TIMEOUT;
        RFRole_BasicInit( &conf );
    }
    TPROPERTIES_CFG Properties;
    {
        Properties.cfgVal = PHY_MODE_PHY_2M;        // BLE 2M模式
        PRINT("cfgVal=%x\n",Properties.cfgVal);
    }

    // RX相关参数，全局变量
    {
        gRxParam.accessAddress = ACCESS_ADR;        // Rx接入地址
        gRxParam.crcInit = CRC_INIT;
        gRxParam.crcPoly = CRC_POLY;
        gRxParam.properties = Properties.cfgVal;
        gRxParam.rxDMA = (uint32_t)RxBuf;
        gRxParam.rxMaxLen = TEST_DATA_LEN;
    }

    // TX相关参数，全局变量
    {
        gTxParam.accessAddress = ACCESS_ADR;         // Tx接入地址
        gTxParam.crcInit = CRC_INIT;
        gTxParam.crcPoly = CRC_POLY;
        gTxParam.properties = Properties.cfgVal;
        gTxParam.waitTime = 80*2;
        gTxParam.txPowerVal = LL_TX_PWR_6_DBM;    // Tx发射功率
        gTxParam.txLen = TEST_DATA_LEN;
    }

    PFIC_EnableIRQ( BLEB_IRQn );
    PFIC_EnableIRQ( BLEL_IRQn );

#if( TEST_MODE == MODE_RX )
    PRINT("----------------- rx -----------------\n");
    gTxCount = 0;
    gRxCount = 0;
    PRINT("start rx...%d\n",GetSysClock());
    rf_rx_start();
    PRINT("rx channel...%d\n",gRxParam.frequency);
    TMR_TimerInit( GetSysClock() / 2 );             // 500ms统计一次
    TMR_ITCfg(ENABLE, TMR_IT_CYC_END);// 开启中断
    PFIC_EnableIRQ(TMR_IRQn);
#else
    PRINT("----------------- tx -----------------\n");
    gTxCount = 0;
    gRxCount = 0;
    PRINT("start tx timer...\n");
    TMR_TimerInit( GetSysClock() / RF_DEVICE_PERIDOC );
    TMR_ITCfg(ENABLE, TMR_IT_CYC_END); // 开启中断
    PFIC_EnableIRQ(TMR_IRQn);
#endif
}

/******************************** endfile @rf ******************************/
