# CRC Calculation

## Overview

CH32H417 has a hardware CRC (Cyclic Redundancy Check) calculator supporting CRC-32 and configurable polynomial. Useful for data integrity verification in communication protocols and storage systems.

## Key API Functions

```c
void CRC_DeInit(void);
void CRC_ResetDR(void);
uint32_t CRC_CalcCRC(uint32_t Data);
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);
uint32_t CRC_GetCRC(void);
void CRC_SetIDRegister(uint8_t IDValue);
uint8_t CRC_GetIDRegister(void);
```

## CRC Block Calculation Example

```c
#include "ch32h417.h"

uint32_t SRC_BUF[32] = {
    0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
    0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
    0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
    0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
    0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
    0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
    0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
    0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

void CRC_Calculation_Example(void)
{
    uint32_t crc_value;

    // Enable CRC clock
    RCC_HBPeriphClockCmd(RCC_HBPeriph_CRC, ENABLE);

    // Check if CRC is already in use (ID register)
    if (CRC_GetIDRegister() == 0xAA)
        CRC_ResetDR();

    // Calculate CRC for the entire block
    crc_value = CRC_CalcBlockCRC(SRC_BUF, 32);

    // Mark CRC as "in use" via ID register
    CRC_SetIDRegister(0xAA);

    printf("CRC Value: 0x%08X\r\n", crc_value);
    // Expected: 0x199AC3CA for this data
}
```

## Incremental CRC Calculation

```c
uint32_t CRC_Incremental(uint32_t* data, uint32_t length)
{
    CRC_ResetDR();

    for (uint32_t i = 0; i < length; i++)
    {
        CRC_CalcCRC(data[i]);
    }

    return CRC_GetCRC();
}
```

## CRC ID Register Usage

```c
uint8_t CRC_Is_InUse(void)
{
    return (CRC_GetIDRegister() == 0xAA) ? 1 : 0;
}

uint32_t CRC_Calculate_WithCheck(uint32_t* data, uint32_t length)
{
    if (CRC_Is_InUse())
        CRC_ResetDR();

    uint32_t result = CRC_CalcBlockCRC(data, length);
    CRC_SetIDRegister(0xAA);
    return result;
}
```

## Important Notes

- Enable clock: `RCC_HBPeriphClockCmd(RCC_HBPeriph_CRC, ENABLE)`
- Always call `CRC_ResetDR()` before a new CRC calculation
- The ID register can be used to track whether CRC has been calculated
- Data is processed as 32-bit words
- Default polynomial is CRC-32 (0x04C11DB7)
