# Recipe: Timer and PWM

## When to Use
User wants to generate periodic interrupts, PWM output, input capture, or encoder interface.

## API Reference (from ch32v20x_tim.h)

### TimeBase Init Structure
```c
typedef struct {
    uint16_t TIM_Prescaler;         // Prescaler (0-65535), divides timer clock
    uint16_t TIM_CounterMode;       // TIM_CounterMode_Up, _Down, _CenterAligned1/2/3
    uint16_t TIM_Period;            // Auto-reload value (0-65535)
    uint16_t TIM_ClockDivision;     // TIM_CKD_DIV1, _DIV2, _DIV4
    uint8_t TIM_RepetitionCounter;  // For TIM1/TIM8 only
} TIM_TimeBaseInitTypeDef;
```

### Output Compare Init Structure
```c
typedef struct {
    uint16_t TIM_OCMode;        // TIM_OCMode_Timing, _Active, _Inactive, _Toggle, _PWM1, _PWM2
    uint16_t TIM_OutputState;   // TIM_OutputState_Enable, _Disable
    uint16_t TIM_OutputNState;  // Complementary output (TIM1/TIM8 only)
    uint16_t TIM_Pulse;         // Compare value (duty cycle for PWM)
    uint16_t TIM_OCPolarity;    // TIM_OCPolarity_High, _Low
    uint16_t TIM_OCNPolarity;   // Complementary polarity
    uint16_t TIM_OCIdleState;   // Idle state output
    uint16_t TIM_OCNIdleState;  // Complementary idle state
} TIM_OCInitTypeDef;
```

### Key Functions
```c
void     TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void     TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void     TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);
void     TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState);  // TIM1/TIM8 only
void     TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState);
void     TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1);
void     TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2);
void     TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3);
void     TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4);
uint16_t TIM_GetCounter(TIM_TypeDef *TIMx);
void     TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter);
void     TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload);
void     TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void     TIM_EncoderInterfaceConfig(TIM_TypeDef *TIMx, uint16_t TIM_EncoderMode, uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
```

### PWM Frequency Calculation
```
PWM_Frequency = Timer_Clock / ((Prescaler + 1) * (Period + 1))
Duty_Cycle = Compare / (Period + 1)
```

## Example: TIM2 CH1 PWM Output (PA0)

```c
#include "ch32v20x.h"

void TIM2_PWM_Init(uint16_t prescaler, uint16_t period)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 as TIM2 CH1 AF push-pull
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Time base
    TIM_TimeBaseStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = period;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

    // PWM Mode 1 on CH1
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = period / 2;  // 50% duty
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

// Usage: 72MHz clock, prescaler=71, period=999 => 1kHz PWM
// TIM2_PWM_Init(71, 999);
// TIM_SetCompare1(TIM2, 500);  // 50% duty
```

## Example: TIM1 Complementary PWM (for motor drive)

```c
void TIM1_Complementary_PWM_Init(uint16_t prescaler, uint16_t period)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_BDTRInitTypeDef TIM_BDTRStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA8 = TIM1_CH1, PA7 = TIM1_CH1N (complementary)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Time base
    TIM_TimeBaseStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = period;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    // PWM Mode 1
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStruct.TIM_Pulse = period / 2;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    // Break and dead-time
    TIM_BDTRStruct.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRStruct.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRStruct.TIM_DeadTime = 10;  // Dead-time in timer clocks
    TIM_BDTRStruct.TIM_Break = TIM_Break_Disable;
    TIM_BDTRStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRStruct);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);  // Required for TIM1
    TIM_Cmd(TIM1, ENABLE);
}
```

## Example: Periodic Timer Interrupt

```c
volatile uint32_t tick_count = 0;

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        tick_count++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void TIM2_Interrupt_Init(uint16_t prescaler, uint16_t period)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = period;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM2, ENABLE);
}
```

## Timer Instances

| Timer | Type | APB | Channels |
|-------|------|-----|----------|
| TIM1 | Advanced | APB2 | 4 + complementary |
| TIM2 | General | APB1 | 4 |
| TIM3 | General | APB1 | 4 |
| TIM4 | General | APB1 | 4 |
| TIM5 | General (32-bit on D8) | APB1 | 4 |
| TIM6 | Basic | APB1 | 0 |
| TIM7 | Basic | APB1 | 0 |
| TIM8 | Advanced | APB2 | 4 + complementary |
| TIM9 | General | APB2 | 2 |
| TIM10 | General | APB2 | 1 |

## Pitfalls
- **TIM1/TIM8 require `TIM_CtrlPWMOutputs()`** -- without it, outputs stay low
- **Prescaler is 0-based** -- `TIM_Prescaler = 71` means divide by 72
- **Period is auto-reload value** -- PWM frequency = clock / ((PSC+1)*(ARR+1))
- **Dead-time for complementary outputs** -- set in BDTR register for motor drive safety
