# Recipe: Power Management

## When to Use
User wants to implement low-power modes (Sleep, Stop, Standby), PVD monitoring, or wakeup sources.

## API Reference (from ch32v20x_pwr.h)

### Key Functions
```c
void       PWR_DeInit(void);
void       PWR_BackupAccessCmd(FunctionalState NewState);
void       PWR_PVDCmd(FunctionalState NewState);
void       PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void       PWR_WakeUpPinCmd(FunctionalState NewState);
void       PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void       PWR_EnterSTANDBYMode(void);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void       PWR_ClearFlag(uint32_t PWR_FLAG);

// RAM retention variants (CH32V20x)
void       PWR_EnterSTANDBYMode_RAM(void);
void       PWR_EnterSTANDBYMode_RAM_LV(void);
void       PWR_EnterSTOPMode_RAM_LV(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
```

### PVD Levels
| Constant | Voltage |
|----------|---------|
| `PWR_PVDLevel_2V2` | 2.2V |
| `PWR_PVDLevel_2V3` | 2.3V |
| `PWR_PVDLevel_2V4` | 2.4V |
| `PWR_PVDLevel_2V5` | 2.5V |
| `PWR_PVDLevel_2V6` | 2.6V |
| `PWR_PVDLevel_2V7` | 2.7V |
| `PWR_PVDLevel_2V8` | 2.8V |
| `PWR_PVDLevel_2V9` | 2.9V |

### Stop Mode Entry
| Constant | Description |
|----------|-------------|
| `PWR_STOPEntry_WFI` | Enter Stop via WFI instruction |
| `PWR_STOPEntry_WFE` | Enter Stop via WFE instruction |

### Regulator in Stop Mode
| Constant | Description |
|----------|-------------|
| `PWR_Regulator_ON` | Regulator on in Stop |
| `PWR_Regulator_LowPower` | Regulator low-power in Stop |

### Power Flags
| Flag | Description |
|------|-------------|
| `PWR_FLAG_WU` | Wakeup flag |
| `PWR_FLAG_SB` | Standby flag |
| `PWR_FLAG_PVDO` | PVD output flag |

## Power Modes Summary

| Mode | CPU | SRAM | Peripherals | Wakeup Source | Wakeup Time |
|------|-----|------|-------------|---------------|-------------|
| Sleep (WFI) | Off | On | On | Any interrupt | Fast |
| Stop | Off | On | Off | EXTI, RTC, PVD | Medium |
| Standby | Off | Off | Off | WKUP pin, RTC, IWDG | Slow (reset) |

## Example: Enter Stop Mode with EXTI Wakeup

```c
#include "ch32v20x.h"

void Enter_Stop_Mode(void)
{
    // Configure PA0 as wakeup source (EXTI0, falling edge)
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // Enter Stop mode
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    // Execution resumes here after wakeup
    // System clock may need reconfiguration
    SystemInit();
}

void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
```

## Example: Enter Standby Mode

```c
void Enter_Standby_Mode(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Enable WKUP pin (PA0) as wakeup source
    PWR_WakeUpPinCmd(ENABLE);

    // Clear wakeup flag
    PWR_ClearFlag(PWR_FLAG_WU);

    // Enter Standby (system resets on wakeup)
    PWR_EnterSTANDBYMode();

    // Code here is never reached -- system resets on wakeup
}
```

## Example: PVD Configuration

```c
void PVD_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Configure PVD level
    PWR_PVDLevelConfig(PWR_PVDLevel_2V8);
    PWR_PVDCmd(ENABLE);

    // EXTI16 = PVD output
    EXTI_InitStruct.EXTI_Line = EXTI_Line16;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void PVD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PVD_IRQHandler(void)
{
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO))
    {
        // Voltage below threshold -- save critical data
    }
    EXTI_ClearITPendingBit(EXTI_Line16);
}
```

## Pitfalls
- **Stop mode requires EXTI wakeup** -- configure EXTI line before entering Stop
- **System clock lost in Stop** -- must reconfigure after wakeup (`SystemInit()`)
- **Standby mode causes reset** -- all state lost except backup registers
- **PWR clock must be enabled** -- `RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE)`
- **Clear wakeup flags** before entering low-power mode to avoid immediate wakeup
