# DataFlash Storage

> **Summary**: Read and write DataFlash on CH5xx 8051 chips for non-volatile data storage -- unlock sequence, byte programming, sector erase.

## Trigger Intent

- "DataFlash read/write"
- "Store data in flash"
- "Non-volatile storage"
- "Flash programming"
- "EEPROM emulation"

## Flash Memory Map

| Chip | Code Flash | DataFlash Address | Boot Loader |
|---|---|---|---|
| CH552 | 16KB (0x0000-0x3FFF) | 0xC000 (512 bytes) | 0x3800 |
| CH551 | 10KB | 0xC000 (256 bytes) | 0x3800 |
| CH554 | 14KB (0x0000-0x37FF) | 0xC000 (512 bytes) | 0x3800 |
| CH555 | 64KB | 0xC000 | -- |
| CH559 | 64KB | 0xC000 | 0xF000 |
| CH543 | 14KB | 0xC000 | -- |
| CH549 | 16KB | 0xC000 | -- |

## Flash Registers

| Register | Address | Purpose |
|---|---|---|
| ROM_ADDR_L | 0x84 | Address low byte |
| ROM_ADDR_H | 0x85 | Address high byte |
| ROM_DATA_L | 0x8E | Data byte for write/read |
| ROM_DATA_H | 0x8F | Data high byte (code flash word write) |
| ROM_CTRL | 0x86 | Command (write) / Status (read) |
| SAFE_MOD | 0xA1 | Safe mode entry |
| GLOBAL_CFG | 0xB1 | Write enable bits |

### Key Constants

| Name | Value | Purpose |
|---|---|---|
| ROM_CMD_WRITE | 0x9A | Flash/DataFlash write command |
| ROM_CMD_READ | 0x8E | DataFlash read command |
| bROM_ADDR_OK | 0x40 | (RO) Address valid flag |
| bROM_CMD_ERR | 0x02 | (RO) Command error flag |
| bCODE_WE | 0x08 | Enable code Flash write |
| bDATA_WE | 0x04 | Enable DataFlash write |
| DATA_FLASH_ADDR | 0xC000 | DataFlash start address |

## Key Rules

1. **DataFlash is byte-addressable** -- Code flash is word-addressable (2 bytes at a time)
2. **DataFlash addresses must be even** -- ROM_ADDR_L must be even
3. **Must enter safe mode to modify GLOBAL_CFG** -- 0x55, 0xAA sequence
4. **Disable interrupts during Flash operations** -- EA=0 before, restore after
5. **DataFlash does not need sector erase** -- Can overwrite bytes directly
6. **Code Flash erases in 512-byte sectors** -- Must erase before writing

## Step-by-Step

### 1. Flash Unlock (Enable Write)

```c
UINT8 Flash_Op_Check_Byte1 = 0x00;
UINT8 Flash_Op_Check_Byte2 = 0x00;

UINT8 Flash_Op_Unlock(UINT8 flash_type)
{
    bit ea_sts;

    // Safety check
    if (Flash_Op_Check_Byte1 != 0x5A || Flash_Op_Check_Byte2 != 0xA5)
        return 0xFF;

    ea_sts = EA;
    EA = 0;                    // Disable interrupts

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= flash_type; // bCODE_WE or bDATA_WE
    SAFE_MOD = 0x00;

    EA = ea_sts;               // Restore interrupts
    return 0x00;
}
```

### 2. Flash Lock (Disable Write)

```c
void Flash_Op_Lock(UINT8 flash_type)
{
    bit ea_sts;

    ea_sts = EA;
    EA = 0;

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~flash_type;
    SAFE_MOD = 0x00;

    EA = ea_sts;
}
```

### 3. Read DataFlash Byte

```c
UINT8 ReadDataFlash(UINT8 addr)
{
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;  // 0xC0
    ROM_ADDR_L = addr << 1;             // Even address (byte addr * 2)
    ROM_CTRL = ROM_CMD_READ;
    return ROM_DATA_L;
}
```

### 4. Read DataFlash Block

```c
UINT8 ReadDataFlashBlock(UINT8 addr, PUINT8 buf, UINT8 len)
{
    UINT8 i;
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    for (i = 0; i < len; i++) {
        ROM_ADDR_L = (addr + i) << 1;   // Even addresses
        ROM_CTRL = ROM_CMD_READ;
        buf[i] = ROM_DATA_L;
    }
    return i;
}
```

### 5. Write DataFlash Byte

```c
UINT8 WriteDataFlash(UINT8 addr, UINT8 dat)
{
    if (Flash_Op_Unlock(bDATA_WE))
        return 0xFF;

    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = addr << 1;             // Even address
    ROM_DATA_L = dat;

    if (ROM_STATUS & bROM_ADDR_OK) {    // Address valid
        ROM_CTRL = ROM_CMD_WRITE;        // Execute write
    }

    if (ROM_STATUS & bROM_CMD_ERR)
        return 0x02;                     // Command error

    Flash_Op_Lock(bDATA_WE);
    return 0x00;
}
```

### 6. Write DataFlash Block

```c
UINT8 WriteDataFlashBlock(UINT8 addr, PUINT8 buf, UINT8 len)
{
    UINT8 i;

    if (Flash_Op_Unlock(bDATA_WE))
        return 0xFF;

    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    for (i = 0; i < len; i++) {
        ROM_ADDR_L = (addr + i) << 1;
        ROM_DATA_L = buf[i];
        if (ROM_STATUS & bROM_ADDR_OK) {
            ROM_CTRL = ROM_CMD_WRITE;
        }
        if (ROM_STATUS & bROM_CMD_ERR)
            break;
    }

    Flash_Op_Lock(bDATA_WE);
    return i;
}
```

### 7. Full Example: Store Config

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

#define CFG_MAGIC     0xA5
#define CFG_ADDR      0x00       // DataFlash byte address

typedef struct {
    UINT8 magic;
    UINT8 mode;
    UINT16 threshold;
} Config;

void SaveConfig(Config *cfg)
{
    WriteDataFlashBlock(CFG_ADDR, (PUINT8)cfg, sizeof(Config));
}

void LoadConfig(Config *cfg)
{
    ReadDataFlashBlock(CFG_ADDR, (PUINT8)cfg, sizeof(Config));
    if (cfg->magic != CFG_MAGIC) {
        // First run: set defaults
        cfg->magic = CFG_MAGIC;
        cfg->mode = 0;
        cfg->threshold = 100;
        SaveConfig(cfg);
    }
}

void main()
{
    Config cfg;
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();

    LoadConfig(&cfg);
    printf("Mode=%d, Threshold=%d\n", (UINT16)cfg.mode, cfg.threshold);

    cfg.mode = 1;
    cfg.threshold = 200;
    SaveConfig(&cfg);
    printf("Config saved\n");

    while(1);
}
```

## Code Flash Write (Advanced)

Code flash is word-addressable and requires sector erase before writing:

```c
// Erase 512-byte code flash sector at address (must be 512-byte aligned)
void EraseCodeSector(UINT16 sectorAddr)
{
    bit ea_sts = EA;
    EA = 0;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bCODE_WE;
    SAFE_MOD = 0x00;

    ROM_ADDR = sectorAddr & 0xFE00;  // Align to 512 bytes
    ROM_DATA_L = 0xFF;
    ROM_DATA_H = 0xFF;
    // Write all 0xFF to trigger sector erase
    for (UINT16 i = 0; i < 256; i++) {
        ROM_CTRL = ROM_CMD_WRITE;
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bCODE_WE;
    SAFE_MOD = 0x00;
    EA = ea_sts;
}
```

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Write returns 0x02 | Safe mode not entered correctly | Verify 0x55/0xAA sequence, EA=0 |
| Data corrupted | Writing odd address | Use even addresses (addr << 1) |
| Verify fails after write | Interrupts during Flash op | Always disable EA during unlock/write/lock |
| DataFlash lost after ISP | ISP erases DataFlash | Check ISP tool settings for DataFlash preserve |
| Can't write code flash | bCODE_WE not set | Must unlock code flash separately |
