/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *TIM continue Mode routine:
 *TIM1_CH1(PA10)
 *This example demonstrates that the TIM_CH1(PA10) pin outputs PWM in PWM mode by continue mode
 *
 */

#include "debug.h"

/* Global define */

/* Global Variable */
 u16 pbuf[18] = {0x0081,0x0000,0x0000,0x0000,
                  0x001f,0x0000,0x0070,0x0000,
                  0x0001,0x0000,0x270F,0x0063,
                  0x0000,0x0032,0x0000,0x0000,0x0000,0x8000};

/*********************************************************************
 * @fn      TIM1_PWMOut_Init
 *
 * @brief   Initializes TIM1 PWM output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_TIM1|RCC_HB2Periph_AFIO, ENABLE);
	
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

     TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
}

/*********************************************************************
 * @fn      TIM1_DMA_Init
 *
 * @brief   Initializes the TIM DMAy Channelx configuration.
 *
 * @param   DMAy_CHx -
 *            y can be 1 to 2.   
 *            x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory0 base address.
 *          memadr1 - Memory1 base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void TIM1_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_Cmd(DMA_CHx, ENABLE);

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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	printf("TIM_Continuous\r\n");
    TIM1_PWMOut_Init();
    TIM1_DMA_Init(DMA1_Channel5,  (u32)&TIM1->DMAADR, (u32)pbuf, 18);      
    TIM_DMAConfig(TIM1, TIM_DMABase_CR1, TIM_DMABurstLength_18Bytes);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);	
	while(1)
	{

	}
}
