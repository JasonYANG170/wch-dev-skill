# Recipe: Timer and PWM

## Overview

Use TMR0, TMR1, TMR2 for timing/counting, PWM generation, and input capture
on CH56x chips. Also covers the independent PWM0-3 channels.

## TMR0-2: Timer/Counter

### Basic Timer

```c
#include "CH56x_common.h"

// Timer interrupt handler
void TMR0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TMR0_IRQHandler(void)
{
    if (TMR0_GetITFlag(TMR_IT_CYC_END)) {
        TMR0_ClearITFlag(TMR_IT_CYC_END);
        // Timer period elapsed
        GPIOA_InverseBits(GPIO_Pin_5);  // Toggle LED
    }
}

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_TMR0);

    // Timer init: period in system clock ticks
    // At 120MHz: 120000000 ticks = 1 second
    TMR0_TimerInit(120000000);  // 1 second period

    // Enable interrupt
    TMR0_ITCfg(ENABLE, TMR_IT_CYC_END);
    PFIC_EnableIRQ(TMR0_IRQn);

    while(1) {}
}
```

### Get Current Count

```c
UINT32 count = TMR0_GetCurrentCount();
// Returns current counter value (max 67108864 = 2^26)
```

### External Signal Counter

```c
// Count external pulses on TMR0 input pin
TMR0_EXTSignalCounterInit(1000);  // Count to 1000 then interrupt
```

### Disable Timer

```c
TMR0_Disable();
```

## TMR1 and TMR2

Identical API, just replace TMR0 with TMR1/TMR2:

```c
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_TMR1);
TMR1_TimerInit(60000000);  // 0.5 second at 120MHz
TMR1_ITCfg(ENABLE, TMR_IT_CYC_END);
PFIC_EnableIRQ(TMR1_IRQn);

PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_TMR2);
TMR2_TimerInit(12000000);
TMR2_ITCfg(ENABLE, TMR_IT_CYC_END);
PFIC_EnableIRQ(TMR2_IRQn);
```

### TMR1/TMR2 DMA Support

```c
// TMR1 and TMR2 support DMA
TMR1_DMACfg(ENABLE, startAddr, endAddr, Mode_LOOP);
// Mode_Single: single transfer
// Mode_LOOP: continuous loop
```

## PWM Output (via TMR0-2)

### PWM on Timer Channel

```c
// Configure PWM on TMR0
TMR0_PWMCycleCfg(12000);       // PWM period (ticks)
TMR0_PWMInit(high_on_low, PWM_Times_1);
// Polarities:
//   high_on_low: default low, active high
//   low_on_high: default high, active low
// Repeat: PWM_Times_1/4/8/16

// Set duty cycle
TMR0_PWMActDataWidth(6000);    // 50% duty (6000/12000)
```

### PWM Repeat Times

```c
PWM_Times_1   // PWM effective output repeats 1 time
PWM_Times_4   // PWM effective output repeats 4 times
PWM_Times_8   // PWM effective output repeats 8 times
PWM_Times_16  // PWM effective output repeats 16 times
```

## Input Capture (via TMR0-2)

```c
// Configure capture on TMR0
TMR0_CAPTimeoutCfg(33554432);  // Capture timeout (max value)
TMR0_CapInit(Edge_To_Edge);    // Capture on any edge

// Capture modes:
// CAP_NULL:              No capture
// Edge_To_Edge:          Any edge to any edge
// FallEdge_To_FallEdge:  Falling to falling
// RiseEdge_To_RiseEdge:  Rising to rising

// Read captured data
UINT8 count = TMR0_CAPDataCounter();  // Number of captured values
if (count > 0) {
    UINT32 pulse = TMR0_CAPGetData();  // Read pulse width
}
```

## Independent PWM Channels (PWM0-3)

The PWM0-3 channels are separate from the timer PWM and provide
simple 8-bit PWM output.

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_PWMX);

    // Configure PWM clock divider
    PWMX_CLKCfg(0);  // 0 = no division

    // Set PWM period (256 or 255 cycles)
    PWMX_CycleCfg(PWMX_Cycle_256);

    // Set individual channel duty cycles (0-255)
    PWM0_ActDataWidth(128);  // ~50% duty
    PWM1_ActDataWidth(64);   // ~25% duty
    PWM2_ActDataWidth(192);  // ~75% duty
    PWM3_ActDataWidth(32);   // ~12.5% duty

    // Enable PWM output
    PWMX_ACTOUT(CH_PWM0 | CH_PWM1, 128, High_Level, ENABLE);
    // Channels: CH_PWM0, CH_PWM1, CH_PWM2, CH_PWM3 (can OR together)
    // Polarity: High_Level or Low_Level
    // Enable: ENABLE or DISABLE

    while(1) {}
}
```

## Complete Example: LED Breathing Effect

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_PWMX);
    PWMX_CLKCfg(0);
    PWMX_CycleCfg(PWMX_Cycle_256);

    // PWM0 on PA5 (check pin mapping for your board)
    PWMX_ACTOUT(CH_PWM0, 0, High_Level, ENABLE);

    UINT8 brightness = 0;
    INT8 dir = 1;

    while(1)
    {
        PWM0_ActDataWidth(brightness);
        brightness += dir;
        if (brightness == 255) dir = -1;
        if (brightness == 0) dir = 1;
        mDelaymS(10);
    }
}
```

## Notes

- TMR0-2 counter max value: 67,108,864 (2^26)
- PWM0-3 are 8-bit resolution (0-255 duty steps)
- Timer PWM has higher resolution (up to 26-bit)
- Capture timeout max: 33,554,432 (2^25)
- For interrupt handlers, use `__attribute__((interrupt("WCH-Interrupt-fast")))`
