# Key Scan (KEYSCAN)

> **Applicable summary**: Configure the hardware key scan matrix for low-power button detection with interrupt and sleep/wake support.

## Trigger Intent

- "key scan"
- "keyscan"
- "matrix keyboard"
- "button matrix"
- "low power key detection"

## Prerequisites

| Condition | Requirement |
|---|---|
| Reference project | `CH572EVT/EVT/EXAM/KEYSCAN/` |

## Call Chain

```
Step 1: Configure UART debug output (remapped pins)
Step 2: Initialize key scan with KeyScan_Cfg()
Step 3: Enable key scan interrupt
Step 4: (Optional) Configure wake from sleep
Step 5: Handle KEYSCAN interrupt
```

## Step-by-Step Instructions

### Key Scan Pin Assignment

The key scan module uses a matrix of GPIO pins. By default it uses PA3 and PA8 (shared with ISP function). The TXD pin is remapped to PA7 for debug output.

| Function | Pin | Description |
|---|---|---|
| KEYSCAN Row/Col | PA3, PA8 (default) | Matrix scan pins |
| UART TX (debug) | PA7 | Remapped TXD |

### Key Scan Initialization

```c
#include "CH57x_common.h"

volatile uint8_t KeyDownflag = 1;
uint32_t LSI_CurCnt, LSI_AfterCnt = 0;

void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_7);
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);
    UART_Remap(ENABLE, UART_TX_REMAP_PA7, UART_RX_REMAP_PA2);
    UART_DefInit();
}

int main(void)
{
    // Disable debug pin (reclaim PA3/PA8 for keyscan)
    R16_PIN_ALTERNATE &= ~RB_PIN_DEBUG_EN;
    HSECFG_Capacitance(HSECap_18p);
    SetSysClock(CLK_SOURCE_HSE_PLL_100MHz);

    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

    // Initialize key scan: all keys, clock div 2, repeat 7
    KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV2, KEYSCAN_REP7);

    // Enable key pressed interrupt
    KeyScan_ITCfg(ENABLE, RB_KEY_PRESSED_IE);
    PFIC_EnableIRQ(KEYSCAN_IRQn);

    while (1) {
        // Debounce: only report key once per ~1 second (16000 LSI cycles)
        LSI_CurCnt = RTC_GetCycleLSI();
        if (LSI_CurCnt < LSI_AfterCnt) {
            if ((LSI_CurCnt + RTC_MAX_COUNT) - LSI_AfterCnt > 16000) {
                KeyDownflag = 1;
                LSI_AfterCnt = LSI_CurCnt;
            }
        }
        if (LSI_CurCnt - LSI_AfterCnt > 16000) {
            KeyDownflag = 1;
            LSI_AfterCnt = LSI_CurCnt;
        }
    }
}
```

### Key Scan Interrupt Handler

```c
__INTERRUPT
__HIGH_CODE
void KEYSCAN_IRQHandler(void)
{
    if (KeyScan_GetITFlag(RB_KEY_PRESSED_IF)) {
        KeyScan_ClearITFlag(RB_KEY_PRESSED_IF);
    }
    if (KeyDownflag) {
        PRINT("KeyPressed\n");
        PRINT("KEY:%x\n", KeyValue);
        KeyDownflag = 0;
    }
}
```

### Sleep and Wake via Key Press

The key scan module can wake the chip from sleep mode:

```c
// Enable key press wake source
KeyPress_Wake(ENABLE);

// Enter sleep mode
PRINT("sleep mode sleep\n");
DelayMs(2);
LowPower_Sleep(RB_PWR_RAM12K | RB_PWR_EXTEND);

// Execution resumes here after wake
PRINT("wake..\n");
```

### KeyScan_Cfg Parameters

```c
KeyScan_Cfg(enable, key_mask, clock_div, repeat);
```

| Parameter | Values | Description |
|---|---|---|
| `enable` | `ENABLE` / `DISABLE` | Enable or disable key scan module |
| `key_mask` | `KEYSCAN_ALL` | Which keys to scan (all matrix positions) |
| `clock_div` | `KEYSCAN_DIV1` .. `KEYSCAN_DIVn` | Clock divider for scan rate |
| `repeat` | `KEYSCAN_REP1` .. `KEYSCAN_REP7` | Repeat count for debounce |

### KeyScan_ITCfg Parameters

```c
KeyScan_ITCfg(enable, interrupt_mask);
```

| Interrupt Flag | Description |
|---|---|
| `RB_KEY_PRESSED_IE` | Key pressed interrupt enable |
| `RB_KEY_RELEASED_IE` | Key released interrupt enable (if supported) |

### Reading Key Value

The `KeyValue` global variable (declared in the driver) contains the scan code of the pressed key. The encoding depends on the matrix position.

## Common Errors

| Error | Cause | Solution |
|---|---|---|
| No key detected | PA3/PA8 still in debug mode | Clear `RB_PIN_DEBUG_EN` in `R16_PIN_ALTERNATE` |
| Multiple key reports | No debounce | Use LSI timer-based debounce or increase `KEYSCAN_REPn` |
| Wake fails | `KeyPress_Wake()` not called before sleep | Call `KeyPress_Wake(ENABLE)` before `LowPower_Sleep()` |
| ISP conflict | Key scan pins shared with ISP | Ensure ISP entry is not triggered; or remap UART TX |

## Reference Project

- `CH572EVT/EVT/EXAM/KEYSCAN/` -- Complete key scan example with interrupt and sleep wake
