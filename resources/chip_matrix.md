# WCH Chip Matrix

Use this matrix to route a WCH request to the correct local family directory before reading recipes or examples.

| Chip or Family | Directory | Architecture | Typical Header | Toolchain | Main Evidence |
|---|---|---|---|---|---|
| CH572, CH579 | `chips/ch57x/` | RISC-V | `CH57x_common.h` | MounRiver | BLE/RF/USB/PM examples under `resources/EXAM/` |
| CH583, CH585, CH592, CH595 | `chips/ch58x-ch59x/` | RISC-V | `CH58x_common.h`, `CH59x_common.h` | MounRiver | BLE, USB, LCD, NET, NFCA, ENCODER examples |
| CH32V103, CH32V20x, CH32V307, CH32V407 | `chips/ch32v-general/` | RISC-V | `ch32v10x.h`, `ch32v20x.h`, `ch32v30x.h`, `ch32v4x7.h` | MounRiver | StdPeriphDriver-style examples |
| CH32V003, CH32V006, CH32L103 | `chips/ch32v-lowcost/` | RISC-V | `ch32v00x.h`, `ch32v00X.h`, `ch32l103.h` | MounRiver | Low-cost GPIO/UART/SPI/I2C/USB-PD examples |
| CH32F103, CH32F20x, CH32M030 | `chips/ch32f-arm/` | ARM Cortex-M | `ch32f10x.h`, `ch32f20x.h` | Keil MDK, MounRiver | ARM startup/vector and StdPeriph examples |
| CH32X035, CH32X315, CH643, CH641, CH634 | `chips/ch32x-usbpd/` | RISC-V | `ch32x035.h`, `ch32x3x5.h`, `ch643.h`, `ch641.h` | MounRiver | USB-PD, USB, PIOC, GPIO examples |
| CH569 | `chips/ch56x-ethernet/` | RISC-V | `CH56x_common.h` | MounRiver | Ethernet, USB 3.0, eMMC, HSPI examples |
| CH561, CH563 | `chips/ch561-ch563/` | ARM7TDMI | `CH561SFR.H`, `CH563SFR.H` | Keil MDK | Register/SFR style Ethernet and USB examples |
| CH32H417 | `chips/ch32h-highperf/` | RISC-V | `ch32h417.h` | MounRiver | USB 3.0, LTDC, SerDes, dual-core examples |
| CH543, CH545, CH549, CH552, CH554, CH555, CH559 | `chips/ch5xx-8051/` | 8051 | `CH5xx.H` | Keil C51, SDCC | 8051 memory model, USB, TouchKey examples |

## Routing Rules

- Exact chip number wins over marketing family names.
- If a chip appears in multiple examples, prefer the example whose header and startup file match the user's project.
- Do not cross architecture boundaries for interrupt syntax, startup files, linker scripts, or memory qualifiers.
- For features such as BLE, USB-PD, USB 3.0, Ethernet, and IAP, read the family recipe plus the closest `resources/EXAM/` project before writing code.
