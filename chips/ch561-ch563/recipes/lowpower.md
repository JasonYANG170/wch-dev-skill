# Recipe: Low Power Modes (Register-Based)

## Overview

Configure CH561/CH563 power-saving modes with GPIO wakeup sources.
The chips support low-power modes with decreasing power consumption.

## Power Modes

CH561/CH563 support sleep mode with wakeup from GPIO or peripheral interrupts.

| Mode | Description | Wakeup Source | CPU State |
|------|-------------|---------------|-----------|
| Idle | CPU halted, peripherals running | Any interrupt | Suspended (WFI) |
| Sleep | CPU and most peripherals halted | GPIO, USB, Ethernet | Stopped |

## Peripheral Clock Gating

Disable unused peripheral clocks to save power:

```c
// Disable unused peripheral clocks
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_ETH;    // Disable Ethernet
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_SPI0;   // Disable SPI0
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_SPI1;   // Disable SPI1
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_TMR0;   // Disable Timer0
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_TMR1;   // Disable Timer1

// CH563 only: disable USB clock
R8_SLP_CLK_OFF0 |= RB_SLP_CLK_USB;

// Enable only needed peripheral clocks
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_UART0;  // Enable UART0
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_GPIO;   // Enable GPIO
```

## GPIO Wakeup Configuration

Configure GPIO pins as wakeup sources before entering sleep:

```c
// Configure PB3 as wakeup source (low-level trigger)
R32_INT_ENABLE_PB |= (1 << 3);     // Enable PB3 interrupt
R32_INT_MODE_PB &= ~(1 << 3);      // Level trigger
R32_INT_POLAR_PB &= ~(1 << 3);     // Low level

// Clear pending flags
R32_INT_STATUS_PB = 0xFFFFF;

// Enable PB interrupt in IRQ controller
R8_INT_EN_IRQ_1 |= RB_IE_IRQ_PB;
R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;
```

## Entering Sleep Mode

```c
// ARM WFI (Wait For Interrupt) instruction
// The CPU halts until an interrupt occurs

// Method 1: Using inline assembly
__asm volatile("WFI");

// Method 2: Using a helper function
void enter_idle(void)
{
    // Set SLEEPDEEP bit for deep sleep
    // SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    __asm volatile("WFI");
}
```

## Complete Example: Sleep with GPIO Wakeup

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

#define LED         (1<<3)
#define LED_INIT()  { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }
#define LED_ON()    { R32_PB_CLR |= LED; }
#define LED_OFF()   { R32_PB_OUT |= LED; }

void mInitSTDIO(void)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;
    x = (x + 5) / 10;
    R8_UART0_LCR = 0x80;
    R8_UART0_DIV = 1;
    R8_UART0_DLM = x >> 8;
    R8_UART0_DLL = x & 0xff;
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= RXD0 | TXD0;
    R32_PB_PU |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f) { R8_UART0_THR = c; while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0); return c; }

volatile UINT8 wakeup_flag = 0;

__irq void IRQ_Handler(void)
{
    // Check PB interrupt
    if (R8_INT_FLAG_1 & RB_IF_PB) {
        UINT32 status = R32_INT_STATUS_PB;
        if (status & (1 << 3)) {
            // PB3 wakeup
            wakeup_flag = 1;
        }
        R32_INT_STATUS_PB = 0xFFFFF;  // Clear flags
    }
}

__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    mInitSTDIO();
    LED_INIT();

    // Set all unused pins to pull-up input (lowest power)
    R32_PA_DIR = 0;
    R32_PA_PU = 0xFFFFFFFF;
    R32_PB_DIR &= ~(0xFFFFFFFF & ~((1<<3) | TXD0));  // Keep LED and TXD
    R32_PB_PU = 0xFFFFFFFF;

    PRINT("Sleep Mode Test\n");

    // Configure PB3 as wakeup source
    R32_INT_ENABLE_PB |= (1 << 3);
    R32_INT_MODE_PB &= ~(1 << 3);      // Level trigger
    R32_INT_POLAR_PB &= ~(1 << 3);     // Low level
    R32_INT_STATUS_PB = 0xFFFFF;        // Clear flags
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_PB;
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;

    PRINT("Entering sleep... Pull PB3 low to wake\n");

    // Enter sleep (WFI)
    __asm volatile("WFI");

    // Execution resumes here after wakeup
    PRINT("Woke up!\n");

    while(1) {
        LED_ON();
        Delay_ms(500);
        LED_OFF();
        Delay_ms(500);
    }
}
```

## WFI vs WFE

| Feature | WFI | WFE |
|---------|-----|-----|
| Wakeup trigger | Interrupt (must be enabled) | Event (no interrupt handler needed) |
| ISR execution | Yes, ISR runs after wakeup | No, resumes after WFI/WFE call |
| Use case | Need to handle interrupt | Simple wakeup, no processing needed |

## Notes

- Set all unused GPIO pins to pull-up input mode for lowest leakage current
- Disable unused peripheral clocks before entering sleep
- After wakeup from sleep, peripheral clocks may need re-enabling
- CH563: USB wakeup is supported via `RB_SLP_USB_WAKE`
- Ethernet wakeup is supported via Ethernet interrupt
- The `__asm volatile("WFI")` instruction halts the CPU until interrupt

## Example Project

- `CH561EVT/EXAM/POWER/` - Power management examples
- `CH563EVT/EXAM/POWER/` - Power management examples
