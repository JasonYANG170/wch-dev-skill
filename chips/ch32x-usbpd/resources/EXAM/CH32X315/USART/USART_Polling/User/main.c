/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/18
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 polling transceiver mode, master/slave transceiver routine:
 Master:USART2_Tx(PA4)\USART2_Rx(PA5).
 Slave:USART3_Tx(PA13)\USART3_Rx(PA14).
 This example demonstrates sending from USART2 and receiving from USART3.

   Hardware connection:
                     PA4 -- PA14
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
#define TxSize     (size(TxBuffer))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global Variable */
u8         TxBuffer[] = "Buffer Send from USART2 to USART3 by polling!";
u8         RxBuffer[TxSize] = {0};
u8         TxCnt = 0, RxCnt = 0;
TestStatus TransferStatus = FAILED;

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
    USART_Cmd(USART2, ENABLE);

    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
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
    printf("USART Polling TEST\r\n");
    USARTx_CFG(); /* USART2 & USART3 INIT */

    while(TxCnt < TxSize)
    {
        USART_SendData(USART2, TxBuffer[TxCnt++]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
            /* waiting for sending finish */
        }
        while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
        {
            /* waiting for receiving finish */
        }
        RxBuffer[RxCnt++] = (USART_ReceiveData(USART3));
    }

    TransferStatus = Buffercmp(TxBuffer, RxBuffer, TxSize);

    if(TransferStatus)
    {
        printf("send success!\r\n");
        printf("TXBuffer: %s \r\n", TxBuffer);
        printf("RxBuffer: %s \r\n", RxBuffer);
    }
    else
    {
        printf("send fail!\r\n");
        printf("TXBuffer: %s \r\n", TxBuffer);
        printf("RxBuffer: %s \r\n", RxBuffer);
    }
    while(1)
    {
    }
}
