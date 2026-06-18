# CH32H417 Memory Layout

## Flash Memory

| Region | Start Address | Size | Description |
|--------|---------------|------|-------------|
| Bootloader | `0x00000000` | 64KB (0x10000) | Built-in bootloader (USB/UART download) |
| Application (V5F) | `0x00010000` | 416KB or 896KB | V5F application code |
| Application (V3F) | Configurable | Configurable | V3F application code |

### Flash Capacity Options
- **480KB**: Application region = 416KB (0x10000 - 0x7FFFF)
- **960KB**: Application region = 896KB (0x10000 - 0xEFFFF)

### Flash Sector Size
- Erase unit: 4KB (one page)
- Program unit: 4 bytes (one word)
- Fast programming: Full page at once

## RAM Memory (V5F Core)

| Region | Start Address | Size | Description |
|--------|---------------|------|-------------|
| ITCM RAM | `0x200A0000` | 128KB | Instruction TCM - for code execution |
| DTCM RAM | `0x200C0000` | 256KB | Data TCM - for .data, .bss, stack |
| RAM_LOAD | `0x200C0000` + 512 | 256 bytes | For `.load` section |

### DTCM RAM Layout (V5F)
```
0x200C0000 +------------------+
           | .load section    |  256 bytes
0x200C0200 +------------------+
           | .data section    |  Initialized data
           | .bss section     |  Zero-initialized data
           | Heap             |  Dynamic allocation
           |                  |
           | Stack            |  2048 bytes (default)
0x200FFFFF +------------------+
```

### ITCM RAM Layout (V5F)
```
0x200A0000 +------------------+
           | .highcode section|  Performance-critical code
           | .text section    |  Code execution from RAM
           |                  |
0x200BFFFF +------------------+
```

## RAM Memory (V3F Core)

The V3F core has its own memory region, separate from V5F. Check the V3F linker script for exact addresses.

## Peripheral Memory Map

| Peripheral | Base Address | Bus |
|------------|-------------|-----|
| GPIOA | `0x40010800` | APB2 |
| GPIOB | `0x40010C00` | APB2 |
| GPIOC | `0x40011000` | APB2 |
| GPIOD | `0x40011400` | APB2 |
| GPIOE | `0x40011800` | APB2 |
| GPIOF | `0x40011C00` | APB2 |
| USART1 | `0x40013800` | APB2 |
| USART2 | `0x40004400` | APB1 |
| USART3 | `0x40004800` | APB1 |
| UART4 | `0x40004C00` | APB1 |
| UART5 | `0x40005000` | APB1 |
| SPI1 | `0x40013000` | APB2 |
| SPI2 | `0x40003800` | APB1 |
| SPI3 | `0x40003C00` | APB1 |
| I2C1 | `0x40005400` | APB1 |
| I2C2 | `0x40005800` | APB1 |
| TIM1 | `0x40012C00` | APB2 |
| TIM2 | `0x40000000` | APB1 |
| TIM3 | `0x40000400` | APB1 |
| TIM4 | `0x40000800` | APB1 |
| TIM5 | `0x40000C00` | APB1 |
| TIM6 | `0x40001000` | APB1 |
| TIM7 | `0x40001400` | APB1 |
| TIM8 | `0x40013400` | APB2 |
| TIM9 | `0x40001800` | APB1 |
| TIM10 | `0x40001C00` | APB1 |
| TIM11 | `0x40002000` | APB1 |
| TIM12 | `0x40002400` | APB1 |
| ADC1 | `0x40012400` | APB2 |
| ADC2 | `0x40012800` | APB2 |
| CAN1 | `0x40006400` | APB1 |
| CAN2 | `0x40006800` | APB1 |
| CAN3 | `0x40006C00` | APB1 |
| ETH | `0x40028000` | AHB |
| USBPD | `0x4000D000` | APB1 |
| LTDC | `0x40016800` | AHB |
| DMA1 | `0x40020000` | AHB |
| DMA2 | `0x40020400` | AHB |
| FLASH | `0x40022000` | AHB |
| RCC | `0x40021000` | AHB |
| CRC | `0x40023000` | AHB |

## Linker Script Details

### V5F Linker Script (`Link_v5f.ld`)

```
MEMORY
{
    FLASH (rx)     : ORIGIN = 0x00010000, LENGTH = 128K
    RAM_CODE (xrw) : ORIGIN = 0x200A0000, LENGTH = 128K    /* ITCM */
    RAM (xrw)      : ORIGIN = (0x200C0000+768), LENGTH = (256K-768)  /* DTCM */
    RAM_LOAD (xrw) : ORIGIN = (0x200C0000+512), LENGTH = 256
}
```

### Section Placement

| Section | Memory Region | Description |
|---------|---------------|-------------|
| `.init` | FLASH | Initialization code |
| `.highcode` | RAM_CODE (ITCM) | Performance-critical code |
| `.text` | FLASH | Application code |
| `.data` | RAM (DTCM) | Initialized global variables |
| `.bss` | RAM (DTCM) | Zero-initialized variables |
| `.load` | RAM_LOAD | Bootloader load section |
| Stack | RAM (DTCM) | Stack (default 2048 bytes) |

## `.highcode` Section

For performance-critical code, use the `.highcode` section attribute:
```c
void __attribute__((section(".highcode"))) FastFunction(void)
{
    // This code runs from ITCM RAM (faster than Flash)
}
```

## External Memory (FMC)

The FMC (Flexible Memory Controller) supports:
- NOR Flash / SRAM (up to 4 banks)
- NAND Flash
- SDRAM

Address space for external memory:
- Bank 1: `0x60000000` - `0x6FFFFFFF`
- Bank 2: `0x70000000` - `0x7FFFFFFF`
- Bank 3: `0x80000000` - `0x8FFFFFFF`
- Bank 4: `0x90000000` - `0x9FFFFFFF`
