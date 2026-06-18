/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/08
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *HSI_Calibration:
 *Calibration of HSI through HSE.
 *
 */

#include "debug.h"

#warning Please confirm that the HSE or HSE bypass is available and accurate.

/* Global define */
#define PLL_CLK 144000000
#define ADJUST_STEP 22000

/* Global Variable */

static void Set_Clock_2_HSI() {
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xFFFFFFFC;
    RCC->CFGR0 &= (uint32_t)0xE8FF0003;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFFFEFFFF;
    RCC->INTR = 0x009F0000;
}

static void turnon_hsepll() {
    unsigned short overdelay = 0;
    RCC->CTLR |= RCC_HSE_ON;
    while (!(RCC->CTLR & RCC_HSERDY)) {
        Delay_Ms (10);
        overdelay++;
        if (overdelay > 100) {
            printf ("Turnning HSE Failed\r\n");
            return;
        }
    }
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE);
    RCC->CTLR |= RCC_PLLON;
    while ((RCC->CTLR & RCC_PLLRDY) == 0) {
    }
    printf ("PLL for TIM has been motifited\r\n");
}

void TIM2_TimebaseInit (u16 arr, u16 psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_PB1PeriphClockCmd (RCC_PB1Periph_TIM2, ENABLE);

    TIM_CLKSourceConfig (TIM2, TIM_CLKSource_PLL);  // HSCK_MODE置1

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 仅支持增量计数
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseInitStructure);

    TIM_ARRPreloadConfig (TIM2, ENABLE);

    TIM_ITConfig (TIM2, TIM_IT_Update | TIM_IT_CC3, ENABLE);

    NVIC_EnableIRQ (TIM2_IRQn);

    TIM_Cmd (TIM2, ENABLE);
}

float Meas_Frq = 0.0;

unsigned long timupdate_times = 0, captured_cnt = 0;
long target_cnt = 0;

__attribute__ ((interrupt ("WCH-Interrupt-fast"))) void TIM2_IRQHandler() {
    if (TIM_GetITStatus (TIM2, TIM_IT_Update)) {
        timupdate_times++;
        TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
    }

    if (TIM_GetITStatus (TIM2, TIM_IT_CC3)) {

        long totol_cnt = timupdate_times * 65536 + TIM2->CNT;
        timupdate_times = 0;
        printf ("%ld %ld\r\n", totol_cnt, target_cnt);
        Meas_Frq = (float)HSI_VALUE + (float)HSI_VALUE * ((float)( target_cnt-totol_cnt) / (float)target_cnt);
        printf ("Meas Frq: %f\r\n", Meas_Frq);
        TIM2->CNT = 0;
        TIM_ClearITPendingBit (TIM2, TIM_IT_CC3);
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    u8 i = 0;
    Set_Clock_2_HSI();
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf ("GPIO Toggle TEST\r\n");
    turnon_hsepll();
    target_cnt = 0xffff * (PLL_CLK / (SystemCoreClock / 0xdd));
    TIM2_TimebaseInit (0xffff, 0xdd);
    Delay_Ms(4000);

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    
    /* MCO Output GPIOA 8 */
    RCC_MCOConfig (RCC_MCO_HSI);

    long diff = (long)Meas_Frq - (long)HSI_VALUE;
    unsigned long tmpreg = (RCC->CTLR >> 3) & 0x1f;
    printf("%ld\r\n",tmpreg);
    printf("%ld\r\n",(long)diff );
    tmpreg -= (diff / ADJUST_STEP);
    printf("%ld\r\n",tmpreg);
    RCC_AdjustHSICalibrationValue (tmpreg);

    while (1) {
        Delay_Ms (250);
        GPIO_WriteBit (GPIOB, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
    }
}
