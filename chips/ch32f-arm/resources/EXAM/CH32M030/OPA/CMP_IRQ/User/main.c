/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2024/12/24
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *CMP_IRQ routine:
 *The routine uses Timer 1 to trigger the CMP3 output state, and enter the interrupt
 *when the CMP3 output is high, and the interrupt flags are CMP3_FLAG_CHOUT0-3 in order.
 *
*/

#include "debug.h"

/*********************************************************************
 * @fn      CMP_Init
 *
 * @brief   Initializes CMP3
 *
 * @return  none
 */
void CMP_Init(void)
{
    CMP3_InitTypeDef    CMP3_InitStruct = {0};
    GPIO_InitTypeDef    GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB|RCC_PB2Periph_GPIOA, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_OPCM, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CMP3_InitStruct.CMP3_INT_EN = CMP3_INT_EN_ON;
    CMP3_InitStruct.CMP3_CAP = CMP3_CAP_EN_T2_Channels_OFF;
    CMP3_InitStruct.CMP3_PSEL = CMP3_PSEL_PB3;
    CMP3_InitStruct.CMP3_NSEL = CMP3_NSEL_PA6;
    CMP3_InitStruct.CMP3_HYS = CMP3_HYS_0mv;
    CMP3_InitStruct.CMP3_RMID = CMP3_RMID_EN_OFF;
    CMP3_InitStruct.CMP3_AT_IO = CMP3_AT_IO_ON;
    CMP3_InitStruct.CMP3_PT_IO = CMP3_PT_IO_ON;
    CMP3_InitStruct.CMP3_CH_SW_NUM = CMP3_CH_SW_NUM_4;
    CMP3_InitStruct.CMP3_PT_IO_SEL = CMP3_PT_IO_PB4;
    CMP3_InitStruct.CMP3_BK_EN = CMP3_BK_EN_TIM1BKIN_OFF;
    CMP3_InitStruct.CMP3_TRG_GATE = CMP3_TRG_GATE_ON;
    CMP3_InitStruct.CMP3_TRG_SRC = CMP3_TRG_SRC_T1_CC4;
    CMP3_Init(&CMP3_InitStruct);

    NVIC_EnableIRQ(OPA_IRQn);
    CMP3_Cmd(ENABLE);

}


void TIM1_Init(u16 arr, u16 psc, u16 ccp)
{
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};


    RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init( TIM1, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC4PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_ARRPreloadConfig( TIM1, ENABLE );
    TIM_Cmd( TIM1, ENABLE );
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
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    CMP_Init();
    TIM1_Init(7200-1,1000,3600);
    while(1);

}

void OPA_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      OPA_IRQHandler
 *
 * @brief   CMP3 Interrupt Service Function.
 *
 * @return  none
 */
void OPA_IRQHandler()
{

    if(CMP_GetFlagStatus( CMP3_FLAG_CHOUT0))
    {
       printf("CH0 HIGH\r\n");
       CMP_ClearFlag(CMP3_FLAG_CHOUT0);
    }

    if(CMP_GetFlagStatus( CMP3_FLAG_CHOUT1) )
    {
        printf("CH1 HIGH\r\n");
        CMP_ClearFlag(CMP3_FLAG_CHOUT1);
    }

    if(CMP_GetFlagStatus( CMP3_FLAG_CHOUT2) )
    {

        printf("CH2 HIGH\r\n");
        CMP_ClearFlag(CMP3_FLAG_CHOUT2);
    }

    if(CMP_GetFlagStatus( CMP3_FLAG_CHOUT3) )
    {

        printf("CH3 HIGH\r\n");
        CMP_ClearFlag(CMP3_FLAG_CHOUT3);
    }

}
