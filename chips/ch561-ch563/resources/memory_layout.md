# CH561/CH563 Memory Layout

## Memory Map

```
Address         Region        Size    Description
-----------     -----------   ------  --------------------------------
0x00000000      FLASH         224KB   Code and constant data
0x00038000      FLASH end     -       End of Code Flash

0x00038000      Data-Flash    28KB    EEPROM data storage (byte-writable)
0x0003F000      Data-Flash end -      End of Data-Flash

0x00400000      SFR           64KB    Peripheral registers
0x0040FFFF      SFR end       -       End of SFR

0x00808000      SRAM          32-96KB General-purpose SRAM
                (configurable via MEM_DATA in SYSFREQ.H)
0x00820000      SRAM end      -       End of SRAM (max 96KB)

0x00C00000      XBUS          1MB     External bus (if available)
0x00CFFFFF      XBUS end      -       End of XBUS
```

## Flash ROM (0x00000000) - 224KB

- Code storage (read-only after programming)
- 4KB erase block granularity
- 4-byte write minimum (dword)
- 256-byte page recommended for best performance
- Typical layout:
  - 0x00000000-0x00000FFF: Bootloader (IAP, 4KB)
  - 0x00001000-0x00037FFF: Application code (~220KB)

## Data-Flash / EEPROM (0x00038000) - 28KB

- Byte-writable (no need to erase for 0->1 bit changes)
- Erase needed only for 1->0 bit changes
- 4KB erase block granularity
- Good for frequently updated data (counters, config)
- Address range: 0x00038000 - 0x0003EFFF

## SFR (0x00400000) - 64KB

Peripheral registers are memory-mapped in this region:

```
0x00400000 +-----------------------+
           | System Registers      |  PLL, power, clock, reset
0x00400100 +-----------------------+
           | GPIO Registers        |  PA, PB, interrupt
0x00400200 +-----------------------+
           | UART0 Registers       |  0x00400200-0x004002FF
0x00400300 +-----------------------+
           | UART1 Registers       |  0x00400300-0x004003FF
0x00400400 +-----------------------+
           | SPI0 Registers        |  0x00400400-0x004004FF
0x00400500 +-----------------------+
           | SPI1 Registers        |  0x00400500-0x004005FF
0x00400600 +-----------------------+
           | Timer0 Registers      |  0x00400600-0x004006FF
0x00400700 +-----------------------+
           | Timer1 Registers      |  0x00400700-0x004007FF
0x00400800 +-----------------------+
           | Timer2 Registers      |  0x00400800-0x004008FF
0x00400900 +-----------------------+
           | Timer3 Registers      |  0x00400900-0x004009FF
0x00400A00 +-----------------------+
           | ADC Registers         |  0x00400A00-0x00400AFF
0x00400B00 +-----------------------+
           | Ethernet MAC Registers|  0x00400B00-0x00400BFF
0x00404000 +-----------------------+
           | USB Registers (CH563) |  0x00404000-0x00405FFF
0x0040FFFF +-----------------------+
```

## SRAM (0x00808000) - 32/64/96KB

Configurable via `MEM_DATA` in SYSFREQ.H:

| MEM_DATA | SRAM Size | End Address |
|----------|-----------|-------------|
| 0 | 32KB | 0x00810000 |
| 1 | 64KB | 0x00818000 |
| 2 | 96KB | 0x00820000 |

```c
// In SYSFREQ.H:
#define MEM_DATA    2    // 96KB SRAM
```

### Stack Placement

```
SRAM Layout:
0x00808000 +-----------------------+
           | .data (initialized)   |
           | .bss (zero-init)      |
           | Heap                  |
           |         ...           |
           | Stack (grows down)    |
0x008xxxxx +-----------------------+  <- Stack top
```

Stack top is set in STARTUP.S:
```
__initial_sp    EQU     0x0081FC00   ; For 96KB SRAM
```

## Scatter File (Keil MDK)

```
LR_IAP 0x00000000 0x00038000 {
    ER_IAP 0x00000000 0x00038000 {
        *.o (RESET, +First)
        *(InRoot$$Sections)
        .ANY (+RO)
        .ANY (+XO)
    }
    RW_IRAM1 0x00808000 UNINIT 0x00004000 {
        .ANY (+RW +ZI)
    }
    RW_IRAM2 0x0080C000 UNINIT 0x00004000 {
        .ANY (+RW2)
    }
}
```

## GPIO Port D (CH563 Only)

CH563 has an additional GPIO port D (PD0-PD31) with registers at:
- R32_PD_DIR, R32_PD_PIN, R32_PD_OUT, R32_PD_CLR
- R32_PD_PU, R32_PD_PD, R32_PD_DRV, R32_PD_SMT

## USB Registers (CH563 Only)

CH563 has USB SFR registers at 0x00404000-0x00405FFF.
CH561 does NOT have USB.

## Peripheral Clock Gating

Peripheral clocks are controlled by R8_SLP_CLK_OFF0 and R8_SLP_CLK_OFF1:

```c
// R8_SLP_CLK_OFF0 (0=clock on, 1=clock off)
// Bit 0: TMR0    Bit 1: TMR1    Bit 2: TMR2    Bit 3: TMR3
// Bit 4: SPI0    Bit 5: SPI1    Bit 6: UART0   Bit 7: UART1

// R8_SLP_CLK_OFF1
// Bit 4: ADC     Bit 5: GPIO    Bit 6: USB(CH563)  Bit 7: ETH
```

## Notes

- Flash erase unit: 4KB (4096 bytes)
- Flash write minimum: 4 bytes (dword)
- Data-Flash (EEPROM): 28KB, byte-writable
- SRAM size is configurable via MEM_DATA (32/64/96KB)
- SFR region is 64KB at 0x00400000
- CH563 has additional USB and GPIO PD registers compared to CH561
