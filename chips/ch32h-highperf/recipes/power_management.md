# Power Management

## Overview

CH32H417 supports multiple power modes for low-power applications: Sleep, Stop, and Standby. The PWR peripheral also provides PVD (Programmable Voltage Detector) and VIO18 configuration.

## Key API Functions

```c
void PWR_DeInit(void);
void PWR_BackupAccessCmd(FunctionalState NewState);
void PWR_PVDCmd(FunctionalState NewState);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void PWR_VIO18ModeCfg(uint32_t PWR_VIO18CfgMode);
void PWR_VIO18LevelCfg(uint16_t VIO18Level);
PWR_VIO18InitialStatus PWR_GetVIO18InitialStatus(void);
void PWR_VDD12ExternPower(void);
```

## Sleep Mode (WFI/WFE)

```c
// Enter Sleep mode (CPU stopped, peripherals running)
__asm volatile ("wfi");  // Wait For Interrupt
// or
__asm volatile ("wfe");  // Wait For Event
```

## Stop Mode Example

```c
#include "ch32h417.h"

void Enter_StopMode(void)
{
    // Configure PVD if needed
    PWR_PVDCmd(ENABLE);
    PWR_PVDLevelConfig(PWR_PVDLevel_MODE0);  // ~2.2V threshold

    // Enter Stop mode with regulator in low-power
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    // Execution resumes here after wake-up
    // Reconfigure system clock (it was stopped)
    SystemInit();
}
```

## PVD Configuration

```c
void PVD_Init(void)
{
    PWR_PVDCmd(ENABLE);
    PWR_PVDLevelConfig(PWR_PVDLevel_MODE4);  // Set threshold

    // Configure EXTI for PVD
    EXTI_InitTypeDef EXTI_InitStruct = {0};
    EXTI_InitStruct.EXTI_Line = EXTI_Line16;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct = {0};
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
        // Voltage below threshold - take protective action
    }
    EXTI_ClearITPendingBit(EXTI_Line16);
}
```

## PVD Levels

| Level | Threshold |
|-------|-----------|
| `PWR_PVDLevel_MODE0` | ~2.2V |
| `PWR_PVDLevel_MODE1` | ~2.3V |
| `PWR_PVDLevel_MODE2` | ~2.4V |
| `PWR_PVDLevel_MODE3` | ~2.5V |
| `PWR_PVDLevel_MODE4` | ~2.6V |
| `PWR_PVDLevel_MODE5` | ~2.7V |
| `PWR_PVDLevel_MODE6` | ~2.8V |
| `PWR_PVDLevel_MODE7` | ~2.9V |

## VIO18 Configuration

CH32H417 has configurable 1.8V I/O voltage domain:

```c
// Hardware mode (default)
PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_HW);

// Software mode (manual control)
PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
PWR_VIO18LevelCfg(PWR_VIO18Level_MODE2);
```

## Backup Domain Access

```c
// Enable access to backup registers and RTC
PWR_BackupAccessCmd(ENABLE);

// Now you can write to backup registers
// and configure RTC
```

## Power Mode Summary

| Mode | CPU | Peripherals | SRAM | Wake-up |
|------|-----|-------------|------|---------|
| Run | Active | Active | Active | - |
| Sleep | Stopped | Active | Active | Any interrupt |
| Stop | Stopped | Stopped | Retained | EXTI |
| Standby | Stopped | Stopped | Lost | WKUP pin, RTC, IWDG |
