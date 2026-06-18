# Recipe: SDIO SD Card

## When to Use
User wants to interface with SD cards or eMMC via the SDIO peripheral, read/write SD card sectors, or initialize SD card in SD mode.

## Availability
- CH32V307: SDIO peripheral (4-bit SD mode)
- CH32V407: SDIO peripheral (4-bit SD mode)
- CH32V103 / CH32V20x: No SDIO peripheral

## API Reference (from ch32v*_sdio.h)

### SDIO Init Structure
```c
typedef struct {
    uint32_t SDIO_ClockEdge;           // SDIO_ClockEdge_Rising/Falling
    uint32_t SDIO_ClockBypass;         // SDIO_ClockBypass_Enable/Disable
    uint32_t SDIO_ClockPowerSave;      // SDIO_ClockPowerSave_Enable/Disable
    uint32_t SDIO_BusWide;             // SDIO_BusWide_1b/4b/8b
    uint32_t SDIO_HardwareFlowControl; // SDIO_HardwareFlowControl_Enable/Disable
    uint8_t  SDIO_ClockDiv;            // 0-0xFF clock divider
} SDIO_InitTypeDef;
```

### SDIO Command Init Structure
```c
typedef struct {
    uint32_t SDIO_Argument;  // Command argument
    uint32_t SDIO_CmdIndex;  // Command index (0-63)
    uint32_t SDIO_Response;  // SDIO_Response_No/Short/Long
    uint32_t SDIO_Wait;      // SDIO_Wait_No/Interrupt
    uint32_t SDIO_CPSM;      // SDIO_CPSM_Enable/Disable
} SDIO_CmdInitTypeDef;
```

### SDIO Data Init Structure
```c
typedef struct {
    uint32_t SDIO_DataTimeOut;   // Data timeout in bus clock periods
    uint32_t SDIO_DataLength;    // Number of bytes to transfer
    uint32_t SDIO_DataBlockSize; // Block size (SDIO_DataBlockSize_1b..2048b)
    uint32_t SDIO_TransferDir;   // SDIO_TransferDir_ToCard/ToSDIO
    uint32_t SDIO_TransferMode;  // SDIO_TransferMode_Block/Stream
    uint32_t SDIO_DPSM;          // SDIO_DPSM_Enable/Disable
} SDIO_DataInitTypeDef;
```

### Key Functions (WCH SDIO driver)
```c
// High-level API (from sdio.c/h in example projects)
SD_Error SD_Init(void);
SD_Error SD_ReadDisk(u8 *buf, u32 sector, u8 cnt);
SD_Error SD_WriteDisk(u8 *buf, u32 sector, u8 cnt);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
SD_Error SD_SelectDeselect(u32 addr);
SD_Error SD_EnableWideBusOperation(u32 wmode);
SD_Error SD_SetDeviceMode(u32 mode);

// Low-level SDIO API
void SDIO_DeInit(void);
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct);
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct);
void SDIO_ClockCmd(FunctionalState NewState);
void SDIO_SetPowerState(uint32_t SDIO_PowerState);
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
void SDIO_SendDataState(FunctionalState NewState);
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct);
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP);
uint32_t SDIO_GetDataCounter(void);
uint32_t SDIO_ReadData(void);
void SDIO_WriteData(uint32_t Data);
void SDIO_DMACmd(FunctionalState NewState);
void SDIO_Clock_Set(u8 clkdiv);
```

### SDIO Pin Mapping (CH32V407)
| Signal | Pin |
|--------|-----|
| CLK | PC12 |
| CMD | PD2 |
| D0 | PB14 |
| D1 | PB15 |
| D2 | PC10 |
| D3 | PC11 |

### SD Card Types
| Constant | Description |
|----------|-------------|
| `SDIO_STD_CAPACITY_SD_CARD_V1_1` | SDSC v1.1 |
| `SDIO_STD_CAPACITY_SD_CARD_V2_0` | SDSC v2.0 |
| `SDIO_HIGH_CAPACITY_SD_CARD` | SDHC v2.0 |
| `SDIO_MULTIMEDIA_CARD` | MMC card |

### SD Operation Modes
| Constant | Description |
|----------|-------------|
| `SD_POLLING_MODE` | Polling mode (CPU waits) |
| `SD_DMA_MODE` | DMA mode (recommended) |

## Example: SD Card Init and Read/Write

```c
#include "ch32v4x7.h"
#include "sdio.h"
#include "string.h"

u8 write_buf[512];
u8 read_buf[512];

void SD_Card_Test(void)
{
    u32 i;
    SD_Error status;

    // Initialize SD card (includes GPIO, SDIO, card detection, wide bus)
    while(SD_Init() != SD_OK)
    {
        printf("SD Card Init Error!\r\n");
        Delay_Ms(1000);
    }

    // Print card info
    printf("Card Type: %d\r\n", SDCardInfo.CardType);
    printf("Card Capacity: %d MB\r\n", (u32)(SDCardInfo.CardCapacity >> 20));
    printf("Card BlockSize: %d\r\n", SDCardInfo.CardBlockSize);

    // Fill write buffer
    for(i = 0; i < 512; i++)
        write_buf[i] = i & 0xFF;

    // Write and verify sector 0
    status = SD_WriteDisk(write_buf, 0, 1);
    if(status != SD_OK)
    {
        printf("Write Error: %d\r\n", status);
        return;
    }

    // Read back
    status = SD_ReadDisk(read_buf, 0, 1);
    if(status != SD_OK)
    {
        printf("Read Error: %d\r\n", status);
        return;
    }

    // Verify
    if(memcmp(write_buf, read_buf, 512) == 0)
        printf("Verify OK\r\n");
    else
        printf("Verify FAILED\r\n");
}
```

## Example: SDIO GPIO Configuration

```c
void SDIO_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC |
                          RCC_PB2Periph_GPIOD | RCC_PB2Periph_AFIO, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_SDIO, ENABLE);

    // CLK - PC12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // D0, D1 - PB14, PB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CMD - PD2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Remap SDIO pins (if needed)
    AFIO->PCFR2 |= 1 << 12;
}
```

## Pitfalls
- **SDIO clock on HB bus** -- Use `RCC_HBPeriph_SDIO` (not APB) for CH32V307/V407
- **AFIO remap required** -- Some pin configurations require `AFIO->PCFR2 |= 1 << 12` for SDIO pin remap
- **Card detection** -- Always check `SD_Init()` return value; retry if card not inserted
- **Sector size** -- SD cards use 512-byte sectors; eMMC may use different block sizes
- **Wide bus** -- 4-bit mode requires `SD_EnableWideBusOperation(1)` after card init
- **DMA mode** -- Use `SD_SetDeviceMode(SD_DMA_MODE)` for better performance; polling mode blocks CPU
- **Alignment** -- SDIO data buffer must be 4-byte aligned: `__attribute__((aligned(4))) u8 buf[512];`
