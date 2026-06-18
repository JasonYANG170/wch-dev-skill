# Recipe: PWMX Independent PWM Output

## Overview

Use the PWMX peripheral for independent PWM (Pulse Width Modulation) output on
CH56x chips. PWMX provides 4 independent channels (PWM0-PWM3) with configurable
clock divider, cycle length, duty cycle, and output polarity.

## PWMX Channels

| Channel  | Constant   | Default Pin |
|----------|------------|-------------|
| PWM0     | `CH_PWM0`  | PB15        |
| PWM1     | `CH_PWM1`  | PA4         |
| PWM2     | `CH_PWM2`  | PB1         |
| PWM3     | `CH_PWM3`  | PB2         |

Note: PWM0 shares PB15 with the external reset pin. To use PWM0, the reset function
must be disabled first (see Pin Configuration below).

## API Functions

```c
// Set clock divider: PWM_CLK = Fsys / (div + 1)
void PWMX_CLKCfg(UINT8 div);

// Set cycle length: PWMX_Cycle_256 (256 cycles) or PWMX_Cycle_255 (255 cycles)
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);

// Output PWM on a channel
// ch: CH_PWM0..CH_PWM3
// da: duty cycle (0..255 for Cycle_256, 0..254 for Cycle_255)
// pr: polarity (High_Level or Low_Level)
// s: ENABLE or DISABLE
void PWMX_ACTOUT(UINT8 ch, UINT8 da, PWMX_PolarTypeDef pr, UINT8 s);

// Set duty cycle individually (macro)
#define PWM0_ActDataWidth(d)  (R8_PWM0_DATA = d)
#define PWM1_ActDataWidth(d)  (R8_PWM1_DATA = d)
#define PWM2_ActDataWidth(d)  (R8_PWM2_DATA = d)
#define PWM3_ActDataWidth(d)  (R8_PWM3_DATA = d)
```

## PWM Frequency Calculation

```
PWM_frequency = Fsys / ((div + 1) * cycle_length)
```

Where:
- `div` = clock divider value (0-255), set by `PWMX_CLKCfg(div)`
- `cycle_length` = 256 or 255, set by `PWMX_CycleCfg()`

Example at 80MHz system clock:
- `PWMX_CLKCfg(4)` -> PWM_CLK = 80MHz / 5 = 16MHz
- `PWMX_CycleCfg(PWMX_Cycle_256)` -> PWM = 16MHz / 256 = 62.5kHz

## Duty Cycle

Duty cycle is set as a fraction of the cycle length:

| Duty Cycle | `da` value (Cycle_256) | Percentage |
|------------|------------------------|------------|
| 0%         | 0                      | 0%         |
| 12.5%      | 32 (256/8)             | 12.5%      |
| 25%        | 64 (256/4)             | 25%        |
| 50%        | 128 (256/2)            | 50%        |
| 75%        | 192 (256*3/4)          | 75%        |
| 100%       | 255                    | ~100%      |

## Pin Configuration

Configure GPIO pins as push-pull output before enabling PWM:

```c
// PWM1 on PA4
R32_PA_PD  &= ~(1 << 4);   // Disable pull-down
R32_PA_DRV &= ~(1 << 4);   // Normal drive strength
R32_PA_DIR |= (1 << 4);    // Output mode

// PWM2 on PB1
R32_PB_PD  &= ~(1 << 1);
R32_PB_DRV &= ~(1 << 1);
R32_PB_DIR |= (1 << 1);

// PWM3 on PB2
R32_PB_PD  &= ~(1 << 2);
R32_PB_DRV &= ~(1 << 2);
R32_PB_DIR |= (1 << 2);

// PWM0 on PB15 (disable reset function first)
R32_PB_PD  &= ~(1 << 15);
R32_PB_DRV &= ~(1 << 15);
R32_PB_DIR |= (1 << 15);
```

## Complete Example: Multi-Channel PWM

```c
#include "CH56x_common.h"

#define FREQ_SYS  80000000

void DebugInit(UINT32 baudrate)
{
    UINT32 x, t = FREQ_SYS;
    x = 10 * t * 2 / 16 / baudrate;
    x = (x + 5) / 10;
    R8_UART1_DIV = 1;
    R16_UART1_DL = x;
    R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R32_PA_SMT |= (1 << 8) | (1 << 7);
    R32_PA_DIR |= (1 << 8);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);
    DebugInit(115200);

    printf("PWMX Example\r\n");

    // Configure PWM output pins
    // PWM1 on PA4
    R32_PA_PD  &= ~(1 << 4);
    R32_PA_DRV &= ~(1 << 4);
    R32_PA_DIR |= (1 << 4);

    // PWM2 on PB1
    R32_PB_PD  &= ~(1 << 1);
    R32_PB_DRV &= ~(1 << 1);
    R32_PB_DIR |= (1 << 1);

    // PWM3 on PB2
    R32_PB_PD  &= ~(1 << 2);
    R32_PB_DRV &= ~(1 << 2);
    R32_PB_DIR |= (1 << 2);

    // PWM0 on PB15
    R32_PB_PD  &= ~(1 << 15);
    R32_PB_DRV &= ~(1 << 15);
    R32_PB_DIR |= (1 << 15);

    // PWMX configuration
    PWMX_CLKCfg(4);                   // Fsys / 5 = 16MHz
    PWMX_CycleCfg(PWMX_Cycle_256);   // 256 cycles -> 62.5kHz

    // Set different duty cycles
    PWMX_ACTOUT(CH_PWM0, 256 / 8, Low_Level, ENABLE);  // 12.5%
    PWMX_ACTOUT(CH_PWM1, 256 / 8, Low_Level, ENABLE);  // 12.5%
    PWMX_ACTOUT(CH_PWM2, 256 / 4, Low_Level, ENABLE);  // 25%
    PWMX_ACTOUT(CH_PWM3, 256 / 8, Low_Level, ENABLE);  // 12.5%

    while(1);
}
```

## Dynamic Duty Cycle Update

Change duty cycle at runtime without re-initializing:

```c
// Update individual channel duty
PWM0_ActDataWidth(128);  // 50% duty
PWM1_ActDataWidth(64);   // 25% duty

// Or call PWMX_ACTOUT again
PWMX_ACTOUT(CH_PWM2, 192, Low_Level, ENABLE);  // 75% duty
```

## Polarity

| Constant     | Description                          |
|--------------|--------------------------------------|
| `High_Level` | Output starts high, goes low on match |
| `Low_Level`  | Output starts low, goes high on match |

## Notes

- PWM0 on PB15 requires disabling the reset function. This is done automatically
  by the GPIO direction configuration.
- The PWMX peripheral clock must be enabled via `PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_PWMX)`
  if peripheral clock gating is used.
- PWMX runs independently of the timer peripheral (TMR0-2). Use timers for
  timing/counting, use PWMX for waveform output.
- All 4 PWM channels share the same clock divider and cycle length. Only the
  duty cycle is per-channel.

## Example Project

`CH569EVT/EVT/EXAM/PWMX/` - Multi-channel PWM output demo.
