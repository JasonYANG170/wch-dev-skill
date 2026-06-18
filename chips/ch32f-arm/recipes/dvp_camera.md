# Recipe: DVP Camera Interface

## Overview

Configure the Digital Video Port (DVP) interface on CH32F20x to capture image data from cameras like OV2640. Supports JPEG and RGB565 modes with DMA double-buffering.

**Availability**: CH32F20x only (not available on CH32F10x or CH32M030).

## Key Concepts

- DVP is a parallel camera interface that captures 8-bit or 10-bit data from image sensors
- Uses DMA double-buffering (DMA_BUF0 / DMA_BUF1) for continuous frame capture
- Interrupts signal row done, frame done, start/stop frame, and FIFO overflow
- Camera sensor (e.g., OV2640) is configured via I2C (SCCB) before DVP capture begins

## DVP Register Access

DVP is controlled via direct register access (no StdPeriphDriver library):

```c
// Enable DVP clock
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);

// Configure DVP mode
DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;      // Clear data mode bits
DVP->CR0 |= RB_DVP_D8_MOD | RB_DVP_V_POLAR;  // 8-bit data, VSYNC high active
DVP->CR0 |= RB_DVP_JPEG;               // JPEG mode (optional)

DVP->CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);  // Clear status

// Set image dimensions
DVP->COL_NUM = width;                   // Columns per row (JPEG: buffer width)
DVP->ROW_NUM = height;                  // Rows per frame (RGB565 mode)

// Set DMA buffers (must be in RAM)
DVP->DMA_BUF0 = buffer0_addr;
DVP->DMA_BUF1 = buffer1_addr;

// Set frame capture rate
DVP->CR1 &= ~RB_DVP_FCRC;
DVP->CR1 |= DVP_RATE_100P;             // 100% capture rate

// Enable interrupts
DVP->IER |= RB_DVP_IE_STP_FRM;        // Stop frame interrupt
DVP->IER |= RB_DVP_IE_FIFO_OV;        // FIFO overflow interrupt
DVP->IER |= RB_DVP_IE_FRM_DONE;       // Frame done interrupt
DVP->IER |= RB_DVP_IE_ROW_DONE;       // Row done interrupt
DVP->IER |= RB_DVP_IE_STR_FRM;        // Start frame interrupt

// Enable DMA and DVP
DVP->CR1 |= RB_DVP_DMA_EN;
DVP->CR0 |= RB_DVP_ENABLE;
```

## DVP Interrupt Flags

| Flag | Description |
|------|-------------|
| RB_DVP_IF_STR_FRM | Start of frame detected |
| RB_DVP_IF_STP_FRM | End of frame detected |
| RB_DVP_IF_FRM_DONE | Frame capture complete |
| RB_DVP_IF_ROW_DONE | Row (line) capture complete |
| RB_DVP_IF_FIFO_OV | DMA FIFO overflow |

## DVP Capture Rate

| Constant | Rate |
|----------|------|
| DVP_RATE_100P | Capture every frame |
| DVP_RATE_50P | Capture every 2nd frame |
| DVP_RATE_25P | Capture every 4th frame |

## Pin Mapping (OV2640)

| DVP Signal | Pin | Description |
|------------|-----|-------------|
| D0-D7 | PA9, PA10, PC8, PC9, PC10, PC11, PB6, PB8 | Data bus |
| D8-D11 | PB9, PC12, PD2, PD6 | Extended data (JPEG mode) |
| DPCLK | PA6 | Pixel clock input |
| HERF | PA4 | Horizontal reference |
| SYNC | PA5 | Vertical sync (VSYNC) |
| SDA | PB11 | I2C data (SCCB) |
| SDCLK | PB10 | I2C clock (SCCB) |
| DPWDN | PC3 | Camera power down |
| RESET | PC13 | Camera reset |

## Example: JPEG Mode Capture with UART Output

```c
#include "debug.h"
#include "ov.h"

#define JPEG_MODE  1

UINT32 JPEG_DVPDMAaddr0 = 0x20005000;
UINT32 JPEG_DVPDMAaddr1 = 0x20005000 + OV2640_JPEG_WIDTH;

volatile UINT32 frame_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 href_cnt = 0;

void UART2_Send_Byte(u8 t)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, t);
}

void DVP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);

    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;

    // VSYNC/HSYNC high active, 8-bit mode, JPEG mode
    DVP->CR0 |= RB_DVP_D8_MOD | RB_DVP_V_POLAR | RB_DVP_JPEG;
    DVP->CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);

    DVP->COL_NUM = OV2640_JPEG_WIDTH;
    DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;
    DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;

    // Capture rate 25%
    DVP->CR1 &= ~RB_DVP_FCRC;
    DVP->CR1 |= DVP_RATE_25P;

    // Enable all interrupts
    DVP->IER |= RB_DVP_IE_STP_FRM | RB_DVP_IE_FIFO_OV |
                RB_DVP_IE_FRM_DONE | RB_DVP_IE_ROW_DONE |
                RB_DVP_IE_STR_FRM;

    NVIC_InitStructure.NVIC_IRQChannel = DVP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DVP->CR1 |= RB_DVP_DMA_EN;
    DVP->CR0 |= RB_DVP_ENABLE;
}

void DVP_IRQHandler(void)
{
    if(DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        DVP->IFR &= ~RB_DVP_IF_ROW_DONE;
        href_cnt++;
        if(addr_cnt % 2) {
            addr_cnt++;
            DVP->DMA_BUF1 += OV2640_JPEG_WIDTH * 2;
        } else {
            addr_cnt++;
            DVP->DMA_BUF0 += OV2640_JPEG_WIDTH * 2;
        }
    }

    if(DVP->IFR & RB_DVP_IF_FRM_DONE)
    {
        DVP->IFR &= ~RB_DVP_IF_FRM_DONE;
        DVP->CR0 &= ~RB_DVP_ENABLE;

        // Send JPEG data via UART2
        UINT32 i;
        href_cnt = href_cnt * OV2640_JPEG_WIDTH;
        for(i = 0; i < href_cnt; i++) {
            UART2_Send_Byte(*(UINT8*)(0x20005000 + i));
        }

        // Reset DMA buffers
        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;
        href_cnt = 0;
        addr_cnt = 0;
        DVP->CR0 |= RB_DVP_ENABLE;
    }

    if(DVP->IFR & RB_DVP_IF_STR_FRM)
    {
        DVP->IFR &= ~RB_DVP_IF_STR_FRM;
        frame_cnt++;
    }

    if(DVP->IFR & RB_DVP_IF_STP_FRM)
        DVP->IFR &= ~RB_DVP_IF_STP_FRM;

    if(DVP->IFR & RB_DVP_IF_FIFO_OV)
        DVP->IFR &= ~RB_DVP_IF_FIFO_OV;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    while(OV2640_Init()) {
        printf("Camera Model Err\r\n");
        Delay_Ms(1000);
    }

    RGB565_Mode_Init();
    Delay_Ms(1000);
    JPEG_Mode_Init();
    Delay_Ms(1000);

    DVP_Init();

    while(1);
}
```

## Example: RGB565 Mode with LCD Display

For RGB565 mode, configure DVP with `ROW_NUM` and `COL_NUM` for the image dimensions, and use DMA to transfer pixel data to the FSMC-connected LCD:

```c
void DVP_Init_RGB565(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);

    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;
    DVP->CR0 |= RB_DVP_D8_MOD | RB_DVP_V_POLAR;  // No JPEG flag
    DVP->CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);

    DVP->ROW_NUM = RGB565_ROW_NUM;
    DVP->COL_NUM = RGB565_COL_NUM;
    DVP->DMA_BUF0 = RGB565_DVPDMAaddr0;
    DVP->DMA_BUF1 = RGB565_DVPDMAaddr1;

    DVP->CR1 &= ~RB_DVP_FCRC;
    DVP->CR1 |= DVP_RATE_100P;

    // Enable interrupts and DMA as above...
    DVP->CR1 |= RB_DVP_DMA_EN;
    DVP->CR0 |= RB_DVP_ENABLE;
}
```

## Pitfalls

- **UART1 conflict**: DVP uses PA9/PA10 (D0/D1), which conflicts with UART1. Use UART2 for debug output and set `#define DEBUG DEBUG_UART2` in debug.h.
- **DMA buffer alignment**: DMA buffers must be in RAM and properly aligned. Use addresses like `0x20005000`.
- **FIFO overflow**: If processing is too slow, FIFO overflow occurs. Use DMA double-buffering and process data during the next row capture.
- **Camera I2C init first**: Always initialize the camera sensor (OV2640) via I2C/SCCB before enabling DVP capture.
- **JPEG frame extraction**: In JPEG mode, extract data starting from 0xFF 0xD8 and ending at 0xFF 0xD9.

## Related Examples

- `CH32F20xEVT/EVT/EXAM/DVP/DVP_UART` - JPEG capture with UART output
- `CH32F20xEVT/EVT/EXAM/DVP/DVP_TFTLCD` - RGB565 capture with LCD display
