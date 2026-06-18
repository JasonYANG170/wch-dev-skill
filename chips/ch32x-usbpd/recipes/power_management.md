# Power Management

> **Summary**: Configure low-power modes (sleep, standby) and PVD on CH32X/CH6xx chips.

## Trigger Intent

- "Low power mode"
- "Sleep mode"
- "Standby mode"
- "PVD voltage detection"
- "Power saving"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | `RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE)` |
| Reference | `CH32X035EVT/EVT/EXAM/PWR/` |

## Step-by-Step

### STOP Mode (WFI Entry)

```c
#include "ch32x035.h"
#include "debug.h"

void Enter_STOP_Mode(void) {
    // Enable PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Enter STOP mode via WFI (Wait For Interrupt)
    // CPU stops, peripherals stop, SRAM retained
    // Wakeup via EXTI interrupt
    PWR_EnterSTOPMode(PWR_STOPEntry_WFI);
}

// Wakeup handler - system resumes here after interrupt
// Note: after wakeup from STOP, HSI is used as system clock
// You may need to reconfigure the system clock
void ReinitClockAfterStop(void) {
    SystemCoreClockUpdate();
    // Re-init any time-dependent peripherals (USART, etc.)
}
```

### STOP Mode (WFE Entry)

```c
void Enter_STOP_Mode_WFE(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // WFE entry: wakes on event (no interrupt nesting)
    PWR_EnterSTOPMode(PWR_STOPEntry_WFE);
}
```

### STANDBY Mode

```c
void Enter_STANDBY_Mode(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // STANDBY mode: lowest power, SRAM lost
    // Wakeup via: WKUP pin rising edge, RTC alarm, IWDG reset, NRST
    PWR_EnterSTANDBYMode();
    // Code after this line is NOT reached (system resets on wakeup)
}
```

### PVD (Programmable Voltage Detector)

```c
__attribute__((interrupt("WCH-Interrupt-fast")))
void PVD_IRQHandler(void) {
    if(PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET) {
        // VDD dropped below threshold - save critical data
        printf("PVD: Low voltage detected!\r\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line16);
}

void PVD_Init(void) {
    EXTI_InitTypeDef EXTI_InitStruct = {0};
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Configure PVD threshold (2.1V, 2.3V, 3.0V, or 4.0V)
    PWR_PVDLevelConfig(PWR_PVDLevel_3V0);

    // Enable PVD EXTI line
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
```

### Detect VDD Supply Voltage

```c
void Check_VDD(void) {
    PWR_VDD vdd = PWR_VDD_SupplyVoltage();
    if(vdd == PWR_VDD_5V) {
        printf("VDD = 5V\r\n");
    } else {
        printf("VDD = 3.3V\r\n");
    }
}
```

## Power Mode Summary

| Mode | CPU | Peripherals | SRAM | Wakeup Sources | Current |
|------|-----|-------------|------|----------------|---------|
| Run | Active | Active | Active | -- | ~10mA |
| STOP (WFI) | Halted | Halted | Retained | EXTI interrupt | ~uA |
| STOP (WFE) | Halted | Halted | Retained | EXTI event | ~uA |
| STANDBY | Off | Off | Lost | WKUP/RTC/IWDG/NRST | ~uA (lowest) |

## PVD Threshold Reference

| Level | Constant | Threshold |
|-------|----------|-----------|
| 0 | `PWR_PVDLevel_2V1` | 2.1V |
| 1 | `PWR_PVDLevel_2V3` | 2.3V |
| 2 | `PWR_PVDLevel_3V0` | 3.0V |
| 3 | `PWR_PVDLevel_4V0` | 4.0V |

## Common Errors

- Not reconfiguring system clock after wakeup from STOP mode -- USART baud rate wrong
- Forgetting `RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE)` -- PWR register access fails
- STANDBY mode loses all SRAM -- save critical data to Flash before entering
- PVD interrupt priority should be highest (0,0) for critical voltage monitoring
