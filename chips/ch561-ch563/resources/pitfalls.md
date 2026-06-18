# CH561/CH563 Common Pitfalls and Fixes

## 1. Using RISC-V Code on ARM Chips

**Symptom**: Code from ch56x-ethernet-dev-skill (CH569) does not compile on CH561/CH563.

**Cause**: CH561/CH563 are ARM7TDMI (ARM) chips, not RISC-V. They use different:
- Interrupt model (`__irq` keyword, not `__attribute__((interrupt(...)))`)
- Startup code (ARM assembly STARTUP.S, not RISC-V)
- Project files (.uvproj for Keil MDK, not .wvproj for MounRiver)
- No peripheral driver library -- all register-based

**Fix**: Use this skill (`ch561-ch563-arm-dev-skill`) for CH561/CH563.

## 2. Peripheral Clock Not Enabled

**Symptom**: Peripheral register reads return 0, peripheral does not respond.

**Cause**: Peripheral clocks are gated by default to save power.

**Fix**: Enable the peripheral clock before any register access:

```c
// Enable UART0 clock
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_UART0;

// Enable SPI0 clock
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_SPI0;

// Enable Timer0 clock
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

// Enable Ethernet clock
R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_ETH;

// Enable GPIO clock
R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_GPIO;
```

## 3. Flash Erase Unit is 4KB (Not 256 Bytes)

**Symptom**: Data corruption after Flash write operations.

**Cause**: CH561/CH563 Flash erases in 4KB (4096 byte) blocks, not 256-byte
sectors like CH569. Writing to a partially-filled 4KB block without reading
first erases the existing data.

**Fix**: Always read-modify-write:

```c
// Read existing data
UINT8 sector_buf[4096];
memcpy(sector_buf, (void *)target_addr, 4096);

// Modify the part you want to change
memcpy(sector_buf + offset, new_data, new_len);

// Erase the 4KB block
FLASH_ROM_ERASE(target_addr, 4096);

// Write back all data
FLASH_ROM_WRITE(target_addr, (UINT32 *)sector_buf, 4096);
```

## 4. Missing IRQ_Handler and FIQ_Handler

**Symptom**: Program hangs or crashes on interrupt.

**Cause**: ARM7TDMI requires `IRQ_Handler` and `FIQ_Handler` functions to be
defined. If missing, the default vector jumps to an undefined address.

**Fix**: Always define both handlers:

```c
__irq void IRQ_Handler(void)
{
    // Check and handle specific interrupt sources
    if (R8_INT_FLAG_0 & RB_IF_TMR0) {
        if (R8_TMR0_INT_FLAG & RB_TMR_IF_CYC_END) {
            R8_TMR0_INT_FLAG = RB_TMR_IF_CYC_END;
            // Handle timer interrupt
        }
    }
}

__irq void FIQ_Handler(void)
{
    while(1);  // Or handle FIQ
}
```

## 5. Wrong Interrupt Clear Mechanism

**Symptom**: Interrupt fires continuously, system hangs in ISR.

**Cause**: CH561/CH563 interrupt flags are "write 1 to clear". Reading the flag
does not clear it.

**Fix**: Write the flag bit to clear it:

```c
// CORRECT - write 1 to clear
R8_TMR0_INT_FLAG = RB_TMR_IF_CYC_END;

// WRONG - reading does not clear
UINT8 flag = R8_TMR0_INT_FLAG;  // Flag still set!
```

## 6. Safe Access Sequence Not Used

**Symptom**: Cannot modify protected registers (PLL, power control).

**Cause**: Some registers require a safe access unlock sequence before writing.

**Fix**: Use the 0x57/0xA8 unlock sequence:

```c
R8_SAFE_ACCESS_SIG = 0x57;  // First unlock
R8_SAFE_ACCESS_SIG = 0xA8;  // Second unlock
// Now can write protected registers
R8_SLP_POWER_CTRL = xxx;
R8_SAFE_ACCESS_SIG = 0x00;  // Re-lock
```

## 7. System Clock Not Configured

**Symptom**: UART baud rate is wrong, timer periods are incorrect.

**Cause**: FREQ_SYS must be defined in SYSFREQ.H before compilation.
SysFreq() is called automatically by STARTUP.S before main().

**Fix**: Set FREQ_SYS in SYSFREQ.H:

```c
// In SYSFREQ.H:
#define FREQ_SYS    100000000   // 100MHz
// Supported: 25MHz to 150MHz
```

## 8. GPIO Pin Conflict with Peripheral

**Symptom**: GPIO output does not work, or peripheral does not respond.

**Cause**: Pin is configured for both GPIO and a peripheral function.

**Fix**: Do not configure pins used by peripherals as GPIO output.
Check pin assignments:

| Pin | Peripheral Function |
|-----|-------------------|
| PB8/PB9 | UART0 RXD/TXD |
| PB10/PB11 | UART1 RXD/TXD |
| PB12-PB15 | SPI0 CS/SCK/MOSI/MISO |
| PB16-PB19 | SPI1 CS/SCK/SDO/SDI |
| PB0/PB2/PB4/PB6 | PWM0-3 (Timer output) |
| PB1/PB3/PB5/PB7 | Timer capture input |

## 9. Stack Overflow with Large Local Buffers

**Symptom**: Hard fault, memory corruption, or unpredictable behavior.

**Cause**: Default stack is limited. Large local arrays overflow the stack.

**Fix**: Use global or static arrays:

```c
// WRONG - stack overflow risk
void process(void) {
    UINT8 buf[1500];  // Too large for stack
}

// CORRECT - global buffer
UINT8 buf[1500];
```

## 10. Ethernet DMA Buffer Alignment

**Symptom**: Ethernet TX/RX fails or DMA hangs.

**Cause**: DMA buffers must be word-aligned (4-byte boundary).

**Fix**: Always align DMA buffers:

```c
// CORRECT
UINT8 Eth_Buf[1518] __attribute__((aligned(4)));

// WRONG
UINT8 Eth_Buf[1518];  // May not be aligned
```

## 11. CH561 vs CH563 Peripheral Differences

**Symptom**: Code works on CH561 but not CH563, or vice versa.

**Cause**: CH563 has additional peripherals (USB, GPIO PD port) that CH561 lacks.

**Fix**: Check chip-specific features:
- CH561: No USB, no GPIO PD port
- CH563: Has USB (SFR at 0x00404000-0x00405FFF), GPIO PD port
- Both: Ethernet, SPI0/1, UART0/1, Timer0-3, ADC

```c
// CH563 only: USB clock enable
R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_USB;

// CH563 only: PD port
R32_PD_DIR |= (1 << 0);
```

## 12. Keil MDK Project Configuration

**Symptom**: Build errors or incorrect memory layout.

**Cause**: Keil MDK project settings not configured for CH561/CH563.

**Fix**: Verify project settings:
- Device: ARM7TDMI
- Scatter file: correct Flash/SRAM layout
- Include path: contains CH561SFR.H or CH563SFR.H
- Preprocessor: defines FREQ_SYS via SYSFREQ.H
- Linker: includes ISPXT56X.O library
