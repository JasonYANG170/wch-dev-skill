# GPIO Control

## Overview

The CH32H417 has up to 112 GPIO pins across 7 ports (GPIOA-GPIOF). Each pin can be configured as input, output, alternate function, or analog. The chip uses a flexible AF (Alternate Function) mapping system with 16 AF options per pin.

## Key API Functions

```c
// Initialization
void GPIO_DeInit(GPIO_TypeDef *GPIOx);
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);

// Read
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);

// Write
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal);

// Configuration
void GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, uint8_t GPIO_PinSource, uint8_t GPIO_AF);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_IPD_Unused(void);
```

## GPIO Modes

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

## GPIO Speed

| Speed | Description |
|-------|-------------|
| `GPIO_Speed_Low` | Low speed |
| `GPIO_Speed_Medium` | Medium speed |
| `GPIO_Speed_High` | High speed |
| `GPIO_Speed_Very_High` | Very high speed (default for most AF uses) |

## Alternate Function Mapping

CH32H417 uses AF0-AF15 for pin multiplexing:

```c
// Example: Configure PA9 as USART1_TX (AF7)
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_Init(GPIOA, &GPIO_InitStruct);

// Select AF7 for USART1
GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF7);
```

## Common AF Mappings

| AF | Typical Use |
|----|-------------|
| AF0 | SWD/JTAG, RTC |
| AF1 | TIM1, TIM2 |
| AF2 | TIM3, TIM4, TIM5 |
| AF3 | TIM8, TIM9, TIM10, TIM11 |
| AF4 | I2C1, I2C2 |
| AF5 | SPI1, SPI2, I2S |
| AF6 | SPI3 |
| AF7 | USART1, USART2, USART3 |
| AF8 | UART4, UART5, USART6 |
| AF9 | CAN1, CAN2, ETH |
| AF10 | USB, SDIO |
| AF11 | LTDC |
| AF12 | FMC |
| AF13 | DCMI |
| AF14 | Various |
| AF15 | EVENTOUT |

## Example: Button Input with EXTI

```c
void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    EXTI_InitTypeDef EXTI_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // Configure PA0 as input with pull-up
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Connect EXTI0 to PA0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // Configure EXTI
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configure NVIC
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // Handle button press
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
```

## Pin Remapping

Some peripherals have multiple possible pin assignments:

```c
// Enable SWJ disable to free up PA13/PA14
GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
```

## VIO Voltage Configuration

CH32H417 supports multiple I/O voltage domains:

```c
// For 1.8V I/O (high-speed capable)
GPIO_PinRemapConfig(GPIO_Remap_VIO1V8_IO_HSLV, ENABLE);

// For 3.3V I/O (standard)
GPIO_PinRemapConfig(GPIO_Remap_VIO3V3_IO_HSLV, ENABLE);
```
