# GPHA Graphics Accelerator

## Overview

CH32H417 has a GPHA (Graphics Hardware Accelerator, also known as DMA2D) for hardware-accelerated 2D graphics operations. Supports Register-to-Memory (R2M) solid color fill and Memory-to-Memory (M2M) pixel format conversion with alpha blending. Much faster than CPU-based fills.

## Key API Functions

```c
void GPHA_DeInit(void);
void GPHA_Init(GPHA_InitTypeDef* GPHA_InitStruct);
void GPHA_StartTransfer(void);
FlagStatus GPHA_GetFlagStatus(uint32_t GPHA_FLAG);
void GPHA_ClearFlag(uint32_t GPHA_FLAG);
void GPHA_FGInit(GPHA_FG_InitTypeDef* GPHA_FG_InitStruct);
void GPHA_BGInit(GPHA_BG_InitTypeDef* GPHA_BG_InitStruct);
```

## Register-to-Memory (R2M) Solid Fill

```c
#include "ch32h417.h"

void GPHA_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                   uint32_t color, uint32_t origin_width, uint32_t origin_height,
                   uint8_t *image)
{
    GPHA_InitTypeDef GPHA_InitStruct = {0};

    uint32_t alpha = (color >> 24) & 0xff;
    uint32_t red   = (color >> 16) & 0xff;
    uint32_t green = (color >> 8) & 0xff;
    uint32_t blue  = color & 0xff;

    uint32_t offset  = 4 * (origin_width * y + x);
    uint32_t address = (uint32_t)image + offset;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPHA, ENABLE);

    GPHA_InitStruct.GPHA_Mode            = GPHA_R2M;
    GPHA_InitStruct.GPHA_CMode           = GPHA_ARGB8888;
    GPHA_InitStruct.GPHA_OutputGreen     = green;
    GPHA_InitStruct.GPHA_OutputBlue      = blue;
    GPHA_InitStruct.GPHA_OutputRed       = red;
    GPHA_InitStruct.GPHA_OutputAlpha     = alpha;
    GPHA_InitStruct.GPHA_OutputMemoryAdd = address;
    GPHA_InitStruct.GPHA_OutputOffset    = origin_width - width;
    GPHA_InitStruct.GPHA_NumberOfLine    = height;
    GPHA_InitStruct.GPHA_PixelPerLine    = width;

    GPHA_Init(&GPHA_InitStruct);
    GPHA_StartTransfer();

    while (GPHA_GetFlagStatus(GPHA_FLAG_TC) == RESET);
    GPHA_ClearFlag(GPHA_FLAG_TC);
}
```

## Performance Comparison

```c
#include "ch32h417.h"

#define WIDTH  200
#define HEIGHT 200

uint32_t framebuffer[WIDTH * HEIGHT];

void Fill_Comparison(void)
{
    // Software fill (slow)
    for (size_t i = 0; i < HEIGHT; i++)
        for (size_t j = 0; j < WIDTH; j++)
            framebuffer[i * WIDTH + j] = 0xff00ff00;

    // GPHA fill (fast - hardware accelerated)
    GPHA_DrawRect(0, 0, WIDTH, HEIGHT, 0xff00ff00,
                  WIDTH, HEIGHT, (uint8_t*)framebuffer);
}
```

## GPHA Operating Modes

| Macro | Description |
|-------|-------------|
| `GPHA_R2M` | Register to Memory (solid fill) |
| `GPHA_M2M` | Memory to Memory (copy/convert) |
| `GPHA_M2M_PFC` | Memory to Memory with Pixel Format Conversion |
| `GPHA_M2M_Blending` | Memory to Memory with blending |

## Pixel Formats

| Macro | Description |
|-------|-------------|
| `GPHA_ARGB8888` | 32-bit ARGB |
| `GPHA_RGB888` | 24-bit RGB |
| `GPHA_RGB565` | 16-bit RGB |
| `GPHA_ARGB1555` | 16-bit ARGB |
| `GPHA_ARGB4444` | 16-bit ARGB |

## GPHA Flags

| Flag | Description |
|------|-------------|
| `GPHA_FLAG_TC` | Transfer Complete |
| `GPHA_FLAG_TW` | Transfer Watermark |
| `GPHA_FLAG_CE` | Configuration Error |

## Important Notes

- Enable clock: `RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPHA, ENABLE)`
- Wait for `GPHA_FLAG_TC` before accessing the output buffer
- For M2M mode, configure foreground/background layers with `GPHA_FGInit()` / `GPHA_BGInit()`
- Useful for LVGL display rendering acceleration
