# CH569 Memory Layout

## Memory Map

```
Address         Region        Size    Description
-----------     -----------   ------  --------------------------------
0x00000000      FLASH         448KB   Code and constant data
0x00070000      FLASH (data)  ~64KB   Data storage area (last 64KB)
0x00080000      FLASH end     -       End of Flash

0x20000000      RAM           16KB    General-purpose SRAM
0x20004000      RAM end       -       End of RAM

0x20020000      RAMX          32KB    Executable RAM (DMA accessible)
0x20028000      RAMX end      -       End of RAMX
```

## Linker Script (Link.ld)

```
ENTRY( _start )

__stack_size = 2048;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 16K
    RAMX (xrw) : ORIGIN = 0x20020000, LENGTH = 32K
}
```

## Section Placement

| Section | Region | Description |
|---------|--------|-------------|
| `.init` | FLASH | Initialization code |
| `.vector` | FLASH | Interrupt vector table |
| `.text` | FLASH | Code and read-only data |
| `.data` | RAM | Initialized global/static variables |
| `.bss` | RAM | Zero-initialized variables |
| `.stack` | RAM | Stack (top of RAM, 2048 bytes) |
| `.dmadata` | RAMX | DMA-accessible buffers |
| `.highcode` | RAMX | Fast-executing code (ISRs) |

## RAM (0x20000000) - 16KB

- General-purpose data storage
- Stack (grows downward from 0x20004000)
- Initialized (.data) and uninitialized (.bss) variables
- NOT accessible by DMA controllers
- Default stack size: 2048 bytes

## RAMX (0x20020000) - 32KB

- Executable RAM (can run code from here)
- Accessible by DMA controllers
- Used for `.dmadata` section (DMA buffers)
- Used for `.highcode` section (fast ISR code)
- Ethernet, eMMC, and other DMA buffers MUST be placed here
- Can also be used as extended code space

## FLASH (0x00000000) - 448KB

- Code storage (read-only after programming)
- 256-byte erase sector granularity
- 4-byte write granularity
- Typical layout:
  - 0x00000000-0x00000FFF: Bootloader (IAP, 4KB)
  - 0x00001000-0x0006FFFF: Application code (~444KB)
  - 0x00070000-0x0007FFFF: Data storage (~64KB)

## IAP Memory Layout

When using In-Application Programming:

```
FLASH Layout with IAP:
0x00000000 +-----------------------+
           | IAP Bootloader        |  4KB
0x00001000 +-----------------------+
           | Application (current) |
           |         ...           |
0x00040000 +-----------------------+
           | Download buffer       |  For new firmware
           |         ...           |
0x00070000 +-----------------------+
           | Config/Data storage   |  Persistent data
0x00080000 +-----------------------+
```

## DMA Buffer Requirements

All DMA-accessible buffers must be in RAMX:

```c
// Ethernet TX/RX buffers
__attribute__((aligned(4), section(".dmadata")))
uint8_t eth_tx_buf[4][1518];

__attribute__((aligned(4), section(".dmadata")))
uint8_t eth_rx_buf[4][1518];

// eMMC buffers
__attribute__((aligned(4), section(".dmadata")))
uint8_t emmc_buf[512];

// SPI DMA buffers
__attribute__((aligned(4), section(".dmadata")))
uint8_t spi_dma_buf[256];
```

## Stack Usage Guidelines

- Default stack: 2048 bytes (at top of RAM)
- Increase if using deep call chains or large local variables
- Modify in Link.ld: `__stack_size = 4096;`
- Use global/static buffers instead of large local arrays
- Monitor stack usage during development (fill with pattern, check after run)

## Peripheral Register Base Addresses

Peripherals are memory-mapped. Register bases are defined in CH56xSFR.h:

```
ETH_BASE        Ethernet MAC registers
USBSS_BASE      USB 3.0 registers (CH569 only)
USBHS_BASE      USB 2.0 High-Speed registers
EMMC_BASE       eMMC/SDIO registers
SPI0_BASE       SPI0 registers
SPI1_BASE       SPI1 registers
UART0_BASE      UART0 registers
UART1_BASE      UART1 registers
UART2_BASE      UART2 registers
UART3_BASE      UART3 registers
TMR0_BASE       Timer0 registers
TMR1_BASE       Timer1 registers
TMR2_BASE       Timer2 registers
GPIO_BASE       GPIO registers
```
