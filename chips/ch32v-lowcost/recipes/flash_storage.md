# Recipe: Flash Storage

## Scenario
Read, write, erase Flash memory for data storage and option byte configuration.

## API Quick Reference

### Flash Status
```c
typedef enum
{
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
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data);
FLASH_Status FLASH_EraseOptionBytes(void);
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STDBY, uint16_t OB_RST, uint16_t OB_PowerON_Start_Mode);
void         FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);

// Fast programming (CH32V003)
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);

// System reset mode
void SystemReset_StartMode(uint32_t Mode);
```

### Page Sizes
| Chip | Page Size | Total Flash |
|------|-----------|-------------|
| CH32V003 | 64 bytes | 16KB |
| CH32V006 | 1024 bytes | 62KB |
| CH32L103 | 1024 bytes | 64KB |

### Flash Flags
```c
FLASH_FLAG_BSY       // Flash busy
FLASH_FLAG_EOP       // End of operation
FLASH_FLAG_WRPRTERR  // Write protection error
FLASH_FLAG_OPTERR    // Option byte error
```

### Option Byte Constants
```c
// IWDG
OB_IWDG_SW    // Software watchdog
OB_IWDG_HW    // Hardware watchdog

// STOP mode reset
OB_STOP_NoRST  // No reset on STOP
OB_STOP_RST    // Reset on STOP

// STDBY mode reset
OB_STDBY_NoRST // No reset on standby
OB_STDBY_RST   // Reset on standby

// Reset pin (PD7)
OB_RST_NoEN        // Disable reset pin
OB_RST_EN_DT12ms   // Enable with 12ms delay
OB_RST_EN_DT1ms    // Enable with 1ms delay
OB_RST_EN_DT128us  // Enable with 128us delay

// Power-on start mode
OB_PowerON_Start_Mode_BOOT  // Boot from bootloader
OB_PowerON_Start_Mode_USER  // Boot from user code
```

## Complete Call Chains

### Standard Erase and Write (CH32V003, 64-byte pages)
```c
void Flash_Write_Data(uint32_t addr, uint16_t *data, uint32_t count)
{
    FLASH_Status status;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);

    // Erase page(s)
    for(uint32_t i = 0; i < count; i += 32)  // 32 half-words = 64 bytes = 1 page
    {
        status = FLASH_ErasePage(addr + i);
        if(status != FLASH_COMPLETE)
        {
            printf("Erase error at 0x%08x\r\n", addr + i);
            FLASH_Lock();
            return;
        }
    }

    // Write data
    for(uint32_t i = 0; i < count; i++)
    {
        status = FLASH_ProgramHalfWord(addr + i * 2, data[i]);
        if(status != FLASH_COMPLETE)
        {
            printf("Write error at 0x%08x\r\n", addr + i * 2);
            FLASH_Lock();
            return;
        }
    }

    FLASH_Lock();
}

// Read (no unlock needed)
uint16_t Flash_Read_HalfWord(uint32_t addr)
{
    return *(__IO uint16_t *)addr;
}
```

### Fast Program (CH32V003)
```c
#define FADR  0x08003000
#define FSIZE (256 >> 2)  // 256 bytes = 64 words

void Flash_Fast_Write(void)
{
    u32 buf[FSIZE];
    FLASH_Status s;

    // Prepare data
    for(u32 i = 0; i < FSIZE; i++)
        buf[i] = i;

    // Erase
    s = FLASH_ROM_ERASE(FADR, FSIZE * 4);
    if(s != FLASH_COMPLETE)
    {
        printf("Fast erase error\r\n");
        return;
    }

    // Write
    s = FLASH_ROM_WRITE(FADR, buf, FSIZE * 4);
    if(s != FLASH_COMPLETE)
    {
        printf("Fast write error\r\n");
        return;
    }

    // Verify
    for(u32 i = 0; i < FSIZE; i++)
    {
        if(buf[i] != *(u32 *)(FADR + 4 * i))
        {
            printf("Verify fail at offset %d\r\n", i);
            return;
        }
    }
    printf("Fast write OK\r\n");
}
```

### Option Byte Configuration
```c
void Flash_Configure_Options(void)
{
    FLASH_Unlock();
    FLASH_EraseOptionBytes();
    FLASH_UserOptionByteConfig(
        OB_IWDG_SW,           // Software watchdog
        OB_STDBY_NoRST,       // No reset on standby
        OB_RST_EN_DT12ms,     // Enable reset pin with 12ms delay
        OB_PowerON_Start_Mode_USER  // Start from user code
    );
    FLASH_Lock();
}
```

### Store Configuration Data (Read-Modify-Write)
```c
typedef struct {
    uint16_t device_id;
    uint16_t baud_rate;
    uint32_t checksum;
} Config_t;

#define CONFIG_ADDR  0x08003F00  // Last page of 16K Flash

void Config_Save(Config_t *cfg)
{
    FLASH_Unlock();

    // Erase the page containing config
    FLASH_ErasePage(CONFIG_ADDR);

    // Write config structure
    uint16_t *p = (uint16_t *)cfg;
    for(int i = 0; i < sizeof(Config_t) / 2; i++)
    {
        FLASH_ProgramHalfWord(CONFIG_ADDR + i * 2, p[i]);
    }

    FLASH_Lock();
}

void Config_Load(Config_t *cfg)
{
    uint16_t *p = (uint16_t *)cfg;
    for(int i = 0; i < sizeof(Config_t) / 2; i++)
    {
        p[i] = *(__IO uint16_t *)(CONFIG_ADDR + i * 2);
    }
}
```

## Common Errors

1. **Forgetting FLASH_Unlock()** -- Write/erase will fail silently
2. **Forgetting FLASH_Lock()** -- Security risk, accidental writes possible
3. **Wrong page size** -- CH32V003 uses 64-byte pages, not 1KB
4. **Not erasing before write** -- Flash can only change 1->0, not 0->1
5. **Writing to bootloader area** -- IAP apps must not overwrite 0x0000-0x0FFF
6. **Missing flag clear** -- Always clear flags before starting operations
7. **Address alignment** -- Half-word writes must be 2-byte aligned, word writes 4-byte aligned
