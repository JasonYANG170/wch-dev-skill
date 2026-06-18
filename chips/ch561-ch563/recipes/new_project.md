# Recipe: Create a New CH561/CH563 ARM Project

## Overview

Create a new firmware project for CH561 or CH563 (ARM architecture) from scratch
in Keil MDK (uVision).

## Prerequisites

- Keil MDK-ARM (uVision) installed
- WCH-LinkE or ULINK2 debugger
- CH561 or CH563 evaluation board

## Steps

### 1. Copy a Template Project

The fastest approach is to copy an existing example project:

```bash
# Copy the simplest example (GPIO) as a starting point
cp -r CH561EVT/EXAM/GPIO/GPIOA_IO/ MyProject/
```

### 2. Project Structure

Your project must include these files:

```
MyProject/
  APP/
    main.c                 # Your application code
  SRC/
    CH561SFR.H             # Register definitions (use CH563SFR.H for CH563)
    CH561BAS.H             # Base type definitions
    SYSFREQ.H              # Clock configuration
    SYSFREQ.C              # PLL initialization (called before main)
    ISPXT56X.H             # Flash/EEPROM programming library header
    ISPXT56X.O             # Pre-compiled Flash library object
    STARTUP.S              # ARM assembly startup file
    CH561D32.SCF           # Keil scatter file
  MyProject.uvproj         # Keil uVision project file
```

### 3. Configure Clock (SYSFREQ.H)

Edit SYSFREQ.H to set your desired system clock:

```c
#ifndef FREQ_SYS
#define FREQ_SYS            100000000    // 100MHz system clock
#endif

#ifndef FREQ_OSC
#define FREQ_OSC            30000000     // 30MHz crystal (default)
#endif

#ifndef MEM_DATA
#define MEM_DATA            64           // 64KB SRAM
#endif
```

Supported FREQ_SYS values (with 30MHz crystal):
25MHz, 50MHz, 62.5MHz, 75MHz, 83.33MHz, 100MHz, 110MHz, 125MHz, 137.5MHz, 150MHz

### 4. Minimal main.c

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"    // Use CH563SFR.H for CH563
#include "SYSFREQ.H"

// LED on PB3 (board-specific)
#define LED         (1<<3)
#define LED_INIT()  { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }
#define LED_ON()    { R32_PB_CLR |= LED; }
#define LED_OFF()   { R32_PB_OUT |= LED; }

// UART0 init for printf (115200 baud)
void mInitSTDIO(void)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;
    x = (x + 5) / 10;
    R8_UART0_LCR = 0x80;           // DLAB=1
    R8_UART0_DIV = 1;
    R8_UART0_DLM = x >> 8;
    R8_UART0_DLL = x & 0xff;
    R8_UART0_LCR = RB_LCR_WORD_SZ; // 8-bit
    R8_UART0_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= RXD0 | TXD0;
    R32_PB_PU  |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f)
{
    R8_UART0_THR = c;
    while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0);
    return c;
}

__irq void IRQ_Handler(void) { while(1); }
__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    LED_INIT();
    LED_ON();
    Delay_ms(100);
    LED_OFF();

    mInitSTDIO();
    PRINT("CH561 project started, FREQ=%dHz\n", FREQ_SYS);

    while(1)
    {
        // Application loop
    }
}
```

### 5. Configure Keil Project

1. Open the `.uvproj` file in Keil uVision
2. Project -> Options for Target -> Target tab:
   - Set Xtal (MHz) to match your crystal (e.g., 30.0)
   - Set Memory Model as needed
3. Project -> Options for Target -> Output tab:
   - Check "Create HEX File"
4. Project -> Options for Target -> Linker tab:
   - Use scatter file (e.g., CH561D32.SCF)

### 6. Build and Flash

1. Open `.uvproj` in Keil uVision
2. Project -> Build Target (F7)
3. Connect WCH-LinkE or ULINK2 to the board
4. Debug -> Start/Stop Debug Session (Ctrl+F5)

### 7. UART Debug Output

By default, `printf()` is routed to UART0 at 115200 baud, 8N1.
Connect a USB-TTL adapter to UART0 TX pin (PB9) to see output.

## Verification

- Build should complete with zero errors
- UART0 output should show "CH561 project started" message
- LED on PB3 should blink on power-up
