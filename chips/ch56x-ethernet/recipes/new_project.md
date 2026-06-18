# Recipe: Create a New CH56x Project

## Overview

Create a new firmware project for CH569 from scratch in MounRiver Studio.

## Prerequisites

- MounRiver Studio IDE installed
- WCH-LinkE debugger
- CH56x evaluation board (CH569EVT, CH563EVT, or CH561EVT)

## Steps

### 1. Copy a Template Project

The fastest approach is to copy an existing example project:

```bash
# Copy the simplest example (GPIO or UART) as a starting point
cp -r CH569EVT/EVT/EXAM/GPIO/ MyProject/
```

### 2. Project Structure

Your project must include these linked resources:

```
MyProject/
  APP/
    main.c                 # Your application code
  SRC/
    Peripheral/
      inc/                 # CH56x_*.h headers
      src/                 # CH56x_*.c source files
    Ld/Link.ld             # Linker script
    Startup/               # Assembly startup (startup_CH569.S)
    RVMSIS/                # Core RISC-V system headers
    Flash_Lib/             # Flash programming library
  .project                 # MounRiver project file
  *.wvproj                 # MounRiver workspace project
```

### 3. Minimal main.c

```c
#include "CH56x_common.h"

int main(void)
{
    // System clock: 120MHz
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    // Enable UART0 for debug output
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();  // 115200 baud

    printf("CH569 project started, clock=%dHz\n", GetSysClock());

    while(1)
    {
        // Application loop
    }
}
```

### 4. Configure Linked Resources in .project

Edit `.project` to link shared SRC/ directories:

```xml
<link>
  <name>Peripheral</name>
  <type>2</type>
  <locationURI>PARENT-2-PROJECT_LOC/EVT/EXAM/SRC/Peripheral</locationURI>
</link>
<link>
  <name>Ld</name>
  <type>2</type>
  <locationURI>PARENT-2-PROJECT_LOC/EVT/EXAM/SRC/Ld</locationURI>
</link>
<link>
  <name>Startup</name>
  <type>2</type>
  <locationURI>PARENT-2-PROJECT_LOC/EVT/EXAM/SRC/Startup</locationURI>
</link>
```

### 5. Build and Flash

1. Open `.wvproj` in MounRiver Studio
2. Project -> Build Project (Ctrl+B)
3. Connect WCH-LinkE to the board
4. Run -> Debug to flash and start debugging

### 6. UART Debug Output

By default, `printf()` is routed to UART0 at 115200 baud, 8N1.
Connect a USB-TTL adapter to UART0 TX pin to see output.

To change debug UART, modify `CH56x_common.h`:
```c
#define Debug_UART0   0   // Use UART0 (default)
// or
#define Debug_UART1   1   // Use UART1
```

## Verification

- Build should complete with zero errors
- UART0 output should show "CH569 project started" message
- LED or GPIO toggle should work on the evaluation board
