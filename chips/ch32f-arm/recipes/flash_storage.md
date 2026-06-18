# Recipe: Flash Storage

## Overview

Read, write, and erase internal Flash memory on CH32F10x/CH32F20x.

## Key API Functions

```c
// Lock/Unlock
void FLASH_Unlock(void);
void FLASH_Lock(void);

// Erase
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);

// Program
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

// Fast programming (CH32F10x)
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_BufReset(void);
void FLASH_BufLoad(uint32_t Address, uint32_t Data0, uint32_t Data1, uint32_t Data2, uint32_t Data3);
void FLASH_ErasePage_Fast(uint32_t Page_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address);

// Status
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
```

## Flash Status Values

| Status | Description |
|--------|-------------|
| FLASH_BUSY | Flash is busy |
| FLASH_ERROR_PG | Programming error |
| FLASH_ERROR_WRP | Write protection error |
| FLASH_COMPLETE | Operation complete |
| FLASH_TIMEOUT | Operation timeout |

## Flash Layout

### CH32F10x
- **Page size**: 1KB (1024 bytes)
- **Total Flash**: 64KB (CH32F103C8T6) or 128KB (CH32F103CBT6)
- **Start address**: 0x08000000

### CH32F20x
- **Page size**: 256 bytes (fast mode) or 1KB (standard mode)
- **Total Flash**: 128KB to 256KB depending on variant
- **Start address**: 0x08000000

## Important Notes

- Flash must be erased before writing (erased state is 0xFF)
- Cannot write to Flash while executing from it (use RAM execution or different bank)
- Always check return status of Flash operations
- Interrupts should be disabled during Flash operations if code is in Flash

## Example: Store Configuration Data

```c
#include "debug.h"

#define CONFIG_FLASH_ADDR   0x0800FC00  // Last page of 64KB Flash
#define CONFIG_MAGIC        0x434F4E46  // "CONF"

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t baudrate;
    uint32_t checksum;
} ConfigData;

uint8_t Flash_SaveConfig(ConfigData* config)
{
    FLASH_Status status;
    uint32_t* src = (uint32_t*)config;
    uint32_t addr = CONFIG_FLASH_ADDR;

    // Calculate checksum
    config->checksum = config->magic ^ config->version ^ config->baudrate;

    // Unlock Flash
    FLASH_Unlock();

    // Erase page
    status = FLASH_ErasePage(CONFIG_FLASH_ADDR);
    if(status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return 1;
    }

    // Write data (word by word)
    for(uint32_t i = 0; i < sizeof(ConfigData) / 4; i++)
    {
        status = FLASH_ProgramWord(addr, src[i]);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return 2;
        }
        addr += 4;
    }

    // Lock Flash
    FLASH_Lock();
    return 0;
}

uint8_t Flash_LoadConfig(ConfigData* config)
{
    ConfigData* stored = (ConfigData*)CONFIG_FLASH_ADDR;

    // Check magic number
    if(stored->magic != CONFIG_MAGIC)
    {
        return 1;  // No valid config
    }

    // Verify checksum
    if(stored->checksum != (stored->magic ^ stored->version ^ stored->baudrate))
    {
        return 2;  // Checksum error
    }

    // Copy data
    *config = *stored;
    return 0;
}

int main(void)
{
    ConfigData config;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // Try to load config
    if(Flash_LoadConfig(&config) != 0)
    {
        printf("No valid config, creating default...\r\n");
        config.magic = CONFIG_MAGIC;
        config.version = 1;
        config.baudrate = 115200;
        Flash_SaveConfig(&config);
    }
    else
    {
        printf("Config loaded: baudrate=%d\r\n", config.baudrate);
    }

    while(1)
    {
        Delay_Ms(1000);
    }
}
```

## Example: Fast Page Write (CH32F10x)

```c
void Flash_FastWritePage(uint32_t pageAddr, uint32_t* data)
{
    FLASH_Unlock_Fast();
    FLASH_BufReset();

    // Load 16 words (64 bytes) into buffer
    for(uint32_t i = 0; i < 16; i += 4)
    {
        FLASH_BufLoad(pageAddr + i * 4, data[i], data[i+1], data[i+2], data[i+3]);
    }

    // Erase and program page
    FLASH_ErasePage_Fast(pageAddr);
    FLASH_ProgramPage_Fast(pageAddr);

    FLASH_Lock_Fast();
}
```
