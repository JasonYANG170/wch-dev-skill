# Recipe: Power Management

## Overview

Configure low-power modes (Sleep, Stop, Standby) on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void PWR_DeInit(void);

// PVD (Power Voltage Detector)
void PWR_PVDCmd(FunctionalState NewState);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);

// Wakeup
void PWR_WakeUpPinCmd(FunctionalState NewState);

// Low power modes
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterSTANDBYMode(void);

// Backup access
void PWR_BackupAccessCmd(FunctionalState NewState);

// Status
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void PWR_ClearFlag(uint32_t PWR_FLAG);
PWR_VDD PWR_VDD_SupplyVoltage(void);
```

## Power Modes

| Mode | Description | Wakeup Source | Current |
|------|-------------|---------------|---------|
| Sleep | CPU stopped, peripherals running | Any interrupt | ~mA |
| Stop | All clocks stopped, SRAM retained | EXTI line, RTC | ~uA |
| Standby | All power off, only wakeup pin | WKUP pin, RTC, IWDG | ~uA |

## PVD Levels

| Constant | Voltage |
|----------|---------|
| PWR_PVDLevel_2V7 | 2.7V |
| PWR_PVDLevel_2V9 | 2.9V |
| PWR_PVDLevel_3V1 | 3.1V |
| PWR_PVDLevel_3V3 | 3.3V |
| PWR_PVDLevel_3V5 | 3.5V |
| PWR_PVDLevel_3V8 | 3.8V |
| PWR_PVDLevel_4V1 | 4.1V |
| PWR_PVDLevel_4V4 | 4.4V |

## Example: Enter Stop Mode

```c
#include "debug.h"

void Enter_Stop_Mode(void)
{
    // Enable PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Configure EXTI line for wakeup (e.g., PA0)
    // ... EXTI configuration ...

    // Enter Stop mode
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    // Execution resumes here after wakeup
    SystemCoreClockUpdate();  // Reconfigure clocks
}

void Enter_Standby_Mode(void)
{
    // Enable PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Enable WKUP pin (PA0)
    PWR_WakeUpPinCmd(ENABLE);

    // Clear wakeup flag
    PWR_ClearFlag(PWR_FLAG_WU);

    // Enter Standby mode
    PWR_EnterSTANDBYMode();

    // Code will restart from main() after wakeup
}
```

## Example: PVD Configuration

```c
void PVD_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // Configure EXTI line 16 for PVD
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Configure NVIC
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Configure PVD level and enable
    PWR_PVDLevelConfig(PWR_PVDLevel_3V3);
    PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        // Voltage below threshold - save data and prepare for power loss
        printf("Low voltage detected!\r\n");
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}
```
