# FMC External Memory Controller

## Overview

CH32H417 has an FMC (Flexible Memory Controller) supporting external SRAM, NOR Flash, NAND Flash, and SDRAM. The FMC provides memory-mapped access with configurable timing parameters and supports 8/16/32-bit data widths.

## Key API Functions

```c
void FMC_NORSRAMInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState);
void FMC_NORSRAMStructInit(FMC_NORSRAMInitTypeDef* FMC_NORSRAMInitStruct);
void FMC_NANDInit(FMC_NANDInitTypeDef* FMC_NANDInitStruct);
void FMC_NANDCmd(uint32_t FMC_Bank, FunctionalState NewState);
void FMC_SDRAMInit(FMC_SDRAMInitTypeDef* FMC_SDRAMInitStruct);
void FMC_SDRAMCmd(uint32_t FMC_Bank, FunctionalState NewState);
```

## External SRAM Initialization (16-bit)

```c
#include "ch32h417.h"

#define Bank1_SRAM1_ADDR  ((uint32_t)(0x60000000))

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOA |
                          RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |
                          RCC_HB2Periph_GPIOD | RCC_HB2Periph_GPIOE |
                          RCC_HB2Periph_GPIOF, ENABLE);

    // NE[1] PD7(AF12), NADV PB7(AF12), NOE PD4(AF12), NWE PD5(AF12)
    // A[16:17] PD11/PD12(AF12), D[0:15] on PD/PD/PE (AF12)
    // NBL[0:1] PC12/PC11(AF0)
    // Configure all FMC pins with GPIO_PinAFConfig(..., GPIO_AF12)

    // Example: NE[1] on PD7
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // ... configure remaining FMC pins similarly
}

void FMC_SRAM_Init(void)
{
    FMC_NORSRAMInitTypeDef FMC_NORSRAMInitStructure = {0};
    FMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC, ENABLE);
    GPIO_Config();

    readWriteTiming.FMC_AddressSetupTime = 0x01;
    readWriteTiming.FMC_AddressHoldTime = 0x00;
    readWriteTiming.FMC_DataSetupTime = 0x03;
    readWriteTiming.FMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FMC_AccessMode = FMC_AccessMode_A;

    FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
    FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Enable;
    FMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_SRAM;
    FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_16b;
    FMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
    FMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
    FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
    FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
    FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
    FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &readWriteTiming;
    FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &readWriteTiming;

    FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);
    FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
}
```

## SRAM Read/Write (Direct Access)

```c
void FMC_SRAM_WriteBuffer(uint32_t* pBuffer, uint32_t WriteAddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *(volatile uint32_t*)(Bank1_SRAM1_ADDR + WriteAddr) = *pBuffer;
        WriteAddr += 4;
        pBuffer++;
    }
}

void FMC_SRAM_ReadBuffer(uint32_t* pBuffer, uint32_t ReadAddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *pBuffer = *(volatile uint32_t*)(Bank1_SRAM1_ADDR + ReadAddr);
        ReadAddr += 4;
        pBuffer++;
    }
}
```

## SRAM DMA Transfer

```c
void FMC_SRAM_WriteBuffer_DMA(uint32_t* pBuffer, uint32_t WriteAddr, uint32_t n)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_StructInit(&DMA_InitStructure);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile uint32_t)(Bank1_SRAM1_ADDR + WriteAddr);
    DMA_InitStructure.DMA_Memory0BaseAddr = (volatile uint32_t)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = n / 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_256;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_256;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_ClearFlag(DMA1, DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC3) == RESET);
}
```

## FMC Memory Banks

| Bank | Base Address | Description |
|------|-------------|-------------|
| `FMC_Bank1_NORSRAM1` | 0x60000000 | NOR/SRAM Bank 1 |
| `FMC_Bank1_NORSRAM2` | 0x64000000 | NOR/SRAM Bank 2 |
| `FMC_Bank1_NORSRAM3` | 0x68000000 | NOR/SRAM Bank 3 |
| `FMC_Bank1_NORSRAM4` | 0x6C000000 | NOR/SRAM Bank 4 |

## Memory Types

| Macro | Description |
|-------|-------------|
| `FMC_MemoryType_SRAM` | External SRAM |
| `FMC_MemoryType_NOR` | External NOR Flash |

## VIO18 Configuration

For external SRAM operating at 1.8V, configure VIO18:

```c
RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
```
