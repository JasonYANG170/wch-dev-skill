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
 multiprocessor communication mode routine:
 Master:USART2_Tx(PA4)\USART2_Rx(PA5).
 Slave:USART3_Tx(PA13)\USART3_Rx(PA14).

 This routine demonstrates that USART2 works as a master, USART3 works as a slave, and USART2 sends address 0x02 to make USART3 exit
 Silent mode, complete follow-up communication.

     Hardware connection:PA4 -- PA14
                         PA5 -- PA13

*/

#include "debug.h"

/* Global typedef */

/* Global define */

/* Global Variable */

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
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Init(USART3, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_SetAddress(USART2, 0x1);
    USART_SetAddress(USART3, 0x2);
    USART_WakeUpConfig(USART3, USART_WakeUp_AddressMark);
    USART_ReceiverWakeUpCmd(USART3, ENABLE); /* USART3 Into Silence */
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
    printf("USART MultiProcessor TEST\r\n");
    USARTx_CFG(); /* USART2 & USART3 Initializes */

    while(1)
    {
        USART_SendData(USART2, 0x102);                              /* Send USART3 addr */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
        USART_SendData(USART2, 0xAA);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
        if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET)
        {
            if(USART_ReceiveData(USART3) == 0xAA)
            {
                printf("USART3 Receive Data\r\n");
                Delay_Ms(1000);
            }
        }
    }
}
