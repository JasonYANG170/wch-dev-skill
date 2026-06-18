# LPTIM Low-Power Timer

## Overview

CH32H417 has LPTIM (Low-Power Timer) peripherals that can run from LSI (internal low-speed oscillator) or external clock sources. LPTIM continues operating in Sleep and Stop modes, making it ideal for periodic wakeup from low-power states.

## Key API Functions

```c
void LPTIM_DeInit(LPTIM_TypeDef* LPTIMx);
void LPTIM_Cmd(LPTIM_TypeDef* LPTIMx, FunctionalState NewState);
void LPTIM_TimeBaseInit(LPTIM_TypeDef* LPTIMx, LPTIM_TimeBaseInitTypeDef* LPTIM_TimeBaseInitStruct);
void LPTIM_TimeBaseStructInit(LPTIM_TimeBaseInitTypeDef* LPTIM_TimeBaseInitStruct);
void LPTIM_SetCompare(LPTIM_TypeDef* LPTIMx, uint32_t Compare);
void LPTIM_SetPeriod(LPTIM_TypeDef* LPTIMx, uint32_t Period);
uint32_t LPTIM_GetCounter(LPTIM_TypeDef* LPTIMx);
FlagStatus LPTIM_GetFlagStatus(LPTIM_TypeDef* LPTIMx, uint32_t LPTIM_FLAG);
void LPTIM_ClearFlag(LPTIM_TypeDef* LPTIMx, uint32_t LPTIM_FLAG);
```

## LPTIM Internal Clock Wakeup Example

```c
#include "ch32h417.h"

void LPTIM1_WKUP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void LPTIM1_WKUP_IRQHandler(void)
{
    if (LPTIM_GetFlagStatus(LPTIM1, LPTIM_FLAG_CMPM) == SET)
    {
        printf("LPTIM wakeup!\r\n");
        // Re-init UART if needed after wakeup from Stop mode
        SystemAndCoreClockUpdate();
        USART_Printf_Init(115200);
    }
    EXTI_ClearITPendingBit(EXTI_Line23);
    LPTIM_ClearFlag(LPTIM1, LPTIM_FLAG_CMPM);
}

void LPTIM_Internal_Init(uint16_t arr)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    LPTIM_TimeBaseInitTypeDef LPTIM_TimeBaseInitStruct = {0};

    // Enable clocks
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP |
                          RCC_HB1Periph_LPTIM1, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE | RCC_HB2Periph_AFIO, ENABLE);

    // Configure EXTI for LPTIM wakeup
    EXTI_InitStructure.EXTI_Line = EXTI_Line23;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_SetPriority(LPTIM1_WKUP_IRQn, 0);
    NVIC_EnableIRQ(LPTIM1_WKUP_IRQn);

    LPTIM_Cmd(LPTIM1, ENABLE);

    // Enable LSI as clock source
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);

    // Configure LPTIM with internal LSI clock
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSource = LPTIM_ClockSource_In;
    LPTIM_TimeBaseInitStruct.LPTIM_CountSource = LPTIM_CountSource_Internal;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPrescaler = LPTIM_TClockPrescaler_DIV128;
    LPTIM_TimeBaseInitStruct.LPTIM_InClockSource = LPTIM_InClockSource_LSI;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPolarity = LPTIM_ClockPolarity_Falling;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSampleTime = LPTIM_ClockSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_TriggerSampleTime = LPTIM_TriggerSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_ExTriggerPolarity = LPTIM_ExTriggerPolarity_Disable;
    LPTIM_TimeBaseInitStruct.LPTIM_TimeOut = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_OutputPolarity = LPTIM_OutputPolarity_High;
    LPTIM_TimeBaseInitStruct.LPTIM_UpdateMode = LPTIM_UpdateMode0;
    LPTIM_TimeBaseInitStruct.LPTIM_Encoder = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ForceOutHigh = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_SingleMode = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ContinuousMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_PWMOut = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_Pulse = arr / 4;
    LPTIM_TimeBaseInitStruct.LPTIM_Period = arr;

    LPTIM_TimeBaseInit(LPTIM1, &LPTIM_TimeBaseInitStruct);
}
```

## Sleep with LPTIM Wakeup

```c
void Sleep_Wakeup_LPTIM(void)
{
    LPTIM_Internal_Init(4000);

    printf("Entering Sleep...\r\n");
    __WFI();  // Wait for interrupt (LPTIM will wake up)

    printf("Woke up!\r\n");
}
```

## Stop Mode with LPTIM Wakeup

```c
void Stop_Wakeup_LPTIM(void)
{
    LPTIM_Internal_Init(4000);

    printf("Entering Stop mode...\r\n");
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    // After wakeup, need to reconfigure system clock
    SystemAndCoreClockUpdate();
    USART_Printf_Init(115200);
    printf("Woke up from Stop!\r\n");
}
```

## External Clock Source

```c
void LPTIM_External_Init(uint16_t arr)
{
    LPTIM_TimeBaseInitTypeDef LPTIM_TimeBaseInitStruct = {0};

    // Configure for external clock on PE0
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF1);

    LPTIM_TimeBaseInitStruct.LPTIM_ClockSource = LPTIM_ClockSource_Ex;
    LPTIM_TimeBaseInitStruct.LPTIM_CountSource = LPTIM_CountSource_External;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPrescaler = LPTIM_TClockPrescaler_DIV8;
    LPTIM_TimeBaseInitStruct.LPTIM_InClockSource = LPTIM_InClockSource_PCLK1;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPolarity = LPTIM_ClockPolarity_Falling;
    LPTIM_TimeBaseInitStruct.LPTIM_ContinuousMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_Period = arr;
    LPTIM_TimeBaseInitStruct.LPTIM_Pulse = arr / 4;

    LPTIM_TimeBaseInit(LPTIM1, &LPTIM_TimeBaseInitStruct);
}
```

## Clock Prescaler Options

| Macro | Division |
|-------|----------|
| `LPTIM_TClockPrescaler_DIV1` | /1 |
| `LPTIM_TClockPrescaler_DIV2` | /2 |
| `LPTIM_TClockPrescaler_DIV4` | /4 |
| `LPTIM_TClockPrescaler_DIV8` | /8 |
| `LPTIM_TClockPrescaler_DIV16` | /16 |
| `LPTIM_TClockPrescaler_DIV32` | /32 |
| `LPTIM_TClockPrescaler_DIV64` | /64 |
| `LPTIM_TClockPrescaler_DIV128` | /128 |

## Important Notes

- LPTIM runs in Sleep/Stop modes when using LSI or external clock
- LSI frequency is approximately 40 kHz (varies per chip)
- After Stop mode wakeup, must call `SystemAndCoreClockUpdate()` and re-init peripherals
- The LPTIM wakeup interrupt is routed through EXTI Line 23
