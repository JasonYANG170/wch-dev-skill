# Recipe: Power Management

## Scenario
Configure low-power modes -- sleep mode, standby mode, PVD voltage monitoring, and auto-wakeup.

## API Quick Reference

### PWR Functions
```c
void       PWR_DeInit(void);
void       PWR_PVDCmd(FunctionalState NewState);
void       PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void       PWR_AutoWakeUpCmd(FunctionalState NewState);
void       PWR_AWU_SetPrescaler(uint32_t AWU_Prescaler);
void       PWR_AWU_SetWindowValue(uint8_t WindowValue);
void       PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
```

### PVD Levels
```c
PWR_PVDLevel_2V9  // 2.9V threshold
PWR_PVDLevel_3V1  // 3.1V
PWR_PVDLevel_3V3  // 3.3V
PWR_PVDLevel_3V5  // 3.5V
PWR_PVDLevel_3V7  // 3.7V
PWR_PVDLevel_3V9  // 3.9V
PWR_PVDLevel_4V1  // 4.1V
PWR_PVDLevel_4V4  // 4.4V
```

### AWU Prescaler
```c
PWR_AWU_Prescaler_1      // Fastest wakeup
PWR_AWU_Prescaler_2
PWR_AWU_Prescaler_4
PWR_AWU_Prescaler_8
PWR_AWU_Prescaler_16
PWR_AWU_Prescaler_32
PWR_AWU_Prescaler_64
PWR_AWU_Prescaler_128
PWR_AWU_Prescaler_256
PWR_AWU_Prescaler_512
PWR_AWU_Prescaler_1024
PWR_AWU_Prescaler_2048
PWR_AWU_Prescaler_4096
PWR_AWU_Prescaler_10240
PWR_AWU_Prescaler_61440   // Slowest wakeup
```

### Standby Entry Mode
```c
PWR_STANDBYEntry_WFI  // Wait For Interrupt
PWR_STANDBYEntry_WFE  // Wait For Event
```

### RISC-V Low-Power Instructions
```c
__WFI();  // Wait For Interrupt -- CPU sleeps until interrupt
__WFE();  // Wait For Event -- CPU sleeps until event
```

### Power Flags
```c
PWR_FLAG_PVDO  // PVD output (voltage below threshold)
```

## Complete Call Chains

### Sleep Mode (WFI)
```c
// Sleep mode: CPU stops, peripherals continue running
// Wakeup by any enabled interrupt

void Enter_Sleep_Mode(void)
{
    // Configure wakeup source (e.g., EXTI on PD0)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    printf("Entering Sleep Mode...\r\n");
    __WFI();  // CPU sleeps here
    printf("Woke up from Sleep Mode!\r\n");
}

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
```

### Standby Mode
```c
// Standby mode: Lowest power, all clocks stopped
// Wakeup by: NRST pin, IWDG reset, or WKUP pin (PA0)

void Enter_Standby_Mode(void)
{
    printf("Entering Standby Mode...\r\n");

    // Enable auto-wakeup if needed
    PWR_AutoWakeUpCmd(ENABLE);
    PWR_AWU_SetPrescaler(PWR_AWU_Prescaler_10240);
    PWR_AWU_SetWindowValue(63);

    // Enter standby
    PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFI);

    // Code here runs after wakeup (reset-like)
    printf("Woke up from Standby Mode!\r\n");
}
```

### PVD Voltage Monitoring
```c
void PVD_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // Configure PVD at 3.3V threshold
    PWR_PVDLevelConfig(PWR_PVDLevel_3V3);
    PWR_PVDCmd(ENABLE);

    // Connect PVD to EXTI Line 8
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void PVD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PVD_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        if(PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET)
        {
            printf("WARNING: Voltage below 3.3V!\r\n");
            // Save critical data, reduce power consumption
        }
        else
        {
            printf("Voltage recovered above 3.3V\r\n");
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}
```

### Auto-Wakeup (AWU)
```c
void AWU_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    PWR_AutoWakeUpCmd(ENABLE);
    PWR_AWU_SetPrescaler(PWR_AWU_Prescaler_10240);
    PWR_AWU_SetWindowValue(63);  // ~1 second wakeup interval

    NVIC_InitStructure.NVIC_IRQChannel = AWU_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void AWU_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void AWU_IRQHandler(void)
{
    // AWU wakeup flag is cleared by hardware
    printf("AWU Wakeup\r\n");
}
```

## Power Consumption Reference

| Mode | Typical Current | Wakeup Source |
|------|-----------------|---------------|
| Run (24MHz) | ~5mA | -- |
| Sleep (WFI) | ~2mA | Any interrupt |
| Standby | ~10uA | NRST, IWDG, AWU, WKUP |

## Common Errors

1. **PWR clock not enabled** -- `RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE)` required
2. **Wakeup interrupt not configured** -- Must configure EXTI/NVIC before WFI
3. **PVD not enabled** -- Must call both `PWR_PVDLevelConfig()` and `PWR_PVDCmd(ENABLE)`
4. **AWU window value too small** -- Must be >= 1 for auto-wakeup to trigger
