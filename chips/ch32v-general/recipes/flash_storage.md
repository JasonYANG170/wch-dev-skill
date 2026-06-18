# Recipe: Flash Storage

## When to Use
User wants to read/write internal Flash memory for data storage, configuration saving, or firmware update.

## API Reference (from ch32v20x_flash.h)

### Flash Status
```c
typedef enum {
    FLASH_BUSY = 1,
    FLASH_ERROR_PG,
    FLASH_ERROR_WRP,
    FLASH_COMPLETE,
    FLASH_TIMEOUT,
    FLASH_OP_RANGE_ERROR = 0xFD,
    FLASH_ALIGN_ERROR = 0xFE,
    FLASH_ADR_RANGE_ERROR = 0xFF,
} FLASH_Status;
```

### Key Functions
```c
void         FLASH_Unlock(void);
void         FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);

// Fast programming (CH32V20x D8/D8W)
void         FLASH_Unlock_Fast(void);
void         FLASH_Lock_Fast(void);
void         FLASH_ErasePage_Fast(uint32_t Page_Address);
void         FLASH_EraseBlock_32K_Fast(uint32_t Block_Address);
void         FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t *pbuf);

// ROM operations (all families)
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

### Flash Page Size
- CH32V103/CH32V20x: 4KB per page (1KB for some variants)
- CH32V307/CH32V407: 4KB per page

### Flash Flags
| Flag | Description |
|------|-------------|
| `FLASH_FLAG_BSY` | Flash busy |
| `FLASH_FLAG_EOP` | End of operation |
| `FLASH_FLAG_WRPRTERR` | Write protection error |
| `FLASH_FLAG_OPTERR` | Option byte error |

## Example: Read/Write Halfword to Flash

```c
#include "ch32v20x.h"

// Use last page of Flash for data storage
// For 64K flash: last page starts at 0x0800F000
#define FLASH_DATA_ADDR  0x0800F000

FLASH_Status Flash_WriteHalfWord(uint32_t addr, uint16_t data)
{
    FLASH_Status status;

    FLASH_Unlock();

    // Erase page first (required before write)
    status = FLASH_ErasePage(addr & 0xFFFFF000);  // Align to page boundary
    if(status != FLASH_COMPLETE) {
        FLASH_Lock();
        return status;
    }

    // Program halfword
    status = FLASH_ProgramHalfWord(addr, data);

    FLASH_Lock();
    return status;
}

uint16_t Flash_ReadHalfWord(uint32_t addr)
{
    return *(volatile uint16_t *)addr;
}
```

## Example: Read-Modify-Write for Partial Page Update

```c
// Flash must be erased before writing, but erase destroys entire page
// Use read-modify-write to preserve other data in the same page

FLASH_Status Flash_UpdateData(uint32_t addr, uint16_t *data, uint16_t count)
{
    FLASH_Status status;
    uint32_t page_addr = addr & 0xFFFFF000;
    uint16_t page_buf[2048];  // 4KB page / 2 bytes per halfword

    // Step 1: Read entire page
    for(int i = 0; i < 2048; i++) {
        page_buf[i] = *(volatile uint16_t *)(page_addr + i * 2);
    }

    // Step 2: Modify the data in buffer
    uint16_t offset = (addr - page_addr) / 2;
    for(int i = 0; i < count; i++) {
        page_buf[offset + i] = data[i];
    }

    // Step 3: Erase page
    FLASH_Unlock();
    status = FLASH_ErasePage(page_addr);
    if(status != FLASH_COMPLETE) {
        FLASH_Lock();
        return status;
    }

    // Step 4: Write back entire page
    for(int i = 0; i < 2048; i++) {
        status = FLASH_ProgramHalfWord(page_addr + i * 2, page_buf[i]);
        if(status != FLASH_COMPLETE) break;
    }

    FLASH_Lock();
    return status;
}
```

## Example: Store Configuration Struct

```c
typedef struct {
    uint32_t magic;        // 0xDEADBEEF if valid
    uint32_t version;
    uint16_t baudrate;
    uint16_t device_id;
    uint8_t  ip_addr[4];
    uint8_t  reserved[48]; // Pad to 64 bytes
} Config_t;

#define CONFIG_FLASH_ADDR  0x0800F000
#define CONFIG_MAGIC       0xDEADBEEF

void Config_Save(Config_t *cfg)
{
    cfg->magic = CONFIG_MAGIC;
    FLASH_ROM_ERASE(CONFIG_FLASH_ADDR, sizeof(Config_t));
    FLASH_ROM_WRITE(CONFIG_FLASH_ADDR, (uint32_t *)cfg, sizeof(Config_t));
}

int Config_Load(Config_t *cfg)
{
    Config_t *flash_cfg = (Config_t *)CONFIG_FLASH_ADDR;
    if(flash_cfg->magic == CONFIG_MAGIC) {
        *cfg = *flash_cfg;
        return 0;  // Success
    }
    return -1;  // No valid config
}
```

## Pitfalls
- **Must erase before write** -- Flash bits can only be cleared (1->0), not set (0->1)
- **Erase granularity is 4KB page** -- cannot erase individual bytes or halfwords
- **Write alignment** -- `FLASH_ProgramWord` requires 4-byte alignment, `FLASH_ProgramHalfWord` requires 2-byte alignment
- **Cannot execute code from Flash while erasing/writing** -- use RAM-resident functions or disable interrupts
- **FLASH_ROM_WRITE/ROM_ERASE handle unlock/lock internally** -- don't double unlock
