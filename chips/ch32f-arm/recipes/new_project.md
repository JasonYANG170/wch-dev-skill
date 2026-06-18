# Recipe: Create New CH32F Project

## Overview

Create a new project for CH32F10x or CH32F20x ARM Cortex-M3 microcontroller.

## Steps

### 1. Choose Your Chip Family

- **CH32F10x** (CH32F103): STM32F103 compatible, up to 72MHz, include `ch32f10x.h`
- **CH32F20x** (CH32F203/CH32F207/CH32F208): Higher performance, up to 144MHz, BLE support, include `ch32f20x.h`
- **CH32M030**: Motor control variant, include `ch32f10x.h`

### 2. Project File Structure

```
MyProject/
  Main.c                    // main() entry point
  ch32f10x_it.c             // Interrupt handlers (ch32f20x_it.c for F20x)
  ch32f10x_it.h             // Interrupt handler declarations
  ch32f10x_conf.h           // Peripheral include configuration
  system_ch32f10x.c         // System initialization (from EVT/SRC/)
  system_ch32f10x.h         // System function declarations
  debug.h                   // Debug utilities (from EVT/SRC/)
  debug.c                   // Debug implementation
  CH32F10x.uvprojx          // Keil MDK project file
```

### 3. Main.c Template

```c
#include "debug.h"

void Periph_Init(void)
{
    // Initialize peripherals here
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    Periph_Init();

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("My Project Started\r\n");

    while(1)
    {
        // Main loop
        Delay_Ms(100);
    }
}
```

### 4. ch32f10x_conf.h Template

```c
#ifndef __CH32F10x_CONF_H
#define __CH32F10x_CONF_H

#include "ch32f10x_gpio.h"
#include "ch32f10x_rcc.h"
#include "ch32f10x_usart.h"
// Add more includes as needed:
// #include "ch32f10x_spi.h"
// #include "ch32f10x_i2c.h"
// #include "ch32f10x_adc.h"
// #include "ch32f10x_tim.h"
// #include "ch32f10x_can.h"
// #include "ch32f10x_dac.h"
// #include "ch32f10x_dma.h"
// #include "ch32f10x_flash.h"
// #include "ch32f10x_pwr.h"
// #include "ch32f10x_exti.h"
// #include "ch32f10x_bkp.h"
// #include "ch32f10x_rtc.h"
// #include "ch32f10x_iwdg.h"
// #include "ch32f10x_wwdg.h"
// #include "ch32f10x_dbgmcu.h"
// #include "ch32f10x_crc.h"

#endif
```

### 5. ch32f10x_it.c Template

```c
#include "ch32f10x_it.h"

void NMI_Handler(void) { }
void HardFault_Handler(void) { NVIC_SystemReset(); while(1); }
void MemManage_Handler(void) { while(1); }
void BusFault_Handler(void) { while(1); }
void UsageFault_Handler(void) { while(1); }
void SVC_Handler(void) { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void) { }
void SysTick_Handler(void) { }
```

### 6. Keil MDK Configuration

- Device: Select CH32F103C8T6 (or your specific variant)
- C/C++ -> Include Paths: Add paths to `EVT/EXAM/SRC/StdPeriphDriver/inc/` and `EVT/EXAM/SRC/RVMSIS/`
- C/C++ -> Define: Add `DEBUG` for debug builds
- Linker: Use default scatter file from the EVT package

### 7. System Clock Configuration

The `system_ch32f10x.c` file contains `SystemInit()` which is called before `main()`. It configures the system clock to 72MHz (CH32F10x) or 144MHz (CH32F20x) using the internal HSI or external HSE oscillator.

To change clock settings, modify the `SystemCoreClockUpdate()` function or configure PLL in `SystemInit()`.

## CH32F20x Differences

For CH32F20x projects:
- Include `ch32f20x.h` instead of `ch32f10x.h`
- Use `ch32f20x_conf.h` for peripheral includes
- Use `ch32f20x_it.c` for interrupt handlers
- Use `system_ch32f20x.c` for system initialization
- BLE projects follow a different structure (see BLE section in SKILL.md)
