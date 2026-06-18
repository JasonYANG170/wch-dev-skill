/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/08/28
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *7-bit addressing mode, slave mode, transceiver routine:
 *I2C1_SCL(PA13)\I2C1_SDA(PA14).
 *This routine demonstrates that Slave receives.
 *Note: The two boards download the Master and Slave programs respectively,
 *    and power on at the same time.
 *
 */

#include "debug.h"


/* Global define */
#define Size   6
#define RXAdderss   0x02

/* Global Variable */
u8 RxData[5][Size];

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture );

    I2C_Cmd( I2C1, ENABLE );
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
    u8 p = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(460800);
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    printf("IIC Slave mode\r\n");
    IIC_Init(RXAdderss);

    for( p =0; p < 5; p++)
    {

        i = 0;
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED ) );
        while( i < 6 )
        {
            if( I2C_GetFlagStatus( I2C1, I2C_FLAG_RXNE ) !=  RESET )
            {
                RxData[p][i] = I2C_ReceiveData( I2C1 );
                i++;
            }
        }
       I2C1->CTLR1 &= I2C1->CTLR1;
    }

    printf( "RxData:\r\n" );
    for(p=0; p<5; p++)
    {
    for( i = 0; i < 6; i++ )
    {
       printf( "%02x ", RxData[p][i] );
    }
       printf( "\r\n ");
    }

    while(1);
}
