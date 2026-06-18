# CH56x Common Pitfalls and Fixes

## 1. Ethernet DMA Buffers in Wrong Memory Region

**Symptom**: Ethernet TX/RX works intermittently, DMA hangs, or no frames are sent/received.

**Cause**: DMA controllers on CH56x cannot access the regular 16KB RAM at 0x20000000.
They can only access RAMX at 0x20020000+ and Flash.

**Fix**: Place all DMA buffers in the `.dmadata` section:

```c
// CORRECT - buffer in RAMX
__attribute__((aligned(4), section(".dmadata")))
uint8_t Eth_Buf[1518];

// WRONG - buffer in regular RAM
uint8_t Eth_Buf[1518];  // DMA cannot access this!
```

The linker script places `.dmadata` in RAMX (0x20020000+).

## 2. Peripheral Clock Not Enabled

**Symptom**: Peripheral register reads return 0, peripheral does not respond.

**Cause**: Peripheral clocks are gated by default to save power.

**Fix**: Enable the peripheral clock before any register access:

```c
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_ETH);    // Ethernet
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);   // UART0
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_SPI0);    // SPI0
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_TMR0);    // Timer0
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_EMMC);    // eMMC
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_USBSS);   // USB 3.0 (CH569 only)
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_ECDC);    // Encryption engine
```

## 3. CH561/CH563 Are ARM Architecture

**Symptom**: Code from this skill doesn't compile on CH561/CH563.

**Cause**: CH561 and CH563 are ARM chips with different APIs and toolchain.
This skill covers CH569 (RISC-V) only.

**Fix**: Use `ch561-ch563-arm-dev-skill` for CH561/CH563.

## 4. Flash Sector Erase Destroys Data

**Symptom**: Data corruption after Flash write operations.

**Cause**: Flash erases entire 256-byte sectors. Writing to a partially-filled sector
without reading first erases the existing data.

**Fix**: Always read-modify-write:

```c
// Read existing data
uint8_t buf[256];
memcpy(buf, (void *)target_addr, 256);

// Modify the part you want to change
buf[offset] = new_value;

// Erase the sector
FLASH_ROM_ERASE(target_addr, 256);

// Write back all data
FLASH_ROM_WRITE(target_addr, buf, 256);
```

## 5. Interrupt Handler Missing `.highcode` Attribute

**Symptom**: Interrupt handler runs slowly or causes timing issues.

**Cause**: By default, interrupt handlers run from Flash. Flash wait states cause
latency. Time-critical ISRs should run from RAMX.

**Fix**: Add the `.highcode` section attribute:

```c
void TMR0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TMR0_IRQHandler(void)
{
    // ISR code runs from RAMX for faster execution
    if (TMR0_GetITFlag(TMR_IT_CYC_END)) {
        TMR0_ClearITFlag(TMR_IT_CYC_END);
    }
}
```

Alternatively, use the `.highcode` attribute explicitly:
```c
__attribute__((section(".highcode")))
void fast_isr(void) { ... }
```

## 6. Ethernet PHY Not Responding

**Symptom**: `ETH_ReadPHYRegister()` returns 0xFFFF.

**Cause**: PHY address is wrong, MDC/MDIO pins not configured, or PHY not powered.

**Fix**:
1. Check PHY address (0-31, check board schematic / PHY datasheet)
2. Verify MDC/MDIO pin mapping via `GPIOPinRemap()`
3. Ensure PHY has power and 25MHz clock
4. Check that Ethernet peripheral clock is enabled

```c
// Scan for PHY address
for (int addr = 0; addr < 32; addr++) {
    uint16_t id1 = ETH_ReadPHYRegister(addr, 2);  // PHY ID1 register
    if (id1 != 0xFFFF && id1 != 0x0000) {
        printf("PHY found at address %d, ID=0x%04X\n", addr, id1);
        break;
    }
}
```

## 7. System Clock Not Set Before Peripheral Init

**Symptom**: UART baud rate is wrong, timer periods are incorrect, delays are wrong.

**Cause**: `SystemInit()` must be called before initializing any peripheral that
depends on the system clock.

**Fix**: Always call `SystemInit()` first:

```c
int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);  // FIRST
    Delay_Init(GetSysClock());           // SECOND
    // ... then peripheral init
}
```

## 8. Stack Overflow with Large Local Buffers

**Symptom**: Hard fault, memory corruption, or unpredictable behavior.

**Cause**: Default stack is only 2048 bytes. Large local arrays overflow the stack.

**Fix**: Use global or static arrays, or place large buffers in RAMX:

```c
// WRONG - stack overflow risk
void process(void) {
    uint8_t buf[1500];  // Too large for 2KB stack
}

// CORRECT - global buffer
__attribute__((section(".dmadata")))
uint8_t buf[1500];
```

Or increase stack size in `Link.ld`:
```
__stack_size = 4096;  // Increase from 2048
```

## 9. ECDC Key Not Set Before Use

**Symptom**: Encrypted/decrypted output is all zeros or garbage.

**Cause**: ECDC key register contains random values at boot.

**Fix**: Always call `ECDC_SetKey()` or `ECDC_Init()` before encryption:

```c
uint32_t key[4] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
uint32_t count[4] = {0};

// ECDC_Init sets both key and counter
ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, key, count);
```

## 10. eMMC Buffer Alignment

**Symptom**: eMMC read/write fails or returns error.

**Cause**: eMMC DMA requires word-aligned buffers.

**Fix**: Always align eMMC buffers to 4 bytes:

```c
// CORRECT
__attribute__((aligned(4), section(".dmadata")))
uint8_t emmc_buf[512];

// WRONG
uint8_t emmc_buf[512];  // May not be aligned
```

## 11. Watchdog Not Kicked

**Symptom**: System resets periodically.

**Cause**: Watchdog is enabled but counter is not refreshed.

**Fix**: Kick the watchdog in the main loop, or disable it:

```c
// In main loop:
while(1) {
    WWDG_SetCounter(0xFF);  // Refresh watchdog
    // ... application code
}

// Or disable watchdog reset:
WWDG_ResetCfg(DISABLE);
```

## 12. GPIO Pin Conflict with Peripheral

**Symptom**: GPIO output does not work, or peripheral does not respond.

**Cause**: Pin is configured for both GPIO and a peripheral function.

**Fix**: Use `GPIOPinRemap()` to select the correct function, and do not configure
pins used by peripherals as GPIO:

```c
// If PA9 is used for UART0 TX, do NOT configure it as GPIO
// Instead, configure alternate pins if needed:
GPIOPinRemap(ENABLE, xxx_REMAP);  // Remap UART0 to different pins
```
