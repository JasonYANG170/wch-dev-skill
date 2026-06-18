# CH58x/CH59x Memory Layout Reference

## Chip Memory Map

### CH583

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash  | 0x00000000 | 0x0006FFFF | 448KB | Code and constant data |
| RAM    | 0x20000000 | 0x20007FFF | 32KB | Stack, heap, variables |
| Boot   | 0x00000000 | 0x00000FFF | 4KB | Bootloader (if IAP) |

### CH585

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash  | 0x00000000 | 0x0006FFFF | 448KB | Code and constant data |
| RAM    | 0x20000000 | 0x2001FFFF | 128KB | Stack, heap, variables |
| Boot   | 0x00000000 | 0x00000FFF | 4KB | Bootloader (if IAP) |

**Note**: CH585 has 4x the RAM of CH583 (128KB vs 32KB). The BLE stack can use a larger heap. A CH584 variant exists with 96KB RAM (comment in linker script).

### CH592

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash  | 0x00000000 | 0x0006FFFF | 448KB | Code and constant data |
| RAM    | 0x20000000 | 0x200067FF | 26KB | Stack, heap, variables |
| Boot   | 0x00000000 | 0x00000FFF | 4KB | Bootloader (if IAP) |

### CH595

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Flash  | 0x00000000 | 0x0003BFFF | 240KB | Code and constant data |
| RAM    | 0x20000000 | 0x20007FFF | 32KB | Stack, heap, variables |
| Boot   | 0x00000000 | 0x00000FFF | 4KB | Bootloader (if IAP) |

**Note**: CH595 has smaller Flash (240KB vs 448KB) but more RAM (32KB vs 26KB) compared to CH592. Plan code size carefully — BLE stack + NFCA/ENCODER/KEYSCAN drivers can consume significant Flash.

## Standard Application Linker Script

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K  /* CH595: 240K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K   /* CH585: 128K, CH592: 26K */
}

SECTIONS
{
    .init :
    {
        _sinit = .;
        . = ALIGN(4);
        KEEP(*(SORT_NONE(.init)))
        . = ALIGN(4);
        _einit = .;
    } >FLASH AT>FLASH

    .highcode :
    {
        . = ALIGN(4);
        *(.highcode*)
        . = ALIGN(4);
    } >FLASH AT>FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH AT>FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON*)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .stack :
    {
        . = ALIGN(8);
        . = . + 512;  /* 512-byte stack */
        . = ALIGN(8);
    } >RAM
}
```

## IAP Linker Script (Application at 0x1000)

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K  /* Offset by 4KB; CH595: 236K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K   /* CH585: 128K, CH592: 26K */
}
```

## Section Placement Guidelines

| Section | Location | Purpose |
|---------|----------|---------|
| `.highcode` | Flash (loaded), RAM (executed) | Interrupt handlers — fast execution from RAM |
| `.text` | Flash | Code and constants |
| `.data` | RAM (loaded from Flash) | Initialized global/static variables |
| `.bss` | RAM | Zero-initialized global/static variables |
| `.stack` | RAM (end) | Call stack |

## BLE Stack Memory

BLE stack requires dedicated heap memory configured in `config.h`:

```c
#define BLE_MEMHEAP_SIZE    (1024*6)   // 6KB minimum for BLE stack
#define BLE_BUFF_MAX_LEN    27         // Max BLE packet length (ATT_MTU + 4)
```

## Flash Sector Operations

- **Erase granularity**: 256 bytes per sector
- **Write granularity**: 4 bytes (word-aligned)
- **Write must follow erase** — cannot overwrite non-0xFF bytes
- **Read-modify-write required** for partial sector updates
