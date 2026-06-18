/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/13
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 * APP routine: this routine support USB and UART mode,
 * and you can choose the command method to jump to the IAP .
 * Key  parameters: CalAddr - address in flash(same in IAP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.50.
 */

#include "ch32m030_usbfs_device.h"
#include "debug.h"
#include "iap.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOB.0
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      APP_2_IAP
 *
 * @brief   APP_2_IAP program.
 *
 * @return  none
 */
void APP_2_IAP(void)
{
    NVIC_SystemReset();
    while(1){
    }
}

/*********************************************************************
 * @fn      USART1_IT_CFG
 *
 * @brief   USART1 IT configuration
 *
 * @return  none
 */
void USART1_IT_CFG(void)
{
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
    u8 i = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    GPIO_Toggle_INIT();

    /* Usb Init */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );
    USART1_CFG(460800);
    USART1_IT_CFG();
    while(1)
    {
        Delay_Ms(250);
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
        if(*(vu32*)CalAddr == CheckNum)
        {
             Delay_Ms(10);
             APP_2_IAP();
             while(1);
        }
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
    if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){

        UART_Rx_Deal();
    }
}
