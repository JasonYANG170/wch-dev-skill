# Recipe: DVP Camera Interface

## Overview

Use the DVP (Digital Video Port) peripheral to capture image data from parallel
camera sensors (e.g., OV2640) on CH56x chips. The DVP supports both RGB565 and
JPEG capture modes, with double-buffered DMA to RAMX and interrupt-driven line/frame
processing.

## DVP Operating Modes

| Mode    | Description                                    | Data Format        |
|---------|------------------------------------------------|--------------------|
| RGB565  | Raw pixel capture, 16-bit per pixel            | Fixed-size frames  |
| JPEG    | JPEG stream capture, variable-length frames    | Starts 0xFFD8, ends 0xFFD9 |

## GPIO Pin Assignment

DVP uses dedicated pins (active-low naming convention from the sensor perspective):

| Signal   | Direction | Description                    |
|----------|-----------|--------------------------------|
| VSYNC    | Input     | Vertical sync (frame boundary) |
| HSYNC    | Input     | Horizontal sync (line boundary)|
| PCLK     | Input     | Pixel clock                    |
| D0-D7    | Input     | 8-bit parallel data bus        |

## Register Configuration

```c
// Control Register 0
R8_DVP_CR0 |= RB_DVP_D8_MOD;    // 8-bit data mode
R8_DVP_CR0 |= RB_DVP_V_POLAR;   // VSYNC active high
R8_DVP_CR0 |= RB_DVP_JPEG;      // JPEG mode (RGB565: clear this bit)

// Control Register 1
R8_DVP_CR1 |= RB_DVP_DMA_EN;    // Enable DMA
R8_DVP_CR0 |= RB_DVP_ENABLE;    // Enable DVP

// Frame dimensions
R16_DVP_ROW_NUM = rows;          // Number of rows (RGB565)
R16_DVP_COL_NUM = cols;          // Number of columns / DMA buffer width

// Double-buffered DMA addresses (must be in RAMX)
R32_DVP_DMA_BUF0 = addr0;
R32_DVP_DMA_BUF1 = addr1;
```

## Interrupt Flags

| Flag                  | Description                    |
|-----------------------|--------------------------------|
| `RB_DVP_IF_STR_FRM`  | Frame start                    |
| `RB_DVP_IF_STP_FRM`  | Frame stop                     |
| `RB_DVP_IF_FRM_DONE` | Frame reception complete       |
| `RB_DVP_IF_ROW_DONE` | Row (line) reception complete  |
| `RB_DVP_IF_FIFO_OV`  | FIFO overflow error            |

## Complete Example: JPEG Capture with OV2640

```c
#include "CH56x_common.h"

#define FREQ_SYS  80000000

// JPEG frame width (DMA buffer width per line)
#define JPEG_WIDTH  320

// DMA double buffers in RAMX
#define DMA_ADDR0   0x20020000
#define DMA_ADDR1   (0x20020000 + JPEG_WIDTH)

volatile UINT32 frame_cnt = 0;
volatile UINT32 href_cnt = 0;
volatile UINT32 addr_cnt = 0;

void DVP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DVP_Init_JPEG(void)
{
    // JPEG mode: VSYNC active high, HSYNC active high
    R8_DVP_CR0 = RB_DVP_D8_MOD | RB_DVP_V_POLAR | RB_DVP_JPEG;
    R8_DVP_CR1 = 0;
    R8_DVP_CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);

    // In JPEG mode, COL_NUM = DMA transfer width per line
    R16_DVP_COL_NUM = JPEG_WIDTH;

    // DMA buffers
    R32_DVP_DMA_BUF0 = DMA_ADDR0;
    R32_DVP_DMA_BUF1 = DMA_ADDR1;

    // Enable interrupts
    R8_DVP_INT_EN = RB_DVP_IE_STP_FRM | RB_DVP_IE_FIFO_OV |
                    RB_DVP_IE_FRM_DONE | RB_DVP_IE_ROW_DONE |
                    RB_DVP_IE_STR_FRM;

    PFIC_EnableIRQ(DVP_IRQn);

    // Enable DMA and DVP
    R8_DVP_CR1 |= RB_DVP_DMA_EN;
    R8_DVP_CR0 |= RB_DVP_ENABLE;
}

void DVP_IRQHandler(void)
{
    if (R8_DVP_INT_FLAG & RB_DVP_IF_ROW_DONE) {
        R8_DVP_INT_FLAG = RB_DVP_IF_ROW_DONE;
        href_cnt++;

        // Ping-pong DMA buffers
        if (addr_cnt % 2) {
            R32_DVP_DMA_BUF1 += JPEG_WIDTH * 2;
        } else {
            R32_DVP_DMA_BUF0 += JPEG_WIDTH * 2;
        }
        addr_cnt++;
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_FRM_DONE) {
        R8_DVP_INT_FLAG = RB_DVP_IF_FRM_DONE;

        R8_DVP_CR0 &= ~RB_DVP_ENABLE;  // Pause DVP

        // Process JPEG data from RAMX
        UINT32 total = href_cnt * JPEG_WIDTH;
        for (UINT32 i = 0; i < total; i++) {
            UINT8 val = *(UINT8 *)(0x20020000 + i);
            // Send via UART or process...
        }

        // Reset for next frame
        R32_DVP_DMA_BUF0 = DMA_ADDR0;
        R32_DVP_DMA_BUF1 = DMA_ADDR1;
        href_cnt = 0;
        addr_cnt = 0;

        R8_DVP_CR0 |= RB_DVP_ENABLE;  // Resume DVP
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_STR_FRM) {
        R8_DVP_INT_FLAG = RB_DVP_IF_STR_FRM;
        frame_cnt++;
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_STP_FRM) {
        R8_DVP_INT_FLAG = RB_DVP_IF_STP_FRM;
    }

    if (R8_DVP_INT_FLAG & RB_DVP_IF_FIFO_OV) {
        R8_DVP_INT_FLAG = RB_DVP_IF_FIFO_OV;
        printf("FIFO overflow\r\n");
    }
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);

    // Init camera sensor (OV2640 or similar)
    // OV2640_Init();
    // JPEG_Mode_Init();

    DVP_Init_JPEG();

    while(1);
}
```

## RGB565 Mode

For RGB565 capture, configure rows and columns for the full frame:

```c
#define RGB565_ROWS  240
#define RGB565_COLS  320

void DVP_Init_RGB565(void)
{
    R8_DVP_CR0 = RB_DVP_D8_MOD | RB_DVP_V_POLAR;  // No RB_DVP_JPEG
    R8_DVP_CR1 = 0;
    R8_DVP_CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);

    R16_DVP_ROW_NUM = RGB565_ROWS;
    R16_DVP_COL_NUM = RGB565_COLS;

    R32_DVP_DMA_BUF0 = 0x20020000;
    R32_DVP_DMA_BUF1 = 0x20020000 + RGB565_COL_NUM;

    R8_DVP_INT_EN = RB_DVP_IE_FRM_DONE | RB_DVP_IE_ROW_DONE |
                    RB_DVP_IE_STR_FRM | RB_DVP_IE_FIFO_OV;
    PFIC_EnableIRQ(DVP_IRQn);

    R8_DVP_CR1 |= RB_DVP_DMA_EN;
    R8_DVP_CR0 |= RB_DVP_ENABLE;
}
```

## JPEG Frame Detection

In JPEG mode, a valid JPEG frame starts with bytes `0xFF 0xD8` and ends with
`0xFF 0xD9`. Use these markers to validate captured data:

```c
// Check JPEG start marker
if (ramx_buf[0] == 0xFF && ramx_buf[1] == 0xD8) {
    // Valid JPEG start
}

// Check JPEG end marker (search from end of captured data)
if (ramx_buf[total - 2] == 0xFF && ramx_buf[total - 1] == 0xD9) {
    // Valid JPEG end
}
```

## Camera Sensor Initialization

The DVP example includes OV2640 camera initialization via I2C/SPI (SCCB).
The `ov.c` / `ov.h` files in the example handle:

1. Sensor reset and power-on sequence
2. Register configuration for the desired mode (RGB565 or JPEG)
3. Resolution and quality settings

```c
#include "ov.h"

// Initialize OV2640
while (OV2640_Init()) {
    printf("Camera init failed\r\n");
    mDelaymS(500);
}

// Switch to JPEG mode
JPEG_Mode_Init();

// Or switch to RGB565 mode
RGB565_Mode_Init();
```

## Notes

- DMA buffers (`R32_DVP_DMA_BUF0/1`) must be in RAMX (0x20020000+).
- In JPEG mode, `R16_DVP_ROW_NUM` is unused; only `R16_DVP_COL_NUM` defines
  the DMA transfer width per line.
- The double-buffered DMA alternates between BUF0 and BUF1 on each line completion.
  Advance both buffer addresses in the ROW_DONE ISR to fill the entire frame.
- FIFO overflow indicates the DMA cannot keep up with the pixel clock. Reduce
  resolution or increase system clock.
- The DVP peripheral clock must be enabled: `PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_DVP)`.

## Example Project

`CH569EVT/EVT/EXAM/DVP/` - OV2640 camera capture in RGB565 and JPEG modes.
