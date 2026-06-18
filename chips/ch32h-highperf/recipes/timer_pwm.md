# Timer PWM

## Overview

CH32H417 has 12 timers: TIM1, TIM8 (advanced), TIM2-7 (general), TIM9-12 (32-bit general). Advanced timers (TIM1, TIM8) support complementary outputs with dead-time insertion for motor control.

## Key API Functions

```c
void TIM_DeInit(TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare1);

// TIM9-12 (32-bit) functions
void TIM9_12_TimeBaseInit(TIM_TypeDef *TIMx, TIM9_12_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void TIM9_12_OC1Init(TIM_TypeDef *TIMx, TIM9_12_OCInitTypeDef *TIM_OCInitStruct);
void TIM9_12_SetCompare1(TIM_TypeDef *TIMx, uint32_t Compare1);
void TIM9_12_SetCounter(TIM_TypeDef *TIMx, uint32_t Counter);
void TIM9_12_SetAutoreload(TIM_TypeDef *TIMx, uint32_t Autoreload);
```

## Basic PWM Example (TIM2 CH1 on PA0)

```c
#include "ch32h417.h"

void TIM2_PWM_Init(uint16_t period, uint16_t duty)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {0};
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 = TIM2_CH1 (AF1)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF1);

    // Time base: 72MHz / 72 = 1MHz, period = 1000 -> 1kHz PWM
    TIM_TimeBaseStruct.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = period - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

    // PWM Mode 1 on CH1
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = duty;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

// Change duty cycle at runtime
void TIM2_SetDuty(uint16_t duty)
{
    TIM_SetCompare1(TIM2, duty);
}
```

## Advanced PWM with Dead-Time (TIM1 Motor Control)

```c
void TIM1_MotorPWM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {0};
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_BDTRInitTypeDef TIM_BDTRInitStruct = {0};

    // ... GPIO and clock setup for TIM1_CH1/CH1N, CH2/CH2N, CH3/CH3N ...

    // Time base
    TIM_TimeBaseStruct.TIM_Prescaler = 0;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseStruct.TIM_Period = 6000;  // 20kHz PWM at 120MHz
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    // PWM on CH1, CH2, CH3
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 3000;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

    TIM_OC1Init(TIM1, &TIM_OCInitStruct);
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    // Dead-time and break configuration
    TIM_BDTRInitStruct.TIM_DeadTime = 100;  // ~0.83us dead-time
    TIM_BDTRInitStruct.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
    TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
    TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStruct);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}
```

## Input Capture Example

```c
void TIM3_IC_Init(void)
{
    TIM_ICInitTypeDef TIM_ICInitStruct = {0};

    // ... GPIO setup for TIM3_CH1 ...

    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICFilter = 0x00;
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    TIM_Cmd(TIM3, ENABLE);
}

// Read capture value
uint16_t TIM3_ReadCapture(void)
{
    return TIM_GetCapture1(TIM3);
}
```

## Timer Types

| Timer | Type | Counter | Channels |
|-------|------|---------|----------|
| TIM1, TIM8 | Advanced | 16-bit | 4 + complementary |
| TIM2-7 | General | 16-bit | 4 |
| TIM9-12 | General | 32-bit | 4 |

## PWM Mode

| Mode | Description |
|------|-------------|
| `TIM_OCMode_PWM1` | Output high when counter < CCR |
| `TIM_OCMode_PWM2` | Output low when counter < CCR |
