# Recipe: ECDC Hardware Encryption/Decryption

## Overview

Use the ECDC (Encryption/Decryption Controller) hardware engine for AES-128/192/256
and SM4 encryption/decryption on CH56x chips. The ECDC supports ECB and CTR modes,
with single-register, self-DMA, and peripheral-linked DMA operation modes.

## Supported Algorithms

| Mode Constant     | Algorithm | Mode |
|-------------------|-----------|------|
| `MODE_AES_ECB`    | AES       | ECB  |
| `MODE_AES_CTR`    | AES       | CTR  |
| `MODE_SM4_ECB`    | SM4       | ECB  |
| `MODE_SM4_CTR`    | SM4       | CTR  |

## Key Lengths

| Constant            | Bits |
|---------------------|------|
| `KEYLENGTH_128BIT`  | 128  |
| `KEYLENGTH_192BIT`  | 192  |
| `KEYLENGTH_256BIT`  | 256  |

## Clock Configuration

| Constant          | Description       |
|-------------------|-------------------|
| `ECDCCLK_DISABLE` | ECDC clock off    |
| `ECDCCLK_240MHZ`  | 240MHz ECDC clock |
| `ECDCCLK_160MHZ`  | 160MHz ECDC clock |

Note: The ECDC clock (240/160MHz) is derived from the PLL and is independent of the
system clock. The system clock should be at least 80MHz for proper ECDC operation.

## Execution Modes

| Constant                    | Direction              | Description                        |
|-----------------------------|------------------------|------------------------------------|
| `SINGLEREGISTER_ENCRY`      | Encrypt                | Single 128-bit register encrypt    |
| `SINGLEREGISTER_DECRY`      | Decrypt                | Single 128-bit register decrypt    |
| `SELFDMA_ENCRY`             | Encrypt                | DMA from RAMX, in-place            |
| `SELFDMA_DECRY`             | Decrypt                | DMA from RAMX, in-place            |
| `RAM_TO_PERIPHERAL_ENCRY`   | Encrypt, RAM->Periph   | DMA encrypt to peripheral (e.g. HSPI) |
| `RAM_TO_PERIPHERAL_DECRY`   | Decrypt, RAM->Periph   | DMA decrypt to peripheral          |
| `PERIPHERAL_TO_RAM_ENCRY`   | Encrypt, Periph->RAM   | DMA encrypt from peripheral to RAM |
| `PERIPHERAL_TO_RAM_DECRY`   | Decrypt, Periph->RAM   | DMA decrypt from peripheral to RAM |

## API Functions

```c
// Initialize ECDC with algorithm, clock, key length, key, and counter
void ECDC_Init(UINT8 ecdcmode, UINT8 clkmode, UINT8 keylen,
               PUINT32 pkey, PUINT32 pcount);

// Change key without full re-init
void ECDC_SetKey(PUINT32 pkey, UINT8 keylen);

// Change counter (CTR mode only)
void ECDC_SetCount(PUINT32 pcount);

// Set execution mode and endianness, then start
void ECDC_Excute(UINT8 excutemode, UINT8 endianmode);

// Encrypt/decrypt a single 128-bit (16-byte) block
void ECDC_SingleRegister(PUINT32 pWdatbuff, PUINT32 pRdatbuff);

// Encrypt/decrypt a buffer in RAMX via DMA (in-place)
void ECDC_SelfDMA(UINT32 ram_addr, UINT32 ram_len);

// Reload counter for CTR mode (call between DMA blocks)
void ECDC_RloadCount(UINT8 excutemode, UINT8 endianmode, PUINT32 pcount);
```

## Endianness

| Constant          | Description    |
|-------------------|----------------|
| `MODE_BIG_ENDIAN` | Big-endian     |
| `MODE_LIT_ENDIAN` | Little-endian  |

## Single-Register Mode (16-byte block)

Encrypt or decrypt a single 128-bit block directly via registers:

```c
#include "CH56x_common.h"

UINT32 KeyValue[] = {
    0x55acd4c5, 0x97d4570e, 0xb89464ba, 0xe4a0556b,
    0x84af48fd, 0x51af5d2e, 0xadec514f, 0x9642cadd
};
UINT32 CountValue[] = {0x00000001, 0x00000002, 0x00000003, 0x00000300};

int main(void)
{
    SystemInit(80000000);

    // Initialize AES-128 ECB
    ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, KeyValue, NULL);

    UINT32 plaintext[4]  = {0x11112222, 0x33334444, 0x55556666, 0x77778888};
    UINT32 ciphertext[4] = {0};

    // Encrypt
    ECDC_Excute(SINGLEREGISTER_ENCRY, MODE_BIG_ENDIAN);
    ECDC_SingleRegister(plaintext, ciphertext);

    printf("Ciphertext: %08x %08x %08x %08x\n",
           ciphertext[0], ciphertext[1], ciphertext[2], ciphertext[3]);

    // Decrypt
    ECDC_Excute(SINGLEREGISTER_DECRY, MODE_BIG_ENDIAN);
    ECDC_SingleRegister(ciphertext, plaintext);

    printf("Plaintext: %08x %08x %08x %08x\n",
           plaintext[0], plaintext[1], plaintext[2], plaintext[3]);

    while(1);
}
```

## Self-DMA Mode (RAMX buffer, in-place)

Encrypt or decrypt a buffer located in RAMX (0x20020000+). The operation is in-place:
the buffer is overwritten with the result.

```c
#include "CH56x_common.h"

UINT32 KeyValue[] = {0x55acd4c5, 0x97d4570e, 0xb89464ba, 0xe4a0556b,
                     0x84af48fd, 0x51af5d2e, 0xadec514f, 0x9642cadd};

int main(void)
{
    SystemInit(80000000);

    ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, KeyValue, NULL);

    // Write test pattern to RAMX
    UINT32 len = 64;  // 64 x 4 bytes = 256 bytes
    for (UINT32 i = 0; i < len; i++) {
        *(UINT32 *)(0x20020000 + i * 4) = i;
    }

    // Encrypt in-place
    ECDC_Excute(SELFDMA_ENCRY, MODE_BIG_ENDIAN);
    ECDC_SelfDMA((UINT32)0x20020000, len);

    // Decrypt in-place (restores original data)
    ECDC_Excute(SELFDMA_DECRY, MODE_BIG_ENDIAN);
    ECDC_SelfDMA((UINT32)0x20020000, len);

    while(1);
}
```

## CTR Mode with Counter

CTR mode requires a counter (nonce) value. For multi-block DMA, reload the counter
between blocks:

```c
UINT32 KeyValue[]   = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
UINT32 CountValue[] = {0x00000001, 0x00000002, 0x00000003, 0x00000300};

ECDC_Init(MODE_AES_CTR, ECDCCLK_240MHZ, KEYLENGTH_128BIT, KeyValue, CountValue);

// Encrypt first block
ECDC_Excute(SELFDMA_ENCRY, MODE_BIG_ENDIAN);
ECDC_SelfDMA(0x20020000, 64);

// Reload counter for next block
ECDC_RloadCount(SELFDMA_ENCRY, MODE_BIG_ENDIAN, CountValue);
ECDC_SelfDMA(0x20020000 + 256, 64);
```

## ECDC with HSPI (Peripheral-Linked DMA)

The ECDC can encrypt/decrypt data flowing through HSPI automatically:

```c
// Host: encrypt data going out via HSPI
ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, KeyValue, NULL);
ECDC_Excute(RAM_TO_PERIPHERAL_ENCRY, 0);

// Slave: decrypt data coming in via HSPI
ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, KeyValue, NULL);
ECDC_Excute(PERIPHERAL_TO_RAM_DECRY, 0);
```

See the `HSPI_ECDC` example in `CH569EVT/EVT/EXAM/HSPI/HSPI_ECDC/` for a complete
host/slave encrypted HSPI transfer.

## Notes

- The ECDC key register contains random values at boot. Always call `ECDC_Init()`
  or `ECDC_SetKey()` before encryption/decryption.
- Self-DMA operates on RAMX (0x20020000+) only. Regular RAM at 0x20000000 is not
  accessible by the ECDC DMA engine.
- Buffer length for `ECDC_SelfDMA()` is in 32-bit words, not bytes.
- For CTR mode, call `ECDC_RloadCount()` between DMA blocks to reload the counter.
- The ECDC operates on 128-bit (16-byte) blocks. Input data must be a multiple of
  16 bytes for ECB mode.

## Example Project

`CH569EVT/EVT/EXAM/ECDC/` - Single-register and self-DMA encryption/decryption demo.
