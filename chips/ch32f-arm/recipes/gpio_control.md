# Recipe: GPIO Control

## Overview

Configure and control GPIO pins on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);

// Output control
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);

// Input reading
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);

// Pin lock and remap
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

## GPIO Mode Options

| Mode | Value | Description |
|------|-------|-------------|
| GPIO_Mode_AIN | 0x00 | Analog input |
| GPIO_Mode_IN_FLOATING | 0x04 | Floating input |
| GPIO_Mode_IPD | 0x28 | Input with pull-down |
| GPIO_Mode_IPU | 0x48 | Input with pull-up |
| GPIO_Mode_Out_OD | 0x14 | Open-drain output |
| GPIO_Mode_Out_PP | 0x10 | Push-pull output |
| GPIO_Mode_AF_OD | 0x1C | Alternate function open-drain |
| GPIO_Mode_AF_PP | 0x18 | Alternate function push-pull |

## GPIO Speed Options

| Speed | Value |
|-------|-------|
| GPIO_Speed_10MHz | 1 |
| GPIO_Speed_2MHz | 2 |
| GPIO_Speed_50MHz | 3 |

## Example: LED Toggle

```c
#include "debug.h"

void GPIO_Toggle_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Enable GPIOA clock (APB2 bus)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA0 as push-pull output at 50MHz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    GPIO_Toggle_Init();
    printf("GPIO Toggle TEST\r\n");

    while(1)
    {
        Delay_Ms(250);
        // Toggle PA0 using bit access macro
        PAout(0) ^= 1;
    }
}
```

## Example: Button Input

```c
void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA1 as input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Button_Pressed(void)
{
    return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_RESET);
}
```

## Example: Alternate Function (USART TX)

```c
void USART1_TX_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Enable USART1 and GPIOA clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 = USART1_TX (alternate function push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

## Bit Access Macros

The header provides direct bit access macros for each port:

```c
PAout(n)  // Write to PAn (output)
PAin(n)   // Read from PAn (input)
PBout(n)  // Write to PBn
PBin(n)   // Read from PBn
// ... same for PC, PD, PE, PF, PG
```

## GPIO Ports Available

| Port | Description |
|------|-------------|
| GPIOA | Port A (PA0-PA15) |
| GPIOB | Port B (PB0-PB15) |
| GPIOC | Port C (PC0-PC15) |
| GPIOD | Port D (PD0-PD15) |
| GPIOE | Port E (PE0-PE15) |
| GPIOF | Port F (PF0-PF15, CH32F20x) |
| GPIOG | Port G (PG0-PG15, CH32F20x) |
