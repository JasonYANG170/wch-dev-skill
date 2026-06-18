# Recipe: SD Card Read/Write

## Overview

Access SD cards via the SDIO/eMMC controller on CH56x chips. The SD card interface
shares the same hardware controller as eMMC, using the `SD_*` API functions for
SD-specific initialization and data transfer. Supports 1-bit and 4-bit bus modes.

## SD vs eMMC

| Feature      | SD Card                    | eMMC                       |
|--------------|----------------------------|----------------------------|
| API prefix   | `SD_*`                     | `EMMC*`                    |
| Card detect  | Removable                  | Soldered                   |
| Init sequence| ACMD41-based               | CMD1-based                 |
| Bus width    | 1-bit, 4-bit               | 1-bit, 4-bit, 8-bit       |
| API header   | `CH56x_emmc.h` (shared)    | `CH56x_emmc.h`             |

## GPIO Configuration

SD card uses dedicated SDIO pins. Configure pull-ups and drive strength:

```c
void SD_IO_Init(void)
{
    // Pull-ups on CMD and data lines
    R32_PB_PU |= bSDCMD;
    R32_PB_PU |= (0x1f << 17);  // Data lines
    R32_PA_PU |= (7 << 0);

    // CLK output
    R32_PB_DIR |= bSDCK;

    // Drive strength for higher speeds
    R32_PA_DRV |= (7 << 0);
    R32_PB_DRV |= (0x1f << 17);
    R32_PB_DRV |= bSDCMD;
    R32_PB_DRV |= bSDCK;
}
```

## Controller Initialization

```c
void SD_Controller_Init(void)
{
    // Reset controller
    R8_EMMC_CONTROL = RB_EMMC_ALL_CLR | RB_EMMC_RST_LGC;

    // Enable DMA, positive sampling
    R8_EMMC_CONTROL = RB_EMMC_DMAEN | RB_EMMC_NEGSMP;

    // 4-bit bus mode
    R8_EMMC_CONTROL = (R8_EMMC_CONTROL & ~RB_EMMC_LW_MASK) | bLW_OP_DAT4;

    // Clock: divide system clock, enable output
    R16_EMMC_CLK_DIV = RB_EMMC_CLKMode | RB_EMMC_CLKOE | 10;

    // Clear and enable error interrupts
    R16_EMMC_INT_FG = 0xFFFF;
    R16_EMMC_INT_EN = RB_EMMC_IE_FIFO_OV | RB_EMMC_IE_TRANERR |
                      RB_EMMC_IE_DATTMO | RB_EMMC_IE_REIDX_ER |
                      RB_EMMC_IE_RECRC_WR | RB_EMMC_IE_RE_TMOUT;

    // Timeout
    R8_EMMC_TIMEOUT = 14;
}
```

## SD Card Initialization Sequence

The SD card init sequence uses SD-specific functions:

```c
EMMC_PARAMETER sd_param;

UINT8 SD_Init(void)
{
    UINT8 sta;

    // Reset idle (CMD0)
    EMMCResetIdle(&sd_param);
    mDelaymS(30);
    EMMCResetIdle(&sd_param);
    mDelaymS(30);

    // Send CMD8 to check SD version
    sta = EMMCSendCmd(0x01AA, RB_EMMC_CKIDX | RB_EMMC_CKCRC |
                     RESP_TYPE_48 | EMMC_CMD8);
    // Wait for response...

    // ACMD41: send OCR
    sta = SDReadOCR(&sd_param);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    // CMD2: read CID
    sta = EMMCReadCID(&sd_param);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    // CMD3: set RCA
    sta = SDSetRCA(&sd_param);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    // CMD9: read CSD
    sta = SDReadCSD(&sd_param);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    // CMD7: select card
    sta = SelectEMMCCard(&sd_param);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    // CMD6: set 4-bit bus width
    sta = SDSetBusWidth(&sd_param, 1);  // 1 = 4-bit mode
    if (sta != CMD_SUCCESS) return OP_FAILED;

    R8_EMMC_CONTROL = (R8_EMMC_CONTROL & ~RB_EMMC_LW_MASK) | bLW_OP_DAT4;

    // Read SCR register
    sta = SD_ReadSCR(&sd_param, buffer);
    if (sta != CMD_SUCCESS) return OP_FAILED;

    return OP_SUCCESS;
}
```

## SD Card Read/Write

```c
// Read a single 512-byte sector
UINT8 SDCardReadOneSec(PSD_PARAMETER pPara, PUINT8 pBuf, UINT32 LbaAddr);

// Write a single 512-byte sector
UINT8 SDCardWriteONESec(PSD_PARAMETER pPara, PUINT8 pBuf, UINT32 LbaAddr);
```

## Complete Example

```c
#include "CH56x_common.h"

#define FREQ_SYS   80000000

EMMC_PARAMETER sd_param;
__attribute__((aligned(16)))
UINT8 buf[512] __attribute__((section(".DMADATA")));

void EMMC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DebugInit(UINT32 baudrate)
{
    UINT32 x, t = FREQ_SYS;
    x = 10 * t * 2 / 16 / baudrate;
    x = (x + 5) / 10;
    R8_UART1_DIV = 1;
    R16_UART1_DL = x;
    R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R32_PA_SMT |= (1 << 8) | (1 << 7);
    R32_PA_DIR |= (1 << 8);
}

void SD_IO_Init(void)
{
    R32_PB_PU |= bSDCMD;
    R32_PB_PU |= (0x1f << 17);
    R32_PA_PU |= (7 << 0);
    R32_PB_DIR |= bSDCK;

    R8_EMMC_CONTROL = RB_EMMC_ALL_CLR | RB_EMMC_RST_LGC;
    R8_EMMC_CONTROL = RB_EMMC_DMAEN | RB_EMMC_NEGSMP;
    R8_EMMC_CONTROL = (R8_EMMC_CONTROL & ~RB_EMMC_LW_MASK) | bLW_OP_DAT4;
    R16_EMMC_CLK_DIV = RB_EMMC_CLKMode | RB_EMMC_CLKOE | 10;

    R16_EMMC_INT_FG = 0xFFFF;
    R16_EMMC_INT_EN = RB_EMMC_IE_FIFO_OV | RB_EMMC_IE_TRANERR |
                      RB_EMMC_IE_DATTMO | RB_EMMC_IE_REIDX_ER |
                      RB_EMMC_IE_RECRC_WR | RB_EMMC_IE_RE_TMOUT;
    R8_EMMC_TIMEOUT = 14;
}

int main(void)
{
    UINT8 i;
    UINT16 s;

    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);
    DebugInit(921600);

    printf("SD Card Example\r\n");

    // Try different IO modes for auto-detection
    for (i = 0; i < 8; i++) {
        printf("IO mode: %d\r\n", i);
        SD_IO_Init();
        UINT8 sta = SD_Init();
        if (sta == OP_SUCCESS) {
            printf("SD Init OK\r\n");
            break;
        }
    }

    PFIC_EnableIRQ(EMMC_IRQn);

    printf("Block size: %d B\r\n", sd_param.EMMCSecSize);
    printf("Capacity: %ld MB\r\n", sd_param.EMMCSecNum / 2048);

    // Read sector 0
    printf("\nRead sector 0:\r\n");
    SDCardReadOneSec(&sd_param, buf, 0);
    for (s = 0; s < 512; s++) {
        if (!(s % 16)) printf("\r\n");
        printf("%02x ", buf[s]);
    }

    // Modify and write back
    for (s = 0; s < 512; s++) {
        buf[s] = ~buf[s];
    }
    SDCardWriteONESec(&sd_param, buf, 0);

    // Read back to verify
    printf("\n\nRead back:\r\n");
    SDCardReadOneSec(&sd_param, buf, 0);
    for (s = 0; s < 512; s++) {
        if (!(s % 16)) printf("\r\n");
        printf("%02x ", buf[s]);
    }

    while(1);
}

void EMMC_IRQHandler(void)
{
    sd_param.EMMCOpErr = 1;
    UINT16 t = R16_EMMC_INT_FG;
    R16_EMMC_INT_FG = t & (RB_EMMC_IF_FIFO_OV | RB_EMMC_IF_TRANERR |
                           RB_EMMC_IF_DATTMO | RB_EMMC_IF_REIDX_ER |
                           RB_EMMC_IF_RECRC_WR | RB_EMMC_IF_RE_TMOUT);
}
```

## Buffer Requirements

- Buffers must be 16-byte aligned and placed in the `.DMADATA` section (RAMX).
- The `.DMADATA` section is placed at 0x20020000+ by the linker script.
- Use `__attribute__((aligned(16), section(".DMADATA")))` for buffer declarations.

## IO Mode Auto-Detection

The SD example iterates through 8 IO modes to find the correct one for the
connected card. This handles different card types and wiring configurations:

```c
for (i = 0; i < 8; i++) {
    SD_IO_Init(i);         // Try each IO mode
    if (SD_Init(i) == OP_SUCCESS) break;
}
```

## Clock Speed

After initialization, the SDIO clock can be increased for faster transfers:

```c
// After successful init, increase clock
R16_EMMC_CLK_DIV = RB_EMMC_CLKMode | RB_EMMC_CLKOE | 10;
// Divisor 10: clock = sysclk / (2 * 10) = 4MHz at 80MHz sysclk
```

## Notes

- SD card buffers must use the `.DMADATA` section attribute for DMA access.
- The SD and eMMC interfaces share the same hardware controller and GPIO pins.
  Only one can be active at a time.
- The `EMMC_IRQHandler` handles all SDIO error conditions. Always clear the
  interrupt flags in the handler.
- For SDHC/SDXC cards, sector addresses are in units of 512 bytes (LBA).
- The initialization sequence may need multiple retries depending on the card.

## Example Projects

- `CH569EVT/EVT/EXAM/SD/` - SD card read/write demo
- `CH569EVT/EVT/EXAM/EMMC/` - eMMC read/write demo (similar API)
