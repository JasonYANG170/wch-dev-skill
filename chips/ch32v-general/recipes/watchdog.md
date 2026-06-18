# Recipe: Watchdog (IWDG / WWDG)

## When to Use
User wants to implement system reset protection using independent watchdog (IWDG) or window watchdog (WWDG), prevent firmware hangs, or implement periodic health monitoring.

## Availability
All CH32V families: CH32V103, CH32V20x, CH32V307, CH32V407

## IWDG (Independent Watchdog)

### API Reference (from ch32v*_iwdg.h)

#### Key Functions
```c
void       IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);
void       IWDG_SetPrescaler(uint8_t IWDG_Prescaler);
void       IWDG_SetReload(uint16_t Reload);
void       IWDG_ReloadCounter(void);
void       IWDG_Enable(void);
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);
```

#### IWDG Prescaler Values
| Constant | Divider |
|----------|---------|
| `IWDG_Prescaler_4` | /4 |
| `IWDG_Prescaler_8` | /8 |
| `IWDG_Prescaler_16` | /16 |
| `IWDG_Prescaler_32` | /32 |
| `IWDG_Prescaler_64` | /64 |
| `IWDG_Prescaler_128` | /128 |
| `IWDG_Prescaler_256` | /256 |

#### IWDG Timeout Formula
`Timeout = (Reload + 1) * Prescaler / LSI_Frequency`
LSI is typically ~40 kHz (varies by chip/temperature).

| Prescaler | Reload=4095 | Reload=1000 |
|-----------|-------------|-------------|
| /4 | 0.41s | 0.10s |
| /32 | 3.28s | 0.80s |
| /128 | 13.1s | 3.20s |
| /256 | 26.2s | 6.40s |

### Example: IWDG Init and Feed

```c
#include "ch32v30x.h"  // or ch32v10x.h, ch32v20x.h, ch32v4x7.h

void IWDG_Init_3s(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload(4000);   // ~3.2s timeout
    IWDG_ReloadCounter();   // Feed once before enable
    IWDG_Enable();
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}

// In main loop: call IWDG_Feed() periodically before timeout
int main(void)
{
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    IWDG_Init_3s();

    while(1)
    {
        // Do application work...
        ProcessData();

        // Feed the watchdog (must be called before 3.2s timeout)
        IWDG_Feed();
    }
}
```

---

## WWDG (Window Watchdog)

### API Reference (from ch32v*_wwdg.h)

#### Key Functions
```c
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_EnableIT(void);
void WWDG_SetCounter(uint8_t Counter);
void WWDG_Enable(uint8_t Counter);
FlagStatus WWDG_GetFlagStatus(void);
void WWDG_ClearFlag(void);
```

#### WWDG Prescaler Values
| Constant | Divider |
|----------|---------|
| `WWDG_Prescaler_1` | PCLK1/4096/1 |
| `WWDG_Prescaler_2` | PCLK1/4096/2 |
| `WWDG_Prescaler_4` | PCLK1/4096/4 |
| `WWDG_Prescaler_8` | PCLK1/4096/8 |

#### WWDG Window
- Counter range: 0x40 to 0x7F (6-bit, 64-127)
- Must reload when counter is between window value and 0x40
- If counter drops below window value before reload -> reset
- If counter is above window value when reloaded -> reset

### Example: WWDG with Window

```c
#include "ch32v30x.h"

#define WWDG_CNT  0x7F

static void WWDG_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    WWDG_SetCounter(tr);
    WWDG_SetPrescaler(prv);
    WWDG_SetWindowValue(wr);
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Config();
    WWDG_EnableIT();
}

// Feed WWDG (must be called within the window)
void WWDG_Feed(void)
{
    WWDG_SetCounter(WWDG_CNT);
}

int main(void)
{
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);

    // Window: must feed between 0x40 and 0x5F (counter counting down from 0x7F)
    // Prescaler /8: window ~ (0x7F - 0x5F) * 8 * 4096 / PCLK1
    WWDG_Config(0x7F, 0x5F, WWDG_Prescaler_8);

    while(1)
    {
        Delay_Ms(50);
        // Check if counter is within window before feeding
        uint8_t wwdg_tr = WWDG->CTLR & 0x7F;
        uint8_t wwdg_wr = WWDG->CFGR & 0x7F;
        if(wwdg_tr < wwdg_wr)
        {
            WWDG_Feed();
        }
    }
}

// WWDG early wakeup interrupt
void WWDG_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void WWDG_IRQHandler(void)
{
    WWDG_ClearFlag();
    // Counter reached 0x40 - about to reset if not fed
}
```

---

## IWDG vs WWDG Comparison

| Feature | IWDG | WWDG |
|---------|------|------|
| Clock source | LSI (~40 kHz) | PCLK1 |
| Timeout range | 0.1ms - 26s | ~50us - several ms |
| Window | No (feed anytime) | Yes (must feed within window) |
| Reset when | Counter reaches 0 | Counter reaches 0x3F |
| Interrupt | No | Yes (early wakeup at 0x40) |
| Stop in Standby | No (keeps running) | Yes (stops) |
| Use case | Long timeout, simple | Short timeout, strict timing |

## Pitfalls
- **IWDG cannot be disabled** -- Once enabled, IWDG runs until reset; no software disable
- **IWDG uses LSI clock** -- LSI frequency varies (~30-60 kHz); timeout is approximate
- **WWDG window violation** -- Feeding WWDG when counter > window value causes immediate reset
- **WWDG counter range** -- Counter must be 0x40-0x7F; values below 0x40 trigger reset
- **Interrupt priority** -- WWDG interrupt must have highest priority if used for time-critical feeding
- **Standby mode** -- IWDG continues in Standby/Stop modes; WWDG stops
- **Boot timeout** -- Feed watchdog early in init code to prevent reset during slow initialization (e.g., LSE startup)
