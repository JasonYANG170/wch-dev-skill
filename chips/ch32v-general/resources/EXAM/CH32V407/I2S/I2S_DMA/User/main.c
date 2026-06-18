/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   I2S master transmitter slave receiver DMA routine:
      This routine demonstrates that I2S3 acts as a slave to receive,
      and I2S2 acts as a master to transmit.
 SPI2-I2S2:
     WS -- PB12
     CK -- PB13
     SD -- PB15
     MCK-- PC6

 SPI3-I2S3:
     WS -- PA15
     CK -- PB3
     SD -- PB5
     MCK-- PC7

*/

#include "debug.h"
#include "string.h"

/* Global Variable */
#define Len (10)

s16 I2S2_Tx[Len];
s16 I2S3_Rx[Len];

/*********************************************************************
 * @fn      I2S2_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef  I2S_InitStructure  = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_SPI2, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC, ENABLE);

    // WS PB12
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CK PB13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD PB15
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MCK PC6
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode       = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard   = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq  = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL       = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);
}

/*********************************************************************
 * @fn      I2S3_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef  I2S_InitStructure  = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_SPI3, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO | RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC, ENABLE);

    // WS PA15
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CK PB3
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD PB5
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode       = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard   = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq  = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL       = I2S_CPOL_High;
    I2S_Init(SPI3, &I2S_InitStructure);

    I2S_SlaveSyncWS(SPI2, ENABLE);
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the I2S3 DMA Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u32 i;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    Delay_Ms(500);

    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    SPI_I2S_DeInit(SPI3);
    SPI_I2S_DeInit(SPI2);

    I2S2_Init();
    I2S3_Init();

    for (size_t i = 0; i < 10; i++)
    {
        I2S2_Tx[i] = i << 8 | i;
    }

    DMA_Rx_Init(DMA2_Channel1, (u32)&SPI3->DATAR, (u32)I2S3_Rx, Len);
    DMA_Tx_Init(DMA1_Channel5, (u32)&SPI2->DATAR, (u32)I2S2_Tx, Len);

    I2S_Cmd(SPI2, ENABLE);
    I2S_Cmd(SPI3, ENABLE);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA2_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    while ((!DMA_GetFlagStatus(DMA1_FLAG_TC5)))
    {
    }
    while ((!DMA_GetFlagStatus(DMA2_FLAG_TC1)))
    {
    }

    I2S_Cmd(SPI3, DISABLE);
    I2S_Cmd(SPI2, DISABLE);

    printf("Rx data:\r\n");
    for (i = 0; i < Len; i++)
    {
        printf("%08x %08x\r\n", I2S2_Tx[i], I2S3_Rx[i]);
    }

    while (1)
        ;
}
