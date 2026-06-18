# Timer and PWM

> **Summary**: Configure timers for periodic interrupts, PWM output, and input capture on CH32X/CH6xx chips.

## Trigger Intent

- "Timer interrupt"
- "PWM output"
- "Generate PWM"
- "Input capture"
- "Timer periodic"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | APB2 for TIM1, APB1 for TIM2/TIM3 |
| Reference | `CH32X035EVT/EVT/EXAM/TIM/` |

## Step-by-Step

### Timer Periodic Interrupt

```c
#include "ch32x035.h"
#include "debug.h"

volatile uint32_t timer_count = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
void TIM2_IRQHandler(void) {
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        timer_count++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void TIM2_Init(uint16_t prescaler, uint16_t period) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    // Enable clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Timer configuration
    TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = period;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // Enable update interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // NVIC
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM2, ENABLE);
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // 1ms interrupt at 48MHz: PSC=47, ARR=999
    TIM2_Init(47, 999);
    printf("TIM2 1ms interrupt started\r\n");

    while(1) {
        if(timer_count >= 1000) {
            timer_count = 0;
            printf("1 second elapsed\r\n");
        }
    }
}
```

### PWM Output (TIM1 CH1 on PA8)

```c
void TIM1_PWM_Init(uint16_t prescaler, uint16_t period, uint16_t duty) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA8 = TIM1 CH1 (alternate function push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Time base
    TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = period;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    // PWM mode on channel 1
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStruct.TIM_Pulse = duty;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    // Enable PWM output (required for TIM1)
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

// Usage: 1kHz PWM at 50% duty, 48MHz system clock
// PSC=0, PERIOD=47999, DUTY=23999
TIM1_PWM_Init(0, 47999, 23999);

// Change duty cycle dynamically:
TIM_SetCompare1(TIM1, new_duty);
```

### Input Capture (TIM2 CH1 on PA0)

```c
volatile uint16_t capture_value = 0;
volatile uint8_t capture_flag = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
void TIM2_IRQHandler(void) {
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
        capture_value = TIM_GetCapture1(TIM2);
        capture_flag = 1;
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}

void TIM2_Capture_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_ICInitTypeDef TIM_ICInitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 = TIM2 CH1 input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Time base: free-running counter
    TIM_TimeBaseInitStruct.TIM_Prescaler = 47;  // 1us resolution @ 48MHz
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // Input capture on CH1, rising edge
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICFilter = 0;
    TIM_ICInit(TIM2, &TIM_ICInitStruct);

    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM2, ENABLE);
}
```

## Timer Instance Reference

| Timer | Type | Channels | Clock Bus | Complementary Output |
|-------|------|----------|-----------|---------------------|
| TIM1 | Advanced | 4 | APB2 | Yes (CH1N-CH4N) |
| TIM2 | General | 4 | APB1 | No |
| TIM3 | General | 4 | APB1 | No |

## PWM Frequency Calculation

```
PWM_freq = SystemCLK / ((Prescaler + 1) * (Period + 1))

Example: 1kHz PWM at 48MHz
  Prescaler = 0, Period = 47999
  PWM_freq = 48000000 / (1 * 47999) = 1000 Hz

Duty cycle = Pulse / (Period + 1) * 100%
  Pulse = 23999 -> Duty = 23999 / 48000 * 100% = 50%
```

## Common Errors

- Forgetting `TIM_CtrlPWMOutputs(TIM1, ENABLE)` for TIM1 -- no PWM output
- Not clearing interrupt flag in handler -- infinite interrupt loop
- Using wrong clock bus (APB1 vs APB2) for RCC enable
