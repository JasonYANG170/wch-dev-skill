# Recipe: Flash and EEPROM Storage

## Overview

Read, write, and erase Flash ROM and Data-Flash (EEPROM) on CH561/CH563 for
persistent data storage. Flash programming uses the ISPXT56X pre-compiled
library.

## Memory Layout

```
Flash ROM (Code):
0x00000000 +-----------------------+
           | IAP Bootloader        |  4KB (optional)
0x00001000 +-----------------------+
           | Application Code      |
           |         ...           |
0x00038000 +-----------------------+
           | End of Code Flash     |  224KB total
0x00038000 +-----------------------+

Data-Flash (EEPROM):
0x00038000 +-----------------------+
           | Data Storage          |  28KB (byte-writable)
0x0003F000 +-----------------------+
```

## Key Differences from CH569

- **Flash erase unit**: 4KB (not 256 bytes)
- **Data-Flash (EEPROM)**: Separate 28KB area, byte-writable
- **Flash write**: Dword (4-byte) minimum, page (256 bytes) recommended
- **Library**: ISPXT56X.O pre-compiled library (not inline functions)

## Flash ROM Operations

### Read (Memory-Mapped)

```c
// Flash is memory-mapped, read directly
UINT32 *pFlash = (UINT32 *)0x00001000;
UINT32 value = *pFlash;

// Read a buffer
UINT8 buf[256];
memcpy(buf, (void *)0x00001000, 256);
```

### Erase (4KB blocks)

```c
// Erase one 4KB block
UINT8 status = FLASH_ROM_ERASE(0x00030000, 4096);
if (status == 0) {
    // Erase successful
}

// Erase multiple blocks
FLASH_ROM_ERASE(0x00030000, 4096 * 4);  // 16KB
```

### Write (4-byte minimum)

```c
// Write data (must erase first)
UINT32 data[4] = {0x12345678, 0xABCDEF00, 0x11223344, 0x55667788};

UINT8 status = FLASH_ROM_WRITE(0x00030000, data, 16);
if (status == 0) {
    // Write successful
}

// Write must be to erased (all 0xFF) locations
// Minimum write size: 4 bytes (dword)
// Recommended: 256 bytes (page) for best performance
```

### Verify

```c
// Verify written data
UINT32 result = FLASH_ROM_VERIFY(0x00030000, data, 16);
if (result == 0) {
    // Verify successful
}
```

## Data-Flash (EEPROM) Operations

### Read

```c
UINT8 buf[256];
EEPROM_READ(0x0000, buf, 256);  // Read 256 bytes from EEPROM offset 0
```

### Erase (4KB blocks)

```c
UINT8 status = EEPROM_ERASE(0x0000, 4096);  // Erase 4KB block
```

### Write (byte-writable)

```c
UINT8 data[] = {0x01, 0x02, 0x03, 0x04};
UINT8 status = EEPROM_WRITE(0x0000, data, 4);

// EEPROM supports byte-level writes (no need to erase first for new writes)
// But erase is needed to change 0 bits back to 1
```

## Complete Example: Boot Counter

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define STORAGE_ADDR  0x00030000    // Flash data storage area
#define SECTOR_SIZE   4096          // 4KB erase block

void mInitSTDIO(void)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;
    x = (x + 5) / 10;
    R8_UART0_LCR = 0x80;
    R8_UART0_DIV = 1;
    R8_UART0_DLM = x >> 8;
    R8_UART0_DLL = x & 0xff;
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= RXD0 | TXD0;
    R32_PB_PU |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f) { R8_UART0_THR = c; while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0); return c; }
__irq void IRQ_Handler(void) { while(1); }
__irq void FIQ_Handler(void) { while(1); }

void flash_store_counter(UINT32 counter)
{
    // Read existing data
    UINT8 sector_buf[SECTOR_SIZE];
    memcpy(sector_buf, (void *)STORAGE_ADDR, SECTOR_SIZE);

    // Modify counter field
    memcpy(sector_buf, &counter, sizeof(UINT32));

    // Erase block
    FLASH_ROM_ERASE(STORAGE_ADDR, SECTOR_SIZE);

    // Write back
    FLASH_ROM_WRITE(STORAGE_ADDR, sector_buf, SECTOR_SIZE);
}

UINT32 flash_read_counter(void)
{
    UINT32 counter;
    memcpy(&counter, (void *)STORAGE_ADDR, sizeof(UINT32));
    if (counter == 0xFFFFFFFF) return 0;  // Erased state
    return counter;
}

int main(void)
{
    mInitSTDIO();

    UINT32 boot_count = flash_read_counter();
    PRINT("Boot count: %d\n", boot_count);

    boot_count++;
    flash_store_counter(boot_count);

    PRINT("New boot count: %d\n", boot_count);

    while(1);
}
```

## Data-Flash Example

```c
// Use EEPROM for frequently updated data (byte-writable)
UINT32 counter = 0;
EEPROM_READ(0x0000, &counter, sizeof(UINT32));
if (counter == 0xFFFFFFFF) counter = 0;

counter++;

// Erase and write (EEPROM also needs erase for 0->1 bit changes)
EEPROM_ERASE(0x0000, 256);
EEPROM_WRITE(0x0000, &counter, sizeof(UINT32));
```

## Lock/Unlock Flash

```c
// Lock Flash (protect from writes)
FLASH_ROM_LOCK(0x50);   // Lock 480KB
FLASH_ROM_LOCK(0x70);   // Lock 32KB
FLASH_ROM_LOCK(0x2C);   // Lock 256KB
FLASH_ROM_LOCK(0x3C);   // Lock all
FLASH_ROM_LOCK(0x00);   // Unlock
```

## Notes

- Flash erase unit is 4KB (4096 bytes), not 256 bytes
- Flash write minimum is 4 bytes (dword)
- Data-Flash (EEPROM) is 28KB, byte-writable
- Never write to Flash while executing from Flash
- Always verify writes by reading back
- Use Data-Flash for frequently updated data (byte-level writes)
