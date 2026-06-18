# Recipe: GPIO Control (Register-Based)

## Overview

Configure GPIO pins for digital input, output, and interrupts on CH561/CH563
using direct register manipulation. There is NO GPIO_InitTypeDef or
GPIOA_ModeCfg() function -- all GPIO control is via R32_PA_xxx, R32_PB_xxx
registers.

## GPIO Ports

- **GPIOA**: PA0-PA21 (22 pins) - R32_PA_xxx registers
- **GPIOB**: PB0-PB23 (24 pins) - R32_PB_xxx registers
- **GPIOD**: PD0-PD31 (32 pins, CH563 only) - R32_PD_xxx registers

## GPIO Register Map

Each port has these registers (offset from base address):

| Register | R/W | Description |
|----------|-----|-------------|
| `R32_PA_DIR` | RW | Direction: 0=input, 1=output |
| `R32_PA_PIN` | RO | Input pin values |
| `R32_PA_OUT` | RW | Output values |
| `R32_PA_CLR` | WZ | Clear output (1=clear, 0=keep) |
| `R32_PA_PU` | RW | Pull-up enable |
| `R32_PA_PD` | RW | Pull-down / open-drain enable |
| `R32_PA_DRV` | RW | Drive strength: 0=4mA, 1=16mA |
| `R32_PA_SMT` | RW | Schmitt trigger / slew rate |

Replace PA with PB or PD for other ports.

## Output Mode Configuration

```c
// Set PA5 as push-pull output (16mA drive, pull-up)
R32_PA_DIR |= (1 << 5);       // Direction: output
R32_PA_DRV |= (1 << 5);       // 16mA drive strength
R32_PA_PU  |= (1 << 5);       // Pull-up enabled
R32_PA_SMT |= (1 << 5);       // Slow slew rate

// Set PB0 as push-pull output (4mA drive)
R32_PB_DIR |= (1 << 0);       // Direction: output
// Default: 4mA drive, no pull-up/pull-down
```

## Input Mode Configuration

```c
// Set PA3 as floating input
R32_PA_DIR &= ~(1 << 3);      // Direction: input
R32_PA_PU  &= ~(1 << 3);      // No pull-up
R32_PA_PD  &= ~(1 << 3);      // No pull-down

// Set PB10 as pull-up input with Schmitt trigger
R32_PB_DIR &= ~(1 << 10);     // Direction: input
R32_PB_PU  |= (1 << 10);      // Pull-up enabled
R32_PB_SMT |= (1 << 10);      // Schmitt trigger enabled

// Set PA7 as pull-down input
R32_PA_DIR &= ~(1 << 7);      // Direction: input
R32_PA_PD  |= (1 << 7);       // Pull-down enabled
```

## Open-Drain Output

```c
// Set PA5 as open-drain output
R32_PA_DIR |= (1 << 5);       // Direction: output
R32_PA_PD  |= (1 << 5);       // Open-drain mode (PD bit enables open-drain)
```

## Digital Output

```c
// Set pin high
R32_PA_OUT |= (1 << 5);       // PA5 high

// Set pin low (using clear register)
R32_PA_CLR |= (1 << 5);       // PA5 low

// Toggle pin
R32_PA_OUT ^= (1 << 5);       // Toggle PA5

// Set multiple pins
R32_PA_OUT |= ((1 << 5) | (1 << 6));   // PA5, PA6 high
R32_PA_CLR |= ((1 << 5) | (1 << 6));   // PA5, PA6 low
```

## Digital Input

```c
// Read single pin
UINT32 val = R32_PA_PIN & (1 << 3);     // Read PA3
if (val) {
    // Pin is high
} else {
    // Pin is low
}

// Read entire port
UINT32 portA = R32_PA_PIN;              // Read all PA pins
UINT32 portB = R32_PB_PIN;              // Read all PB pins
```

## GPIO Interrupts

GPIO interrupts are configured via dedicated interrupt registers:

```c
// Enable PB[7:0] as interrupt sources
R32_INT_ENABLE_PB |= 0x000000FF;        // Enable PB0-PB7 interrupts

// Set interrupt mode (edge or level)
R32_INT_MODE_PB |= 0x000000FF;          // 1=edge trigger, 0=level trigger

// Set interrupt polarity
R32_INT_POLAR_PB &= ~0x000000FF;        // 0=falling edge/low level
R32_INT_POLAR_PB |= 0x000000FF;         // 1=rising edge/high level

// Clear pending interrupt flags
R32_INT_STATUS_PB = 0xFFFFF;            // Write 1 to clear

// Enable PB interrupt in IRQ controller
R8_INT_EN_IRQ_1 |= RB_IE_IRQ_PB;       // Enable PB IRQ
R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;  // Enable global IRQ
```

### PA Interrupt Registers

```c
R32_INT_STATUS_PA   // RW1, PA interrupt flag (write 1 to clear)
R32_INT_ENABLE_PA   // RW, PA interrupt enable
R32_INT_MODE_PA     // RW, PA interrupt mode (0=level, 1=edge)
R32_INT_POLAR_PA    // RW, PA interrupt polarity
```

### PB Interrupt Registers

```c
R32_INT_STATUS_PB   // RW1, PB interrupt flag
R32_INT_ENABLE_PB   // RW, PB interrupt enable
R32_INT_MODE_PB     // RW, PB interrupt mode
R32_INT_POLAR_PB    // RW, PB interrupt polarity
```

## Interrupt Handler

```c
__irq void IRQ_Handler(void)
{
    // Check PB interrupt
    if ((R32_INT_FLAG >> 8) & RB_IF_PB) {
        // Read which PB pins triggered
        UINT32 status = R32_INT_STATUS_PB & 0x000000FF;

        // Handle specific pins
        if (status & (1 << 0)) {
            // PB0 interrupt
        }

        // Clear interrupt flags (write 1 to clear)
        R32_INT_STATUS_PB = 0xFFFFF;
    }
}
```

## Peripheral Pin Functions

Some GPIO pins have alternate functions for peripherals:

| Pin | Default | Alternate |
|-----|---------|-----------|
| PB8 | GPIO | UART0 RXD (RXD0) |
| PB9 | GPIO | UART0 TXD (TXD0) |
| PB10 | GPIO | UART1 RXD (RXD1) |
| PB11 | GPIO | UART1 TXD (TXD1) |
| PB12 | GPIO | SPI0 CS (SCS) |
| PB13 | GPIO | SPI0 SCK (SCK0) |
| PB14 | GPIO | SPI0 MOSI |
| PB15 | GPIO | SPI0 MISO |
| PA13 | GPIO | Ethernet LED (ELED) |
| PA18 | GPIO | Ethernet Link (ELINK) |

When a pin is used for a peripheral function, do NOT configure it as GPIO output.

## Complete Example: LED Blink

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

#define LED         (1<<3)                    // PB3
#define LED_INIT()  { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }
#define LED_ON()    { R32_PB_CLR |= LED; }
#define LED_OFF()   { R32_PB_OUT |= LED; }

__irq void IRQ_Handler(void) { while(1); }
__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    LED_INIT();

    while(1)
    {
        LED_ON();
        Delay_ms(500);
        LED_OFF();
        Delay_ms(500);
    }
}
```

## Pin Assignment Notes

- PA0/PA1: Often used for crystal oscillator
- PB8/PB9: UART0 TX/RX (used for printf debug output)
- Check the schematic for your specific evaluation board
- Some pins share functions with Ethernet MDC/MDIO
