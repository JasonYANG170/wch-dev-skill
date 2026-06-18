# Recipe: Timer and PWM

## Scenario
Configure timers for periodic interrupts, PWM output, input capture, and encoder interface.

## API Quick Reference

### Timer Init Structure
```c
typedef struct
{
    uint16_t TIM_Prescaler;         // 0..65535, divides timer clock
    uint16_t TIM_CounterMode;       // TIM_CounterMode_Up, Down, CenterAligned1/2/3
    uint16_t TIM_Period;            // 0..65535, auto-reload value
    uint16_t TIM_ClockDivision;     // TIM_CKD_DIV1, DIV2, DIV4
    uint8_t TIM_RepetitionCounter;  // TIM1 only
} TIM_TimeBaseInitTypeDef;
```

### PWM Init Structure
```c
typedef struct
{
    uint16_t TIM_OCMode;        // TIM_OCMode_PWM1, TIM_OCMode_PWM2
    uint16_t TIM_OutputState;   // TIM_OutputState_Enable
    uint16_t TIM_OutputNState;  // TIM_OutputNState_Enable (TIM1 complementary)
    uint16_t TIM_Pulse;         // PWM duty cycle value (0..Period)
    uint16_t TIM_OCPolarity;    // TIM_OCPolarity_High, TIM_OCPolarity_Low
    uint16_t TIM_OCNPolarity;
    uint16_t TIM_OCIdleState;
    uint16_t TIM_OCNIdleState;
} TIM_OCInitTypeDef;
```

### Key Functions
```c
void     TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void     TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void     TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void     TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void     TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void     TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);
void     TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState);  // TIM1 only
void     TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState);
void     TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1);
void     TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2);
void     TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3);
void     TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4);
void     TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload);
void     TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t Prescaler);
void     TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void     TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
```

### Timer Channels (CH32V003)
| Timer | CH1 | CH2 | CH3 | CH4 |
|-------|-----|-----|-----|-----|
| TIM1 | PD2 | PA1 | PC3 | PC4 |
| TIM2 | PD4 | PD3 | PC0 | PD7 |

### Interrupt Flags
```c
TIM_IT_Update  // Update (overflow)
TIM_IT_CC1     // Capture/Compare 1
TIM_IT_CC2     // Capture/Compare 2
TIM_IT_CC3     // Capture/Compare 3
TIM_IT_CC4     // Capture/Compare 4
TIM_IT_Trigger // Trigger
TIM_IT_Break   // Break
```

## Complete Call Chains

### Periodic Interrupt (1ms)
```c
void TIM2_1ms_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 24MHz / 24 / 1000 = 1kHz (1ms)
    TIM_TimeBaseStructure.TIM_Prescaler = 24 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 1ms tick handler
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
```

### PWM Output (1kHz, 50% duty)
```c
void TIM1_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1, ENABLE);

    // TIM1_CH3 on PC3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Timer base: 24MHz / 24 / 1000 = 1kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 24 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWM mode 1, 50% duty
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500;  // 50% of 1000
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);  // Required for TIM1
    TIM_Cmd(TIM1, ENABLE);
}

// Change duty cycle at runtime:
TIM_SetCompare3(TIM1, new_duty);  // 0..999
```

### Input Capture
```c
volatile uint32_t capture_value = 0;

void TIM2_IC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_ICInitTypeDef TIM_ICInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // TIM2_CH1 on PD4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {
        capture_value = TIM_GetCapture1(TIM2);
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}
```

## PWM Frequency Calculation

```
PWM_Frequency = Timer_Clock / (Prescaler + 1) / (Period + 1)
Duty_Cycle% = Compare_Value / (Period + 1) * 100
```

Example with 24MHz system clock:
- Prescaler=23, Period=999 -> 24MHz/24/1000 = 1kHz
- Prescaler=23, Period=99 -> 24MHz/24/100 = 10kHz
- Prescaler=0, Period=999 -> 24MHz/1/1000 = 24kHz

## Common Errors

1. **TIM1 needs `TIM_CtrlPWMOutputs(ENABLE)`** -- Without this, TIM1 PWM outputs stay low
2. **Wrong GPIO mode** -- PWM output must be `GPIO_Mode_AF_PP`
3. **Prescaler off by one** -- Prescaler value is divider minus 1 (value 23 = divide by 24)
4. **Period off by one** -- Period value counts from 0 (value 999 = 1000 counts)
