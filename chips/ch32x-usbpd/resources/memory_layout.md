# CH32X/CH6xx Memory Layout Reference

## Chip Memory Map Summary

### CH32X035 (62KB Flash, 20KB RAM)

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash | 0x00000000 | 0x0000F7FF | 62KB | Code and constant data |
| RAM | 0x20000000 | 0x20004FFF | 20KB | Data, BSS, stack |

### CH32X315 (192KB Flash, 64KB RAM)

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash | 0x00000000 | 0x0002FFFF | 192KB | Code and constant data |
| RAM | 0x20000000 | 0x2000FFFF | 64KB | Data, BSS, stack |

### CH643 (192KB Flash, 64KB RAM)

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash | 0x00000000 | 0x0002FFFF | 192KB | Code and constant data |
| RAM | 0x20000000 | 0x2000FFFF | 64KB | Data, BSS, stack |

### CH641 (16KB Flash, 2KB RAM)

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash | 0x00000000 | 0x00003FFF | 16KB | Code and constant data |
| RAM | 0x20000000 | 0x200007FF | 2KB | Data, BSS, stack |

## Standard Application Linker Script

### CH32X035

```
ENTRY( _start )
__stack_size = 2048;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 62K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

### CH32X315 / CH643

```
ENTRY( _start )
__stack_size = 2048;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 192K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

### CH641

```
ENTRY( _start )
__stack_size = 256;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}
```

## IAP Memory Layout

### Bootloader (4KB at start of Flash)

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K  /* CH32X035 */
}
```

### Application (after bootloader)

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 60K   /* CH32X035: 62K - 2K = 60K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* For CH32X315/CH643: */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 188K  /* 192K - 4K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}

/* For CH641: */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 12K   /* 16K - 4K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}
```

## Linker Script Section Layout

```
SECTIONS
{
    .init :     { *(.init) }                          >FLASH AT>FLASH
    .vector :   { *(.vector); . = ALIGN(64); }        >FLASH AT>FLASH
    .text :     { *(.text); *(.rodata); }              >FLASH AT>FLASH
    .data :     { *(.data); *(.sdata); }               >RAM AT>FLASH
    .bss :      { *(.bss); *(.sbss); }                 >RAM
    .stack :    { . = ORIGIN(RAM) + LENGTH(RAM) - __stack_size; } >RAM
}
```

Key points:
- `.vector` section contains the interrupt vector table, must be at start of Flash
- `.data` is initialized from Flash at startup (copied by startup code)
- `.bss` is zero-initialized at startup
- Stack grows downward from top of RAM
- No `.highcode` section in these chips (unlike CH57x)

## Peripheral Register Base Addresses (CH32X035)

| Peripheral | Base Address | Bus |
|------------|-------------|-----|
| GPIOA | 0x40010800 | APB2 |
| GPIOB | 0x40010C00 | APB2 |
| GPIOC | 0x40011000 | APB2 |
| USART1 | 0x40013800 | APB2 |
| SPI1 | 0x40013000 | APB2 |
| TIM1 | 0x40012C00 | APB2 |
| ADC1 | 0x40012400 | APB2 |
| USART2 | 0x40004400 | APB1 |
| USART3 | 0x40004800 | APB1 |
| USART4 | 0x40004C00 | APB1 |
| TIM2 | 0x40000000 | APB1 |
| TIM3 | 0x40000400 | APB1 |
| I2C1 | 0x40005400 | APB1 |
| USBFS | 0x40005C00 | AHB |
| USBPD | 0x40006000 | AHB |
| DMA1 | 0x40020000 | AHB |
| FLASH | 0x40022000 | AHB |
| RCC | 0x40021000 | AHB |
| PIOC | 0x40006800 | APB2 |

## Flash Sector Operations

Flash operates on 256-byte pages:
- **Read**: Any address, any length (direct memory access)
- **Erase**: `FLASH_ErasePage(addr)` -- addr must be 256-byte aligned
- **Program**: `FLASH_ProgramWord(addr, data)` -- 4 bytes at a time, addr must be 4-byte aligned
- **Fast Program**: `FLASH_ProgramPage_Fast(addr)` -- programs full 256-byte page from buffer

## Stack and Heap

Default stack size: 2048 bytes (256 bytes for CH641)

Stack placement: top of RAM, growing downward

Heap: from end of BSS to bottom of stack (if any remaining space)

## Unique ID

Each chip has a unique ID readable from the device signature area:
```c
// Read chip ID (for identification)
uint32_t chip_id = DBGMCU_GetCHIPID();
```
