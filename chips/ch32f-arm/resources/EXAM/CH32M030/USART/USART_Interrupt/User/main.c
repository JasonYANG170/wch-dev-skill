/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART interrupt routine:
 *USART1_Tx(PC1)\USART1_Rx(PC0).

 *This routine demonstrates that two boards use query to send and interrupt to
 *receive. After successful sending and receiving, PB0 is connected to LED,
 *and the LED light flashes.
 *
 *Hardware connection:PC1 -- PC0
 *                    PC0 -- C1
 *                    PB0 -- LED
 *
 */

#include "debug.h"

/* Global define */
#define TxSize1    (size(TxBuffer1))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global typedef */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

/* Global Variable */
u8 TxBuffer1[] = "*Buffer1 Send from USART1 using Interrupt!";
u8 RxBuffer1[TxSize1] = {0};


volatile u8 TxCnt1 = 0, RxCnt1 = 0;

volatile u8 Rxfinish1 = 0;

TestStatus TransferStatus1 = FAILED;


/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOD.0
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

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
 * @brief   Initializes the USART1 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_SetPriority(USART1_IRQn,1<<6);
    NVIC_EnableIRQ(USART1_IRQn);

    USART_Cmd(USART1, ENABLE);
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
    u8 i=0;

    SystemCoreClockUpdate();
    Delay_Init();

    GPIO_Toggle_INIT();
    USARTx_CFG();
    Delay_Ms(1000);

    while(TxCnt1 < TxSize1)
    {
        Delay_Ms(50);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
        USART_SendData(USART1, TxBuffer1[TxCnt1++]);
    }

    Delay_Ms(100);

    while(!Rxfinish1) /* waiting for receiving int finish */
    {
    }

    TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer1, TxSize1);

    if(TransferStatus1)
    {
        while(1){
            Delay_Ms(250);
            GPIO_WriteBit(GPIOB, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
        }
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    }

    while(1)
    {
    }
}

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   This function handles USART1 global interrupt request.
 *
 * @return  none
 */
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART1);

        if(RxCnt1 == TxSize1)
        {
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
            Rxfinish1 = 1;
        }
    }
}
