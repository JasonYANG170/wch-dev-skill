/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/11
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *CAN test mode, including silent mode, loopback mode and loopback silent mode:
 *Standard_Frame: includes a 32bit filter mask bit pattern.
 *
 */

#include "debug.h"

/* CAN Test Mode Definition */
#define SILENT_MODE 1
#define LOOPBACK_MODE 2
#define SILENT_LOOPBACK_MODE 3

/* Test Mode Selection */
// #define TEST_MODE   SILENT_MODE
#define TEST_MODE LOOPBACK_MODE
// #define TEST_MODE   SILENT_LOOPBACK_MODE

/* Receive can data in interrupt */
// #define USE_INTERRUPT_TO_REC

#define GET_PIN_BIT(x) ((uint16_t)0x1 << x)

/**
 * @struct CANPinTable_t
 * @note   CAN pin table structure
 */
typedef struct
{
    CAN_TypeDef *CANx;
    GPIO_TypeDef *PORT;
    uint8_t Pin;
    uint32_t AF;
} CANPinTable_t;

/**
 * @brief  CAN Tx pin table
 * @note   CAN1: PA12, PB9, PD1
 */
const CANPinTable_t CANTXPinTable[] = {
    {CAN1, GPIOA, 12, 0}, {CAN1, GPIOB, 9, GPIO_Remap1_CAN1}, {CAN1, GPIOD, 1, GPIO_Remap2_CAN1},
};

/**
 * @brief  CAN Rx pin table
 * @note   CAN1: PA11, PB8, PD0
 */
const CANPinTable_t CANRXPinTable[] = {
    {CAN1, GPIOA, 11, 0}, {CAN1, GPIOB, 8, GPIO_Remap1_CAN1}, {CAN1, GPIOD, 0, GPIO_Remap2_CAN1},
};

CAN_TypeDef *CANTable[] = {CAN1};

volatile uint8_t TxBuff[8], RxBuff[8];

volatile uint8_t RxInterruptFlag[1] = {0};

/*********************************************************************
 * @fn      CAN_Mode_Init
 *
 * @brief   CAN mode initialization
 *
 * @param   CANx: CAN1
 *          RxTab: CAN Rx pin table
 *          TxTab: CAN Tx pin table
 *          CAN_InitSturcture: CAN initialization structure
 *
 * @return  ErrorStatus: READY or NoREADY
 */
ErrorStatus CAN_Mode_Init(CAN_TypeDef *CANx, const CANPinTable_t *RxTab, const CANPinTable_t *TxTab,
                          CAN_InitTypeDef *CAN_InitSturcture)
{
    GPIO_InitTypeDef GPIO_InitSturcture = {0};

    if (CANx != RxTab->CANx || CANx != TxTab->CANx || RxTab->AF != TxTab->AF)
    {
        return NoREADY;
    }

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO | (RCC_PB2Periph_GPIOA << (((void *)(RxTab->PORT) - (void *)GPIOA) / sizeof(GPIO_TypeDef))) |
                              (RCC_PB2Periph_GPIOA << ((TxTab->PORT - GPIOA) / sizeof(GPIO_TypeDef))),
                          ENABLE);

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_CAN1,ENABLE);

    GPIO_PinRemapConfig(RxTab->AF, ENABLE);
    GPIO_PinRemapConfig(TxTab->AF, ENABLE);

    GPIO_InitSturcture.GPIO_Pin = GET_PIN_BIT(TxTab->Pin);
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(TxTab->PORT, &GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin = GET_PIN_BIT(RxTab->Pin);
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(RxTab->PORT, &GPIO_InitSturcture);

    CAN_Init(CANx, CAN_InitSturcture);

    return READY;
}

/*********************************************************************
 * @fn     	CAN_Send_Msg
 *
 * @brief   	CAN Transmit function.
 *
 * @param   	CANx - where x can be 1...3 to select the CAN peripheral.
 *		   	IdType - The target ID type for sending the message.
 *				- CAN_Id_Standard
 *				- CAN_Id_Extended
 *		   	Identifier - The target ID for sending the message.
 *		   	msg - Transmit data buffer.
 *          	len - Data length.
 *
 * @return  	0 - Send successful.
 *          	1 - Send failed.
 */
uint8_t CAN_Send_Msg(CAN_TypeDef *CANx, uint32_t IdType, uint32_t Identifier, uint8_t *msg, uint8_t len)
{
    u8 mbox;
    u16 i = 0;

    CanTxMsg CanTxStructure = {0};

    IdType == CAN_Id_Extended ? (CanTxStructure.ExtId = Identifier) : (CanTxStructure.StdId = Identifier);
    CanTxStructure.IDE = IdType;
    CanTxStructure.RTR = CAN_RTR_Data;
    CanTxStructure.DLC = len;

    for (i = 0; i < len; i++)
    {
        CanTxStructure.Data[i] = msg[i];
    }

    mbox = CAN_Transmit(CANx, &CanTxStructure);
    i = 0;

    while ((CAN_TransmitStatus(CANx, mbox) != CAN_TxStatus_Ok) && (i < 0xFFF))
    {
        i++;
    }

    if (i == 0xFFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
 * @fn      CAN_Receive_Msg
 *
 * @brief   CAN Receive function.
 *
 * @param   CANx - where x can be 1...3 to select the CAN peripheral.
 *			RxBuff - Pointer to the memory used to store received data.
 *
 * @return  CanRxStructure.DLC - Receive data length.
 */
uint8_t CAN_Receive_Msg(CAN_TypeDef *CANx, uint8_t *RxBuff)
{
    u8 i;

    CanRxMsg CanRxStructure = {0};

    if (CAN_MessagePending(CANx, CAN_FIFO0) == 0)
    {
        return 0;
    }

    CAN_Receive(CANx, CAN_FIFO0, &CanRxStructure);

    for (i = 0; i < 8; i++)
    {
        RxBuff[i] = CanRxStructure.Data[i];
    }

    return CanRxStructure.DLC;
}

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   program to test can in loopback mode
 *
 * @return  none
 */
int main(void)
{
    uint32_t Circle_CNT = 0;
    CAN_FilterInitTypeDef CAN_FilterInitSturcture = {0};
    CAN_FilterInitSturcture.CAN_FilterNumber = 0;
#if (Frame_Format == Standard_Frame)
    /* identifier/mask mode, One 32-bit filter, StdId: 0x317 */
    CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;
    CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
    CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFE0;
    CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x0006;
    /* identifier list mode, One 32-bit filter, StdId: 0x317,0x316 */
//	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdList;
//	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
//	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;
//	CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
//	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x62C0;
//	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0;
#elif (Frame_Format == Extended_Frame)
    /* identifier/mask mode, One 32-bit filter, ExtId: 0x12124567 */
    CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x9092;
    CAN_FilterInitSturcture.CAN_FilterIdLow = 0x2B3C;
    CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFFF;
    CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0xFFFE;
#endif

    CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;

    CAN_InitTypeDef CAN_InitSturcture = {0};
    CAN_InitSturcture.CAN_TTCM = DISABLE;
    CAN_InitSturcture.CAN_ABOM = DISABLE;
    CAN_InitSturcture.CAN_AWUM = DISABLE;
    CAN_InitSturcture.CAN_NART = ENABLE;
    CAN_InitSturcture.CAN_RFLM = DISABLE;
    CAN_InitSturcture.CAN_TXFP = DISABLE;
#if (TEST_MODE == SILENT_LOOPBACK_MODE)
    CAN_InitSturcture.CAN_Mode = CAN_Mode_Silent_LoopBack;
#elif (TEST_MODE == LOOPBACK_MODE)
    CAN_InitSturcture.CAN_Mode = CAN_Mode_LoopBack;
#elif (TEST_MODE == SILENT_MODE)
    CAN_InitSturcture.CAN_Mode = CAN_Mode_Silent;
#endif
    CAN_InitSturcture.CAN_SJW = CAN_SJW_1tq;
    CAN_InitSturcture.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitSturcture.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitSturcture.CAN_Prescaler = 16;
    CAN_Mode_Init(CAN1, CANRXPinTable, CANTXPinTable, &CAN_InitSturcture);
#ifdef USE_INTERRUPT_TO_REC
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
    NVIC_EnableIRQ(CAN2_RX0_IRQn);
    CAN_ITConfig(CAN3, CAN_IT_FMP0, ENABLE);
    NVIC_EnableIRQ(CAN3_RX0_IRQn);
#endif

    CAN_FilterInit(&CAN_FilterInitSturcture);

    while (1)
    {
        printf("------------Sending data-------------\n");
        for (int x = 0; x < 1; x++)
        {
            printf("CAN%d: ", x + 1);
            for (uint8_t i = 0; i < sizeof(TxBuff) / sizeof(*TxBuff); i++)
            {
                TxBuff[i] = (i + Circle_CNT) % 256;
                printf("%#04x ", TxBuff[i]);
            }
            if (!CAN_Send_Msg(CANTable[x], CAN_Id_Standard, 0x317, (uint8_t *)TxBuff, 7))
            {
                printf("\033[32m\nSuccessful\033[0m\n");
            }
            else
            {
                printf("\033[31m\nFail\033[0m\n");
            }
        }
        printf("-------------------------------------\n");

#ifdef USE_INTERRUPT_TO_REC
        printf("************Receive data*************\n");
        for (uint8_t x = 0; x < sizeof(RxInterruptFlag) / sizeof(*RxInterruptFlag); x++)
        {
            printf("CAN%d: ", x + 1);
            if (RxInterruptFlag[x])
            {

                for (uint8_t i = 0; i < RxInterruptFlag[x]; i++)
                {
                    printf("%#04x ", RxBuff[i]);
                }
                RxInterruptFlag[x] = 0;
            }
            else
            {
                printf("\033[34mNone");
            }
            printf("\033[0m\n");
        }
        printf("*************************************\n");

#else
        printf("************Receive data*************\n");
        uint8_t rxlen;
        for (int x = 0; x < sizeof(CANTable) / sizeof(*CANTable); x++)
        {
            printf("CAN%d: ", x + 1);
            if ((rxlen = CAN_Receive_Msg(CANTable[x], (uint8_t *)RxBuff)))
            {
                for (uint8_t i = 0; i < rxlen; i++)
                {
                    printf("%#04x ", RxBuff[i]);
                }
                rxlen = 0;
            }
            else
            {
                printf("\033[34mNone");
            }
            printf("\033[0m\n");
        }
        printf("*************************************\n");
#endif
        Circle_CNT++;
        Delay_Ms(500);
    }
}

#ifdef USE_INTERRUPT_TO_REC
__attribute__((interrupt("WCH-Interrupt-fast"))) void CAN1_RX0_IRQHandler()
{
    int i;
    uint8_t px, pbuf[8];
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0))
    {
        px = CAN_Receive_Msg(CAN1, pbuf);
        for (i = 0; i < px; i++)
        {
            RxBuff[i] = pbuf[i];
        }
        if (px)
        {
            RxInterruptFlag[0] = px;
        }
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}
#endif
