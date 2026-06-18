# Memory Layout Reference

Flash and RAM sizes for CH32V103, CH32V20x, CH32V307, and CH32V407 variants.

---

## Address Map (All Families)

| Region | Start Address | Description |
|--------|---------------|-------------|
| Flash | 0x00000000 | Program code, constants |
| SRAM | 0x20000000 | Stack, heap, variables |
| APB1 | 0x40000000 | Low-speed peripherals |
| APB2 | 0x40010000 | High-speed peripherals |
| AHB | 0x40020000 | DMA, ETH, USB |
| EXTI | 0x40010400 | External interrupts |
| AFIO | 0x40010000 | Alternate function I/O |
| Flash registers | 0x40022000 | FLASH->ACR, etc. |

---

## CH32V103

| Variant | Flash | RAM | Package |
|---------|-------|-----|---------|
| CH32V103C8T6 | 64KB | 20KB | LQFP48 |
| CH32V103C6T6 | 32KB | 10KB | LQFP48 |
| CH32V103R8T6 | 64KB | 20KB | LQFP64 |
| CH32V103R6T6 | 32KB | 10KB | LQFP64 |
| CH32V103CBT6 | 128KB | 20KB | LQFP48 |

**Linker Script**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K   /* or 32K, 128K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K   /* or 10K */
}
```

---

## CH32V20x

| Variant | Flash | RAM | Notes |
|---------|-------|-----|-------|
| CH32V203C8T6 | 64KB | 20KB | Standard |
| CH32V203C6T6 | 32KB | 10KB | Standard |
| CH32V203K8T6 | 64KB | 20KB | 32-pin |
| CH32V203G8R6 | 64KB | 20KB | QFN28 |
| CH32V203F8P6 | 64KB | 20KB | TSSOP20 |
| CH32V203F6P6 | 32KB | 10KB | TSSOP20 |
| CH32V203RBT6 | 128KB | 64KB | LQFP64, BLE capable |
| CH32V208WBU6 | 128KB | 64KB | QFN28, BLE, USB |
| CH32V208GBU6 | 128KB | 64KB | QFN28, BLE |
| CH32V208RBT6 | 128KB | 64KB | LQFP64, BLE |

**Linker Script (D8 variant, 448KB Flash / 64KB RAM)**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

**Standard variant (64KB Flash / 20KB RAM)**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

**BLE variant (128KB Flash / 64KB RAM)**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

---

## CH32V307

| Variant | Flash | RAM | Notes |
|---------|-------|-----|-------|
| CH32V307RCT6 | 256KB | 64KB | LQFP64 |
| CH32V307VCT6 | 256KB | 64KB | LQFP100 |
| CH32V307RBT6 | 128KB | 64KB | LQFP64 |
| CH32V307VBT6 | 128KB | 64KB | LQFP100 |
| CH32V307WCU6 | 256KB | 64KB | QFN28 |

**Linker Script**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K   /* or 128K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

**Special Features**: FPU, FSMC, DAC, SDIO, Ethernet (internal 10BASE-T PHY), RNG

---

## CH32V407

| Variant | Flash | RAM | Notes |
|---------|-------|-----|-------|
| CH32V407VCT6 | 256KB | 64KB | LQFP100 |
| CH32V407RCT6 | 256KB | 64KB | LQFP64 |
| CH32V407RGT6 | 1MB | 64KB | LQFP64 |
| CH32V407VGT6 | 1MB | 64KB | LQFP100 |

**Linker Script (256KB variant)**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

**Special Features**: FPU, FSMC, DAC, SDIO, Ethernet, LTDC, ARGB, DVP, USB-HS

---

## Flash Page Size

| Family | Page Size | Erase Unit |
|--------|-----------|------------|
| CH32V103 | 1KB | 1 page |
| CH32V20x | 4KB | 1 page |
| CH32V307 | 4KB | 1 page |
| CH32V407 | 4KB | 1 page |

**Note**: Flash can only be written in half-word (16-bit) or word (32-bit) units. Always erase before writing.

---

## IAP Memory Layout

For In-Application Programming with a bootloader:

```
0x00000000 +-------------------+
           | Bootloader (16KB) |
0x00004000 +-------------------+
           | Application       |
           | (Flash_size - 16K)|
           +-------------------+
```

**Bootloader Linker Script**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

**Application Linker Script**:
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 240K   /* 256K - 16K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

Application must relocate vector table:
```c
SCB->VTOR = 0x00004000;
```

---

## Stack and Heap

Default values from Link.ld (may vary by project):

| Parameter | Default | Location |
|-----------|---------|----------|
| Stack size | 2048 bytes | End of RAM |
| Heap size | 256 bytes | After .bss |

To change, edit the Link.ld file:
```
_Min_Heap_Size = 0x200;   /* 512 bytes */
_Min_Stack_Size = 0x800;  /* 2048 bytes */
```

---

## Peripheral Register Base Addresses

| Peripheral | Base Address | Bus |
|------------|--------------|-----|
| GPIOA | 0x40010800 | APB2 |
| GPIOB | 0x40010C00 | APB2 |
| GPIOC | 0x40011000 | APB2 |
| GPIOD | 0x40011400 | APB2 |
| USART1 | 0x40013800 | APB2 |
| USART2 | 0x40004400 | APB1 |
| USART3 | 0x40004800 | APB1 |
| SPI1 | 0x40013000 | APB2 |
| SPI2 | 0x40003800 | APB1 |
| I2C1 | 0x40005400 | APB1 |
| I2C2 | 0x40005800 | APB1 |
| TIM1 | 0x40012C00 | APB2 |
| TIM2 | 0x40000000 | APB1 |
| TIM3 | 0x40000400 | APB1 |
| TIM4 | 0x40000800 | APB1 |
| ADC1 | 0x40012400 | APB2 |
| CAN1 | 0x40006400 | APB1 |
| DMA1 | 0x40020000 | AHB |
| DMA2 | 0x40020400 | AHB |
| ETH | 0x40028000 | AHB |
| USBFS | 0x40005C00 | APB1 |
| USBHS | 0x40040000 | AHB (V407) |
| FSMC | 0xA0000000 | AHB (V307/V407) |
| DAC | 0x40007400 | APB1 (V307/V407) |
