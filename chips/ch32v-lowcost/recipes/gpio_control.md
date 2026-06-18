# Recipe: GPIO Control

## Scenario
Configure GPIO pins for input, output, push-pull, open-drain, alternate function, or EXTI interrupt.

## API Quick Reference

### GPIO Init Structure
```c
typedef struct
{
    uint16_t GPIO_Pin;           // GPIO_Pin_0 .. GPIO_Pin_7, GPIO_Pin_All
    GPIOSpeed_TypeDef GPIO_Speed; // GPIO_Speed_10MHz, GPIO_Speed_2MHz, GPIO_Speed_30MHz
    GPIOMode_TypeDef GPIO_Mode;   // See modes below
} GPIO_InitTypeDef;
```

### GPIO Modes
| Mode | Value | Use Case |
|------|-------|----------|
| `GPIO_Mode_AIN` | 0x00 | Analog input (ADC) |
| `GPIO_Mode_IN_FLOATING` | 0x04 | Floating input (SPI MISO, external pull) |
| `GPIO_Mode_IPD` | 0x28 | Input pull-down |
| `GPIO_Mode_IPU` | 0x48 | Input pull-up |
| `GPIO_Mode_Out_OD` | 0x14 | Output open-drain |
| `GPIO_Mode_Out_PP` | 0x10 | Output push-pull (LED, digital out) |
| `GPIO_Mode_AF_OD` | 0x1C | Alternate function open-drain (I2C) |
| `GPIO_Mode_AF_PP` | 0x18 | Alternate function push-pull (UART TX, SPI SCK) |

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

### Available Ports (CH32V003)
- `GPIOA` -- Partial (PA1, PA2 only)
- `GPIOC` -- PC0..PC7
- `GPIOD` -- PD0..PD7

### Pin Remap Options
```c
GPIO_Remap_SPI1                // SPI1 alternate pin mapping
GPIO_PartialRemap1_USART1     // USART1 partial remap 1
GPIO_FullRemap_USART1         // USART1 full remap
GPIO_PartialRemap1_TIM1       // TIM1 partial remap 1
GPIO_FullRemap_TIM1           // TIM1 full remap
GPIO_Remap_SDI_Disable        // Reclaim PD7 from SDI debug as GPIO
```

## Complete Call Chains

### Output (LED Toggle)
```c
void GPIO_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

// In main loop:
GPIO_WriteBit(GPIOD, GPIO_Pin_0, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
```

### Input (Button Read)
```c
void GPIO_Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Internal pull-up
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

// Read button:
if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) == Bit_RESET)
{
    // Button pressed (active low)
}
```

### EXTI Interrupt
```c
void EXTI0_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);

    // Configure GPIO as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Connect GPIO to EXTI
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);

    // Configure EXTI
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// Interrupt handler
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // Handle interrupt
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
```

## Common Errors

1. **Forgetting RCC clock enable** -- GPIO init does nothing without `RCC_APB2PeriphClockCmd()`
2. **Wrong GPIO mode for peripheral** -- Use `GPIO_Mode_AF_PP` for UART TX, SPI SCK; `GPIO_Mode_IN_FLOATING` for UART RX, SPI MISO
3. **EXTI line not connected** -- Must call `GPIO_EXTILineConfig()` before `EXTI_Init()`
4. **SDI pin conflict** -- PD7 is SDI debug by default; use `GPIO_Remap_SDI_Disable` to reclaim it
