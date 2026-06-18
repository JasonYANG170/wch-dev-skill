# Recipe: Random Number Generator (RNG)

## Overview

Use the hardware Random Number Generator (RNG) peripheral on CH32F20x to generate true random 32-bit numbers. The RNG uses a hardware entropy source and is much faster and more secure than software pseudo-random generators.

**Availability**: CH32F20x only (not available on CH32F10x or CH32M030).

## Key API Functions

```c
// Initialization and control
void RNG_Cmd(FunctionalState NewState);

// Random number generation
uint32_t RNG_GetRandomNumber(void);

// Status
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG);
```

## RNG Flags

| Flag | Description |
|------|-------------|
| RNG_FLAG_DRDY | Data ready - new random number available |

## Example: Generate Random Numbers

```c
#include "debug.h"

int main(void)
{
    u32 random = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    // Enable RNG clock (AHB bus)
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);

    // Enable RNG peripheral
    RNG_Cmd(ENABLE);

    while(1)
    {
        // Wait for random number to be ready
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);

        // Read 32-bit random number
        random = RNG_GetRandomNumber();
        printf("random: 0x%08x\r\n", random);

        Delay_Ms(500);
    }
}
```

## Example: Random Number in Range

```c
// Generate random number in range [min, max]
u32 RNG_GetRandomRange(u32 min, u32 max)
{
    u32 random;

    while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
    random = RNG_GetRandomNumber();

    return (random % (max - min + 1)) + min;
}

// Generate random byte (0-255)
u8 RNG_GetRandomByte(void)
{
    while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
    return (u8)(RNG_GetRandomNumber() & 0xFF);
}
```

## Example: Fill Buffer with Random Data

```c
void RNG_FillBuffer(u8 *buf, u32 len)
{
    u32 random;

    for(u32 i = 0; i < len; i += 4) {
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
        random = RNG_GetRandomNumber();

        // Copy up to 4 bytes
        for(u32 j = 0; j < 4 && (i + j) < len; j++) {
            buf[i + j] = (u8)(random >> (j * 8));
        }
    }
}
```

## Pitfalls

- **Clock enable**: RNG is on the AHB bus. Enable with `RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE)`.
- **Data ready check**: Always check `RNG_FLAG_DRDY` before calling `RNG_GetRandomNumber()`. Reading before data is ready returns undefined values.
- **Not available on CH32F10x**: The RNG peripheral only exists on CH32F20x. For CH32F10x, use a software PRNG (e.g., LFSR seeded from ADC noise).
- **Startup delay**: After enabling the RNG, there may be a brief delay before the first random number is ready.
- **Not cryptographic by default**: While hardware-generated, verify the RNG meets your security requirements. For cryptographic use, consider post-processing (e.g., hashing multiple outputs).

## Related Examples

- `CH32F20xEVT/EVT/EXAM/RNG/RNG` - Basic random number generation
