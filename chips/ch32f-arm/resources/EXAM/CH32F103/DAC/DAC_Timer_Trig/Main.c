/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2024/12/15
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Timer trigger DAC conversion routine:
 *DAC channel 0 (PA4) output
 *A DAC conversion is triggered by the TIM3_TRGO event, and PA4 outputs the 
 *corresponding voltage.
 *
*/

#include "debug.h"

/* Global define */
#define Num 7

/* Global Variable */
u16 DAC_Value[Num] = {64, 128, 256, 512, 1024, 2048, 4095};


/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dac_Init( void )
{


    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef DAC_InitType = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    DAC_InitType.DAC_Trigger = DAC_Trigger_T3_TRGO;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;
    DAC_Init( DAC_Channel_1, &DAC_InitType );

    DAC_Cmd( DAC_Channel_1, ENABLE );
}


/*********************************************************************
 * @fn      TIM3_Init
 *
 * @brief   Initializes TIM3 collection.
 *
 * @param   arr - TIM_Period
 *          psc - TIM_Prescaler
 *
 * @return  none
 */
void TIM3_Init( u16 arr, u16 psc )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );

    TIM_SelectOutputTrigger( TIM3, TIM_TRGOSource_Update );      /* Enable TRGO */

    TIM_Cmd( TIM3, ENABLE );
}



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main( void )
{
    u8 i = 0;
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init( 115200 );

    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "Timer Trig\r\n" );


    TIM3_Init( 100, 7199 );
    Dac_Init();

    while( 1 )
    {
        DAC->R12BDHR1 = DAC_Value[i];
        i++;
        if( i > Num )
        {
            i = 0;
        }
        Delay_Ms( 1000 );

        // TIM_GenerateEvent(TIM3,TIM_EventSource_Update);
        printf( "run\r\n" );
        printf( "DAC->R12BDHR1:0x%04x\r\n", DAC->R12BDHR1 );
        printf( "DAC->DOR1:0x%04x\r\n", DAC->DOR1 );
        printf( "TIM3->CNT:%d\r\n", TIM3->CNT );

    }
}





