# Timer and PWM

> **Summary**: Configure and use Timer0/1/2 and PWM1/2 on CH5xx 8051 chips -- timer modes, periodic interrupts, baud rate generation, input capture, PWM output.

## Trigger Intent

- "Timer interrupt setup"
- "PWM output configuration"
- "Periodic timer"
- "Input capture"
- "Baud rate generator"
- "Timer mode 2 auto-reload"

## Timer Resources

| Chip | Timers | PWM Channels |
|---|---|---|
| CH552/CH551 | Timer0, Timer1, Timer2 | PWM1, PWM2 |
| CH554 | Timer0, Timer1, Timer2 | PWM1, PWM2 |
| CH555 | Timer0, Timer1, Timer2 | PWM1, PWM2 |
| CH559 | Timer0, Timer1, Timer2, Timer3 | PWM1-PWM4 |
| CH543/CH549 | Timer0, Timer1, Timer2 | PWM1, PWM2 |

## Timer0/1 Registers

| Register | Address | Purpose |
|---|---|---|
| TCON | 0x88 | Control (run, overflow flags, INT0/1 mode) |
| TMOD | 0x89 | Mode (low nibble=T0, high nibble=T1) |
| TL0 | 0x8A | Timer0 count low |
| TH0 | 0x8C | Timer0 count high |
| TL1 | 0x8B | Timer1 count low |
| TH1 | 0x8D | Timer1 count high |

### TMOD Bits

| Bits | Timer | Purpose |
|---|---|---|
| [1:0] | T0 | T0 mode: 00=13bit, 01=16bit, 10=8bit auto-reload, 11=split |
| [2] | T0 | T0 clock: 0=timer(Fsys/12), 1=counter(T0 pin) |
| [3] | T0 | T0 gate: 0=run when TR0=1, 1=run when TR0=1 AND INT0=high |
| [5:4] | T1 | T1 mode (same as T0) |
| [6] | T1 | T1 clock (same as T0) |
| [7] | T1 | T1 gate (same as T0) |

### Timer0/1 Modes

| Mode | Description |
|---|---|
| 0 | 13-bit timer (TH + lower 5 bits of TL) |
| 1 | 16-bit timer (TH:TL) |
| 2 | 8-bit auto-reload (TH=reload, TL=counter) |
| 3 | T0: split into two 8-bit timers; T1: stopped |

## Timer2 Registers

| Register | Address | Purpose |
|---|---|---|
| T2CON | 0xC8 | Control (run, overflow, capture, clock source) |
| T2MOD | 0xC9 | Mode (clock speed, capture mode) |
| TL2 | 0xCC | Timer2 count low |
| TH2 | 0xCD | Timer2 count high |
| RCAP2L | 0xCA | Reload/capture value low |
| RCAP2H | 0xCB | Reload/capture value high |
| T2CAP1L | 0xCE | (RO) Capture1 value low |
| T2CAP1H | 0xCF | (RO) Capture1 value high |

## PWM Registers

| Register | Address | Purpose |
|---|---|---|
| PWM_DATA1 | 0x9C | PWM1 duty data |
| PWM_DATA2 | 0x9B | PWM2 duty data |
| PWM_CTRL | 0x9D | Control (polarity, output enable, interrupt) |
| PWM_CK_SE | 0x9E | Clock divisor |

### PWM_CTRL Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bPWM_IE_END | Enable cycle-end interrupt |
| 6 | bPWM2_POLAR | PWM2 polarity (0=default, 1=inverted) |
| 5 | bPWM1_POLAR | PWM1 polarity |
| 4 | bPWM_IF_END | Cycle-end interrupt flag |
| 3 | bPWM2_OUT_EN | PWM2 output enable |
| 2 | bPWM1_OUT_EN | PWM1 output enable |
| 1 | bPWM_CLR_ALL | Force clear FIFO and count |

## PWM Pin Assignment

| Channel | Default Pin | Alternate Pin |
|---|---|---|
| PWM1 | P1.5 (MOSI) | P3.0 (RXD) via bPWM1_PIN_X |
| PWM2 | P3.4 (T0) | P3.1 (TXD) via bPWM2_PIN_X |

## Step-by-Step

### 1. Timer0 Mode 1 (16-bit, periodic interrupt)

```c
void Timer0_Init(UINT16 interval)
{
    TMOD = TMOD & 0xF0 | 0x01;  // Timer0 mode 1 (16-bit)
    UINT16 reload = 65536 - interval;
    TH0 = (reload >> 8) & 0xFF;
    TL0 = reload & 0xFF;
    ET0 = 1;                     // Enable Timer0 interrupt
    TR0 = 1;                     // Start Timer0
    EA = 1;
}

void Timer0_ISR(void) interrupt INT_NO_TMR0 using 1
{
    // Reload (mode 1 is NOT auto-reload)
    TH0 = (reload >> 8) & 0xFF;
    TL0 = reload & 0xFF;
    // Toggle LED or set flag
    P1_5 = ~P1_5;
}
```

### 2. Timer1 Mode 2 (8-bit auto-reload, for baud rate)

```c
void Timer1_BaudRate(UINT32 baud)
{
    TMOD = TMOD & 0x0F | 0x20;  // Timer1 mode 2
    TH1 = 0 - FREQ_SYS/16/baud; // Auto-reload value
    TL1 = TH1;
    TR1 = 1;                     // Start Timer1
    // Timer1 is now generating baud rate clock for UART0
}
```

### 3. Timer2 16-bit Auto-Reload

```c
void Timer2_Init(UINT16 interval)
{
    RCLK = 0; TCLK = 0;
    CP_RL2 = 0;                  // Auto-reload mode
    C_T2 = 0;                    // Timer mode (not counter)

    UINT16 reload = 65536 - interval;
    RCAP2L = TL2 = reload & 0xFF;
    RCAP2H = TH2 = (reload >> 8) & 0xFF;

    ET2 = 1;                     // Enable Timer2 interrupt
    TR2 = 1;                     // Start Timer2
    EA = 1;
}

void Timer2_ISR(void) interrupt INT_NO_TMR2 using 1
{
    if (TF2) {
        TF2 = 0;                 // Clear overflow flag (must clear manually)
        // Periodic task
    }
}
```

### 4. Timer2 Input Capture

```c
void Capture2_Init(UINT8 mode)
{
    RCLK = 0; TCLK = 0;
    C_T2 = 0;
    EXEN2 = 1;                   // Enable T2EX trigger
    CP_RL2 = 1;                  // Capture mode

    // Edge select: mode 0=falling-to-falling, 1=any edge, 3=rising-to-rising
    T2MOD = (T2MOD & ~0x0C) | (mode << 2);

    TR2 = 1;
    ET2 = 1;
    EA = 1;
}

// In Timer2 ISR:
if (EXF2) {
    UINT16 capVal = RCAP2H << 8 | RCAP2L;  // Captured value
    EXF2 = 0;
}
```

### 5. PWM1 Output

```c
void PWM1_Init(UINT8 duty)
{
    // Set PWM1 pin as output
    P1_MOD_OC &= ~bPWM1;       // Push-pull
    P1_DIR_PU |= bPWM1;

    PWM_CK_SE = 0;              // Clock divider (0=max speed)
    PWM_DATA1 = duty;           // Duty cycle (0-255)
    PWM_CTRL |= bPWM1_OUT_EN;  // Enable PWM1 output
    PWM_CTRL |= bPWM_IF_END;   // Clear interrupt flag
}
```

### 6. PWM2 Output with Interrupt

```c
void PWM2_InitInt(UINT8 duty)
{
    // Set PWM2 pin
    P3_MOD_OC &= ~bPWM2;
    P3_DIR_PU |= bPWM2;

    PWM_CK_SE = 0;
    PWM_DATA2 = duty;
    PWM_CTRL |= bPWM2_OUT_EN;
    PWM_CTRL |= bPWM_IE_END;   // Enable cycle-end interrupt
    PWM_CTRL |= bPWM_IF_END;   // Clear flag
    IE_PWMX = 1;
    EA = 1;
}

void PWM_ISR(void) interrupt INT_NO_PWMX using 1
{
    PWM_CTRL |= bPWM_IF_END;   // Clear interrupt flag
    // Update duty if needed
    PWM_DATA1 = newDuty;
}
```

### 7. Change PWM Duty Cycle

```c
// PWM duty = PWM_DATAn / 256 * 100%
// Example: PWM_DATA1 = 128 -> 50% duty
void PWM_SetDuty(UINT8 channel, UINT8 duty)
{
    if (channel == 1) PWM_DATA1 = duty;
    else if (channel == 2) PWM_DATA2 = duty;
}
```

### 8. Timer Clock Speed Options

| T2MOD bits | Clock source |
|---|---|
| bTMR_CLK=0, bTn_CLK=0 | Standard: Fsys/12 |
| bTMR_CLK=0, bTn_CLK=1 | Fast: Fsys/4 |
| bTMR_CLK=1, bTn_CLK=1 | Fastest: Fsys (no divider) |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Timer interrupt not firing | ET0/ET1/ET2=0 or EA=0 | Enable timer and global interrupts |
| TF2 not auto-cleared | Timer2 requires manual clear | Always clear TF2=0 in ISR |
| PWM output stuck | Pin not configured as output | Set push-pull output for PWM pin |
| Wrong period | FREQ_SYS mismatch | Verify CLOCK_CFG matches FREQ_SYS definition |
| PWM1 conflicts with SPI | Both use P1.5 | Remap PWM1 to P3.0 via bPWM1_PIN_X |
