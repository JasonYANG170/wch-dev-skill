/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/18
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART DMA, master/slave mode transceiver routine:
 Master:USART2_Tx(PA4)\USART2_Rx(PA5).
 Slave:USART3_Tx(PA13)\USART3_Rx(PA14).

 This example demonstrates UART2 and USART3 use DAM data to send and receive.

    Hardware connection:PA4 -- PA14
               PA5 -- PA13

*/

#include "debug.h"

/* Global typedef */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

/* Global define */
#define TxSize1    (size(TxBuffer1))
#define TxSize2    (size(TxBuffer2))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global Variable */
u8 TxBuffer1[] = "*Buffer1 Send from USART2 to USART3 using DMA!"; /* Send by UART2 */
u8 TxBuffer2[] = "#Buffer2 Send from USART3 to USART2 using DMA!"; /* Send by UART3 */
u8 RxBuffer1[TxSize1] = {0};                                       /* USART2 Using  */
u8 RxBuffer2[TxSize2] = {0};                                       /* USART3 Using  */

u8 TxCnt1 = 0, RxCnt1 = 0;
u8 TxCnt2 = 0, RxCnt2 = 0;

u8 Rxfinish1 = 0, Rxfinish2 = 0;

TestStatus TransferStatus1 = FAILED;
TestStatus TransferStatus2 = FAILED;

/*********************************************************************
 * @fn      Buffercmp
 *
 * @brief   Compares two buffers
 *
 * @param   Buf1,Buf2 - buffers to be compared
 *          BufferLength - buffer's length
 *
 * @return  PASSED - Buf1 identical to Buf
 *          FAILED - Buf1 differs from Buf2
 */
TestStatus Buffercmp(uint8_t *Buf1, uint8_t *Buf2, uint16_t BufLength)
{
    while(BufLength--)
    {
        if(*Buf1 != *Buf2)
        {
            return FAILED;
        }
        Buf1++;
        Buf2++;
    }
    return PASSED;
}

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_USART2 | RCC_HB1Periph_USART3, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_AFIO, ENABLE);

    /* USART2 TX-->A.4   RX-->A.5 */
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* USART3 TX-->A.13  RX-->A.14 */
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource13,GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Init(USART3, &USART_InitStructure);

    DMA_Cmd(DMA1_Channel7, ENABLE); /* USART2 Tx */
    DMA_Cmd(DMA1_Channel6, ENABLE); /* USART2 Rx */
    DMA_Cmd(DMA1_Channel2, ENABLE); /* USART3 Tx */
    DMA_Cmd(DMA1_Channel3, ENABLE); /* USART3 Rx */

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

/*********************************************************************
 * @fn      DMA_INIT
 *
 * @brief   Configures the DMA for USART2 & USART3.
 *
 * @return  none
 */
void DMA_INIT(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR); /* USART2->DATAR:0x40004404 */
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)TxBuffer1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = TxSize1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)RxBuffer1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = TxSize2;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DATAR); /* USART3->DATAR:0x40004804 */
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = TxSize2;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)RxBuffer2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = TxSize1;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
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
     
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("USART DMA TEST\r\n");
    DMA_INIT();
    USARTx_CFG(); /* USART2 & USART3 INIT */
    USART_DMACmd(USART2, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET) /* Wait until USART2 TX DMA1 Transfer Complete */
    {
    }
    while(DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET) /* Wait until USART2 RX DMA1 Transfer Complete */
    {
    }
    while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET) /* Wait until USART3 TX DMA1 Transfer Complete */
    {
    }
    while(DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET) /* Wait until USART3 RX DMA1 Transfer Complete */
    {
    }
    TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer2, TxSize1);
    TransferStatus2 = Buffercmp(TxBuffer2, RxBuffer1, TxSize2);

    if(TransferStatus1 && TransferStatus2)
    {
        printf("Send Success!\r\n");
    }
    else
    {
        printf("Send Fail!\r\n");
    }

    printf("TxBuffer1:%s\r\n", TxBuffer1);
    printf("RxBuffer1:%s\r\n", RxBuffer1);
    printf("TxBuffer2:%s\r\n", TxBuffer2);
    printf("RxBuffer2:%s\r\n", RxBuffer2);

    while(1)
    {
    }
}
