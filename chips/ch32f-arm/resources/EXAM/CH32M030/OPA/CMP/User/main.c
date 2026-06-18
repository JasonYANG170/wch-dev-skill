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
 *CMP routine:
 * CMP1_P1--PB3
 * CMP1_N0--PA6
 * CMP1_OUT--PB4
 * In this example, PB3and PA6 are positive and negative input, PB4 is the output of the comparator.
 * When the voltage of PB3 is greater than the voltage of PA6, the output of PB4 is high level,
 * otherwise the output of PB4is low level.  Or When the output signal is high level, TIM1 brake is effective.
 *
*/

#include "debug.h"

/* ADC WWDG Mode Definition*/
#define CMP3_TIM1_BKIN_ON      0
#define CMP3_TIM1_BKIN_OFF     1

/* ADC WWDG Mode Selection*/
#define CMP3_TIM1_BKIN  CMP3_TIM1_BKIN_ON
//#define CMP3_TIM1_BKIN   CMP3_TIM1_BKIN_OFF

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


    CMP3_InitStruct.CMP3_INT_EN = CMP3_INT_EN_OFF;
    CMP3_InitStruct.CMP3_CAP = CMP3_CAP_EN_T2_Channels_OFF;
    CMP3_InitStruct.CMP3_PSEL = CMP3_PSEL_PB3;
    CMP3_InitStruct.CMP3_NSEL = CMP3_NSEL_PA6;
    CMP3_InitStruct.CMP3_HYS = CMP3_HYS_0mv;
    CMP3_InitStruct.CMP3_RMID = CMP3_RMID_EN_OFF;
    CMP3_InitStruct.CMP3_AT_IO = CMP3_AT_IO_ON;
    CMP3_InitStruct.CMP3_PT_IO = CMP3_PT_IO_ON;
    CMP3_InitStruct.CMP3_CH_SW_NUM = CMP3_CH_SW_NUM_0;
    CMP3_InitStruct.CMP3_PT_IO_SEL = CMP3_PT_IO_PB4;

#if  (CMP3_TIM1_BKIN == CMP3_TIM1_BKIN_ON)
    CMP3_InitStruct.CMP3_TRG_GATE = CMP3_TRG_GATE_OFF;
    CMP3_InitStruct.CMP3_BK_EN = CMP3_BK_EN_TIM1BKIN_ON;
    CMP3_InitStruct.CMP3_COM_EN = CMP3_COM_MODE_ON;

#else
    CMP3_InitStruct.CMP3_BK_EN = CMP3_BK_EN_TIM1BKIN_OFF;
#endif

    CMP3_Init(&CMP3_InitStruct);

    CMP3_Cmd(ENABLE);

}

/*********************************************************************
 * @fn      TIM1_PWM_Out
 *
 * @brief   Initializes TIM1 PWM OUT.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWM_Out( u16 arr, u16 psc, u16 ccp )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    RCC_PB2PeriphClockCmd( RCC_PB2Periph_GPIOB | RCC_PB2Periph_TIM1 , ENABLE );

    /* TIM1_CH1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* TIM1_CH1N */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure );

    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 0x01;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig( TIM1, &TIM_BDTRInitStructure );

    TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
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
#if  (CMP3_TIM1_BKIN == CMP3_TIM1_BKIN_ON)
    TIM1_PWM_Out( 1000, 4800-1, 500 );
#endif
    while(1);
}
