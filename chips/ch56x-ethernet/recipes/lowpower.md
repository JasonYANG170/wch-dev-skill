# Recipe: Low Power Modes

## Overview

Configure CH56x power-saving modes (Idle, Halt, Sleep) with GPIO wakeup sources.
The CH56x supports three low-power modes with decreasing power consumption and
increasing wakeup latency.

## Power Modes

| Mode   | Function            | Wakeup Source      | CPU State        | Peripherals      |
|--------|---------------------|--------------------|------------------|------------------|
| Idle   | `LowPower_Idle()`   | Any interrupt      | Suspended (WFI)  | Running          |
| Halt   | `LowPower_Halt()`   | GPIO, USB, RTC     | Stopped          | Most stopped     |
| Sleep  | `LowPower_Sleep()`  | GPIO, USB, RTC     | Stopped          | Almost all off   |

Each mode has a WFI (Wait For Interrupt) and WFE (Wait For Event) variant:

```c
void LowPower_Idle(void);          // Idle, WFI wakeup
void LowPower_Idle_WFE(void);      // Idle, WFE wakeup
void LowPower_Halt(void);          // Halt, WFI wakeup
void LowPower_Halt_WFE(void);      // Halt, WFE wakeup
void LowPower_Sleep(void);         // Sleep, WFI wakeup
void LowPower_Sleep_WFE(void);     // Sleep, WFE wakeup
```

## Wakeup Sources

Configure wakeup sources before entering low-power mode:

```c
// Enable GPIO as wakeup source
PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE);

// Enable USB as wakeup source (for Halt/Sleep)
PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_USBHS_WAKE);
```

## GPIO Wakeup Configuration

Only specific GPIO pins support wakeup (same pins that support interrupts):
- GPIOA: PA2, PA3, PA4
- GPIOB: PB3, PB4, PB11, PB12, PB15

```c
// Configure PA2 as low-level wakeup source
GPIOA_ITModeCfg(GPIO_Pin_2, GPIO_ITMode_LowLevel);

// For WFI mode, also enable the GPIO interrupt
PFIC_SetPriority(GPIO_IRQn, 0x1 << 4);
PFIC_EnableIRQ(GPIO_IRQn);
```

## Complete Example: Sleep with GPIO Wakeup (WFI)

```c
#include "CH56x_common.h"

#define FREQ_SYS  80000000

void GPIO_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DebugInit(UINT32 baudrate)
{
    UINT32 x, t = FREQ_SYS;
    x = 10 * t * 2 / 16 / baudrate;
    x = (x + 5) / 10;
    R8_UART1_DIV = 1;
    R16_UART1_DL = x;
    R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R32_PA_SMT |= (1 << 8) | (1 << 7);
    R32_PA_DIR |= (1 << 8);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);

    // Set all pins to pull-up input (lowest power)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);

    DebugInit(115200);
    PRINT("Sleep Mode Test (WFI)\r\n");
    DelayMs(10);

    // Configure wakeup: PA2 low-level
    PFIC_SetPriority(GPIO_IRQn, 0x1 << 4);
    PFIC_EnableIRQ(GPIO_IRQn);
    GPIOA_ITModeCfg(GPIO_Pin_2, GPIO_ITMode_LowLevel);
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE);

    // Enter Sleep mode
    LowPower_Sleep();

    // Execution resumes here after wakeup
    PRINT("Woke up!\r\n");

    while(1) {
        PRINT("Running\r\n");
        DelayMs(1000);
    }
}

void GPIO_IRQHandler(void)
{
    if (GPIOA_2_ReadITFlagBit()) {
        PRINT("GPIO interrupt!\r\n");
        GPIOA_2_ClearITFlagBit();
    }
}
```

## Complete Example: Sleep with GPIO Wakeup (WFE)

WFE mode does not require an interrupt handler. The CPU wakes on event directly:

```c
#include "CH56x_common.h"

#define FREQ_SYS  80000000

void DebugInit(UINT32 baudrate)
{
    UINT32 x, t = FREQ_SYS;
    x = 10 * t * 2 / 16 / baudrate;
    x = (x + 5) / 10;
    R8_UART1_DIV = 1;
    R16_UART1_DL = x;
    R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R32_PA_SMT |= (1 << 8) | (1 << 7);
    R32_PA_DIR |= (1 << 8);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);

    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);

    DebugInit(115200);
    PRINT("Sleep Mode Test (WFE)\r\n");
    DelayMs(10);

    // Configure wakeup: PA2 low-level
    GPIOA_ITModeCfg(GPIO_Pin_2, GPIO_ITMode_LowLevel);
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE);

    // Enter Sleep mode (WFE)
    LowPower_Sleep_WFE();

    // After wakeup from Halt/Sleep, restore USB PHY if needed
    R8_SAFE_ACCESS_SIG = 0x57;
    R8_SAFE_ACCESS_SIG = 0xA8;
    R8_SLP_POWER_CTRL &= ~RB_SLP_USBHS_PWRDN;
    USBSS->LINK_CFG |= 0x1 << 12;
    R8_SAFE_ACCESS_SIG = 0x00;

    PRINT("Woke up!\r\n");

    while(1) {
        PRINT("Running\r\n");
        DelayMs(1000);
    }
}
```

## Peripheral Clock Gating for Low Power

Disable unused peripheral clocks before entering low-power modes:

```c
// Disable unused peripheral clocks
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_ETH);    // Disable Ethernet
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_USBSS);  // Disable USB 3.0
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_EMMC);   // Disable eMMC
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_ECDC);   // Disable ECDC
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_DVP);    // Disable DVP
```

## WFI vs WFE

| Feature | WFI | WFE |
|---------|-----|-----|
| Wakeup trigger | Interrupt (must be enabled) | Event (no interrupt handler needed) |
| ISR execution | Yes, ISR runs after wakeup | No, resumes after LowPower call |
| Use case | Need to handle interrupt | Simple wakeup, no processing needed |

## USB PHY Recovery After Halt/Sleep

After waking from Halt or Sleep mode, the USB PHY may be powered down.
Restore it if USB is needed:

```c
R8_SAFE_ACCESS_SIG = 0x57;
R8_SAFE_ACCESS_SIG = 0xA8;
R8_SLP_POWER_CTRL &= ~RB_SLP_USBHS_PWRDN;  // Power on USBHS
USBSS->LINK_CFG |= 0x1 << 12;               // Restore USB PHY
R8_SAFE_ACCESS_SIG = 0x00;
```

## Notes

- Set all unused GPIO pins to pull-up input mode for lowest leakage current.
- Idle mode only halts the CPU; all peripherals continue running. Use Halt or
  Sleep for meaningful power savings.
- Sleep mode has the lowest power consumption but the longest wakeup latency.
- After wakeup from Halt/Sleep with WFE, safe access is required to modify
  protected registers (use the 0x57/0xA8 unlock sequence).
- The `RB_SLP_GPIO_WAKE` wakeup source works with the interrupt-capable GPIO
  pins (PA2-4, PB3-4, PB11-12, PB15).

## Example Project

`CH569EVT/EVT/EXAM/LOWPOWER/` - Idle, Halt, and Sleep mode demo with GPIO wakeup.
