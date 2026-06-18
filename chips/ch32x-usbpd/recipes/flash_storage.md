# Flash Storage

> **Summary**: Read, write, and erase on-chip Flash memory on CH32X/CH6xx chips.

## Trigger Intent

- "Flash read write"
- "Store data to Flash"
- "Flash erase"
- "Option bytes"
- "Fast program"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Reference | `CH32X035EVT/EVT/EXAM/FLASH/` |

## Step-by-Step

### Standard Flash Page Write

```c
#include "ch32x035.h"
#include "debug.h"

// Flash page size = 256 bytes
#define FLASH_PAGE_SIZE  256

void Flash_WritePage(uint32_t page_addr, uint32_t *data) {
    FLASH_Unlock();

    // Erase page first (required before write)
    FLASH_Status status = FLASH_ErasePage(page_addr);
    if(status != FLASH_COMPLETE) {
        printf("Erase failed: %d\r\n", status);
        FLASH_Lock();
        return;
    }

    // Program word by word (4 bytes each)
    for(int i = 0; i < FLASH_PAGE_SIZE / 4; i++) {
        status = FLASH_ProgramWord(page_addr + i * 4, data[i]);
        if(status != FLASH_COMPLETE) {
            printf("Program failed at offset %d\r\n", i);
            break;
        }
    }

    FLASH_Lock();
}

// Read Flash (direct memory access, no unlock needed)
void Flash_Read(uint32_t addr, uint32_t *buf, uint32_t word_count) {
    uint32_t *src = (uint32_t *)addr;
    for(uint32_t i = 0; i < word_count; i++) {
        buf[i] = src[i];
    }
}
```

### Fast Flash Program (Buffer-based)

```c
// Fast program writes a full page using internal buffer
void Flash_FastProgramPage(uint32_t page_addr, uint32_t *data) {
    FLASH_Unlock();
    FLASH_Unlock_Fast();

    // Reset internal buffer
    FLASH_BufReset();

    // Load 64 words (256 bytes) into buffer
    for(int i = 0; i < 64; i++) {
        FLASH_BufLoad(page_addr + i * 4, data[i]);
    }

    // Erase and program page in one operation
    FLASH_ErasePage_Fast(page_addr);
    FLASH_ProgramPage_Fast(page_addr);

    FLASH_Lock_Fast();
    FLASH_Lock();
}
```

### Read-Modify-Write Pattern

```c
// Update a single byte without losing other data in the page
void Flash_UpdateByte(uint32_t page_addr, uint16_t offset, uint8_t value) {
    uint8_t buf[FLASH_PAGE_SIZE];

    // Read existing page
    Flash_Read(page_addr, (uint32_t *)buf, FLASH_PAGE_SIZE / 4);

    // Modify target byte
    buf[offset] = value;

    // Erase and rewrite
    FLASH_Unlock();
    FLASH_ErasePage(page_addr);
    for(int i = 0; i < FLASH_PAGE_SIZE / 4; i++) {
        FLASH_ProgramWord(page_addr + i * 4, ((uint32_t *)buf)[i]);
    }
    FLASH_Lock();
}
```

### Option Byte Configuration

```c
void Flash_ConfigOptionBytes(void) {
    FLASH_Unlock();

    // Erase option bytes
    FLASH_EraseOptionBytes();

    // Configure: software IWDG, no reset on STOP, no reset on STANDBY
    FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_STOP_NoRST, OB_STDBY_NoRST, OB_RST_EN_DT1ms);

    FLASH_Lock();

    // Option bytes take effect after reset
}
```

### System Reset Start Mode

```c
// Switch between user application and bootloader
void JumpToBootloader(void) {
    SystemReset_StartMode(Start_Mode_BOOT);  // Boot mode
    NVIC_SystemReset();                       // Trigger reset
}

void JumpToApplication(void) {
    SystemReset_StartMode(Start_Mode_USER);  // User mode
    NVIC_SystemReset();
}
```

## Flash Memory Map

| Region | Start | Size | Description |
|--------|-------|------|-------------|
| Boot/System | 0x00000000 | 4KB | System bootloader (ROM, not writable) |
| Application | 0x00001000 | Varies | User code |
| End of Flash | Chip dependent | -- | Last page |

### CH32X035: 62KB total, last page at 0x0000F800
### CH32X315/CH643: 192KB total, last page at 0x0002F800
### CH641: 16KB total, last page at 0x00003800

## Flash Status Reference

| Status | Value | Description |
|--------|-------|-------------|
| `FLASH_BUSY` | 1 | Flash operation in progress |
| `FLASH_ERROR_PG` | 2 | Programming error |
| `FLASH_ERROR_WRP` | 3 | Write protection error |
| `FLASH_COMPLETE` | 4 | Operation successful |
| `FLASH_TIMEOUT` | 5 | Operation timeout |
| `FLASH_RDP` | 6 | Read protection error |
| `FLASH_OP_RANGE_ERROR` | 0xFD | Operation range error |
| `FLASH_ALIGN_ERROR` | 0xFE | Alignment error |
| `FLASH_ADR_RANGE_ERROR` | 0xFF | Address range error |

## Common Errors

- Forgetting `FLASH_Unlock()` before erase/program -- operation silently fails
- Writing to Flash without erasing first -- data corruption (Flash can only clear bits, not set them)
- Not checking `FLASH_ProgramWord()` return value -- silent write failures
- Writing to bootloader area (0x0000-0x0FFF) -- bricks the chip
- Using `FLASH_ROM_WRITE()` (CH57x API) -- these chips use `FLASH_ProgramWord()` / `FLASH_ProgramPage_Fast()`
