# DFSDM Digital Filter

## Overview

CH32H417 has a DFSDM (Digital Filter for Sigma-Delta Modulator) peripheral for high-resolution analog-to-digital conversion using external sigma-delta modulators or internal ADC data. Supports multiple channels, filters (Sinc1-Sinc5), oversampling, DMA, and various data sources (external serial, internal ADC, parallel).

## Key API Functions

```c
void DFSDM_ChannelStructInit(DFSDM_ChannelInitTypeDef* DFSDM_ChannelInitStruct);
void DFSDM_FilterStructInit(DFSDM_FilterInitTypeDef* DFSDM_FilterInitStruct);
void DFSDM_RcStructInit(DFSDM_RcInitTypeDef* DFSDM_RcInitStruct);
void DFSDM_ChannelInit(uint32_t DFSDM_Channel, DFSDM_ChannelInitTypeDef* DFSDM_ChannelInitStruct);
void DFSDM_FilterInit(DFSDM_FLT_TypeDef* DFSDM_FLTx, DFSDM_FilterInitTypeDef* DFSDM_FilterInitStruct);
void DFSDM_RcInit(DFSDM_FLT_TypeDef* DFSDM_FLTx, DFSDM_RcInitTypeDef* DFSDM_RcInitStruct);
void DFSDM_ChannelCmd(uint32_t DFSDM_Channel, FunctionalState NewState);
void DFSDM_FilterCmd(DFSDM_FLT_TypeDef* DFSDM_FLTx, FunctionalState NewState);
void DFSDM_Cmd(FunctionalState NewState);
void DFSDM_RcSoftStartConversion(DFSDM_FLT_TypeDef* DFSDM_FLTx);
FlagStatus DFSDM_GetFlagStatus(DFSDM_FLT_TypeDef* DFSDM_FLTx, uint32_t DFSDM_FLAG);
void DFSDM_WriteParallelDataStanMode(uint32_t DFSDM_Channel, int16_t Data);
```

## Internal ADC Data Filter Example (Standard Mode)

```c
#include "ch32h417.h"

#define FLT_Sinc_Order    DFSDM_FLT_Sinc3
#define FLT_Over_Sample   32
#define RxBuffSize        10
#define TxbuffSize        ((FLT_Over_Sample * (2 + FLT_Sinc_Order + 1) + 2) * RxBuffSize)

int16_t Txdata[TxbuffSize] = {0};
int32_t Rxdata[RxBuffSize] = {0};

void DFSDM_Internal_Init(void)
{
    DFSDM_ChannelInitTypeDef DFSDM_ChannelInitStructure = {0};
    DFSDM_FilterInitTypeDef DFSDM_FilterInitStructure = {0};
    DFSDM_RcInitTypeDef DFSDM_RcInitStructure = {0};

    // Enable DFSDM clock
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_DFSDM, ENABLE);

    // Initialize structures
    DFSDM_ChannelStructInit(&DFSDM_ChannelInitStructure);
    DFSDM_FilterStructInit(&DFSDM_FilterInitStructure);
    DFSDM_RcStructInit(&DFSDM_RcInitStructure);

    // Channel 0: internal input, standard mode
    DFSDM_ChannelInitStructure.DFSDM_ChDataPackMode = DFSDM_StandardMode;
    DFSDM_ChannelInitStructure.DFSDM_ChDataMultiplexer = DFSDM_InternalInput;
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectCurrent;
    DFSDM_ChannelInitStructure.DFSDM_ChDataRightBitShift = 3;
    DFSDM_ChannelInit(DFSDM_Channel0, &DFSDM_ChannelInitStructure);

    // Filter 0: Sinc3, 32x oversampling
    DFSDM_FilterInitStructure.DFSDM_FltSincOrder = DFSDM_FLT_Sinc3;
    DFSDM_FilterInitStructure.DFSDM_FltOverSample = FLT_Over_Sample;
    DFSDM_FilterInitStructure.DFSDM_FltIntegratorOverSample = 1;
    DFSDM_FilterInit(DFSDM_FLT0, &DFSDM_FilterInitStructure);

    // Regular conversion: channel 0, continuous, DMA enabled
    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel0;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcSynchronousMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT0, &DFSDM_RcInitStructure);

    // Enable channel, filter, and interface
    DFSDM_ChannelCmd(DFSDM_Channel0, ENABLE);
    DFSDM_FilterCmd(DFSDM_FLT0, ENABLE);
    DFSDM_Cmd(ENABLE);
}
```

## DFSDM with DMA

```c
void DFSDM_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (int32_t)&DFSDM_FLT0->RDATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (int32_t)Rxdata;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = RxBuffSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // DMA MUX for DFSDM_FLT0 = 107
    DMA_MuxChannelConfig(DMA_MuxChannel1, 107);
}
```

## Data Processing

```c
void DFSDM_Process(void)
{
    uint16_t i;

    // Generate test data
    int8_t m = -10;
    for (i = 0; i < TxbuffSize; i++)
    {
        Txdata[i] = m;
        if ((i + 1) % (TxbuffSize / RxBuffSize) == 0)
            m++;
    }

    DFSDM_Internal_Init();
    DFSDM_DMA_Init();
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // Start conversion
    DFSDM_RcSoftStartConversion(DFSDM_FLT0);

    // Write parallel data
    for (i = 0; i < TxbuffSize; i++)
    {
        while (DFSDM_GetFlagStatus(DFSDM_FLT0, DFSDM_FLAG_FLTx_RCIP) == RESET);
        DFSDM_WriteParallelDataStanMode(DFSDM_Channel0, Txdata[i]);
    }

    // Wait for DMA completion
    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC1) == RESET);

    // Process results (right-shift by 8 for 24-bit to 16-bit)
    for (i = 0; i < RxBuffSize; i++)
    {
        printf("%d\r\n", (Rxdata[i] >> 8));
    }
}
```

## DFSDM Filter Orders

| Macro | Description | Decimation |
|-------|-------------|------------|
| `DFSDM_FLT_Sinc1` | Sinc filter order 1 | N |
| `DFSDM_FLT_Sinc2` | Sinc filter order 2 | N^2 |
| `DFSDM_FLT_Sinc3` | Sinc filter order 3 | N^3 |
| `DFSDM_FLT_Sinc4` | Sinc filter order 4 | N^4 |
| `DFSDM_FLT_Sinc5` | Sinc filter order 5 | N^5 |

## DFSDM Data Sources

| Macro | Description |
|-------|-------------|
| `DFSDM_InternalInput` | Internal ADC data |
| `DFSDM_ExternalInput` | External serial input (SPI-like) |
| `DFSDM_ParallelInput` | Parallel data input |

## DFSDM Channels

| Channel | Description |
|---------|-------------|
| `DFSDM_Channel0` | DFSDM channel 0 |
| `DFSDM_Channel1` | DFSDM channel 1 |
| `DFSDM_Channel2` | DFSDM channel 2 |
| `DFSDM_Channel3` | DFSDM channel 3 |

## DMA MUX for DFSDM

| Filter | MUX Value |
|--------|-----------|
| DFSDM_FLT0 | 107 |
| DFSDM_FLT1 | 108 |
