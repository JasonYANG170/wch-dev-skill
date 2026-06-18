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
 Timer synchronization mode:
 This example demonstrates 3 timer synchronization modes.

*/

#include "debug.h"

/*********************************************************************
 * @fn      TIM_TimSynchroMode1_Init
 *
 * @brief   Using TIM2 as prescaler for TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode1_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x3E8);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_ITRxExternalClockConfig(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_External1);

    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM_TimSynchroMode2_Init
 *
 * @brief   Using TIM2 to use TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode2_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x3E8);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x64;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC1Ref);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Gated);

    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      TIM_TimSynchroMode3_Init
 *
 * @brief   Using TIM2 to start TIM1.
 *
 * @return  none
 */
void TIM_TimSynchroMode3_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0xFFF);
    TIM_PrescalerConfig(TIM1, 48000 - 1, TIM_PSCReloadMode_Immediate);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);
    TIM_Cmd(TIM1, ENABLE);
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

	printf("Synchro_Timer\r\n");
	/* Timer synchronization Mode Selection */
    TIM_TimSynchroMode1_Init();
   	//TIM_TimSynchroMode2_Init();
    //	TIM_TimSynchroMode3_Init();
      
	while(1)
	{
        printf("TIM1 cnt:%d\r\n", TIM1->CNT);
        printf("TIM2 cnt:%d\r\n", TIM2->CNT);
	}
	while (1)
		;
}
