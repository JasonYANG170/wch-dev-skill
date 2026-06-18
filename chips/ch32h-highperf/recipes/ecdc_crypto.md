# ECDC Encryption/Decryption

## Overview

CH32H417 has an ECDC (Encryption/Decryption Controller) peripheral supporting AES-128/192/256 with ECB and CBC block cipher modes. It can encrypt/decrypt RAM blocks in-place or between source and destination addresses.

## Key API Functions

```c
void ECDC_DeInit(void);
void ECDC_Init(ECDC_InitTypeDef* ECDC_InitStruct);
void ECDC_HardwareClockCmd(FunctionalState NewState);
void ECDC_ClockConfig(uint32_t ECDC_ClockSource);
void ECDC_SetSRC_BaseAddr(uint32_t addr, uint32_t len);
void ECDC_SetDST_BaseAddr(uint32_t addr);
FlagStatus ECDC_GetFlagStatus(uint32_t ECDC_FLAG);
void ECDC_ClearFlag(uint32_t ECDC_FLAG);
```

## RAM Block Encryption Example (AES-128 ECB)

```c
#include "ch32h417.h"

__attribute__((aligned(32))) volatile uint32_t plain_text[128] = {0};
__attribute__((aligned(32))) volatile uint32_t encrypted_text[128] = {0};
__attribute__((aligned(32))) volatile uint32_t decrypted_text[128] = {0};

void ECDC_AES128_ECB_Encrypt(void)
{
    // Initialize test data
    for (int i = 0; i < 128; i++)
        plain_text[i] = i;

    // Set initialization vector (IV)
    ECDC_IV_TypeDef IV;
    IV.IV_31T0   = 0x00000001;
    IV.IV_63T32  = 0x00000002;
    IV.IV_95T64  = 0x00000003;
    IV.IV_127T96 = 0x00000300;

    // Set 128-bit key
    ECDC_KEY_TypeDef key;
    key.KEY_255T224 = 0x9642cadd;
    key.KEY_223T192 = 0xadec514f;
    key.KEY_191T160 = 0x51af5d2e;
    key.KEY_159T128 = 0x84af48fd;
    key.KEY_127T96  = 0xe4a0556b;
    key.KEY_95T64   = 0xb89464ba;
    key.KEY_63T32   = 0x97d4570e;
    key.KEY_31T0    = 0x55acd4c5;

    // Configure ECDC
    ECDC_InitTypeDef ecdc_initstruct = {0};
    ecdc_initstruct.Algorithm      = ECDCAlgorithm_AES;
    ecdc_initstruct.BlockCipherMode = ECDCBlockCipherMode_ECB;
    ecdc_initstruct.ExcuteMode     = ECDC_RAM_Encrypt;
    ecdc_initstruct.ExcuteEndian   = ECDCExcuteEndian_Big;
    ecdc_initstruct.KeyLen         = ECDCKeyLen_128b;
    ecdc_initstruct.IV             = &IV;
    ecdc_initstruct.Key            = &key;

    // Enable ECDC clock
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ECDC, ENABLE);
    ECDC_HardwareClockCmd(ENABLE);
    ECDC_ClockConfig(ECDC_ClockSource_PLLCLK_Div1);

    // Encrypt
    ECDC_Init(&ecdc_initstruct);
    ECDC_SetDST_BaseAddr((uint32_t)encrypted_text);
    ECDC_SetSRC_BaseAddr((uint32_t)plain_text, sizeof(plain_text) * 8 / 128);

    while (!ECDC_GetFlagStatus(ECDC_FLAG_RAM2RAM_END))
        ;
    ECDC_ClearFlag(ECDC_FLAG_RAM2RAM_END);

    // Decrypt
    ecdc_initstruct.ExcuteMode = ECDC_RAM_Decrypt;
    ECDC_Init(&ecdc_initstruct);
    ECDC_SetDST_BaseAddr((uint32_t)decrypted_text);
    ECDC_SetSRC_BaseAddr((uint32_t)encrypted_text, sizeof(encrypted_text) * 8 / 128);

    while (!ECDC_GetFlagStatus(ECDC_FLAG_RAM2RAM_END))
        ;
    ECDC_ClearFlag(ECDC_FLAG_RAM2RAM_END);
}
```

## ECDC Configuration Options

### Algorithm

| Macro | Description |
|-------|-------------|
| `ECDCAlgorithm_AES` | AES algorithm |

### Block Cipher Mode

| Macro | Description |
|-------|-------------|
| `ECDCBlockCipherMode_ECB` | Electronic Codebook mode |
| `ECDCBlockCipherMode_CBC` | Cipher Block Chaining mode |

### Execute Mode

| Macro | Description |
|-------|-------------|
| `ECDC_RAM_Encrypt` | Encrypt RAM block |
| `ECDC_RAM_Decrypt` | Decrypt RAM block |

### Key Length

| Macro | Description |
|-------|-------------|
| `ECDCKeyLen_128b` | 128-bit key |
| `ECDCKeyLen_192b` | 192-bit key |
| `ECDCKeyLen_256b` | 256-bit key |

### Clock Source

| Macro | Description |
|-------|-------------|
| `ECDC_ClockSource_PLLCLK_Div1` | PLL clock / 1 |
| `ECDC_ClockSource_PLLCLK_Div2` | PLL clock / 2 |

## Important Notes

- Buffers must be 32-byte aligned (`__attribute__((aligned(32)))`)
- Data length is specified in 128-bit blocks
- Wait for `ECDC_FLAG_RAM2RAM_END` before reading results
