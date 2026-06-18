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
 *Two-wire full duplex mode, master/slave mode, data transceiver:
 *Master:SPI1_NSS(PA0)\SPI1_SCK(PA1)\SPI1_MISO(PC4)\SPI1_MOSI(PC3).
 *Slave:SPI1_NSS(PA0)\SPI1_SCK(PA1)\SPI1_MISO(PC4)\SPI1_MOSI(PC3).
 *
 *This example demonstrates simultaneous full-duplex transmission and reception
 *between Master and Slave.
 *Note: The two boards download the Master and Slave programs respectively, and
 *power on at the same time.
 *     Hardware connection:
 *                         PA0 -- PA0
 *                         PA1 -- PA1
 *                         PC4 -- PC4
 *                         PC3 -- PC3
 *
 */


#include "debug.h"
#include "string.h"

/* SPI Mode Definition */
#define HOST_MODE     0
#define SLAVE_MODE    1

/* SPI Communication Mode Selection */
#define SPI_MODE      HOST_MODE
//#define SPI_MODE   SLAVE_MODE

/* Global define */
#define Size          18

/* Global Variable */

u16 TxData[Size] = {0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606,
                    0x1111, 0x1212, 0x1313, 0x1414, 0x1515, 0x1616,
                    0x2121, 0x2222, 0x2323, 0x2424, 0x2525, 0x2626};
u16 RxData[Size];

/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOC | RCC_PB2Periph_SPI1, ENABLE);

#if(SPI_MODE == HOST_MODE)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#elif(SPI_MODE == SLAVE_MODE)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif

#if(SPI_MODE == HOST_MODE)
    SPI_SSOutputCmd(SPI1, ENABLE);

#endif

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

#if(SPI_MODE == HOST_MODE)
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

#elif(SPI_MODE == SLAVE_MODE)
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;

#endif

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
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
    u8 i = 0;
    u8 j = 0;
    u8 value;

    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init( 460800 );
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

#if(SPI_MODE == SLAVE_MODE)
    printf("Slave Mode\r\n");
    Delay_Ms(1000);

#endif

    SPI_FullDuplex_Init();

#if(SPI_MODE == HOST_MODE)
    printf("Host Mode\r\n");
    Delay_Ms(2000);

#endif

    while(1)
    {
        while((i < 18) || (j < 18))
        {
            if(i < 18)
            {
                if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET)
                {
                    SPI_I2S_SendData(SPI1, TxData[i]);
                    i++;
                }
            }

            if(j < 18)
            {
                if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET)
                {
                    RxData[j] = SPI_I2S_ReceiveData(SPI1);
                    j++;
                }
            }
        }

        value = memcmp(TxData, RxData, Size);

        if(value == 0)
        {
            printf("Same\r\n");
        }
        else
        {
            printf("Different\r\n");


        }

        for(i = 0; i < Size; i++)
        {
            printf(" RxData:%04x\r\n", RxData[i]);
        }
        while(1);
    }
}
