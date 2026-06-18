/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2024/07/18
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Low power, standby mode routine:
 *AWU automatically wakes up
 *This example demonstrates that WFI enters standby mode and wakes up automatically.
 *Note: In order to reduce power consumption as much as possible, it is recommended
 *to set the unused GPIO to pull-up mode.
 *
 */

#include "debug.h"

/* Global define */
volatile u32 num =0;
volatile u32 AWU_CNT =0;
u32 FLAG1 =0;
#define  PSC1  4800-1       //TIM1 PSC
#define  LSIPSC 60000
#define  NUM1 3
#define  SYSCLK1  48000000  //TIM1 Clock

#define  CAL_FREQ_CLOSE   0
#define  CAL_FREQ_OPEN    1

#ifndef  LSI_FREQ
#define  LSI_FREQ   CAL_FREQ_OPEN
#endif
/* Global Variable */

/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief   Initializes EXTI collection.
 *
 * @return  none
 */
void EXTI_INT_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef        NVIC_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = AWU_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************
 * @fn      TIM_Init
 *
 * @brief   Initializes TIM1 .
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void TIM_Init(u16 arr, u16 psc)
{
     TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

     RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

     TIM_TimeBaseInitStructure.TIM_Period = arr;
     TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
     TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
     TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
     TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
     TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

     TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      CAL_AWU_TIME
 *
 * @brief   Calculate automatic wake-up time.
 *
 * @param   none
 *
 * @return  The time returned is MS.
 */
u32 CAL_AWU_TIME(void)
{
    u32 i=0;
#if    (LSI_FREQ==CAL_FREQ_OPEN)
    PWR_AWU_SetWindowValue(1-1);    // LSI time base,60K frequency division
    TIM_Init(0xFFFF,PSC1);
    num=TIM1->CNT;
    PWR_AutoWakeUpCmd(ENABLE);
    Delay_Ms(600);
    num=num/(NUM1-1);
    i= (480*LSIPSC)/(48*num);//48=(PSC1+1)/100;480=SYSCLK1/100000

#elif  (LSI_FREQ==CAL_FREQ_CLOSE)
    i = RCC_GetLSIFreq();

#endif
    return ((LSIPSC*10)/i+5)/10;
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
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    u32 TIM_MS=0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    EXTI_INT_INIT();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("Standby Mode Test\r\n");

    TIM_MS=CAL_AWU_TIME();    //Calculate the time for writing one in the AWU_WindowValue.
    printf( "TIM_MS:%d\r\n", TIM_MS );
    Delay_Ms(1);
    PWR_AWU_SetWindowValue(2);//SetWindowValue is set based on TIM_MS and AWU time
    PWR_AutoWakeUpCmd(ENABLE);
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);

    USART_Printf_Init(115200);
    printf("\r\n Auto wake up \r\n");

    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }

}

void AWU_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      AWU_IRQHandler
 *
 * @brief   This function AWU exception.
 *
 * @return  none
 */
void AWU_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line17)!=RESET)
    {
        if(AWU_CNT==0)
        {
            num=0;
            TIM1->CNT=0;
            AWU_CNT++;
        }
        else if(0<AWU_CNT&& AWU_CNT<NUM1)
        {
            num+=(TIM1->CNT);
            TIM1->CNT=0;
            AWU_CNT++;
        }
        else if(AWU_CNT==NUM1)
        {
            TIM_Cmd(TIM1, DISABLE);
            PWR_AutoWakeUpCmd(DISABLE);
        }
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
}


