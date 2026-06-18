# Recipe: SPI Communication

## Overview

Configure SPI0 and SPI1 for master/slave communication on CH56x chips.
Supports polling, interrupt, and DMA transfer modes.

## Available SPI Instances

| SPI | Pins (default) | Notes |
|-----|----------------|-------|
| SPI0 | SCK, MISO, MOSI, CS | Primary SPI |
| SPI1 | SCK, MISO, MOSI, CS | Secondary SPI |

## SPI Data Modes

```c
typedef enum {
    Mode0_LowBitINFront = 0,  // CPOL=0 CPHA=0, LSB first
    Mode0_HighBitINFront,     // CPOL=0 CPHA=0, MSB first
    Mode3_LowBitINFront,      // CPOL=1 CPHA=1, LSB first
    Mode3_HighBitINFront,     // CPOL=1 CPHA=1, MSB first
} ModeBitOrderTypeDef;
```

## SPI0 Master Mode

### Basic Initialization

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_SPI0);

    // Default master init (Mode0, MSB first)
    SPI0_MasterDefInit();

    // Set data mode
    SPI0_DataMode(Mode0_HighBitINFront);  // CPOL=0, CPHA=0, MSB first

    while(1) {}
}
```

### Single Byte Transfer

```c
// Send single byte
SPI0_MasterSendByte(0xAA);

// Receive single byte (sends 0xFF to clock data in)
UINT8 rx = SPI0_MasterRecvByte();
```

### Multi-Byte Transfer (Polling)

```c
UINT8 txBuf[] = {0x01, 0x02, 0x03, 0x04};
UINT8 rxBuf[4];

// Send multiple bytes
SPI0_MasterTrans(txBuf, 4);

// Receive multiple bytes (sends 0xFF for each clock)
SPI0_MasterRecv(rxBuf, 4);
```

### DMA Transfer

```c
// DMA send (buffer must be accessible by DMA)
UINT8 txData[] = {0x55, 0xAA, 0x55, 0xAA};
SPI0_MasterDMATrans(txData, 4);

// DMA receive
UINT8 rxData[4];
SPI0_MasterDMARecv(rxData, 4);
```

## SPI0 Slave Mode

```c
// Initialize as slave
SPI0_SlaveInit();

// Set first data byte to send when master reads
SetFirst0Data(0xAB);

// Send data to master
UINT8 txBuf[] = {0x10, 0x20, 0x30};
SPI0_SlaveTrans(txBuf, 3);

// Receive data from master
UINT8 rxBuf[16];
SPI0_SlaveRecv(rxBuf, 16);
```

## SPI1 (Same API Pattern)

```c
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_SPI1);

// Master
SPI1_MasterDefInit();
SPI1_DataMode(Mode0_HighBitINFront);
SPI1_MasterSendByte(0x42);
UINT8 rx = SPI1_MasterRecvByte();
SPI1_MasterTrans(buf, len);
SPI1_MasterDMATrans(buf, len);

// Slave
SPI1_SlaveInit();
SetFirst1Data(0xCD);
SPI1_SlaveTrans(buf, len);
SPI1_SlaveRecv(buf, len);
```

## SPI Interrupt Flags

```c
// Enable/disable interrupt
SPI0_ITCfg(ENABLE, SPI0_IT_DMA_END);   // DMA transfer complete
SPI0_ITCfg(ENABLE, SPI0_IT_BYTE_END);  // Byte transfer complete
SPI0_ITCfg(ENABLE, SPI0_IT_CNT_END);   // Counter end
SPI0_ITCfg(ENABLE, SPI0_IT_FIFO_HF);   // FIFO half full
SPI0_ITCfg(ENABLE, SPI0_IT_FIFO_OV);   // FIFO overflow

// Check and clear flags
if (SPI0_GetITFlag(SPI0_IT_DMA_END)) {
    SPI0_ClearITFlag(SPI0_IT_DMA_END);
    // Handle DMA complete
}
```

### Available Interrupt Flags

| Flag | Description |
|------|-------------|
| `SPI0_IT_FST_BYTE` | First byte received |
| `SPI0_IT_FIFO_OV` | FIFO overflow |
| `SPI0_IT_DMA_END` | DMA transfer end |
| `SPI0_IT_FIFO_HF` | FIFO half full |
| `SPI0_IT_BYTE_END` | Byte transfer end |
| `SPI0_IT_CNT_END` | Counter end |

## Complete Example: SPI Flash Read ID

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_SPI0);
    SPI0_MasterDefInit();
    SPI0_DataMode(Mode0_HighBitINFront);

    // Read SPI Flash JEDEC ID (command 0x9F)
    GPIOA_ResetBits(GPIO_Pin_4);  // CS low
    SPI0_MasterSendByte(0x9F);    // Send command
    UINT8 mfg  = SPI0_MasterRecvByte();  // Manufacturer ID
    UINT8 type = SPI0_MasterRecvByte();  // Memory type
    UINT8 cap  = SPI0_MasterRecvByte();  // Capacity
    GPIOA_SetBits(GPIO_Pin_4);    // CS high

    printf("Flash ID: %02X %02X %02X\n", mfg, type, cap);

    while(1) {}
}
```

## Notes

- SPI chip select (CS) is typically managed manually via GPIO
- For DMA transfers, buffer should ideally be in RAMX (0x20020000+)
- SPI clock frequency is derived from system clock with configurable divider
