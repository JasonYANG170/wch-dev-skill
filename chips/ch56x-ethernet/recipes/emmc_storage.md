# Recipe: eMMC/SD Storage

## Overview

Use the hardware SDIO controller on CH56x to communicate with eMMC or SD cards.
Supports single/multiple sector read/write, high-speed mode, and AES-encrypted transfers.

## eMMC Hardware Interface

```
CH569 SDIO Controller  <---SDIO Bus--->  eMMC/SD Card Slot
   |                                        |
   |  CLK, CMD, DAT0-DAT3 (4-bit bus)       |
   +----------------------------------------+
   |
   +--- DMA for data transfers
```

## Initialization

```c
#include "CH56x_common.h"

EMMC_PARAMETER emmcParam;

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_EMMC);

    // Initialize eMMC IO
    UINT8 status = EMMCIO0Init();
    if (status != CMD_SUCCESS) {
        printf("eMMC init failed: %d\n", status);
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

    printf("eMMC: type=%d, sectors=%d, secsize=%d\n",
           emmcParam.EMMCType,
           emmcParam.EMMCSecNum,
           emmcParam.EMMCSecSize);

    while(1) {}
}
```

## Single Sector Read

```c
UINT8 readBuf[512] __attribute__((aligned(4), section(".dmadata")));
UINT32 sector_addr = 0;  // LBA address

UINT8 status = EMMCCardReadOneSec(&emmcParam, readBuf, sector_addr);
if (status == OP_SUCCESS) {
    printf("Read sector 0 successful\n");
    // Process readBuf[0..511]
} else {
    printf("Read failed: %d\n", status);
}
```

## Multiple Sector Read

```c
UINT8 readBuf[4096] __attribute__((aligned(4), section(".dmadata")));
UINT16 num_sectors = 8;  // Read 8 sectors (4KB)
UINT32 start_sector = 100;

UINT8 status = EMMCCardReadMulSec(&emmcParam, &num_sectors, readBuf, start_sector);
if (status == OP_SUCCESS) {
    printf("Read %d sectors successful\n", num_sectors);
}
```

## Multiple Sector Write

```c
UINT8 writeBuf[4096] __attribute__((aligned(4), section(".dmadata")));
UINT16 num_sectors = 8;
UINT32 start_sector = 200;

// Fill write buffer
memset(writeBuf, 0xAA, sizeof(writeBuf));

UINT8 status = EMMCCardWriteMulSec(&emmcParam, &num_sectors, writeBuf, start_sector);
if (status == OP_SUCCESS) {
    printf("Write %d sectors successful\n", num_sectors);
}
```

## High-Speed Mode

```c
// Set bus width (4-bit mode)
EMMCSetBusWidth(&emmcParam, 4);

// Enable high-speed mode
EMMCSetHighSpeed(&emmcParam);
```

## AES-Encrypted eMMC Transfers

CH56x supports hardware AES encryption for eMMC read/write:

```c
UINT32 aes_key[4] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
UINT32 aes_count[4] = {0};

// Initialize ECDC for AES
ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, aes_key, aes_count);

// Encrypted write
UINT32 writeData[128] __attribute__((aligned(4), section(".dmadata")));
UINT32 count = 128;
AES_EMMCWriteMulSec(&emmcParam, &count, writeData, sector_addr,
                    SELFDMA_ENCRY, MODE_BIG_ENDIAN, &count);

// Encrypted read
UINT32 readData[128] __attribute__((aligned(4), section(".dmadata")));
count = 128;
AES_EMMCReadMulSec(&emmcParam, &count, readData, sector_addr,
                   SELFDMA_DECRY, MODE_BIG_ENDIAN, &count);
```

## eMMC Status and Error Handling

```c
// Check connection status
if (emmcParam.EMMCLinkSatus == EMMCConnect) {
    printf("Card connected\n");
}

// Check operation status
switch (emmcParam.EMMCCardSatus) {
    case EMMCCardSatus_Idle:
        printf("Card idle\n");
        break;
    case EMMCCardSatus_MultReadComp:
        printf("Multi-read complete\n");
        break;
    case EMMCCardSatus_MultWriteComp:
        printf("Multi-write complete\n");
        break;
}

// Handle errors
if (emmcParam.EMMCOpErr) {
    printf("Error: 0x%02X\n", emmcParam.EMMCOpErr);
    EMMCIOTransErrorDeal(&emmcParam);
}
```

## Power Control

```c
// Set SDIO power to 3.3V
EMMCIOSetPower_3_3;

// Set SDIO power to 1.8V
EMMCIOSetPower_1_8;
```

## Read Extended CSD

```c
UINT8 excsd[512];
EMMCCardReadEXCSD(&emmcParam, excsd);
// Parse extended CSD for card capabilities
```

## Notes

- DMA buffers for eMMC must be in RAMX (0x20020000+) for DMA access
- eMMC clock can be configured: 48MHz, 60MHz, 80MHz, 96MHz
- Default clock: 48MHz (`EMMCCLK_48`)
- High-speed clock: 96MHz (`EMMCCLK_96`)
- Sector size is typically 512 bytes
- AES encryption is handled in hardware, no software overhead
- Check `EMMCPLUGIN` bit to detect card insertion
