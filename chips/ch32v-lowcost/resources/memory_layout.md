# Memory Layout Reference

## CH32V003 Memory Map

### Flash Memory
```
0x00000000 +------------------+
           | Boot Area        |  4KB (0x0000 - 0x0FFF) -- Used by bootloader/IAP
0x00000FFF +------------------+
           | User Application |  12KB (0x1000 - 0x3FFF) -- Default application start
0x00003FFF +------------------+
```

### RAM Memory
```
0x20000000 +------------------+
           | .data section    |  Initialized global/static variables
           +------------------+
           | .bss section     |  Zero-initialized variables
           +------------------+
           | Heap             |  Dynamic allocation (grows up)
           +------------------+
           |                  |
           | Stack            |  (grows down from top)
0x200007FF +------------------+
```

### Linker Script (CH32V003)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}
__stack_size = 256;
```

### Peripheral Registers
```
0x40000000 +------------------+
           | APB1 Peripherals |  TIM2, WWDG, I2C1, PWR
0x40010000 +------------------+
           | APB2 Peripherals |  AFIO, GPIOx, ADC1, TIM1, SPI1, USART1
0x40020000 +------------------+
           | AHB Peripherals  |  DMA1, SRAM, Flash interface
```

---

## CH32V006/007/M007 Memory Map

### Flash Memory
```
0x00000000 +------------------+
           | Boot Area        |  4KB (0x0000 - 0x0FFF)
0x00000FFF +------------------+
           | User Application |  58KB (0x1000 - 0xF7FF)
0x0000F7FF +------------------+
```

### RAM Memory
```
0x20000000 +------------------+
           | .data / .bss     |
           | Heap             |
           | Stack            |
0x20001FFF +------------------+
```

### Linker Script Variants
```
/* CH32V002 */
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 4K

/* CH32V004/005 */
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 32K
RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 6K

/* CH32V006/007/M007 */
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 62K
RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 8K
```

---

## CH32L103 Memory Map

### Flash Memory
```
0x00000000 +------------------+
           | Boot Area        |  4KB (0x0000 - 0x0FFF)
0x00000FFF +------------------+
           | User Application |  60KB (0x1000 - 0xFFFF)
0x0000FFFF +------------------+
```

### RAM Memory
```
0x20000000 +------------------+
           | .data / .bss     |
           | Heap             |
           | Stack            |
0x20004FFF +------------------+
```

### Linker Script
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
__stack_size = 512;
```

---

## Memory Size Summary

| Chip | Flash | RAM | Flash Page Size |
|------|-------|-----|-----------------|
| CH32V003 | 16KB | 2KB | 64 bytes |
| CH32V002 | 16KB | 4KB | 1024 bytes |
| CH32V004/005 | 32KB | 6KB | 1024 bytes |
| CH32V006/007/M007 | 62KB | 8KB | 1024 bytes |
| CH32L103 | 64KB | 20KB | 1024 bytes |

---

## Key Linker Symbols

| Symbol | Description |
|--------|-------------|
| `_sinit` | Start of init section (Flash) |
| `_einit` | End of init section |
| `_etext` | End of text/code section (Flash) |
| `_data_vma` | Start of .data section (RAM) |
| `_data_lma` | Load address of .data (Flash, for copy to RAM) |
| `_edata` | End of .data section |
| `_sbss` | Start of .bss section |
| `_ebss` | End of .bss section |
| `_end` | End of all sections (= _ebss) |
| `_susrstack` | Start of user stack |
| `_eusrstack` | End of user stack |
| `_heap_end` | End of heap (before stack) |

---

## Section Descriptions

### .init (Flash)
Startup code that runs before main(). Contains vector table and reset handler.

### .highcode (RAM on CH32V003, Flash on CH32V006)
Performance-critical code copied to RAM for faster execution. On CH32V003, this section is loaded from Flash to RAM at startup.

### .text (Flash)
Application code and read-only data (constants, string literals).

### .data (RAM, loaded from Flash)
Initialized global and static variables. Copied from Flash to RAM during startup.

### .bss (RAM)
Zero-initialized global and static variables. Cleared to zero during startup.

### Stack (RAM, top)
Grows downward. Used for local variables, function calls, interrupt context.

### Heap (RAM, after .bss)
Grows upward. Used by malloc()/free() if available.

---

## Flash Write Considerations

### Page Erase
- CH32V003: Minimum erase unit = 64 bytes
- CH32V006/L103: Minimum erase unit = 1024 bytes
- Erase sets all bits to 1 (0xFFFF)
- Write can only change 1 -> 0

### Read-Modify-Write Pattern
```c
// For partial page updates:
uint8_t buf[PAGE_SIZE];
memcpy(buf, (void*)page_addr, PAGE_SIZE);  // Read
buf[offset] = new_value;                     // Modify
FLASH_ErasePage(page_addr);                  // Erase
FLASH_ProgramWord(page_addr, *(uint32_t*)buf);  // Write (word by word)
```

### Safe Write Area
- Never write to bootloader area (0x0000 - 0x0FFF)
- Application code is in Flash, avoid erasing own code
- Use upper Flash pages for data storage

---

## IAP Memory Layout

### Bootloader (4K)
```
0x00000000 +------------------+
           | Vector table     |
           | UART handler     |
           | Flash write      |
           | Jump logic       |
0x00000FFF +------------------+
```

### Application (remaining Flash)
```
0x00001000 +------------------+
           | Vector table     |  Must be remapped
           | Application code |
           | Config data      |  (optional, at end)
0x00003FFF +------------------+
```

### Vector Table Remapping
In application code, set vector table offset:
```c
NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x1000);
```
This tells the interrupt controller to look for interrupt vectors at 0x1000 instead of 0x0000.
