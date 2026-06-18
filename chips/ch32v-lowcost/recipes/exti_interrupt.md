# Recipe: EXTI External Interrupt

## Scenario
Configure external interrupts (EXTI) on GPIO pins for edge-triggered wakeup, button handling, or event detection.

## EXTI Lines

CH32V003/CH32V006 have EXTI lines 0-7 (one per pin number across all ports). CH32L103 extends to EXTI line 15.

| EXTI Line | Available Pins | IRQ Handler |
|-----------|---------------|-------------|
| EXTI0 | PA0, PC0, PD0 | EXTI7_0_IRQn |
| EXTI1 | PA1, PC1, PD1 | EXTI7_0_IRQn |
| EXTI2 | PC2, PD2 | EXTI7_0_IRQn |
| EXTI3 | PC3, PD3 | EXTI7_0_IRQn |
| EXTI4 | PC4, PD4 | EXTI7_0_IRQn |
| EXTI5 | PC5, PD5 | EXTI7_0_IRQn |
| EXTI6 | PC6, PD6 | EXTI7_0_IRQn |
| EXTI7 | PC7, PD7 | EXTI7_0_IRQn |

> All EXTI lines 0-7 share a single IRQ handler `EXTI7_0_IRQHandler`.

## API Quick Reference

### EXTI Init Structure
```c
typedef struct
{
    uint32_t EXTI_Line;       // EXTI_Line0 .. EXTI_Line15
    uint32_t EXTI_Mode;       // EXTI_Mode_Interrupt or EXTI_Mode_Event
    uint32_t EXTI_Trigger;    // EXTI_Trigger_Rising / Falling / Rising_Falling
    FunctionalState EXTI_LineCmd; // ENABLE or DISABLE
} EXTI_InitTypeDef;
```

### Key Functions
```c
void       EXTI_Init(EXTI_InitTypeDef *EXTI_InitStruct);
void       EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);  // Software trigger
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
void       EXTI_ClearFlag(uint32_t EXTI_Line);
ITStatus   EXTI_GetITStatus(uint32_t EXTI_Line);
void       EXTI_ClearITPendingBit(uint32_t EXTI_Line);
void       GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

### Trigger Modes
```c
EXTI_Trigger_Rising       // Rising edge only
EXTI_Trigger_Falling      // Falling edge only
EXTI_Trigger_Rising_Falling  // Both edges
```

## Complete Call Chains

### Basic External Interrupt on PD0 (Falling Edge)
```c
void EXTI0_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);

    // Configure PD0 as pull-up input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Connect PD0 to EXTI Line 0
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

// IRQ handler -- shared for EXTI lines 0-7
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        printf("EXTI0 triggered!\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0);  // MUST clear flag
    }
}
```

### Multi-Pin EXTI (PD0 + PD1)
```c
void EXTI_Multi_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);

    // PD0 and PD1 as pull-up inputs
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Connect to EXTI
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);

    // EXTI Line 0
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // EXTI Line 1
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC (same IRQ for both)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        printf("PD0 pressed\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        printf("PD1 pressed\r\n");
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
```

### EXTI for Sleep Mode Wakeup
```c
// EXTI can wake the CPU from WFI sleep mode
void Sleep_With_EXTI_Wakeup(void)
{
    // ... EXTI init as above ...

    printf("Entering sleep, press PD0 to wake...\r\n");
    __WFI();  // CPU sleeps until EXTI fires
    printf("Woke up!\r\n");
}
```

### Software-Triggered EXTI
```c
// Trigger EXTI interrupt from software (useful for testing)
EXTI_GenerateSWInterrupt(EXTI_Line0);
```

### Both-Edge Detection
```c
// Detect both press and release
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
EXTI_Init(&EXTI_InitStructure);
```

## GPIO Mode Reference for EXTI

| Use Case | GPIO_Mode | Description |
|----------|-----------|-------------|
| Button (active low) | GPIO_Mode_IPU | Pull-up, falling edge triggers |
| Button (active high) | GPIO_Mode_IPD | Pull-down, rising edge triggers |
| External signal | GPIO_Mode_IN_FLOATING | No internal pull, signal drives |
| Both edges | GPIO_Mode_IPU or IPD | Use EXTI_Trigger_Rising_Falling |

## Common Errors

1. **AFIO clock not enabled** -- Must enable `RCC_APB2Periph_AFIO` for `GPIO_EXTILineConfig()`
2. **Flag not cleared** -- Always call `EXTI_ClearITPendingBit()` in the IRQ handler, or the interrupt fires continuously
3. **Wrong port source** -- `GPIO_EXTILineConfig()` must match the actual port (e.g., `GPIO_PortSourceGPIOD` for PDx)
4. **Pin number mismatch** -- EXTI line number must match pin number (PD0 -> EXTI_Line0, PD3 -> EXTI_Line3)
5. **NVIC not configured** -- EXTI init is incomplete without NVIC setup
6. **Attribute missing** -- IRQ handler must use `__attribute__((interrupt("WCH-Interrupt-fast")))`
