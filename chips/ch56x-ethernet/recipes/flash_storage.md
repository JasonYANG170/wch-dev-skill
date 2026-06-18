# Recipe: Flash Storage

## Overview

Read, write, and erase Flash memory for persistent data storage on CH56x chips.
Flash is also used for IAP (In-Application Programming) firmware updates.

## Flash Layout

```
0x00000000 +-----------------------+
           | Bootloader (IAP)      |  4KB (0x1000 bytes)
0x00001000 +-----------------------+
           | Application Code      |
           |         ...           |
0x00070000 +-----------------------+
           | Data Storage Area     |  Last 64KB (typical)
0x00080000 +-----------------------+  End of 512KB Flash
```

## Key Constraints

- **Sector size**: 256 bytes (minimum erase unit)
- **Write unit**: 4 bytes (32-bit word) at a time
- **Write requires erase first**: Flash bits can only be set 1->0 by writing;
  to set 0->1 you must erase the entire 256-byte sector
- **Read-modify-write**: For partial sector updates, read sector, modify, erase, write back

## Flash Read

```c
// Flash is memory-mapped, just read directly
UINT32 *pFlash = (UINT32 *)0x00070000;  // Read from data area
UINT32 value = *pFlash;

// Read a buffer
UINT8 buf[256];
memcpy(buf, (void *)0x00070000, 256);
```

## Flash Erase

```c
// Erase a 256-byte sector
// Address must be 256-byte aligned
FLASH_ROM_ERASE(0x00070000, 256);

// Erase multiple sectors
FLASH_ROM_ERASE(0x00070000, 256 * 4);  // Erase 4 sectors = 1024 bytes
```

## Flash Write

```c
// Write data to Flash (must erase sector first)
UINT32 data[4] = {0x12345678, 0xABCDEF00, 0x11223344, 0x55667788};

// Write 16 bytes (4 x 32-bit words)
FLASH_ROM_WRITE(0x00070000, data, 16);

// Write must be to erased (all 0xFF) locations
// Write granularity: 4 bytes
```

## Complete Read-Modify-Write Example

```c
#include "CH56x_common.h"

#define STORAGE_ADDR  0x00070000
#define SECTOR_SIZE   256

void flash_store_counter(UINT32 counter)
{
    // Read current sector content
    UINT8 sector_buf[SECTOR_SIZE];
    memcpy(sector_buf, (void *)STORAGE_ADDR, SECTOR_SIZE);

    // Modify the counter field (at offset 0)
    memcpy(sector_buf, &counter, sizeof(UINT32));

    // Erase the sector
    FLASH_ROM_ERASE(STORAGE_ADDR, SECTOR_SIZE);

    // Write back modified data
    FLASH_ROM_WRITE(STORAGE_ADDR, sector_buf, SECTOR_SIZE);
}

UINT32 flash_read_counter(void)
{
    UINT32 counter;
    memcpy(&counter, (void *)STORAGE_ADDR, sizeof(UINT32));
    if (counter == 0xFFFFFFFF) {
        return 0;  // Flash is erased (first boot)
    }
    return counter;
}

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    UINT32 boot_count = flash_read_counter();
    printf("Boot count: %d\n", boot_count);

    boot_count++;
    flash_store_counter(boot_count);

    printf("New boot count: %d\n", boot_count);

    while(1) {}
}
```

## Wear Leveling

Flash has limited erase cycles (~100,000). For frequently updated data,
use wear leveling:

```c
// Simple wear leveling: rotate through multiple sectors
#define NUM_SECTORS    16
#define BASE_ADDR      0x00070000
#define SECTOR_SIZE    256

static UINT8 current_sector = 0;

void store_with_wear_leveling(UINT32 value)
{
    UINT32 addr = BASE_ADDR + (current_sector * SECTOR_SIZE);

    // Erase and write to current sector
    FLASH_ROM_ERASE(addr, SECTOR_SIZE);
    FLASH_ROM_WRITE(addr, &value, sizeof(UINT32));

    // Advance to next sector
    current_sector = (current_sector + 1) % NUM_SECTORS;
}
```

## Notes

- Never write to Flash while executing from Flash (use RAMX for code if needed)
- Flash operations are slow (~ms for erase); avoid in time-critical code paths
- Always verify writes by reading back
- For IAP updates, see the `iap_ota` recipe
