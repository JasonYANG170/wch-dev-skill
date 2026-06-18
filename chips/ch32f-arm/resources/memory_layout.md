# CH32F Memory Layout

## CH32F10x Memory Map

### Flash Memory

| Variant | Flash Size | Start Address | End Address | Page Size |
|---------|-----------|---------------|-------------|-----------|
| CH32F103C6T6 | 32KB | 0x08000000 | 0x08007FFF | 1KB |
| CH32F103C8T6 | 64KB | 0x08000000 | 0x0800FFFF | 1KB |
| CH32F103CBT6 | 128KB | 0x08000000 | 0x0801FFFF | 1KB |
| CH32F103R8T6 | 64KB | 0x08000000 | 0x0800FFFF | 1KB |
| CH32F103RBT6 | 128KB | 0x08000000 | 0x0801FFFF | 1KB |
| CH32F103VET6 | 512KB | 0x08000000 | 0x0807FFFF | 1KB |

### SRAM

| Variant | SRAM Size | Start Address | End Address |
|---------|----------|---------------|-------------|
| CH32F103C6T6 | 10KB | 0x20000000 | 0x200027FF |
| CH32F103C8T6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F103CBT6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F103R8T6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F103RBT6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F103VET6 | 64KB | 0x20000000 | 0x2000FFFF |

### Peripheral Memory

| Peripheral | Start Address | End Address |
|------------|---------------|-------------|
| APB1 | 0x40000000 | 0x4000FFFF |
| APB2 | 0x40010000 | 0x4001FFFF |
| AHB | 0x40018000 | 0x4002FFFF |

### Key Peripheral Addresses

| Peripheral | Address |
|------------|---------|
| TIM2 | 0x40000000 |
| TIM3 | 0x40000400 |
| TIM4 | 0x40000800 |
| USART2 | 0x40004400 |
| USART3 | 0x40004800 |
| I2C1 | 0x40005400 |
| I2C2 | 0x40005800 |
| CAN1 | 0x40006400 |
| DAC | 0x40007400 |
| PWR | 0x40007000 |
| AFIO | 0x40010000 |
| EXTI | 0x40010400 |
| GPIOA | 0x40010800 |
| GPIOB | 0x40010C00 |
| GPIOC | 0x40011000 |
| GPIOD | 0x40011400 |
| GPIOE | 0x40011800 |
| ADC1 | 0x40012400 |
| TIM1 | 0x40012C00 |
| SPI1 | 0x40013000 |
| USART1 | 0x40013800 |
| DMA1 | 0x40020000 |
| RCC | 0x40021000 |
| Flash | 0x40022000 |
| USB | 0x40023400 |

---

## CH32F20x Memory Map

### Flash Memory

| Variant | Flash Size | Start Address | End Address | Page Size |
|---------|-----------|---------------|-------------|-----------|
| CH32F203C8T6 | 64KB | 0x08000000 | 0x0800FFFF | 256B/1KB |
| CH32F203CBT6 | 128KB | 0x08000000 | 0x0801FFFF | 256B/1KB |
| CH32F203RBT6 | 128KB | 0x08000000 | 0x0801FFFF | 256B/1KB |
| CH32F203RCT6 | 256KB | 0x08000000 | 0x0803FFFF | 256B/1KB |
| CH32F207RCT6 | 256KB | 0x08000000 | 0x0803FFFF | 256B/1KB |
| CH32F208RBT6 | 128KB | 0x08000000 | 0x0801FFFF | 256B/1KB |

### SRAM

| Variant | SRAM Size | Start Address | End Address |
|---------|----------|---------------|-------------|
| CH32F203C8T6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F203CBT6 | 20KB | 0x20000000 | 0x20004FFF |
| CH32F203RBT6 | 64KB | 0x20000000 | 0x2000FFFF |
| CH32F203RCT6 | 64KB | 0x20000000 | 0x2000FFFF |
| CH32F207RCT6 | 64KB | 0x20000000 | 0x2000FFFF |
| CH32F208RBT6 | 64KB | 0x20000000 | 0x2000FFFF |

### USB RAM (CH32F20x)

| Region | Address | Size | Description |
|--------|---------|------|-------------|
| USB Endpoint Buffer | 0x40023400 | 512B | USB endpoint data buffers |

---

## IAP Memory Layout

### CH32F10x IAP (64KB Flash)

| Region | Start | End | Size | Description |
|--------|-------|-----|------|-------------|
| Bootloader | 0x08000000 | 0x08000FFF | 4KB | IAP bootloader |
| Application | 0x08001000 | 0x0800FBFF | 60KB | User application |
| Config | 0x0800FC00 | 0x0800FFFF | 1KB | Configuration data |

### CH32F20x IAP (128KB Flash)

| Region | Start | End | Size | Description |
|--------|-------|-----|------|-------------|
| Bootloader | 0x08000000 | 0x08001FFF | 8KB | IAP bootloader |
| Application | 0x08002000 | 0x0801FBFF | 120KB | User application |
| Config | 0x0801FC00 | 0x0801FFFF | 1KB | Configuration data |

---

## Vector Table

The interrupt vector table is located at the start of Flash (0x08000000) or can be relocated to SRAM.

### Key Vector Table Entries

| Offset | Name | Description |
|--------|------|-------------|
| 0x00 | Initial SP | Initial stack pointer value |
| 0x04 | Reset | Reset handler address |
| 0x08 | NMI | NMI handler |
| 0x0C | HardFault | Hard fault handler |
| 0x10 | MemManage | Memory management fault |
| 0x14 | BusFault | Bus fault |
| 0x18 | UsageFault | Usage fault |
| 0x2C | SVCall | SVCall handler |
| 0x38 | PendSV | PendSV handler |
| 0x3C | SysTick | SysTick handler |
| 0x40 | WWDG | Window watchdog |
| 0x44 | PVD | PVD through EXTI |
| 0x54 | FLASH | Flash global |
| 0x58 | RCC | RCC global |
| 0x5C | EXTI0 | EXTI Line0 |
| ... | ... | ... |
| 0xD4 | USART1 | USART1 global |
| 0xD8 | USART2 | USART2 global |

---

## Linker Script Memory Definitions

For Keil MDK or GCC, the memory regions are defined as:

```
/* CH32F103C8T6 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* CH32F203RCT6 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

For IAP application (offset from bootloader):

```
/* CH32F103C8T6 IAP Application */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08001000, LENGTH = 60K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```
