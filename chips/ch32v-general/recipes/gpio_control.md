# Recipe: GPIO Control

## When to Use
User wants to configure GPIO pins for input, output, alternate function, or external interrupt.

## API Reference (from ch32v20x_gpio.h -- same pattern for all families)

### Init Structure
```c
typedef struct {
    uint16_t GPIO_Pin;           // GPIO_Pin_0 .. GPIO_Pin_15, GPIO_Pin_All
    GPIOSpeed_TypeDef GPIO_Speed; // GPIO_Speed_10MHz, GPIO_Speed_2MHz, GPIO_Speed_50MHz
    GPIOMode_TypeDef GPIO_Mode;   // See modes below
} GPIO_InitTypeDef;
```

### GPIO Modes
| Mode | Value | Description |
|------|-------|-------------|
| `GPIO_Mode_AIN` | 0x00 | Analog input |
| `GPIO_Mode_IN_FLOATING` | 0x04 | Floating input |
| `GPIO_Mode_IPD` | 0x28 | Input with pull-down |
| `GPIO_Mode_IPU` | 0x48 | Input with pull-up |
| `GPIO_Mode_Out_OD` | 0x14 | Open-drain output |
| `GPIO_Mode_Out_PP` | 0x10 | Push-pull output |
| `GPIO_Mode_AF_OD` | 0x1C | Alternate function open-drain |
| `GPIO_Mode_AF_PP` | 0x18 | Alternate function push-pull |

### Key Functions
```c
void     GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void     GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void     GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void     GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
uint8_t  GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
void     GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void     GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

## Example: LED Blink on PA0

```c
#include "ch32v20x.h"

int main(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();

    // Step 1: Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Step 2: Configure pin
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    while(1)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        Delay_Ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        Delay_Ms(500);
    }
}
```

## Example: Button Input with EXTI Interrupt

```c
#include "ch32v20x.h"

volatile uint8_t button_pressed = 0;

void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        button_pressed = 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    EXTI_InitTypeDef EXTI_InitStruct = {0};
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // PA0 as input with pull-up
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Connect EXTI0 to PA0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // Configure EXTI for falling edge
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // NVIC config
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
```

## Pitfalls
- **Forgetting RCC clock enable** -- GPIO operations silently fail
- **Forgetting AFIO clock** -- EXTI and pin remap won't work without `RCC_APB2Periph_AFIO`
- **Wrong speed for alternate function** -- Use `GPIO_Speed_50MHz` for USART/SPI/I2C outputs
