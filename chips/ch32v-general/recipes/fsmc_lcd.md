# Recipe: FSMC LCD

## When to Use
User wants to drive a TFT LCD via the FSMC (Flexible Static Memory Controller) parallel interface, access external SRAM/NOR Flash, or use memory-mapped LCD registers.

## Availability
- CH32V307: FSMC with NORSRAM, NAND, PC Card support
- CH32V407: FSMC with NORSRAM, NAND, PC Card support
- CH32V103 / CH32V20x: No FSMC peripheral

## API Reference (from ch32v*_fsmc.h)

### NORSRAM Timing Init Structure
```c
typedef struct {
    uint32_t FSMC_AddressSetupTime;       // 0-0xF HCLK cycles
    uint32_t FSMC_AddressHoldTime;        // 0-0xF HCLK cycles
    uint32_t FSMC_DataSetupTime;          // 0-0xFF HCLK cycles
    uint32_t FSMC_BusTurnAroundDuration;  // 0-0xF HCLK cycles
    uint32_t FSMC_CLKDivision;            // 1-0xF (for sync access)
    uint32_t FSMC_DataLatency;            // 0-0xF (for sync access)
    uint32_t FSMC_AccessMode;             // FSMC_AccessMode_A/B/C/D
} FSMC_NORSRAMTimingInitTypeDef;
```

### NORSRAM Init Structure
```c
typedef struct {
    uint32_t FSMC_Bank;                // FSMC_Bank1_NORSRAM1 .. _4
    uint32_t FSMC_DataAddressMux;      // FSMC_DataAddressMux_Enable/Disable
    uint32_t FSMC_MemoryType;          // FSMC_MemoryType_SRAM/PSRAM/NOR
    uint32_t FSMC_MemoryDataWidth;     // FSMC_MemoryDataWidth_8b/16b
    uint32_t FSMC_BurstAccessMode;     // FSMC_BurstAccessMode_Enable/Disable
    uint32_t FSMC_WaitSignalPolarity;  // FSMC_WaitSignalPolarity_Low/High
    uint32_t FSMC_AsynchronousWait;    // FSMC_AsynchronousWait_Enable/Disable
    uint32_t FSMC_WaitSignalActive;    // FSMC_WaitSignalActive_BeforeWaitState/DuringWaitState
    uint32_t FSMC_WriteOperation;      // FSMC_WriteOperation_Enable/Disable
    uint32_t FSMC_WaitSignal;          // FSMC_WaitSignal_Enable/Disable
    uint32_t FSMC_ExtendedMode;        // FSMC_ExtendedMode_Enable/Disable
    uint32_t FSMC_WriteBurst;          // FSMC_WriteBurst_Enable/Disable
    FSMC_NORSRAMTimingInitTypeDef *FSMC_ReadWriteTimingStruct;
    FSMC_NORSRAMTimingInitTypeDef *FSMC_WriteTimingStruct;
} FSMC_NORSRAMInitTypeDef;
```

### Key Functions
```c
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef *FSMC_NORSRAMInitStruct);
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);
void FSMC_NANDDeInit(uint32_t FSMC_Bank);
void FSMC_NANDInit(FSMC_NANDInitTypeDef *FSMC_NANDInitStruct);
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState);
```

### FSMC Banks
| Constant | Base Address | Description |
|----------|-------------|-------------|
| `FSMC_Bank1_NORSRAM1` | 0x60000000 | NOR/SRAM Bank 1 |
| `FSMC_Bank1_NORSRAM2` | 0x64000000 | NOR/SRAM Bank 2 |
| `FSMC_Bank1_NORSRAM3` | 0x68000000 | NOR/SRAM Bank 3 |
| `FSMC_Bank1_NORSRAM4` | 0x6C000000 | NOR/SRAM Bank 4 |

### FSMC Access Modes
| Constant | Description |
|----------|-------------|
| `FSMC_AccessMode_A` | Mode A: SRAM/ROM (most common for LCD) |
| `FSMC_AccessMode_B` | Mode B: NOR Flash (async) |
| `FSMC_AccessMode_C` | Mode C: NOR Flash (async, wait) |
| `FSMC_AccessMode_D` | Mode D: NOR Flash (async, mux) |

## Example: FSMC LCD Init (16-bit parallel)

LCD typically uses NORSRAM1 with RS (register select) on an address line.

```c
#include "ch32v4x7.h"

// LCD register/data access via memory mapping
// RS connected to PD12 (A17 for Bank1 -> offset 0x20000)
#define LCD_BASE    ((u32)(0x60000000 | (1 << 17)))  // A17 for RS
#define LCD         ((LCD_TypeDef *)LCD_BASE)

typedef struct {
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;

void FSMC_LCD_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef writeTiming;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clocks
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO | RCC_PB2Periph_GPIOB |
                          RCC_PB2Periph_GPIOD | RCC_PB2Periph_GPIOE, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_FSMC, ENABLE);

    // Configure GPIO pins for FSMC
    // NOE (RD) - PD4, NWE (WR) - PD5, NE1 (CS) - PD7, RS - PD12
    // D0-D15 on PD0,PD1,PD8-10,PD14,PD15, PE7-PE11, PB11-PB14
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    // RD - PD4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // WR - PD5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // CS - PD7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // RS - PD12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // Data pins D0-D3: PD0, PD1, PD14, PD15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // Data pins D4-D8: PE7-PE11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    // Data pins D9-D12: PB11-PB14
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // Data pins D13-D15: PD8-PD10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Read timing
    readWriteTiming.FSMC_AddressSetupTime = 0x0F;
    readWriteTiming.FSMC_AddressHoldTime = 0x03;
    readWriteTiming.FSMC_DataSetupTime = 0x0F;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    // Write timing (faster for LCD)
    writeTiming.FSMC_AddressSetupTime = 0x00;
    writeTiming.FSMC_AddressHoldTime = 0x00;
    writeTiming.FSMC_DataSetupTime = 0x03;
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

// Write LCD register
void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG = regval;
}

// Write LCD data
void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM = data;
}

// Write register with value
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;
    LCD->LCD_RAM = LCD_RegValue;
}
```

## Pitfalls
- **Address line for RS** -- RS (register select) must be connected to an FSMC address line; the offset depends on which address line (A17 -> offset 0x20000, A16 -> 0x10000)
- **Clock enable order** -- Enable `RCC_HBPeriph_FSMC` (HB bus, not APB) for CH32V307/V407
- **GPIO mode** -- All FSMC pins must be `GPIO_Mode_AF_PP` (alternate function push-pull)
- **16-bit vs 8-bit** -- `FSMC_MemoryDataWidth_16b` for 16-bit parallel LCD; data pins must match
- **Write timing** -- LCD write can use faster timing than read; use extended mode for separate read/write timing
