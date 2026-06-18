# New Project Setup

> **Summary**: Create a new CH32X035/CH32X315/CH643/CH641 project from scratch with proper project structure.

## Trigger Intent

- "Create new CH32X035 project"
- "New USB-PD project"
- "Start CH643 project"
- "Initialize CH32X315 firmware"

## Prerequisites

| Condition | Requirement |
|---|---|
| IDE | MounRiver Studio installed |
| Example source | Chip EVT directory (e.g., `CH32X035EVT/EVT/EXAM/`) |

## Step-by-Step

### 1. Select Base Example

Choose the closest example from the chip's EVT/EXAM/ directory:

| Use Case | Example Directory |
|---|---|
| GPIO blink | `GPIO/` |
| USART printf | `USART/` |
| USB-PD | `USBPD/` |
| USB device | `USB/` |
| ADC | `ADC/` |
| Timer/PWM | `TIM/` |
| Flash | `FLASH/` |
| IAP | `IAP/` |
| Power management | `PWR/` |
| PIOC | `PIOC/` |
| DMA | `DMA/` |

### 2. Copy Example Structure

Copy the entire example directory preserving the full structure:

```
MyProject/
  SRC/
    Core/               # core_riscv.h, core_riscv.c
    Debug/              # debug.h, debug.c
    Ld/
      Link.ld           # Linker script (memory layout)
    Peripheral/
      inc/              # All peripheral headers
      src/              # All peripheral source
    Startup/
      startup_ch32x035.S  # Assembly startup
  User/
    main.c              # Your application code
  .project              # MounRiver project file
  *.wvproj              # MounRiver workspace project
```

### 3. Modify main.c

Replace the example's main.c with your application:

```c
#include "ch32x035.h"
#include "debug.h"

// Forward declarations
void GPIO_Init_User(void);
void USART_Init_User(void);

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    GPIO_Init_User();

    while(1) {
        // Application loop
    }
}
```

### 4. Update Linker Script (if needed)

For standard application (no IAP), Link.ld should have:

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 62K    /* CH32X035 */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

For IAP application, offset Flash origin:

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 60K    /* After 4KB bootloader */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

### 5. Configure Project in MounRiver Studio

1. Open `.wvproj` file
2. Verify linked resources point to correct `SRC/` paths
3. Build: Project -> Build Project (Ctrl+B)
4. Flash: Run -> Debug (WCH-LinkE)

## Chip-Specific Header Reference

| Chip | Main Header | Startup File |
|------|-------------|--------------|
| CH32X035 | `ch32x035.h` | `startup_ch32x035.S` |
| CH32X315 | `ch32x3x5.h` | `startup_ch32x3x5.S` |
| CH643 | `ch643.h` | `startup_ch643.S` |
| CH641 | `ch641.h` | `startup_ch641.S` |

## Common Errors

- Missing `debug.c` in project -- causes linker errors for `Delay_Init()` / `USART_Printf_Init()`
- Wrong linker script memory size -- check chip variant (62K vs 192K vs 16K Flash)
- Forgetting to add `SRC/Peripheral/src/*.c` files to build -- undefined reference errors
