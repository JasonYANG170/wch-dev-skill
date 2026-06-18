# Recipe: SDIO SD Card Interface

## Overview

Use the SDIO (Secure Digital Input/Output) peripheral on CH32F20x to communicate with SD cards and eMMC storage devices. The example provides a complete driver for card initialization, sector read/write, and card information retrieval.

**Availability**: CH32F20x only (not available on CH32F10x or CH32M030).

## Key API Functions (Custom SDIO Driver)

The SDIO examples include a custom `sdio.c`/`sdio.h` driver (not part of the standard peripheral library):

```c
// Card initialization and info
SD_Error SD_Init(void);                    // Initialize SD card, returns SD_Error code
void show_sdcard_info(void);               // Print card type, capacity, block size

// Read/Write operations
SD_Error SD_WriteDisk(u8 *buf, u32 sector, u8 cnt);   // Write sectors
SD_Error SD_ReadDisk(u8 *buf, u32 sector, u8 cnt);    // Read sectors

// Card info structure
extern SD_CardInfo SDCardInfo;
// SDCardInfo.CardType          - SDSC V1.1, SDSC V2.0, SDHC V2.0, MMC
// SDCardInfo.SD_cid.ManufacturerID - Manufacturer ID
// SDCardInfo.RCA               - Relative Card Address
// SDCardInfo.CardCapacity      - Card capacity in bytes
// SDCardInfo.CardBlockSize     - Block size in bytes
```

## SDIO Pin Connections

| SDIO Signal | Pin | Description |
|-------------|-----|-------------|
| D0 | PC8 | Data line 0 |
| D1 | PC9 | Data line 1 |
| D2 | PC10 | Data line 2 |
| D3 | PC11 | Data line 3 |
| SCK | PC12 | Clock |
| CMD | PD2 | Command |

**Important**: All data lines (D0-D3) and CMD require external 47K pull-up resistors.

## SDIO Clock Configuration

```c
// SDIO clock dividers (defined in sdio.h)
#define SDIO_INIT_CLK_DIV       0xB2    // ~400kHz for initialization
#define SDIO_TRANSFER_CLK_DIV   0x00    // Maximum speed for data transfer
```

## SD Card Types

| Type | Description |
|------|-------------|
| SDIO_STD_CAPACITY_SD_CARD_V1_1 | Standard capacity SD v1.1 (up to 2GB) |
| SDIO_STD_CAPACITY_SD_CARD_V2_0 | Standard capacity SD v2.0 (up to 2GB) |
| SDIO_HIGH_CAPACITY_SD_CARD | High capacity SDHC (up to 32GB) |
| SDIO_MULTIMEDIA_CARD | MMC card |

## Example: SD Card Read/Write

```c
#include "debug.h"
#include "sdio.h"
#include "string.h"

u8 buf[512];        // Write buffer (one sector)
u8 Readbuf[512];    // Read buffer (one sector)

void show_sdcard_info(void)
{
    switch(SDCardInfo.CardType) {
        case SDIO_STD_CAPACITY_SD_CARD_V1_1:
            printf("Card Type: SDSC V1.1\r\n"); break;
        case SDIO_STD_CAPACITY_SD_CARD_V2_0:
            printf("Card Type: SDSC V2.0\r\n"); break;
        case SDIO_HIGH_CAPACITY_SD_CARD:
            printf("Card Type: SDHC V2.0\r\n"); break;
        case SDIO_MULTIMEDIA_CARD:
            printf("Card Type: MMC Card\r\n"); break;
    }
    printf("ManufacturerID: %d\r\n", SDCardInfo.SD_cid.ManufacturerID);
    printf("Card RCA: %d\r\n", SDCardInfo.RCA);
    printf("Card Capacity: %d MB\r\n", (u32)(SDCardInfo.CardCapacity >> 20));
    printf("Card BlockSize: %d\r\n", SDCardInfo.CardBlockSize);
}

int main(void)
{
    u32 i;
    u32 Sector_Nums;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    // Initialize SD card
    while(SD_Init()) {
        printf("SD Card Error!\r\n");
        delay_ms(1000);
    }
    show_sdcard_info();
    printf("SD Card OK\r\n");

    // Calculate total sectors
    Sector_Nums = ((u32)(SDCardInfo.CardCapacity >> 20)) * 2048;
    printf("Sector_Nums: 0x%08x\r\n", Sector_Nums);

    // Fill write buffer with test pattern
    for(i = 0; i < 512; i++) {
        buf[i] = i;
    }

    // Write and verify all sectors
    for(i = 0; i < Sector_Nums; i++) {
        if(SD_WriteDisk(buf, i, 1)) {
            printf("Wr %d sector fail\r\n", i);
        }
        if(SD_ReadDisk(Readbuf, i, 1)) {
            printf("Rd %d sector fail\r\n", i);
        }
        if(memcmp(buf, Readbuf, 512)) {
            printf("%d sector Verify fail\r\n", i);
            break;
        }
    }

    printf("Test complete\r\n");
    while(1);
}
```

## Example: Multi-Sector Read/Write

```c
// Write multiple sectors at once
u8 multi_buf[4096];  // 8 sectors

// Fill buffer
for(int i = 0; i < 4096; i++) multi_buf[i] = i & 0xFF;

// Write 8 sectors starting at sector 100
SD_Error err = SD_WriteDisk(multi_buf, 100, 8);
if(err != SD_OK) {
    printf("Multi-write error: %d\r\n", err);
}

// Read them back
u8 read_multi[4096];
err = SD_ReadDisk(read_multi, 100, 8);
if(err != SD_OK) {
    printf("Multi-read error: %d\r\n", err);
}

// Verify
if(memcmp(multi_buf, read_multi, 4096) == 0) {
    printf("Multi-sector verify OK\r\n");
}
```

## SDIO Error Codes

| Code | Value | Description |
|------|-------|-------------|
| SD_OK | 0 | Success |
| SD_CMD_CRC_FAIL | 1 | Command CRC check failed |
| SD_DATA_CRC_FAIL | 2 | Data CRC check failed |
| SD_CMD_RSP_TIMEOUT | 3 | Command response timeout |
| SD_DATA_TIMEOUT | 4 | Data timeout |
| SD_TX_UNDERRUN | 5 | Transmit FIFO underrun |
| SD_RX_OVERRUN | 6 | Receive FIFO overrun |
| SD_START_BIT_ERR | 7 | Start bit error |
| SD_ADDR_MISALIGNED | 9 | Address misaligned |
| SD_BLOCK_LEN_ERR | 10 | Block length error |
| SD_ILLEGAL_CMD | 16 | Illegal command |
| SD_GENERAL_UNKNOWN_ERROR | 19 | General error |

## Pitfalls

- **Pull-up resistors**: All SDIO data lines (D0-D3) and CMD line require external 47K pull-up resistors. Without them, communication will fail.
- **Initialization clock**: SDIO must use ~400kHz during card initialization (`SDIO_INIT_CLK_DIV = 0xB2`), then switch to full speed for data transfer.
- **Card detection**: The examples do not implement card detect (CD) pin handling. Add card hot-plug detection if needed.
- **Power supply**: SD cards require 3.3V supply. Do not connect to 5V directly.
- **Sector size**: Standard SD cards use 512-byte sectors. SDHC cards always use 512-byte sectors regardless of the reported block size.
- **File system**: The raw SDIO driver provides sector-level access only. For file operations (FAT32, exFAT), integrate a file system library like FatFS.
- **DMA mode**: The SDIO driver supports both polling and DMA modes. DMA mode is defined by `SD_DMA_MODE` in sdio.h.

## Related Examples

- `CH32F20xEVT/EVT/EXAM/SDIO/SDIO_SD` - SD card sector read/write
- `CH32F20xEVT/EVT/EXAM/SDIO/SDIO_eMMC` - eMMC storage interface
