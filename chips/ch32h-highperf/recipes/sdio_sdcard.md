# SDIO SD Card Interface

## Overview

CH32H417 has an SDIO (Secure Digital Input/Output) controller for SD card and eMMC communication. Supports 1-bit and 4-bit bus modes, DMA transfers, and various SD card types (SDSC, SDHC, MMC).

## Key API Functions (sdio.c)

```c
// From project-level sdio.c/h
uint8_t SD_Init(void);
uint8_t SD_ReadDisk(uint8_t* buf, uint32_t sector, uint8_t cnt);
uint8_t SD_WriteDisk(uint8_t* buf, uint32_t sector, uint8_t cnt);
void show_sdcard_info(void);
```

## SDIO Initialization and Card Info

```c
#include "ch32h417.h"
#include "sdio.h"

void SDIO_Init_Example(void)
{
    // Initialize SD card
    while (SD_Init())
    {
        printf("SD Card Error!\r\n");
        Delay_Ms(1000);
    }

    // Display card information
    switch (SDCardInfo.CardType)
    {
        case SDIO_STD_CAPACITY_SD_CARD_V1_1:
            printf("Card Type: SDSC V1.1\r\n");
            break;
        case SDIO_STD_CAPACITY_SD_CARD_V2_0:
            printf("Card Type: SDSC V2.0\r\n");
            break;
        case SDIO_HIGH_CAPACITY_SD_CARD:
            printf("Card Type: SDHC V2.0\r\n");
            break;
        case SDIO_MULTIMEDIA_CARD:
            printf("Card Type: MMC Card\r\n");
            break;
    }

    printf("Card ManufacturerID: %d\r\n", SDCardInfo.SD_cid.ManufacturerID);
    printf("Card RCA: %d\r\n", SDCardInfo.RCA);
    printf("Card Capacity: %d MB\r\n", (uint32_t)(SDCardInfo.CardCapacity >> 20));
    printf("Card BlockSize: %d\r\n", SDCardInfo.CardBlockSize);
}
```

## SD Card Read/Write

```c
uint8_t buf[512];
uint8_t Readbuf[512];

void SD_ReadWrite_Test(void)
{
    uint32_t i;
    uint32_t Sector_Nums;

    // Fill write buffer
    for (i = 0; i < 512; i++)
        buf[i] = i;

    // Write and verify each sector
    Sector_Nums = ((uint32_t)(SDCardInfo.CardCapacity >> 20)) * 2048;
    printf("Total sectors: 0x%08x\r\n", Sector_Nums);

    for (i = 0; i < Sector_Nums; i++)
    {
        if (SD_WriteDisk(buf, i, 1))
            printf("Write sector %d fail\r\n", i);

        if (SD_ReadDisk(Readbuf, i, 1))
            printf("Read sector %d fail\r\n", i);

        if (memcmp(buf, Readbuf, 512))
        {
            printf("Sector %d verify fail\r\n", i);
            break;
        }
    }
    printf("Test complete\r\n");
}
```

## SD Card Types

| Macro | Description |
|-------|-------------|
| `SDIO_STD_CAPACITY_SD_CARD_V1_1` | Standard capacity SD v1.1 |
| `SDIO_STD_CAPACITY_SD_CARD_V2_0` | Standard capacity SD v2.0 |
| `SDIO_HIGH_CAPACITY_SD_CARD` | High capacity SDHC |
| `SDIO_MULTIMEDIA_CARD` | MMC card |

## SDCardInfo Structure

```c
typedef struct {
    uint32_t CardCapacity;      // Card capacity in bytes
    uint32_t CardBlockSize;     // Block size in bytes
    uint16_t RCA;               // Relative Card Address
    uint8_t  CardType;          // Card type enum
    SD_CID  SD_cid;             // Card ID register
} SD_CardInfo;
```

## Important Notes

- SDIO uses `sdio.c` and `sdio.h` from the project's Common directory
- The SD_Init() function handles CMD0, CMD8, ACMD41, CMD2, CMD3, CMD9, CMD7 sequence
- For SDHC cards, sector addressing is used (not byte addressing)
- DMA is recommended for large block transfers
