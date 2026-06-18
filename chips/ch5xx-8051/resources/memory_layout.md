# CH5xx 8051 Memory Layout

This document describes the memory architecture of WCH CH5xx 8051 chips.

---

## 8051 Memory Architecture Overview

The 8051 uses a Harvard architecture with separate memory spaces:

```
+------------------+
| Code Memory      |  Program storage (Flash ROM)
| (ROM)            |  Up to 64KB depending on chip
+------------------+
| Internal Data    |  128 bytes direct addressing (data)
| (data, 0x00-0x7F)|  Fast access, register-like
+------------------+
| Upper 128        |  128 bytes indirect addressing (idata)
| (idata, 0x80-0xFF)| Shared with SFR space
+------------------+
| External Data    |  Up to several KB (xdata)
| (xdata)          |  Accessed via MOVX instruction
+------------------+
| Bit Addressable  |  128 bits (0x20-0x2F in data space)
| (bit)            |  Individual bit access
+------------------+
```

---

## Memory Spaces by Chip

| Chip | Code Flash | data | idata | xdata | DataFlash |
|---|---|---|---|---|---|
| CH552 | 16 KB | 128B | 128B | 1 KB (0x0400) | 512B @ 0xC000 |
| CH551 | 10 KB | 128B | 128B | 512B | 256B @ 0xC000 |
| CH554 | 14 KB | 128B | 128B | 1 KB (0x0400) | 512B @ 0xC000 |
| CH555 | 64 KB | 128B | 128B | 1 KB (0x0400) | -- |
| CH559 | 64 KB | 128B | 128B | 4 KB (0x1000) | -- |
| CH543 | 14 KB | 128B | 128B | 1 KB (0x0400) | 512B @ 0xC000 |
| CH549 | 16 KB | 128B | 128B | 1 KB (0x0400) | 512B @ 0xC000 |

---

## Code Memory Map

```
0x0000 +------------------+
       | Reset Vector     |  LJMP main
0x0003 +------------------+
       | INT0 Vector      |  Interrupt 0
0x000B +------------------+
       | Timer0 Vector    |  Interrupt 1
0x0013 +------------------+
       | INT1 Vector      |  Interrupt 2
0x001B +------------------+
       | Timer1 Vector    |  Interrupt 3
0x0023 +------------------+
       | UART0 Vector     |  Interrupt 4
0x002B +------------------+
       | Timer2 Vector    |  Interrupt 5
0x0033 +------------------+
       | SPI0 Vector      |  Interrupt 6
0x003B +------------------+
       | TouchKey Vector  |  Interrupt 7
0x0043 +------------------+
       | USB Vector       |  Interrupt 8
0x004B +------------------+
       | ADC Vector       |  Interrupt 9
0x0053 +------------------+
       | UART1 Vector     |  Interrupt 10
0x005B +------------------+
       | PWM Vector       |  Interrupt 11
0x0063 +------------------+
       | GPIO Vector      |  Interrupt 12
0x006B +------------------+
       | WDOG Vector      |  Interrupt 13
0x0073 +------------------+
       |                  |
       | Program Code     |  User application
       |                  |
       +------------------+
       | Boot Loader      |  (CH552: 0x3800, CH559: 0xF000)
       +------------------+
       | Chip Config      |  (CH552: 0x3FF8)
       +------------------+
       | Chip ID          |  (CH552: 0x3FFA-0x3FFF)
       +------------------+
```

---

## Internal Data Memory (data/idata)

```
0x00-0x1F  Register Banks (4 banks x 8 registers)
           Bank 0: R0-R7 @ 0x00-0x07 (default)
           Bank 1: R0-R7 @ 0x08-0x0F
           Bank 2: R0-R7 @ 0x10-0x17
           Bank 3: R0-R7 @ 0x18-0x1F

0x20-0x2F  Bit Addressable Area (128 bits)
           Each byte has 8 individually addressable bits
           Bit 0x00 = byte 0x20 bit 0
           Bit 0x7F = byte 0x2F bit 7

0x30-0x7F  General Purpose Data (data segment)
           Direct addressing, fast access
           Default location for 'data' variables

0x80-0xFF  Upper RAM (idata) / SFR Space
           Shared address space:
           - Accessed via indirect addressing = idata (RAM)
           - Accessed via direct addressing = SFR registers
           SFR range: 0x80-0xFF (byte addressable)
           SFR bit range: 0x80-0xFF (bit addressable for some)
```

---

## External Data Memory (xdata)

```
CH552/CH554/CH543/CH549:
0x0000-0x03FF  1 KB xdata SRAM

CH559:
0x0000-0x0FFF  4 KB xdata SRAM

CH555:
0x0000-0x03FF  1 KB xdata SRAM
```

xdata is accessed via MOVX instruction and is slower than data/idata. Use for large buffers and arrays.

---

## DataFlash Memory

DataFlash is a special region of the Flash ROM that can be read/written byte-by-byte (unlike code Flash which requires word access and sector erase).

```
CH552: 0xC000-0xC1FF  (512 bytes)
CH554: 0xC000-0xC1FF  (512 bytes)
CH551: 0xC000-0xC0FF  (256 bytes)
CH543: 0xC000-0xC1FF  (512 bytes)
CH549: 0xC000-0xC1FF  (512 bytes)
```

**Access rules**:
- Read: ROM_ADDR_H = 0xC0, ROM_ADDR_L = byte_address << 1 (even)
- Write: Requires safe mode unlock, EA=0
- No sector erase needed (byte overwrite OK)

---

## Memory Qualifier Reference (Keil C51)

| Qualifier | Segment | Addressing | Size | Speed | Use Case |
|---|---|---|---|---|---|
| `data` | Direct data | Direct | 128B | Fastest | Frequently accessed vars |
| `idata` | Indirect data | Indirect | 256B | Fast | Stack, more vars |
| `xdata` | External data | MOVX | Chip-dep | Slow | Large buffers |
| `code` | Code/ROM | MOVC | Up to 64KB | Read-only | Constants, tables |
| `pdata` | Page data | MOVX (paged) | 256B | Medium | Rarely used |
| `bit` | Bit addressable | Bit ops | 1 bit | Fast | Flags, booleans |

### Type Aliases (from CH5xx.H)

```c
typedef unsigned char           UINT8;
typedef unsigned short          UINT16;
typedef unsigned long           UINT32;
typedef bit                     BOOL;
typedef unsigned char  data     UINT8D;    // data segment
typedef unsigned char  idata    UINT8I;    // idata segment
typedef unsigned char  xdata    UINT8X;    // xdata segment
typedef unsigned char volatile  UINT8V;    // volatile
typedef const unsigned char code UINT8C;   // code (ROM)
typedef unsigned char         *PUINT8;     // generic pointer
typedef unsigned char  xdata  *PUINT8X;    // xdata pointer
```

---

## Stack Location

The 8051 stack grows upward in the data/idata space:

```
Default SP = 0x07 (after reset)
Stack grows: 0x08, 0x09, 0x0A, ...

If using register bank 0 (default):
  Stack starts at 0x08 (after R0-R7)
  
If using register bank 1:
  Stack starts at 0x10 (after bank 1 R0-R7)
```

**Caution**: The stack shares space with general-purpose data variables. Deep call nesting + many local variables = stack overflow = data corruption.

---

## USB DMA Buffer Placement

USB endpoints require DMA buffers in xdata. Typical layout:

```
xdata 0x0000: EP0 buffer (64 bytes)
xdata 0x0040: EP1 buffer (64 bytes)
xdata 0x0080: EP2 buffer (64 bytes)
xdata 0x00C0: EP3 buffer (64 bytes)
xdata 0x0100: Application xdata (remaining)
```

Buffer addresses must be written to UEPn_DMA registers as 16-bit values.

---

## Keil C51 Memory Model Settings

| Model | Default Variable | Code Size | Notes |
|---|---|---|---|
| Small | data | < 4KB | Fast, limited RAM |
| Compact | pdata | < 4KB | Medium |
| Large | xdata | Up to 64KB | Slow, full RAM |

Set in Keil: Options for Target -> Target -> Memory Model

For most CH5xx projects, use **Small** model with explicit `xdata` for large buffers.
