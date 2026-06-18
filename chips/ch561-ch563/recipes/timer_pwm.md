# Recipe: Timer and PWM (Register-Based)

## Overview

Use TMR0-TMR3 for timing/counting, PWM generation, and input capture on
CH561/CH563 using direct register manipulation.

## Timer Register Map

Each timer (TMR0-TMR3) has these registers:

| Register | R/W | Description |
|----------|-----|-------------|
| `R8_TMRx_CTRL_MOD` | RW | Mode control |
| `R8_TMRx_CTRL_DMA` | RW | DMA control |
| `R8_TMRx_INTER_EN` | RW | Interrupt enable |
| `R8_TMRx_INT_FLAG` | RW1 | Interrupt flag (write 1 to clear) |
| `R8_TMRx_FIFO_COUNT` | RO | FIFO count |
| `R32_TMRx_COUNT` | RO | Current count (28-bit) |
| `R32_TMRx_CNT_END` | RW | End count value (period) |
| `R32_TMRx_FIFO` | RW | FIFO register |
| `R16_TMRx_DMA_NOW` | RW | DMA current address |
| `R16_TMRx_DMA_BEG` | RW | DMA begin address |
| `R16_TMRx_DMA_END` | RW | DMA end address |

## Timer Mode

### Basic Timer

```c
void TMR0_Init(UINT32 period_ticks)
{
    // Enable TMR0 clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

    // Clear timer
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = 0;

    // Set period
    R32_TMR0_CNT_END = period_ticks;

    // Enable: timer mode, output enable, count enable
    R8_TMR0_CTRL_MOD = RB_TMR_OUT_EN | RB_TMR_COUNT_EN;

    // Enable interrupt
    R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;
}
```

### Timer Interrupt Handler

```c
__irq void IRQ_Handler(void)
{
    if (R8_INT_FLAG_0 & RB_IF_TMR0) {
        if (R8_TMR0_INT_FLAG & RB_TMR_IF_CYC_END) {
            R8_TMR0_INT_FLAG = RB_TMR_IF_CYC_END;  // Clear flag
            // Timer period elapsed - toggle LED, etc.
        }
    }
}
```

### Get Current Count

```c
UINT32 count = R32_TMR0_COUNT;  // Read current counter value (28-bit)
```

### Disable Timer

```c
R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN;  // Stop counting
```

## PWM Mode

### Timer-Based PWM (TMR0-TMR3)

```c
void TMR0_PWMInit(UINT32 period, UINT32 duty)
{
    // Enable clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

    // Clear
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = 0;

    // Set PWM period
    R32_TMR0_CNT_END = period;

    // Set duty cycle (active data width)
    R32_TMR0_FIFO = duty;

    // Enable: output, count, high-action polarity
    R8_TMR0_CTRL_MOD = RB_TMR_OUT_EN | RB_TMR_COUNT_EN |
                       RB_TMR_PWM_REPEAT;  // Repeat mode

    // Configure output pin (PB0 = PWM0)
    R32_PB_DIR |= (1 << 0);
}
```

### PWM Duty Update

```c
// Update duty cycle
R32_TMR0_FIFO = new_duty;

// PWM output pin aliases:
// PB0 = PWM0 (TMR0 output)
// PB2 = PWM1 (TMR1 output)
// PB4 = PWM2 (TMR2 output)
// PB6 = PWM3 (TMR3 output)
```

## Input Capture Mode

```c
void TMR0_CaptureInit(void)
{
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

    // Clear
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = 0;

    // Set timeout (max capture period)
    R32_TMR0_CNT_END = 0x0FFFFFFF;  // Max 28-bit

    // Enable capture mode: catch on any edge
    R8_TMR0_CTRL_MOD = RB_TMR_MODE_IN |           // Input/capture mode
                       RB_TMR_CAT_WIDTH |          // Min pulse width: 8 clocks
                       (0x01 << 6);                // Edge change detection

    // Enable interrupt
    R8_TMR0_INTER_EN = RB_TMR_IE_DATA_ACT;
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;
}

// In interrupt handler:
if (R8_TMR0_INT_FLAG & RB_TMR_IF_DATA_ACT) {
    R8_TMR0_INT_FLAG = RB_TMR_IF_DATA_ACT;
    UINT32 captured = R32_TMR0_FIFO;  // Read captured value
    // Process captured pulse width
}
```

## Independent PWM Channels (PWM0-PWM3)

CH561/CH563 also have 4 independent 8-bit PWM channels:

```c
// PWM pins: PB0=PWM0, PB2=PWM1, PB4=PWM2, PB6=PWM3
// Configure via TMR0-TMR3 in PWM mode (see above)
```

## External Signal Counter

```c
void TMR0_ExtCounterInit(UINT32 count_max)
{
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = 0;

    R32_TMR0_CNT_END = count_max;

    // Input mode, count external pulses
    R8_TMR0_CTRL_MOD = RB_TMR_MODE_IN | RB_TMR_COUNT_EN;

    // Input pin: PB1 (CAT0) for TMR0
    R32_PB_DIR &= ~(1 << 1);  // PB1 as input
}
```

## TMR3 Force Enable

TMR3 can force TMR0/1/2 to count simultaneously:

```c
// Enable TMR3 force count for TMR0/1/2
R8_TMR3_INTER_EN |= RB_TMR3_FORCE_EN;
```

## Complete Example: LED Blink with Timer

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

#define LED         (1<<3)
#define LED_INIT()  { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }
#define LED_ON()    { R32_PB_CLR |= LED; }
#define LED_OFF()   { R32_PB_OUT |= LED; }

void mInitSTDIO(void)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;
    x = (x + 5) / 10;
    R8_UART0_LCR = 0x80;
    R8_UART0_DIV = 1;
    R8_UART0_DLM = x >> 8;
    R8_UART0_DLL = x & 0xff;
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= RXD0 | TXD0;
    R32_PB_PU |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f) { R8_UART0_THR = c; while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0); return c; }

volatile UINT32 toggle_count = 0;

__irq void IRQ_Handler(void)
{
    if (R8_INT_FLAG_0 & RB_IF_TMR0) {
        if (R8_TMR0_INT_FLAG & RB_TMR_IF_CYC_END) {
            R8_TMR0_INT_FLAG = RB_TMR_IF_CYC_END;
            toggle_count++;
            R32_PB_OUT ^= LED;  // Toggle LED
        }
    }
}

__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    LED_INIT();
    mInitSTDIO();

    // Enable TMR0 clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_TMR0;

    // Configure TMR0: 1 second period at FREQ_SYS
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = 0;
    R32_TMR0_CNT_END = FREQ_SYS;  // 1 second
    R8_TMR0_CTRL_MOD = RB_TMR_OUT_EN | RB_TMR_COUNT_EN;
    R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;

    PRINT("Timer LED blink started\n");

    while(1);
}
```

## Notes

- TMR0-TMR3 counter max value: 268,435,456 (2^28)
- Timer PWM has 28-bit resolution
- TMR0/TMR1 support NRZI encode/decode mode
- TMR3 can force TMR0/1/2 to count together
- TMR1/TMR2 support DMA transfers
