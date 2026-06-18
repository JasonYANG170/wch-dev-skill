# RNG Random Number Generator

## Overview

CH32H417 has a hardware RNG (Random Number Generator) that produces true random numbers from analog noise. Useful for cryptographic key generation, nonces, and other security-sensitive applications.

## Key API Functions

```c
void RNG_DeInit(void);
void RNG_Cmd(FunctionalState NewState);
uint32_t RNG_GetRandomNumber(void);
FlagStatus RNG_GetFlagStatus(uint32_t RNG_FLAG);
```

## Basic RNG Example

```c
#include "ch32h417.h"

void RNG_Init_and_Read(void)
{
    uint32_t random = 0;

    // Enable RNG clock
    RCC_HBPeriphClockCmd(RCC_HBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    // Wait for data ready
    while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);

    random = RNG_GetRandomNumber();
    printf("Random: 0x%08x\r\n", random);
}
```

## Continuous Random Number Generation

```c
void RNG_Continuous(void)
{
    RCC_HBPeriphClockCmd(RCC_HBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    while (1)
    {
        while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
        uint32_t random = RNG_GetRandomNumber();
        printf("Random: 0x%08x\r\n", random);
        Delay_Ms(500);
    }
}
```

## RNG Flags

| Flag | Description |
|------|-------------|
| `RNG_FLAG_DRDY` | Data Ready (1 = new random number available) |

## Important Notes

- Enable clock: `RCC_HBPeriphClockCmd(RCC_HBPeriph_RNG, ENABLE)`
- After enabling, wait for `RNG_FLAG_DRDY` before reading
- The RNG produces a new 32-bit random number periodically
- Do not use the raw output directly for cryptography without proper conditioning
