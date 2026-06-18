# Recipe: HSPI High-Speed SPI Communication

## Overview

**CH561/CH563 do NOT have the HSPI (High-Speed SPI) peripheral.**
HSPI is available only on CH569 (RISC-V). CH561/CH563 have standard SPI0 and SPI1
peripherals only.

## Alternative: SPI0/SPI1 DMA Transfers

For high-speed data transfer on CH561/CH563, use SPI0 or SPI1 with DMA:

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

// SPI0 DMA transfer
void SPI0_DMATransfer(UINT8 *buf, UINT16 len, UINT8 is_tx)
{
    // Enable DMA
    R8_SPI0_CTRL_DMA = RB_SPI_DMA_ENABLE;

    // Set DMA addresses
    R16_SPI0_DMA_BEG = (UINT16)(UINT32)buf;
    R16_SPI0_DMA_END = (UINT16)(UINT32)buf + len;
    R16_SPI0_DMA_NOW = (UINT16)(UINT32)buf;

    // Set total byte count
    R16_SPI0_TOTAL_CNT = len;

    // Set FIFO direction
    if (is_tx) {
        R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;  // 0=out (TX)
    } else {
        R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;   // 1=in (RX)
    }

    // Enable DMA interrupt
    R8_SPI0_INTER_EN = RB_SPI_IE_DMA_END;

    // Wait for DMA complete
    while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_DMA_END));
    R8_SPI0_INT_FLAG = RB_SPI_IF_DMA_END;  // Clear flag
}
```

## SPI Performance Comparison

| Feature       | CH561/CH563 SPI0/1 | CH569 HSPI |
|---------------|---------------------|------------|
| Data width    | 8-bit only          | 8/16/32-bit |
| Max clock     | ~12.5MHz (sysclk/8) | 60MHz (sysclk/2) |
| DMA           | Yes                 | Yes, double-buffered |
| Burst mode    | No                  | Yes |
| CRC check     | No                  | Yes (hardware) |
| ECDC encrypt  | No                  | Yes (link to ECDC) |

## Notes

- CH561/CH563 have SPI0 and SPI1 only (see `recipes/spi_comm.md`)
- For inter-chip communication at higher speeds, consider using Ethernet
- SPI clock = FREQ_SYS / (2 * R8_SPIx_CLOCK_DIV)
- SPI0 FIFO: 32 bytes, SPI1 FIFO: 16 bytes
