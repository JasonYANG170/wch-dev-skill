# Recipe: eMMC/SD Storage (Register-Based)

## Overview

Use the hardware SDIO controller on CH561/CH563 to communicate with eMMC or SD cards.
Supports single/multiple sector read/write and high-speed mode. Uses pre-compiled
eMMC library functions.

## eMMC Hardware Interface

```
CH561/CH563 SDIO Controller  <---SDIO Bus--->  eMMC/SD Card Slot
   |                                              |
   |  CLK, CMD, DAT0-DAT3 (4-bit bus)             |
   +----------------------------------------------+
   |
   +--- DMA for data transfers
```

## Key Difference from CH569 (RISC-V)

- No `PWR_PeriphClkCfg()` -- use `R8_SLP_CLK_OFF0` register directly
- No `__attribute__((interrupt("WCH-Interrupt-fast")))` -- use `__irq` keyword
- No `.dmadata` section -- DMA buffers in DATA SRAM
- Flash erase unit is 4KB (not 256 bytes)
- CH563 has additional USB SFR registers

## Clock Enable

```c
// Enable eMMC peripheral clock
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_EMMC;
```

## Initialization

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

EMMC_PARAMETER emmcParam;

int main(void)
{
    mInitSTDIO();

    // Enable eMMC clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_EMMC;

    // Initialize eMMC IO
    UINT8 status = EMMCIO0Init();
    if (status != CMD_SUCCESS) {
        PRINT("eMMC init failed: %d\n", status);
        while(1);
    }

    // Card initialization sequence
    EMMCResetIdle(&emmcParam);
    EMMCReadOCR(&emmcParam);
    EMMCReadCID(&emmcParam);
    EMMCSetRCA(&emmcParam);
    EMMCReadCSD(&emmcParam);
    SelectEMMCCard(&emmcParam);
    EMMCCardConfig(&emmcParam);

    PRINT("eMMC: type=%d, sectors=%d, secsize=%d\n",
          emmcParam.EMMCType,
          emmcParam.EMMCSecNum,
          emmcParam.EMMCSecSize);

    while(1);
}
```

## Single Sector Read

```c
UINT8 readBuf[512] __attribute__((aligned(4)));
UINT32 sector_addr = 0;  // LBA address

UINT8 status = EMMCCardReadOneSec(&emmcParam, readBuf, sector_addr);
if (status == OP_SUCCESS) {
    PRINT("Read sector 0 successful\n");
} else {
    PRINT("Read failed: %d\n", status);
}
```

## Multiple Sector Read

```c
UINT8 readBuf[4096] __attribute__((aligned(4)));
UINT16 num_sectors = 8;  // Read 8 sectors (4KB)
UINT32 start_sector = 100;

UINT8 status = EMMCCardReadMulSec(&emmcParam, &num_sectors, readBuf, start_sector);
if (status == OP_SUCCESS) {
    PRINT("Read %d sectors successful\n", num_sectors);
}
```

## Multiple Sector Write

```c
UINT8 writeBuf[4096] __attribute__((aligned(4)));
UINT16 num_sectors = 8;
UINT32 start_sector = 200;

memset(writeBuf, 0xAA, sizeof(writeBuf));

UINT8 status = EMMCCardWriteMulSec(&emmcParam, &num_sectors, writeBuf, start_sector);
if (status == OP_SUCCESS) {
    PRINT("Write %d sectors successful\n", num_sectors);
}
```

## High-Speed Mode

```c
// Set bus width (4-bit mode)
EMMCSetBusWidth(&emmcParam, 4);

// Enable high-speed mode
EMMCSetHighSpeed(&emmcParam);
```

## eMMC Status and Error Handling

```c
// Check connection status
if (emmcParam.EMMCLinkSatus == EMMCConnect) {
    PRINT("Card connected\n");
}

// Check operation status
switch (emmcParam.EMMCCardSatus) {
    case EMMCCardSatus_Idle:
        PRINT("Card idle\n");
        break;
    case EMMCCardSatus_MultReadComp:
        PRINT("Multi-read complete\n");
        break;
    case EMMCCardSatus_MultWriteComp:
        PRINT("Multi-write complete\n");
        break;
}

// Handle errors
if (emmcParam.EMMCOpErr) {
    PRINT("Error: 0x%02X\n", emmcParam.EMMCOpErr);
    EMMCIOTransErrorDeal(&emmcParam);
}
```

## Read Extended CSD

```c
UINT8 excsd[512];
EMMCCardReadEXCSD(&emmcParam, excsd);
// Parse extended CSD for card capabilities
```

## Notes

- DMA buffers must be word-aligned
- eMMC clock can be configured: 48MHz, 60MHz, 80MHz, 96MHz
- Default clock: 48MHz (`EMMCCLK_48`)
- High-speed clock: 96MHz (`EMMCCLK_96`)
- Sector size is typically 512 bytes
- Check `EMMCPLUGIN` bit to detect card insertion
- CH561/CH563 do not support ECDC-encrypted eMMC transfers (no ECDC engine)

## Example Project

- `CH561EVT/EXAM/FLASH/` - Flash operations
- `CH563EVT/EXAM/FLASH/` - Flash operations
