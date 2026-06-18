# DVP Camera Interface

## Overview

CH32H417 has a Digital Video Port (DVP) for connecting parallel camera sensors (e.g., OV2640). Supports 8-bit data width, configurable HSYNC/VSYNC polarity, JPEG mode with dual DMA buffers, and interrupt-driven frame capture.

## Key API Functions

```c
void DVP_DeInit(void);
void DVP_Init(DVP_InitTypeDef* DVP_InitStruct);
void DVP_Cmd(FunctionalState NewState);
void DVP_DMACmd(FunctionalState NewState);
void DVP_ReceiveCircuitResetCmd(FunctionalState NewState);
void DVP_FIFO_ResetCmd(FunctionalState NewState);
void DVP_ITConfig(uint32_t DVP_IT, FunctionalState NewState);
ITStatus DVP_GetITStatus(uint32_t DVP_IT);
void DVP_ClearITPendingBit(uint32_t DVP_IT);
```

## DVP Initialization Example (JPEG Mode with OV2640)

```c
#include "ch32h417.h"

// DMA buffers must be 32-byte aligned
__attribute__((aligned(32))) uint32_t JPEG_DVPDMAaddr0 = 0x2011A000;
__attribute__((aligned(32))) uint32_t JPEG_DVPDMAaddr1 = 0x2011A000 + OV2640_JPEG_WIDTH;

volatile uint32_t frame_cnt = 0;
volatile uint32_t href_cnt = 0;

void DVP_Function_Init(void)
{
    DVP_InitTypeDef DVP_InitStructure = {0};

    // Enable DVP clock
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DVP, ENABLE);

    DVP_DeInit();

    DVP_InitStructure.DVP_DataSize = DVP_DataSize_8b;
    DVP_InitStructure.DVP_COL_NUM = OV2640_JPEG_WIDTH;  // Image width in bytes
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
    DVP_InitStructure.DVP_DMA_BUF0_Addr = JPEG_DVPDMAaddr0;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = JPEG_DVPDMAaddr1;
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_25P;
    DVP_InitStructure.DVP_JPEGMode = ENABLE;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    // Enable DVP interrupts
    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);
    DVP_ITConfig(DVP_IT_STR_FRM | DVP_IT_ROW_DONE | DVP_IT_FRM_DONE |
                 DVP_IT_FIFO_OV | DVP_IT_STP_FRM, ENABLE);

    DVP_DMACmd(ENABLE);
    DVP_Cmd(ENABLE);
}
```

## DVP Interrupt Handler

```c
void DVP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void DVP_IRQHandler(void)
{
    if (DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);
        href_cnt++;
        // Advance DMA buffer address for next row
        if (href_cnt % 2)
            DVP->DMA_BUF1 += OV2640_JPEG_WIDTH * 2;
        else
            DVP->DMA_BUF0 += OV2640_JPEG_WIDTH * 2;
    }

    if (DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);
        DVP_Cmd(DISABLE);

        // Process frame data from memory at 0x2011A000
        // Total size = href_cnt * OV2640_JPEG_WIDTH

        // Reset DMA buffers for next frame
        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;
        href_cnt = 0;
        DVP_Cmd(ENABLE);
    }

    if (DVP_GetITStatus(DVP_IT_STR_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_STP_FRM) != RESET)
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);

    if (DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET)
        DVP_ClearITPendingBit(DVP_IT_FIFO_OV);
}
```

## Camera Initialization (OV2640)

```c
#include "ov2640.h"

void Camera_Init(void)
{
    // Initialize OV2640 via I2C (SCCB)
    while (OV2640_Init())
    {
        printf("Camera Model Error\r\n");
        Delay_Ms(1000);
    }
    Delay_Ms(1000);

    // Set to JPEG mode
    JPEG_Mode_Init();
    Delay_Ms(1000);
}
```

## DVP Interrupt Flags

| Flag | Description |
|------|-------------|
| `DVP_IT_STR_FRM` | Start of frame |
| `DVP_IT_ROW_DONE` | Row (HREF) complete |
| `DVP_IT_FRM_DONE` | Frame complete |
| `DVP_IT_FIFO_OV` | FIFO overflow |
| `DVP_IT_STP_FRM` | End of frame |

## DVP Data Size Options

| Macro | Description |
|-------|-------------|
| `DVP_DataSize_8b` | 8-bit data bus |
| `DVP_DataSize_10b` | 10-bit data bus |
| `DVP_DataSize_12b` | 12-bit data bus |
