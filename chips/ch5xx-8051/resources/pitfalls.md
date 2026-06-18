# CH5xx 8051 Common Pitfalls

This document lists common errors and their solutions when developing firmware for WCH CH5xx 8051 chips.

---

## 1. Safe Mode Sequence for Protected Registers

**Problem**: Writing to protected registers (GLOBAL_CFG, CLOCK_CFG, WAKE_CTRL) fails silently.

**Cause**: These registers require entering "safe mode" before modification.

**Fix**:
```c
EA = 0;                        // Disable interrupts (critical!)
SAFE_MOD = 0x55;               // Enter safe mode step 1
SAFE_MOD = 0xAA;               // Enter safe mode step 2
GLOBAL_CFG |= bCODE_WE;        // Now you can write protected register
SAFE_MOD = 0x00;               // Exit safe mode
EA = 1;                        // Restore interrupts
```

**Why EA=0**: If an interrupt fires between the two SAFE_MOD writes, the sequence is broken and the write fails.

---

## 2. Flash/DataFlash Write Without Interrupt Disable

**Problem**: DataFlash write sometimes fails or corrupts data.

**Cause**: Interrupts can interfere with the Flash write sequence, which requires precise timing.

**Fix**: Always disable interrupts around the entire unlock-write-lock sequence:
```c
bit ea_sts = EA;
EA = 0;
// ... unlock, write, lock ...
EA = ea_sts;
```

---

## 3. Wrong Memory Qualifier

**Problem**: Program crashes or variables have wrong values.

**Cause**: Using wrong memory segment qualifier.

| Qualifier | Segment | Size | Speed | Use For |
|---|---|---|---|---|
| (default) / `data` | Direct data | 128 bytes | Fast | Frequently accessed vars |
| `idata` | Indirect data | 256 bytes | Medium | Stack, more vars |
| `xdata` | External data | Chip-dependent | Slow | Large buffers |
| `code` | Code/ROM | Up to 64KB | Read-only | Constants, lookup tables |
| `pdata` | Page data | 256 bytes | Medium | Rarely used |

**Fix**: Use `xdata` for large arrays, `data` for small frequently-used variables:
```c
UINT8   counter;                    // data (fast, limited)
UINT8X  rxBuffer[256];              // xdata (large, slower)
UINT8C  constTable[] = {0,1,2,3};   // code (Flash, read-only)
```

---

## 4. ADC Pin Not Set to Float Input

**Problem**: ADC reads 0 or max value regardless of actual voltage.

**Cause**: Pin is configured as output or has pullup enabled.

**Fix**: Set pin to float input before ADC sampling:
```c
P1_MOD_OC &= ~bAIN0;    // Not open-drain
P1_DIR_PU &= ~bAIN0;    // Input, no pullup
```

---

## 5. UART Baud Rate Mismatch

**Problem**: UART receives garbage data or no data.

**Cause**: FREQ_SYS definition doesn't match actual system clock.

**Fix**: Verify CLOCK_CFG value and update FREQ_SYS in Debug.H:
```c
// If CLOCK_CFG = 0x05 (16MHz):
#define FREQ_SYS  16000000

// Baud rate calculation for UART1:
SBAUD1 = 0 - FREQ_SYS / 16 / 115200;  // For 115200 baud
```

---

## 6. USB Endpoint Buffer Address Misalignment

**Problem**: USB enumeration fails or data corruption.

**Cause**: Endpoint DMA buffer addresses not properly set or overlapping.

**Fix**: Allocate buffers at even addresses in xdata:
```c
UINT8 xdata EP0_BUF[64] _at_ 0x0000;
UINT8 xdata EP1_BUF[64] _at_ 0x0040;
UINT8 xdata EP2_BUF[64] _at_ 0x0080;

// Set DMA addresses
UEP0_DMA_L = 0x00; UEP0_DMA_H = 0x00;
UEP1_DMA_L = 0x40; UEP1_DMA_H = 0x00;
UEP2_DMA_L = 0x80; UEP2_DMA_H = 0x00;
```

---

## 7. Timer0/1 Mode Bits Conflict

**Problem**: Timer0 configuration accidentally changes Timer1 or vice versa.

**Cause**: TMOD register shares bits for both timers. Low nibble = Timer0, high nibble = Timer1.

**Fix**: Use read-modify-write:
```c
// Set Timer0 mode without affecting Timer1
TMOD = (TMOD & 0xF0) | 0x01;  // Timer0 mode 1

// Set Timer1 mode without affecting Timer0
TMOD = (TMOD & 0x0F) | 0x20;  // Timer1 mode 2
```

---

## 8. SPI Slave First Byte Garbage

**Problem**: SPI slave receives incorrect first byte after chip select.

**Cause**: When chip select goes low, slave auto-loads SPI0_S_PRE into shift register. If not pre-loaded, it contains stale data.

**Fix**: Pre-load the first byte before chip select:
```c
SPI0_S_PRE = firstByte;  // Pre-load before CS goes low
// Then CS goes low -> firstByte is sent automatically
```

---

## 9. GPIO Interrupt Flag Not Cleared

**Problem**: GPIO interrupt fires continuously or not at all.

**Cause**: Interrupt flags not properly cleared in ISR.

**Fix**: The GPIO interrupt is cleared by reading the status. For edge-triggered:
```c
void GPIO_ISR(void) interrupt INT_NO_GPIO using 1
{
    // Reading PIN_FUNC & bIO_INT_ACT clears the interrupt
    UINT8 status = PIN_FUNC & bIO_INT_ACT;
    // Handle interrupt based on status
}
```

---

## 10. DataFlash Odd Address

**Problem**: DataFlash read returns wrong data.

**Cause**: DataFlash addresses must be even. ROM_ADDR_L should be (byte_addr << 1).

**Fix**:
```c
ROM_ADDR_L = byteAddress << 1;  // Multiply by 2 for even address
ROM_CTRL = ROM_CMD_READ;
result = ROM_DATA_L;
```

---

## 11. Stack Overflow in 8051

**Problem**: Program crashes, variables corrupted.

**Cause**: Default stack is in internal RAM (128 bytes). Too many local variables or deep call nesting.

**Fix**:
- Use `data` qualifier for small variables
- Use `xdata` for large buffers
- Minimize local variables in deeply nested functions
- Keil C51: check "Memory Model" setting (Small/Medium/Large)

---

## 12. printf Not Working

**Problem**: No output from printf().

**Cause**: UART0 not initialized, or printf not routed to UART.

**Fix**:
```c
#include "stdio.h"
mInitSTDIO();  // Must call before printf
printf("Hello\n");
```

For Keil C51, ensure `putchar()` is implemented to write to SBUF.

---

## 13. Watchdog Reset Loop

**Problem**: Chip resets repeatedly.

**Cause**: Watchdog enabled but not fed regularly.

**Fix**:
```c
// Option A: Feed watchdog periodically
WDOG_COUNT = 0;  // Reset watchdog counter

// Option B: Disable watchdog reset (timer only)
GLOBAL_CFG &= ~bWDOG_EN;  // In safe mode
```

---

## 14. Pin Conflict Between Peripherals

**Problem**: One peripheral works but another doesn't on the same pins.

**Cause**: Multiple peripherals sharing pins without proper multiplexing.

**Fix**: Check PIN_FUNC register and pin alternate functions:
```c
// Example: Use UART1 on alternate pins P3.4/P3.2 instead of P1.6/P1.7
PIN_FUNC |= bUART1_PIN_X;

// Example: Use PWM1 on alternate pin P3.0 instead of P1.5
PIN_FUNC |= bPWM1_PIN_X;
```

---

## 15. Clock Configuration Locked After Boot

**Problem**: Can't change CLOCK_CFG after initial boot.

**Cause**: CLOCK_CFG is a protected register requiring safe mode.

**Fix**: Enter safe mode to change clock:
```c
EA = 0;
SAFE_MOD = 0x55;
SAFE_MOD = 0xAA;
CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | 0x05;  // 16MHz
SAFE_MOD = 0x00;
EA = 1;
mDelaymS(1);  // Wait for clock stable
```
