# GPIO Control

> **Summary**: Configure GPIO pins on CH5xx 8051 chips -- input, output, push-pull, open-drain, quasi-bidirectional mode, and GPIO interrupt setup.

## Trigger Intent

- "Configure GPIO pin"
- "Set pin as output/input"
- "GPIO interrupt setup"
- "Toggle LED on P1.5"
- "Read button on P3.2"

## Pin Configuration Registers

Each port has two configuration registers:

| Register | Purpose |
|---|---|
| `Pn_MOD_OC` | Output mode: 0=push-pull, 1=open-drain |
| `Pn_DIR_PU` | Direction/pullup: 0=input, 1=output (push-pull) or pullup (open-drain) |

Combined mode table:

| Pn_MOD_OC | Pn_DIR_PU | Mode |
|---|---|---|
| 0 | 0 | Float input (no pullup) |
| 0 | 1 | Push-pull output |
| 1 | 0 | Open-drain output, no pullup |
| 1 | 1 | Quasi-bidirectional (standard 8051, with pullup) |

## Available Ports by Chip

| Chip | Ports |
|---|---|
| CH552/CH551 | P1 (8 pins), P3 (8 pins) |
| CH554 | P1, P3 |
| CH555 | P0, P1, P2, P3 |
| CH559 | P0, P1, P2, P3, P4, P5 |
| CH543/CH549 | P1, P3 |

## Step-by-Step

### 1. Set Pin as Push-Pull Output (e.g., P1.5)

```c
P1_MOD_OC &= ~bMOSI;    // Clear bit 5 -> push-pull mode
P1_DIR_PU |= bMOSI;     // Set bit 5 -> output direction
```

Or using bit shift:
```c
P1_MOD_OC &= ~(1<<5);   // Push-pull
P1_DIR_PU |= (1<<5);    // Output
```

### 2. Set Pin as Input (e.g., P1.4 for ADC)

```c
P1_MOD_OC &= ~bAIN1;    // Clear -> not open-drain
P1_DIR_PU &= ~bAIN1;    // Clear -> input mode, no pullup
```

### 3. Set Pin as Quasi-Bidirectional (8051 standard)

```c
P1_MOD_OC |= (1<<5);    // Open-drain
P1_DIR_PU |= (1<<5);    // With pullup = quasi-bidirectional
```

### 4. Read/Write Pin

```c
// Write
P1_5 = 1;               // Set P1.5 high
P1_5 = 0;               // Set P1.5 low

// Read
UINT8 val = P1_4;        // Read P1.4

// Bit operations
P1 |= (1<<5);            // Set P1.5 high
P1 &= ~(1<<5);           // Set P1.5 low
P1 ^= (1<<5);            // Toggle P1.5
```

### 5. GPIO Interrupt Configuration

```c
// Select interrupt mode (edge or level)
GPIO_IE &= ~bIE_IO_EDGE;   // 0 = low/high level trigger
// GPIO_IE |= bIE_IO_EDGE; // 1 = falling/rising edge trigger

// Enable specific pin interrupt
GPIO_IE |= bIE_P1_5_LO;    // P1.5 low level / falling edge
GPIO_IE |= bIE_P1_4_LO;    // P1.4 low level / falling edge
GPIO_IE |= bIE_RST_HI;     // RST pin high level / rising edge

// Enable GPIO interrupt in extended interrupt register
IE_GPIO = 1;                // Enable GPIO interrupt
EA = 1;                     // Enable global interrupts
```

### 6. GPIO Interrupt Service Routine

```c
void GPIO_ISR(void) interrupt INT_NO_GPIO using 1
{
    // Check which pin triggered (read GPIO status)
    if (PIN_FUNC & bIO_INT_ACT) {
        // Handle interrupt
    }
    // Note: GPIO interrupt flag is cleared by reading the status
}
```

### 7. Full Example: LED Blink + Button

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"

#pragma NOAREGS

void main()
{
    CfgFsys();
    mDelaymS(5);

    // P1.5 = LED output (push-pull)
    P1_MOD_OC &= ~bMOSI;
    P1_DIR_PU |= bMOSI;

    // P3.2 = Button input (float)
    P3_MOD_OC &= ~bINT0;
    P3_DIR_PU &= ~bINT0;

    while(1) {
        if (P3_2 == 0) {       // Button pressed (active low)
            P1_5 = 1;          // LED on
        } else {
            P1_5 = 0;          // LED off
        }
        mDelaymS(10);          // Debounce
    }
}
```

## Pin Alternate Functions

Many pins serve multiple functions. Configure `PIN_FUNC` to select:

| Bit | Function |
|---|---|
| bUSB_IO_EN | P3.6/P3.7 as USB (1) or GPIO (0) |
| bUART1_PIN_X | UART1 on P1.6/P1.7 (0) or P3.4/P3.2 (1) |
| bUART0_PIN_X | UART0 on P3.0/P3.1 (0) or P1.2/P1.3 (1) |
| bPWM1_PIN_X | PWM1 on P1.5 (0) or P3.0 (1) |
| bPWM2_PIN_X | PWM2 on P3.4 (0) or P3.1 (1) |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Pin stuck low | Open-drain without pullup | Add external pullup or use quasi-bidirectional mode |
| ADC reads 0 | Pin configured as output | Set pin to float input before ADC sampling |
| GPIO interrupt not firing | IE_GPIO=0 or EA=0 | Enable both GPIO and global interrupts |
