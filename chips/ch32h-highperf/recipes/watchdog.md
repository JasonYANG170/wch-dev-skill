# Watchdog Timers

## Overview

CH32H417 has two watchdog peripherals:
- **IWDG** (Independent Watchdog): Runs from LSI clock, resets MCU if not refreshed within timeout. Cannot be stopped once started.
- **WWDG** (Window Watchdog): Resets MCU if refreshed too early or too late. Uses APB clock with prescaler.

## IWDG Key Functions

```c
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler);
void IWDG_SetReload(uint16_t Reload);
void IWDG_ReloadCounter(void);
void IWDG_Enable(void);
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);
```

## IWDG Example

```c
#include "ch32h417.h"

void IWDG_Init_Example(void)
{
    // Enable write access to IWDG registers
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // Set prescaler: LSI/256 ~ 40kHz/256 = ~156Hz
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    // Set reload value: timeout = reload / (LSI/prescaler)
    // 1562 / 156 = ~10 seconds
    IWDG_SetReload(1562);

    // Reload counter
    IWDG_ReloadCounter();

    // Enable IWDG (cannot be disabled after this)
    IWDG_Enable();
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}
```

## IWDG Prescaler Values

| Macro | Division |
|-------|----------|
| `IWDG_Prescaler_4` | /4 |
| `IWDG_Prescaler_8` | /8 |
| `IWDG_Prescaler_16` | /16 |
| `IWDG_Prescaler_32` | /32 |
| `IWDG_Prescaler_64` | /64 |
| `IWDG_Prescaler_128` | /128 |
| `IWDG_Prescaler_256` | /256 |

## WWDG Key Functions

```c
void WWDG_DeInit(void);
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_EnableIT(void);
void WWDG_SetCounter(uint8_t Counter);
void WWDG_Enable(uint8_t Counter);
FlagStatus WWDG_GetFlagStatus(void);
void WWDG_ClearFlag(void);
```

## WWDG Example

```c
void WWDG_Init_Example(void)
{
    // Enable WWDG clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    // Set prescaler: PCLK1/4096/8
    WWDG_SetPrescaler(WWDG_Prescaler_8);

    // Set window value (must refresh between window and counter)
    WWDG_SetWindowValue(0x50);

    // Enable WWDG with counter value (counter must be > window)
    WWDG_Enable(0x7F);

    // Enable early wakeup interrupt (optional)
    WWDG_EnableIT();
}

void WWDG_Feed(void)
{
    // Refresh counter (must be between window value and 0x7F)
    WWDG_SetCounter(0x7F);
}
```

## WWDG Prescaler Values

| Macro | Division |
|-------|----------|
| `WWDG_Prescaler_1` | /1 |
| `WWDG_Prescaler_2` | /2 |
| `WWDG_Prescaler_4` | /4 |
| `WWDG_Prescaler_8` | /8 |

## IWDG vs WWDG

| Feature | IWDG | WWDG |
|---------|------|------|
| Clock | LSI (~40kHz) | APB clock |
| Timeout | Long (ms to s) | Short (us to ms) |
| Window | No | Yes (must refresh in window) |
| Stop in debug | Configurable | Configurable |
| Once started | Cannot stop | Cannot stop |

## Important Notes

- IWDG: Once enabled, cannot be disabled except by reset
- WWDG: Counter must be refreshed when between window value and 0x3F (lower = reset)
- IWDG timeout = (Reload * Prescaler) / LSI_frequency
- WWDG timeout = (Counter - Window) * Prescaler / PCLK1_frequency * 4096
