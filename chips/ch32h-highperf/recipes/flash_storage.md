# Flash Storage

## Overview

CH32H417 has 480KB or 960KB internal Flash (configurable). Flash is organized in 4KB pages. Writing requires erasing entire pages first (read-modify-write pattern for partial updates).

## Key API Functions

```c
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_EraseBlock_Fast(uint32_t Block_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t* pbuf);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
FLASHCapacity FLASH_GetCapacity(void);
void FLASH_Access_Clock_Cfg(uint32_t FLASH_Access_CLK);
```

## Flash Status Enum

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

## Basic Read/Write Example

```c
#include "ch32h417.h"

// Write a 32-bit word to flash
FLASH_Status Flash_WriteWord(uint32_t addr, uint32_t data)
{
    FLASH_Status status;

    FLASH_Unlock();

    // Erase page containing the address
    uint32_t page_addr = addr & ~0xFFF;  // 4KB page alignment
    status = FLASH_ErasePage(page_addr);
    if(status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return status;
    }

    // Program word
    status = FLASH_ProgramWord(addr, data);

    FLASH_Lock();
    return status;
}

// Read 32-bit word from flash
uint32_t Flash_ReadWord(uint32_t addr)
{
    return *(__IO uint32_t*)addr;
}
```

## Read-Modify-Write Pattern

```c
// Update a single word within a page without losing other data
FLASH_Status Flash_UpdateWord(uint32_t addr, uint32_t new_data)
{
    uint32_t page_addr = addr & ~0xFFF;
    uint32_t page_buf[1024];  // 4KB buffer (1024 words)

    // Read entire page
    for(int i = 0; i < 1024; i++)
    {
        page_buf[i] = *(__IO uint32_t*)(page_addr + i * 4);
    }

    // Modify target word
    uint32_t offset = (addr - page_addr) / 4;
    page_buf[offset] = new_data;

    FLASH_Unlock();

    // Erase page
    FLASH_Status status = FLASH_ErasePage(page_addr);
    if(status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return status;
    }

    // Write back entire page
    for(int i = 0; i < 1024; i++)
    {
        status = FLASH_ProgramWord(page_addr + i * 4, page_buf[i]);
        if(status != FLASH_COMPLETE) break;
    }

    FLASH_Lock();
    return status;
}
```

## Fast Programming

For bulk writes, use fast programming (writes full page at once):

```c
FLASH_Status Flash_WritePage_Fast(uint32_t page_addr, uint32_t *data)
{
    FLASH_Unlock_Fast();

    FLASH_EraseBlock_Fast(page_addr);
    FLASH_ProgramPage_Fast(page_addr, data);

    FLASH_Lock_Fast();
    return FLASH_COMPLETE;
}
```

## Flash ROM API (Higher Level)

```c
// Erase a range of flash
FLASH_Status Flash_EraseRange(uint32_t start_addr, uint32_t length)
{
    return FLASH_ROM_ERASE(start_addr, length);
}

// Write data to flash (handles alignment)
FLASH_Status Flash_WriteRange(uint32_t start_addr, uint32_t *data, uint32_t length)
{
    return FLASH_ROM_WRITE(start_addr, data, length);
}
```

## Flash Capacity

```c
FLASHCapacity cap = FLASH_GetCapacity();
if(cap == FLASHCapacity_480K)
{
    // 480KB flash
}
else if(cap == FLASHCapacity_960K)
{
    // 960KB flash
}
```

## Flash Access Clock Configuration

```c
// Configure flash access clock (important for high system clocks)
FLASH_Access_Clock_Cfg(FLASH_CLK_HCLKDIV2);
```

| Macro | Description |
|-------|-------------|
| `FLASH_CLK_HCLKDIV1` | HCLK/1 |
| `FLASH_CLK_HCLKDIV2` | HCLK/2 |
| `FLASH_CLK_HCLKDIV4` | HCLK/4 |
| `FLASH_CLK_HCLKDIV8` | HCLK/8 |

## Application Boot Mode

```c
// Switch between user application and bootloader
SystemReset_StartMode(Start_Mode_BOOT);  // Enter bootloader on reset
SystemReset_StartMode(Start_Mode_USER);  // Enter user app on reset

// Check current boot mode
uint32_t mode = FLASH_BOOT_GetMode();
```

## Option Bytes

```c
// Configure option bytes
FLASH_Unlock();
FLASH_OB_Unlock();

// Enable USB download in bootloader
FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_USBFSDL_EN, OB_USARTDL_EN);

FLASH_OB_Lock();
FLASH_Lock();
```
