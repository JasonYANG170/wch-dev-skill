# LCD Display (LTDC)

## Overview

CH32H417 has an integrated LTDC (LCD-TFT Display Controller) supporting RGB888, RGB565, ARGB8888, and other pixel formats. It has 2 layers with alpha blending, CLUT (Color Look-Up Table), and color keying. Combined with DMA2D (GPHA), it's ideal for GUI frameworks like LVGL.

## Key API Functions

```c
void LTDC_DeInit(void);
void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct);
void LTDC_StructInit(LTDC_InitTypeDef* LTDC_InitStruct);
void LTDC_Cmd(FunctionalState NewState);
void LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct);
void LTDC_LayerCmd(LTDC_Layer_TypeDef* LTDC_Layerx, FunctionalState NewState);
void LTDC_LayerPosition(LTDC_Layer_TypeDef* LTDC_Layerx, uint16_t OffsetX, uint16_t OffsetY);
void LTDC_LayerAlpha(LTDC_Layer_TypeDef* LTDC_Layerx, uint8_t ConstantAlpha);
void LTDC_LayerAddress(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Address);
void LTDC_LayerSize(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Width, uint32_t Height);
void LTDC_LayerPixelFormat(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t PixelFormat);
void LTDC_ReloadConfig(uint32_t LTDC_Reload);
void LTDC_ColorKeyingConfig(LTDC_Layer_TypeDef* LTDC_Layerx, LTDC_ColorKeying_InitTypeDef* LTDC_colorkeying_InitStruct, FunctionalState NewState);
void LTDC_CLUTCmd(LTDC_Layer_TypeDef* LTDC_Layerx, FunctionalState NewState);
void LTDC_CLUTInit(LTDC_Layer_TypeDef* LTDC_Layerx, LTDC_CLUT_InitTypeDef* LTDC_CLUT_InitStruct);
void LTDC_ITConfig(uint32_t LTDC_IT, FunctionalState NewState);
FlagStatus LTDC_GetFlagStatus(uint32_t LTDC_FLAG);
```

## LTDC Initialization for 800x480 Display

```c
#include "ch32h417.h"
#include "ch32h417_ltdc.h"

// Frame buffer (must be in accessible RAM)
#define LCD_WIDTH  800
#define LCD_HEIGHT 480

// RGB565 frame buffer for Layer 1
uint16_t fb_layer1[LCD_WIDTH * LCD_HEIGHT] __attribute__((aligned(4)));

void LTDC_Init_800x480(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LTDC_InitTypeDef LTDC_InitStruct = {0};
    LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct = {0};

    // Enable LTDC clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_LTDC, ENABLE);

    // Configure GPIO pins for LTDC (parallel RGB interface)
    // Typically: R[7:0], G[7:0], B[7:0], HSYNC, VSYNC, DE, PCLK
    // ... GPIO AF configuration for all LTDC pins ...

    // LTDC timing configuration for 800x480 @ 60Hz
    LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;  // Active Low
    LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
    LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;  // Active High
    LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

    // Timing values for 800x480
    LTDC_InitStruct.LTDC_HorizontalSync = 48 - 1;
    LTDC_InitStruct.LTDC_VerticalSync = 1 - 1;
    LTDC_InitStruct.LTDC_AccumulatedHBP = 48 + 40 - 1;    // HSYNC + HBP
    LTDC_InitStruct.LTDC_AccumulatedVBP = 1 + 29 - 1;     // VSYNC + VBP
    LTDC_InitStruct.LTDC_AccumulatedActiveW = 48 + 40 + 800 - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveH = 1 + 29 + 480 - 1;
    LTDC_InitStruct.LTDC_TotalWidth = 48 + 40 + 800 + 40 - 1;   // +HFP
    LTDC_InitStruct.LTDC_TotalHeigh = 1 + 29 + 480 + 13 - 1;    // +VFP
    LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
    LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;
    LTDC_Init(&LTDC_InitStruct);

    // Layer 1 configuration
    LTDC_Layer_InitStruct.LTDC_HorizontalStart = 0;
    LTDC_Layer_InitStruct.LTDC_HorizontalStop = LCD_WIDTH - 1;
    LTDC_Layer_InitStruct.LTDC_VerticalStart = 0;
    LTDC_Layer_InitStruct.LTDC_VerticalStop = LCD_HEIGHT - 1;
    LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
    LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
    LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
    LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
    LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
    LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
    LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
    LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)fb_layer1;
    LTDC_Layer_InitStruct.LTDC_CFBLineLength = LCD_WIDTH * 2 + 3;  // 2 bytes/pixel + 3
    LTDC_Layer_InitStruct.LTDC_CFBPitch = LCD_WIDTH * 2;
    LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_HEIGHT;
    LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);
    LTDC_LayerCmd(LTDC_Layer1, ENABLE);

    // Reload and enable
    LTDC_ReloadConfig(LTDC_IMReload);
    LTDC_Cmd(ENABLE);
}
```

## Draw Pixel

```c
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if(x < LCD_WIDTH && y < LCD_HEIGHT)
    {
        fb_layer1[y * LCD_WIDTH + x] = color;
    }
}

// Fill screen
void LCD_FillScreen(uint16_t color)
{
    for(uint32_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    {
        fb_layer1[i] = color;
    }
}
```

## Pixel Formats

| Format | Description | Bytes/Pixel |
|--------|-------------|-------------|
| `LTDC_Pixelformat_ARGB8888` | 32-bit ARGB | 4 |
| `LTDC_Pixelformat_RGB888` | 24-bit RGB | 3 |
| `LTDC_Pixelformat_RGB565` | 16-bit RGB | 2 |
| `LTDC_Pixelformat_ARGB1555` | 16-bit ARGB | 2 |
| `LTDC_Pixelformat_ARGB4444` | 16-bit ARGB | 2 |
| `LTDC_Pixelformat_L8` | 8-bit Luminance | 1 |
| `LTDC_Pixelformat_AL44` | 8-bit Alpha+Luminance | 1 |
| `LTDC_Pixelformat_AL88` | 16-bit Alpha+Luminance | 2 |

## LVGL Integration

```c
// LVGL display driver flush callback
void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint16_t *buf = (uint16_t *)px_map;
    for(int y = area->y1; y <= area->y2; y++)
    {
        for(int x = area->x1; x <= area->x2; x++)
        {
            fb_layer1[y * LCD_WIDTH + x] = *buf++;
        }
    }
    lv_display_flush_ready(disp);
}

// Double buffer for LVGL
static uint16_t lvgl_buf1[LCD_WIDTH * 10] __attribute__((aligned(4)));
static uint16_t lvgl_buf2[LCD_WIDTH * 10] __attribute__((aligned(4)));

void LVGL_Init(void)
{
    lv_init();
    lv_display_t *disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_buffers(disp, lvgl_buf1, lvgl_buf2,
                           sizeof(lvgl_buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
}
```

## Alpha Blending (Dual Layer)

```c
// Layer 0: Background image
// Layer 1: Foreground with alpha

// Set Layer 1 alpha to 128 (50% transparency)
LTDC_LayerAlpha(LTDC_Layer1, 128);
LTDC_ReloadConfig(LTDC_IMReload);
```

## References

- See `CH32H417EVT/EVT/EXAM/LTDC/` for complete examples
- GPHA (DMA2D) for hardware-accelerated pixel operations
