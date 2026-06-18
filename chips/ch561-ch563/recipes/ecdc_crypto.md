# Recipe: ECDC Hardware Encryption/Decryption

## Overview

**CH561/CH563 do NOT have the ECDC (Encryption/Decryption Controller) peripheral.**
The ECDC is available only on CH569 (RISC-V). CH561/CH563 do not support hardware
AES/SM4 encryption.

## Alternatives for CH561/CH563

For encryption/decryption on CH561/CH563, use software implementations:

### Software AES (Example Libraries)

```c
// Use a software AES library such as:
// - TinyAES (https://github.com/kokke/tiny-AES-c)
// - mbedTLS
// - WolfSSL

#include "aes.h"

UINT8 key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                  0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
UINT8 plaintext[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
UINT8 ciphertext[16];

// Encrypt
struct AES_ctx ctx;
AES_init_ctx(&ctx, key);
AES_ECB_encrypt(&ctx, plaintext);
memcpy(ciphertext, plaintext, 16);
```

### Hardware CRC for Data Integrity

CH561/CH563 do have hardware CRC support for data integrity checks:

```c
// CRC calculation (if available in hardware)
// Check CH561SFR.H / CH563SFR.H for CRC register definitions
```

## Notes

- CH561/CH563 have no hardware crypto engine
- For encrypted Ethernet or eMMC transfers, use software encryption
- Performance will be significantly slower than hardware ECDC
- Consider CH569 if hardware encryption is required
