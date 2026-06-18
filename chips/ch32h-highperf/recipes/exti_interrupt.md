# EXTI External Interrupts

## Overview

CH32H417 supports external interrupts (EXTI) on GPIO pins. Each GPIO pin can be configured to generate an interrupt on rising edge, falling edge, or both. EXTI lines 0-15 map to the corresponding GPIO pin numbers across all ports.

## Key API Functions

```c
void EXTI_DeInit(void);
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
void EXTI_ClearFlag(uint32_t EXTI_Line);
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

## EXTI0 Interrupt Example (PB0 Falling Edge)

```c
#include "ch32h417.h"

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void EXTI7_0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        printf("EXTI0 triggered!\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI0_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    // Enable clocks
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);

    // PB0 as input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Map PB0 to EXTI Line 0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

    // Configure EXTI
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Enable NVIC
    NVIC_SetPriority(EXTI7_0_IRQn, 0);
    NVIC_EnableIRQ(EXTI7_0_IRQn);
}
```

## EXTI Line Mapping

EXTI lines 0-15 map to GPIO pin numbers. The port is selected via `GPIO_EXTILineConfig()`:

```c
// PA0 -> EXTI_Line0
GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

// PB5 -> EXTI_Line5
GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

// PC13 -> EXTI_Line13
GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
```

## EXTI Trigger Modes

| Macro | Description |
|-------|-------------|
| `EXTI_Trigger_Rising` | Rising edge only |
| `EXTI_Trigger_Falling` | Falling edge only |
| `EXTI_Trigger_Rising_Falling` | Both edges |

## EXTI Mode

| Macro | Description |
|-------|-------------|
| `EXTI_Mode_Interrupt` | Generate interrupt |
| `EXTI_Mode_Event` | Generate event (no interrupt) |

## CH32H417 EXTI IRQ Handlers

On CH32H417, EXTI lines are grouped into shared IRQ handlers:

| IRQ Handler | EXTI Lines |
|-------------|------------|
| `EXTI7_0_IRQn` / `EXTI7_0_IRQHandler` | EXTI 0-7 |
| `EXTI15_8_IRQn` / `EXTI15_8_IRQHandler` | EXTI 8-15 |

## Important Notes

- Always enable both `RCC_HB2Periph_AFIO` and the GPIO port clock
- Use `GPIO_EXTILineConfig()` to select which port maps to the EXTI line
- Clear the interrupt flag in the handler to prevent re-entry
- Use `__attribute__((interrupt("WCH-Interrupt-fast")))` for fast interrupt handling
