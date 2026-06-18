# GPIO Control

> **Summary**: Configure GPIO pins for input, output, interrupt, and pin remapping on CH32X/CH6xx chips.

## Trigger Intent

- "Configure GPIO"
- "Control LED"
- "Read button"
- "GPIO interrupt"
- "Pin remap"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` (or chip-specific equivalent) |
| Clock | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE)` |
| Reference | `CH32X035EVT/EVT/EXAM/GPIO/` |

## Step-by-Step

### GPIO Output (LED Control)

```c
#include "ch32x035.h"
#include "debug.h"

int main(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();

    // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA0 as push-pull output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    while(1) {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0,
            (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0)));
        Delay_Ms(500);
    }
}
```

### GPIO Input (Button Read)

```c
// Enable GPIOB clock
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

// Configure PB1 as pull-up input
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // Input with pull-up
GPIO_Init(GPIOB, &GPIO_InitStruct);

// Read button state (active low)
uint8_t pressed = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET);
```

### GPIO Interrupt (EXTI)

```c
#include "ch32x035.h"
#include "debug.h"

volatile uint8_t key_flag = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
void EXTI0_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        key_flag = 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI0_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    EXTI_InitTypeDef EXTI_InitStruct = {0};
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // Configure PA0 as floating input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Connect PA0 to EXTI0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // Configure EXTI0 for falling edge
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configure NVIC
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    EXTI0_Init();
    printf("EXTI0 ready\r\n");

    while(1) {
        if(key_flag) {
            key_flag = 0;
            printf("Key pressed!\r\n");
        }
        __WFI();
    }
}
```

### GPIO Pin Remap

```c
// Enable AFIO clock for remap
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

// Remap USART1 to PA11(TX)/PA12(RX) instead of PA9/PA8
GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);

// Remap SPI1 to alternate pins
GPIO_PinRemapConfig(GPIO_PartialRemap1_SPI1, ENABLE);

// Remap PIOC function
GPIO_PinRemapConfig(GPIO_Remap_PIOC, ENABLE);
```

## GPIO Mode Reference

| Mode | Value | Description |
|------|-------|-------------|
| `GPIO_Mode_AIN` | 0x00 | Analog input (for ADC) |
| `GPIO_Mode_IN_FLOATING` | 0x04 | Floating input |
| `GPIO_Mode_IPD` | 0x28 | Input with pull-down (PA0-PA15, PC16-PC17 only) |
| `GPIO_Mode_IPU` | 0x48 | Input with pull-up |
| `GPIO_Mode_Out_PP` | 0x10 | Push-pull output |
| `GPIO_Mode_AF_PP` | 0x18 | Alternate function push-pull |

## Common Errors

- Using `GPIO_Mode_Out_PP` for peripheral TX pins -- must use `GPIO_Mode_AF_PP`
- Forgetting `RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE)` before pin remap
- Pull-down only available on PA0-PA15 and PC16-PC17
