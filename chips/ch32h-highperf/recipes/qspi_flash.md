# QSPI Flash

## Overview

CH32H417 has a QSPI (Quad SPI) controller supporting 1/2/4-line SPI modes for external flash memory. Supports indirect read/write, auto-polling for status register, memory-mapped mode, and DMA transfers. Ideal for large external storage (up to 256MB addressable).

## Key API Functions

```c
void QSPI_DeInit(QSPI_TypeDef* QSPIx);
void QSPI_Init(QSPI_TypeDef* QSPIx, QSPI_InitTypeDef* QSPI_InitStruct);
void QSPI_Cmd(QSPI_TypeDef* QSPIx, FunctionalState NewState);
void QSPI_ComConfig_Init(QSPI_TypeDef* QSPIx, QSPI_ComConfig_InitTypeDef* QSPI_ComConfig_InitStruct);
void QSPI_SetAddress(QSPI_TypeDef* QSPIx, uint32_t Address);
void QSPI_SetDataLength(QSPI_TypeDef* QSPIx, uint32_t DataLength);
void QSPI_Start(QSPI_TypeDef* QSPIx);
void QSPI_SendData8(QSPI_TypeDef* QSPIx, uint8_t Data);
uint8_t QSPI_ReceiveData8(QSPI_TypeDef* QSPIx);
FlagStatus QSPI_GetFlagStatus(QSPI_TypeDef* QSPIx, uint32_t QSPI_FLAG);
void QSPI_ClearFlag(QSPI_TypeDef* QSPIx, uint32_t QSPI_FLAG);
void QSPI_EnableQuad(QSPI_TypeDef* QSPIx, FunctionalState NewState);
void QSPI_SetFIFOThreshold(QSPI_TypeDef* QSPIx, uint32_t Threshold);
void QSPI_AutoPollingMode_Config(QSPI_TypeDef* QSPIx, uint32_t Match, uint32_t Mask, uint32_t Mode);
void QSPI_AutoPollingModeStopCmd(QSPI_TypeDef* QSPIx, FunctionalState NewState);
void QSPI_AutoPollingMode_SetInterval(QSPI_TypeDef* QSPIx, uint32_t Interval);
```

## QSPI Initialization

```c
#include "ch32h417.h"

void QSPI1_Config(void)
{
    QSPI_InitTypeDef QSPI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_QSPI1, ENABLE);

    QSPI_InitStructure.QSPI_Prescaler = 3;        // Clock divider
    QSPI_InitStructure.QSPI_CKMode = QSPI_CKMode_Mode3;
    QSPI_InitStructure.QSPI_CSHTime = QSPI_CSHTime_8Cycle;
    QSPI_InitStructure.QSPI_FSize = 22;            // 2^(22+1) = 8MB
    QSPI_InitStructure.QSPI_FSelect = QSPI_FSelect_1;
    QSPI_InitStructure.QSPI_DFlash = QSPI_DFlash_Disable;

    QSPI_Init(QSPI1, &QSPI_InitStructure);
    QSPI_SetFIFOThreshold(QSPI1, 10);
    QSPI_Cmd(QSPI1, ENABLE);
}
```

## GPIO Configuration

```c
void QSPI_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB |
                          RCC_HB2Periph_GPIOC | RCC_HB2Periph_GPIOE |
                          RCC_HB2Periph_GPIOF, ENABLE);

    // SCK PB2(AF9)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource2, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SCSN PB6(AF10)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SIO0 PF8(AF10), SIO1 PF9(AF10), SIO2 PF7(AF10), SIO3 PF6(AF10)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF10);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF10);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF10);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF10);
}
```

## Flash Read (Single Line)

```c
#define FLASH_CMD_Read  0x03

void QSPI_ReadData(uint32_t addr, uint8_t* data, uint32_t len)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};

    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = FLASH_CMD_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);
    QSPI_Start(QSPI1);

    uint32_t i = 0;
    while (i < len)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            data[i] = QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}
```

## Flash Quad Read (4-Line)

```c
#define FLASH_CMD_FastReadQuadIO  0xEB

void QSPI_ReadData_QuadIO(uint32_t addr, uint8_t* data, uint32_t len)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};

    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = FLASH_CMD_FastReadQuadIO;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 6;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);

    QSPI_EnableQuad(QSPI1, ENABLE);
    QSPI_Start(QSPI1);

    uint32_t i = 0;
    while (i < len)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            data[i] = QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    QSPI_EnableQuad(QSPI1, DISABLE);
}
```

## Flash Page Program

```c
#define FLASH_CMD_PageProgram  0x02
#define FLASH_CMD_WriteEnable  0x06

void QSPI_ProgramPage(uint32_t addr, uint8_t* data, uint16_t len)
{
    // Write Enable
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_NoData;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = FLASH_CMD_WriteEnable;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);
    QSPI_SetDataLength(QSPI1, 0);
    QSPI_Start(QSPI1);
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_TC) == RESET);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);

    // Page Program
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Write;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = FLASH_CMD_PageProgram;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);
    QSPI_Start(QSPI1);

    for (uint16_t i = 0; i < len; i++)
    {
        while (!QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT));
        QSPI_SendData8(QSPI1, data[i]);
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}
```

## Flash Sector Erase

```c
#define FLASH_CMD_SectorErase  0x20

void QSPI_EraseSector(uint32_t addr)
{
    // Write Enable first
    // ... (same as above)

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_NoData;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = FLASH_CMD_SectorErase;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, 0);
    QSPI_Start(QSPI1);
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_TC) == RESET);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);

    // Wait for erase completion using auto-polling
    // ...
}
```

## QSPI Flags

| Flag | Description |
|------|-------------|
| `QSPI_FLAG_FT` | FIFO Threshold |
| `QSPI_FLAG_TC` | Transfer Complete |
| `QSPI_FLAG_SM` | Status Match (auto-polling) |
| `QSPI_FLAG_BUSY` | Busy |
| `QSPI_FLAG_TE` | Transfer Error |

## Common Flash Commands

| Command | Code | Description |
|---------|------|-------------|
| Read | 0x03 | Standard read |
| Fast Read Quad I/O | 0xEB | Quad I/O read with dummy |
| Page Program | 0x02 | Write up to 256 bytes |
| Quad Page Program | 0x32 | Write using 4 data lines |
| Sector Erase | 0x20 | Erase 4KB sector |
| Write Enable | 0x06 | Enable write/erase |
| Read Status Reg | 0x05 | Read busy/WIP status |
