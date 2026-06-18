# Create New CH5xx 8051 Project

> **Summary**: Create a new CH5xx 8051 firmware project from scratch with proper Keil C51 project structure, debug setup, and initialization code.

## Trigger Intent

- "Create a new CH552/CH554/CH559 project"
- "Initialize a CH5xx 8051 project"
- "Start a new 8051 firmware project"
- "Set up Keil C51 project for CH552"

## Prerequisites

| Condition | Requirement |
|---|---|
| IDE | Keil C51 (uvproj) installed |
| SDK | CH5xx EVT SDK extracted (e.g., CH554EVT) |
| Programmer | WCH ISP tool or WCH-LinkE (optional) |

## Call Chain

```
Step 1: Create project directory structure
Step 2: Create chip register header (CH5xx.H) reference
Step 3: Create Debug.C/H utility files
Step 4: Write main.c with CfgFsys() and mInitSTDIO()
Step 5: Create peripheral driver files
Step 6: Create Keil C51 .uvproj file
Step 7: Build and flash
```

## Step-by-Step

### Step 1: Create Project Directory Structure

```
MyProject/
+-- Public/
|   +-- CH552.H          # Chip register definitions (copy from EVT SDK)
|   +-- Debug.C          # System utilities (CfgFsys, delay, UART)
|   +-- Debug.H          # Config defines (FREQ_SYS, UART baud)
+-- GPIO/                 # Example peripheral
|   +-- GPIO.C
|   +-- GPIO.H
|   +-- Main.C
+-- MyProject.uvproj      # Keil project file
```

### Step 2: Copy the Chip Header

Copy `CH5xx.H` from the EVT SDK `Public/` directory. This file contains all SFR, sbit, and #define declarations for the target chip. Do NOT modify it.

```c
// CH552.H provides:
sfr P1       = 0x90;       // Port 1
sfr P3       = 0xB0;       // Port 3
sfr SCON     = 0x98;       // UART0 control
sfr TCON     = 0x88;       // Timer control
sfr ADC_CTRL = 0x80;       // ADC control
sfr SPI0_STAT = 0xF8;      // SPI0 status
// ... and hundreds more
```

### Step 3: Create Debug Utility Files

**Debug.H** -- System configuration:
```c
#ifndef __DEBUG_H__
#define __DEBUG_H__

#define SUCCESS  0
#define FAIL     0xFF

#define FREQ_SYS     12000000    // System clock 12MHz (adjust per CLOCK_CFG)
#define UART0_BUAD   57600       // Debug UART baud rate

void CfgFsys();                  // Configure system clock
void mDelayuS(UINT16 n);        // Microsecond delay
void mDelaymS(UINT16 n);        // Millisecond delay
void mInitSTDIO();               // Init UART0 for printf
UINT8 CH554UART0RcvByte();
void CH554UART0SendByte(UINT8 SendDat);

#endif
```

**Debug.C** -- Implement `CfgFsys()` to set CLOCK_CFG, `mInitSTDIO()` to configure UART0 with Timer1 as baud rate generator.

### Step 4: Write Main.C

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

void main()
{
    CfgFsys();                   // Configure system clock to FREQ_SYS
    mDelaymS(5);                 // Wait for oscillator stable
    mInitSTDIO();                // Init UART0 (printf output)
    printf("CH552 project started\n");

    // TODO: Initialize peripherals here

    EA = 1;                      // Enable global interrupts

    while(1) {
        // Application main loop
    }
}
```

### Step 5: Create Peripheral Driver

Each peripheral is a separate `.C`/`.H` pair:

```c
// GPIO.H
#ifndef __GPIO_H__
#define __GPIO_H__
#include "..\Public\CH552.H"

void Port1Cfg(UINT8 Mode, UINT8 Pin);
void Port3Cfg(UINT8 Mode, UINT8 Pin);

#endif
```

```c
// GPIO.C
#include "..\Public\CH552.H"
#include "GPIO.H"

#pragma NOAREGS

void Port1Cfg(UINT8 Mode, UINT8 Pin)
{
    switch(Mode) {
        case 0:  // Float input
            P1_MOD_OC &= ~(1<<Pin);
            P1_DIR_PU &= ~(1<<Pin);
            break;
        case 1:  // Push-pull output
            P1_MOD_OC &= ~(1<<Pin);
            P1_DIR_PU |= (1<<Pin);
            break;
        case 2:  // Open-drain
            P1_MOD_OC |= (1<<Pin);
            P1_DIR_PU &= ~(1<<Pin);
            break;
        case 3:  // Quasi-bidirectional (standard 8051)
            P1_MOD_OC |= (1<<Pin);
            P1_DIR_PU |= (1<<Pin);
            break;
    }
}
```

### Step 6: Keil C51 Project Configuration

In Keil C51:
1. Project -> New Project -> select target chip (e.g., CH552)
2. Add source files to project groups
3. Options for Target:
   - Memory Model: Small (data) or Large (xdata) depending on needs
   - Code Banking: as needed for >64KB chips
   - Output: HEX file for ISP programming

### Step 7: Build and Flash

1. Build: Project -> Build Target (F7)
2. Flash: Use WCH ISP tool via USB, or WCH-LinkE
3. Debug: UART0 output at `UART0_BUAD` baud rate

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| `undefined identifier` | Missing `#include "CH552.H"` | Include chip header first |
| printf no output | UART0 not initialized | Call `mInitSTDIO()` before printf |
| Build size overflow | Too much code for Flash | Use `code` keyword for const data, optimize |
