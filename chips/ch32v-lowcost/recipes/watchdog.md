# Recipe: Watchdog (IWDG and WWDG)

## Scenario
Configure the Independent Watchdog (IWDG) for system reset protection, and the Window Watchdog (WWDG) for detecting stuck execution within a timing window.

## IWDG vs WWDG

| Feature | IWDG | WWDG |
|---------|------|------|
| Clock Source | LSI (~40kHz) | PCLK1 (APB1 bus clock) |
| Timeout Range | ~ms to ~26s | ~ms to ~50ms |
| Window | No window (any-time feed) | Must feed within window |
| Reset Behavior | Counter overflow | Counter below 0x40 or outside window |
| Use Case | General system protection | Strict timing validation |

## API Quick Reference

### IWDG Functions
```c
void       IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);  // Enable/disable register write
void       IWDG_SetPrescaler(uint8_t IWDG_Prescaler);       // Set prescaler divider
void       IWDG_SetReload(uint16_t Reload);                  // Set reload value (0-0xFFF)
void       IWDG_ReloadCounter(void);                         // Feed the watchdog
void       IWDG_Enable(void);                                // Start IWDG (cannot be disabled)
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);           // Check flags
```

### IWDG Prescaler Values
```c
IWDG_Prescaler_4    // Divider 4   -- fastest timeout
IWDG_Prescaler_8    // Divider 8
IWDG_Prescaler_16   // Divider 16
IWDG_Prescaler_32   // Divider 32
IWDG_Prescaler_64   // Divider 64
IWDG_Prescaler_128  // Divider 128
IWDG_Prescaler_256  // Divider 256 -- slowest timeout
```

### IWDG Timeout Formula
```
Timeout = (Reload + 1) * Prescaler / LSI_Frequency
Example: (4000 + 1) * 128 / 40000 = ~12.8 seconds
```

### WWDG Functions
```c
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);  // Set WWDG prescaler
void WWDG_SetWindowValue(uint8_t WindowValue);     // Set window upper bound
void WWDG_Enable(uint8_t Counter);                 // Enable WWDG with initial counter
void WWDG_SetCounter(uint8_t Counter);             // Feed the watchdog
void WWDG_ClearFlag(void);                         // Clear early wakeup flag
void WWDG_EnableIT(void);                          // Enable early wakeup interrupt
```

### WWDG Prescaler Values
```c
WWDG_Prescaler_1  // Divider 1
WWDG_Prescaler_2  // Divider 2
WWDG_Prescaler_4  // Divider 4
WWDG_Prescaler_8  // Divider 8
```

### WWDG Timeout Formula
```
Timeout = (Counter - 0x3F) * Prescaler / PCLK1_Frequency * 4096
Window = (WindowValue - 0x3F) * Prescaler / PCLK1_Frequency * 4096
```

## Complete Call Chains

### IWDG -- Basic Timeout Reset
```c
// Reset after ~4 seconds if not fed
void IWDG_Init_Timeout(u16 prescaler, u16 reload)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prescaler);
    IWDG_SetReload(reload);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

// Usage
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("IWDG Test -- reset after 4s if not fed\r\n");
    Delay_Ms(1000);

    // ~4 second timeout: (4000+1) * 128 / 40000 = 12.8s
    // Use shorter values for faster timeout:
    // (1000+1) * 128 / 40000 = ~3.2s
    IWDG_Init_Timeout(IWDG_Prescaler_128, 1000);

    while(1)
    {
        if(KEY_PRESS() == 1)  // Check user input
        {
            printf("Feed dog\r\n");
            IWDG_ReloadCounter();  // Feed watchdog
        }
        Delay_Ms(10);
    }
}
```

### IWDG with Button Feed
```c
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // Pull-down input
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

u8 KEY_PRESS(void)
{
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 1)
    {
        Delay_Ms(10);  // Debounce
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 1)
            return 1;
    }
    return 0;
}
```

### WWDG -- Window Watchdog
```c
// Window: must feed when counter is between 0x40 and window value
// If fed too early (counter > window) or too late (counter < 0x40), reset occurs

#define WWDG_CNT  0x7F  // Initial counter value

void WWDG_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    // tr = initial counter (0x7F max)
    // wr = window value (0x40 to 0x7F)
    // prv = prescaler (WWDG_Prescaler_1/2/4/8)

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    WWDG_SetCounter(tr);
    WWDG_SetPrescaler(prv);
    WWDG_SetWindowValue(wr);
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Config();
    WWDG_EnableIT();
}

// Feed function -- only feed when counter is within window
void WWDG_Feed(void)
{
    WWDG_SetCounter(WWDG_CNT);
}

// Usage
int main(void)
{
    u8 wwdg_tr, wwdg_wr;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("WWDG Test\r\n");

    // Window: feed only between 0x5F and 0x7F
    // Prescaler 8: timeout ~48M/8/4096 * (0x7F-0x3F) = ~50ms
    WWDG_Config(0x7F, 0x5F, WWDG_Prescaler_8);

    wwdg_wr = WWDG->CFGR & 0x7F;  // Window value

    while(1)
    {
        Delay_Ms(10);

        wwdg_tr = WWDG->CTLR & 0x7F;  // Current counter
        if(wwdg_tr < wwdg_wr)          // Within window
        {
            WWDG_Feed();
        }
    }
}
```

### WWDG Early Wakeup Interrupt
```c
// The WWDG interrupt fires when counter reaches 0x40 (early wakeup)
// This gives a chance to save data before reset

void WWDG_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void WWDG_IRQHandler(void)
{
    if(WWDG->STATR & WWDG_FLAG_EWIF)
    {
        // Counter is about to underflow -- save critical data here
        printf("WWDG Early Wakeup!\r\n");
        WWDG_ClearFlag();
    }
}
```

## Timeout Reference Table

### IWDG (LSI = ~40kHz)
| Prescaler | Reload | Timeout |
|-----------|--------|---------|
| 4 | 1000 | ~100ms |
| 128 | 1000 | ~3.2s |
| 128 | 4000 | ~12.8s |
| 256 | 4095 | ~26.2s |

### WWDG (PCLK1 = 48MHz)
| Prescaler | Counter | Window | Timeout |
|-----------|---------|--------|---------|
| 8 | 0x7F | 0x5F | ~50ms |
| 8 | 0x7F | 0x7F | ~50ms (full window) |
| 1 | 0x7F | 0x5F | ~6ms |

## Common Errors

1. **IWDG cannot be disabled** -- Once `IWDG_Enable()` is called, the watchdog runs until power-off
2. **IWDG write access** -- Must call `IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable)` before setting prescaler/reload
3. **WWDG feed too early** -- Feeding when counter > window value causes reset
4. **WWDG feed too late** -- Not feeding before counter reaches 0x40 causes reset
5. **WWDG clock not enabled** -- Must call `RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE)`
6. **WWDG initial counter** -- Must be 0x40-0x7F; values below 0x40 cause immediate reset
