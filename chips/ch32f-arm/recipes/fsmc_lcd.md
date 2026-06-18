# Recipe: FSMC LCD and Memory

## Overview

Configure the Flexible Static Memory Controller (FSMC) on CH32F20x to interface with TFT LCD displays, external SRAM, and NAND Flash. FSMC maps external devices into the ARM memory space for direct read/write access.

**Availability**: CH32F20x only (not available on CH32F10x or CH32M030).

## Key API Functions

```c
// Initialization
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState);
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState);
uint32_t FSMC_GetECC(uint32_t FSMC_Bank);
```

## FSMC NORSRAM Init Structure

```c
typedef struct {
    uint32_t FSMC_Bank;                // FSMC_Bank1_NORSRAM1..4
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
    FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct;
    FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;
} FSMC_NORSRAMInitTypeDef;
```

## FSMC Timing Init Structure

```c
typedef struct {
    uint32_t FSMC_AddressSetupTime;       // 0-15 HCLK cycles
    uint32_t FSMC_AddressHoldTime;        // 0-15 HCLK cycles
    uint32_t FSMC_DataSetupTime;          // 0-255 HCLK cycles
    uint32_t FSMC_BusTurnAroundDuration;  // 0-15 HCLK cycles
    uint32_t FSMC_CLKDivision;            // 0-15
    uint32_t FSMC_DataLatency;            // 0-15
    uint32_t FSMC_AccessMode;             // FSMC_AccessMode_A/B/C/D
} FSMC_NORSRAMTimingInitTypeDef;
```

## FSMC Bank Addresses

| Bank | Base Address | Description |
|------|-------------|-------------|
| FSMC_Bank1_NORSRAM1 | 0x60000000 | NOR/SRAM bank 1 |
| FSMC_Bank1_NORSRAM2 | 0x64000000 | NOR/SRAM bank 2 |
| FSMC_Bank1_NORSRAM3 | 0x68000000 | NOR/SRAM bank 3 |
| FSMC_Bank1_NORSRAM4 | 0x6C000000 | NOR/SRAM bank 4 |
| FSMC_Bank2_NAND | 0x70000000 | NAND bank 2 |
| FSMC_Bank3_NAND | 0x80000000 | NAND bank 3 |

## Example: TFT LCD via FSMC

LCD register and data access is memory-mapped using address line A16 or A17 as the RS (register select) signal:

```c
#include "debug.h"

// LCD register and data access structure
typedef struct {
    vu16 LCD_REG;   // Register address (RS=0)
    vu16 LCD_RAM;   // Data address (RS=1)
} LCD_TypeDef;

// A17 used as RS, Bank1 sector1
#define LCD_BASE  ((u32)(0x60000000 | 0x0003FFFE))
#define LCD       ((LCD_TypeDef*)LCD_BASE)

void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG = regval;    // Write register command
}

void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM = data;      // Write display data
}

u16 LCD_RD_DATA(void)
{
    return LCD->LCD_RAM;      // Read display data
}

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;
    LCD->LCD_RAM = LCD_RegValue;
}

void FSMC_LCD_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure = {0};
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    // Configure FSMC data and control pins (PD, PE)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                  GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // Timing configuration
    readWriteTiming.FSMC_AddressSetupTime = 0x02;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x05;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;

    // NORSRAM configuration for LCD
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
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    FSMC_LCD_Init();
    LCD_Init();          // LCD controller-specific init (ILI9341, ST7789, etc.)

    while(1)
    {
        LCD_Clear(RED);
        Delay_Ms(1000);
        LCD_Clear(GREEN);
        Delay_Ms(1000);
        LCD_Clear(BLUE);
        Delay_Ms(1000);
    }
}
```

## Example: External SRAM (IS62WV25616BLL)

```c
#define Bank1_SRAM1_ADDR  ((u32)(0x60000000))

void FSMC_SRAM_Init(void)
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    // Configure NADV pin (PB7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure data/address pins on PD, PE...
    // (see FSMC/NANDFLASH example for full pin list)

    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x03;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

// Read/write SRAM via memory-mapped access
void FSMC_SRAM_WriteBuffer(u8 *pBuffer, u32 WriteAddr, u32 n)
{
    for(; n != 0; n--) {
        *(vu8*)(Bank1_SRAM1_ADDR + WriteAddr) = *pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

void FSMC_SRAM_ReadBuffer(u8 *pBuffer, u32 ReadAddr, u32 n)
{
    for(; n != 0; n--) {
        *pBuffer++ = *(vu8*)(Bank1_SRAM1_ADDR + ReadAddr);
        ReadAddr++;
    }
}
```

## Example: NAND Flash (W29N01HV)

```c
// NAND Flash uses FSMC bank 2 or 3
// CLE = A16, ALE = A17, CE = NE1/NCE2, R/B = NWAIT
// Data bus: D0-D7 (8-bit)

// See CH32F20xEVT/EVT/EXAM/FSMC/NANDFLASH/ for complete
// nand_flash.c driver with erase/read/write/ECC functions.
```

## Pitfalls

- **Clock enable**: Must enable both GPIO clocks (APB2) and FSMC clock (AHB) before configuration.
- **Pin conflicts**: FSMC uses many PD/PE pins. Check for conflicts with other peripherals.
- **LCD RS signal**: The address line used for RS (A16, A17, etc.) determines the memory offset. A16 = offset 0x00020000, A17 = offset 0x00040000.
- **16-bit vs 8-bit**: LCD and SRAM typically use 16-bit data width. NAND Flash uses 8-bit.
- **Data/Address mux**: For SRAM with multiplexed bus, enable `FSMC_DataAddressMux_Enable`. For LCD, typically disable it.
- **LCD reset**: Most LCD controllers need a hardware reset pulse (GPIO toggle) after FSMC init.
- **Access mode**: Mode A (SRAM), Mode B (NOR Flash), Mode C/D (LCD). Check your LCD controller datasheet.

## Related Examples

- `CH32F20xEVT/EVT/EXAM/FSMC/LCD` - TFT LCD display via FSMC
- `CH32F20xEVT/EVT/EXAM/FSMC/SRAM` - External SRAM read/write
- `CH32F20xEVT/EVT/EXAM/FSMC/NANDFLASH` - NAND Flash with ECC
