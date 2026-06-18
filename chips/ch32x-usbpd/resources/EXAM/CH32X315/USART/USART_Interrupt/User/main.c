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
 USART interrupt routine:
 Master:USART2_Tx(PA4)\USART2_Rx(PA5).
 Slave:USART3_Tx(PA13)\USART3_Rx(PA14).

 This example demonstrates that UART2 and USART3 use
 query to send and interrupt to receive.

     Hardware connection:
               PA4 -- PA14
               PA5 -- PA13

*/

#include "debug.h"

/* Global define */
#define TxSize1    (size(TxBuffer1))
#define TxSize2    (size(TxBuffer2))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global typedef */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

/* Global Variable */
u8 TxBuffer1[] = "*Buffer1 Send from USART2 to USART3 using Interrupt!"; /* Send by UART2 */
u8 TxBuffer2[] = "#Buffer2 Send from USART3 to USART2 using Interrupt!"; /* Send by UART3 */
u8 RxBuffer1[TxSize1] = {0};                                             /* USART2 Using */
u8 RxBuffer2[TxSize2] = {0};                                             /* USART3 Using  */

volatile u8 TxCnt1 = 0, RxCnt1 = 0;
volatile u8 TxCnt2 = 0, RxCnt2 = 0;

volatile u8 Rxfinish1 = 0, Rxfinish2 = 0;

TestStatus TransferStatus1 = FAILED;
TestStatus TransferStatus2 = FAILED;

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_EnableIRQ(USART3_IRQn);

    USART_Cmd(USART2, ENABLE);
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
    printf("USART Interrupt TEST\r\n");
    USARTx_CFG(); /* USART2 & USART3 INIT */

    while(TxCnt2 < TxSize2) /* USART3--->USART2 */
    {
        USART_SendData(USART3, TxBuffer2[TxCnt2++]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
    }
    while(TxCnt1 < TxSize1) /* USART2--->USART3 */
    {
        USART_SendData(USART2, TxBuffer1[TxCnt1++]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
    }

    Delay_Ms(100);

    while(!Rxfinish1 || !Rxfinish2) /* waiting for receiving int finish */
    {
    }

    TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer2, TxSize1);
    TransferStatus2 = Buffercmp(TxBuffer2, RxBuffer1, TxSize2);

    if(TransferStatus1 && TransferStatus2)
    {
        printf("\r\nSend Success!\r\n");
    }
    else
    {
        printf("\r\nSend Fail!\r\n");
    }
    printf("TxBuffer1---->RxBuffer2     TxBuffer2---->RxBuffer1\r\n");
    printf("TxBuffer1:%s\r\n", TxBuffer1);
    printf("RxBuffer1:%s\r\n", RxBuffer1);
    printf("TxBuffer2:%s\r\n", TxBuffer2);
    printf("RxBuffer2:%s\r\n", RxBuffer2);

    while(1)
    {
    }
}

/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 global interrupt request.
 *
 * @return  none
 */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);

        if(RxCnt1 == TxSize2)
        {
            USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
            Rxfinish1 = 1;
        }
    }
}

/*********************************************************************
 * @fn      USART3_IRQHandler
 *
 * @brief   This function handles USART3 global interrupt request.
 *
 * @return  none
 */
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        RxBuffer2[RxCnt2++] = USART_ReceiveData(USART3);

        if(RxCnt2 == TxSize1)
        {
            USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
            Rxfinish2 = 1;
        }
    }
}
