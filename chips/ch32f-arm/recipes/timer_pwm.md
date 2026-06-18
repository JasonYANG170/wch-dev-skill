# Recipe: Timer and PWM

## Overview

Configure timers for periodic interrupts and PWM output on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void TIM_DeInit(TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);

// PWM output
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);

// Compare value
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4);

// Interrupt
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);

// Counter access
uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);
void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter);
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload);
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);
```

## Timer TimeBase Init Structure

```c
typedef struct {
    uint16_t TIM_Prescaler;         // Prescaler value (0-65535)
    uint16_t TIM_CounterMode;       // TIM_CounterMode_Up, _Down, _CenterAligned1/2/3
    uint16_t TIM_Period;            // Auto-reload value (0-65535)
    uint16_t TIM_ClockDivision;     // TIM_CKD_DIV1, _DIV2, _DIV4
    uint8_t TIM_RepetitionCounter;  // Repetition counter (TIM1/TIM8 only)
} TIM_TimeBaseInitTypeDef;
```

## Timer Output Compare Init Structure

```c
typedef struct {
    uint16_t TIM_OCMode;        // TIM_OCMode_Timing, _Active, _Inactive, _Toggle, _PWM1, _PWM2
    uint16_t TIM_OutputState;   // TIM_OutputState_Enable or _Disable
    uint16_t TIM_OutputNState;  // TIM_OutputNState_Enable or _Disable (TIM1/TIM8 only)
    uint16_t TIM_Pulse;         // Compare value (pulse width)
    uint16_t TIM_OCPolarity;    // TIM_OCPolarity_High or _Low
    uint16_t TIM_OCNPolarity;   // TIM_OCNPolarity_High or _Low (TIM1/TIM8 only)
    uint16_t TIM_OCIdleState;   // TIM_OCIdleState_Set or _Reset (TIM1/TIM8 only)
    uint16_t TIM_OCNIdleState;  // TIM_OCNIdleState_Set or _Reset (TIM1/TIM8 only)
} TIM_OCInitTypeDef;
```

## PWM Frequency Calculation

```
PWM Frequency = Timer_Clock / ((Prescaler + 1) * (Period + 1))
PWM Duty = Compare / (Period + 1)
```

Example: 72MHz clock, 1kHz PWM, 50% duty:
- Prescaler = 71 (72MHz / 72 = 1MHz timer clock)
- Period = 999 (1MHz / 1000 = 1kHz)
- Compare = 500 (500/1000 = 50%)

## Example: TIM3 CH1 PWM Output

```c
#include "debug.h"

void TIM3_PWM_Init(uint16_t prescaler, uint16_t period, uint16_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA6 = TIM3_CH1 (AF push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Time base configuration
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration: Channel 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    // Enable TIM3
    TIM_Cmd(TIM3, ENABLE);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // 72MHz / 72 / 1000 = 1kHz PWM
    TIM3_PWM_Init(71, 999, 500);  // 50% duty

    printf("PWM TEST - 1kHz, 50%% duty\r\n");

    while(1)
    {
        // Fade LED
        for(uint16_t i = 0; i < 1000; i += 10)
        {
            TIM_SetCompare1(TIM3, i);
            Delay_Ms(10);
        }
        for(uint16_t i = 1000; i > 0; i -= 10)
        {
            TIM_SetCompare1(TIM3, i);
            Delay_Ms(10);
        }
    }
}
```

## Example: Timer Interrupt

```c
volatile uint32_t timer_count = 0;

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        timer_count++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void TIM2_Interrupt_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 1 second interrupt at 72MHz
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;  // 72MHz / 7200 = 10kHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 9999;      // 10kHz / 10000 = 1Hz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Enable update interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // Configure NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}
```

## Available Timers

| Timer | Type | Bus | Channels | Notes |
|-------|------|-----|----------|-------|
| TIM1 | Advanced | APB2 | 4 + complementary | Motor control, dead-time |
| TIM2 | General | APB1 | 4 | |
| TIM3 | General | APB1 | 4 | |
| TIM4 | General | APB1 | 4 | |
| TIM5 | General | APB1 | 4 | CH32F20x |
| TIM6 | Basic | APB1 | 0 | DAC trigger |
| TIM7 | Basic | APB1 | 0 | DAC trigger |
| TIM8 | Advanced | APB2 | 4 + complementary | CH32F20x |
| TIM9 | General | APB2 | 2 | CH32F20x |
| TIM10 | General | APB2 | 1 | CH32F20x |
