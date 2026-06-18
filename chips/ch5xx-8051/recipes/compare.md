# Voltage Comparator

> **Summary**: Configure and use the built-in analog comparator on CH5xx 8051 chips for voltage level detection, zero-crossing, and interrupt-driven threshold monitoring.

## Trigger Intent

- "Voltage comparator"
- "Analog comparator"
- "Compare voltage"
- "Level detection"
- "Zero crossing"

## Comparator Registers

| Register | Address | Purpose |
|---|---|---|
| ADC_CFG | 0x9A | Comparator enable (bCMP_EN) |
| ADC_CTRL | 0x80 | Channel select, start, flags |

### ADC_CTRL Bits for Comparator

| Bit | Name | Purpose |
|---|---|---|
| 7 | CMPO | (RO) Comparator output: 1 = V+ > V- |
| 6 | CMP_IF | Comparator interrupt flag (write 0 to clear) |
| 5 | ADC_IF | ADC interrupt flag |
| 4 | ADC_START / CMP_START | Start ADC or comparator |
| 3 | CMP_CHAN | Negative input select: 0=AIN1, 1=AIN3 |
| 1:0 | ADC_CHAN[1:0] | Positive input channel select |

### Channel to Pin Mapping

| ADC_CHAN1 | ADC_CHAN0 | Channel | Pin |
|---|---|---|---|
| 0 | 0 | AIN0 (positive) | P1.1 |
| 0 | 1 | AIN1 (positive or negative) | P1.4 |
| 1 | 0 | AIN2 (positive) | P1.5 |
| 1 | 1 | AIN3 (positive or negative) | P3.2 |

### Negative Input Options

| CMP_CHAN | Negative Input | Pin |
|---|---|---|
| 0 | AIN1 | P1.4 |
| 1 | AIN3 | P3.2 |

> **Note**: The negative input can only be AIN1 or AIN3. The positive input can be any of AIN0-AIN3.

## Step-by-Step

### 1. Enable Comparator

```c
void CMP_Enable(void) {
    ADC_CFG |= bCMP_EN;    // Enable comparator power
}

void CMP_Disable(void) {
    ADC_CFG &= ~bCMP_EN;   // Disable comparator power
}
```

### 2. Configure Channels

```c
// Channel enum
typedef enum { IN0, IN1, IN2, IN3 } CMP_Channel;

// Positive input: any of IN0-IN3
// Negative input: only IN1 (P1.4) or IN3 (P3.2)
UINT8 CMP_Init(CMP_Channel pos_ch, CMP_Channel neg_ch) {
    if (pos_ch == neg_ch) return FAIL;

    ADC_CTRL = 0;  // Reset ADC/CMP module

    // Set negative input
    switch (neg_ch) {
        case IN1: ADC_CTRL &= ~0x08; break;  // CMP_CHAN = 0
        case IN3: ADC_CTRL |=  0x08; break;  // CMP_CHAN = 1
        default:  return FAIL;                 // Only IN1 or IN3
    }

    // Set positive input
    ADC_CTRL |= (pos_ch & 0x03);  // ADC_CHAN[1:0]

    return SUCCESS;
}
```

### 3. Read Comparator Output (Polling)

```c
UINT8 CMP_Read(void) {
    ADC_CFG |= bADC_EN;     // Enable ADC power (required for CMP)
    CMP_START = 1;           // Start comparator
    // CMPO bit reflects V+ vs V-
    return CMPO;             // 1 = V+ > V-, 0 = V+ < V-
}
```

### 4. Interrupt Mode

```c
UINT8 CMP_Flag = 0;

void CMP_InitInterrupt(void) {
    ADC_CFG |= bADC_EN;     // Enable power
    CMP_START = 1;           // Start first comparison
    CMP_IF = 0;              // Clear interrupt flag
    IE_CMP = 1;              // Enable comparator interrupt (= IE_ADC)
    EA = 1;                  // Enable global interrupts
}

// ISR
void CMP_ISR(void) interrupt INT_NO_ADC using 1 {
    if (CMP_IF) {
        CMP_IF = 0;          // Clear flag
        CMP_Flag = 1;        // Set application flag
        CMP_START = 1;       // Restart for next comparison
    }
}
```

### 5. Full Example: Threshold Detection

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

UINT8 CMP_Flag = 0;

void CMP_ISR(void) interrupt INT_NO_ADC using 1 {
    if (CMP_IF) {
        CMP_IF = 0;
        CMP_Flag = 1;
        CMP_START = 1;
    }
}

void main() {
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("Comparator Demo\n");

    // Set AIN pins to float input
    P1_MOD_OC &= ~(bAIN0 | bAIN1 | bAIN2);
    P1_DIR_PU &= ~(bAIN0 | bAIN1 | bAIN2);

    // Enable comparator
    ADC_CFG |= bCMP_EN;

    // Compare AIN0 (P1.1) vs AIN1 (P1.4)
    // Positive = P1.1, Negative = P1.4
    ADC_CTRL = 0;
    // Negative = IN1 (P1.4)
    ADC_CTRL &= ~0x08;       // CMP_CHAN = 0 -> AIN1
    // Positive = IN0 (P1.1)
    ADC_CTRL |= 0x00;        // ADC_CHAN = 0 -> AIN0

    // Enable interrupt
    ADC_CFG |= bADC_EN;
    CMP_START = 1;
    CMP_IF = 0;
    IE_CMP = 1;
    EA = 1;

    while(1) {
        if (CMP_Flag) {
            CMP_Flag = 0;
            if (CMPO) {
                printf("V+ > V- (P1.1 > P1.4)\n");
            } else {
                printf("V+ < V- (P1.1 < P1.4)\n");
            }
        }
        mDelaymS(100);
    }
}
```

### 6. Polled Full Example

```c
void main() {
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("Comparator Polling\n");

    P1_MOD_OC &= ~(bAIN0 | bAIN1);
    P1_DIR_PU &= ~(bAIN0 | bAIN1);

    ADC_CFG |= bCMP_EN;

    // Compare P1.1 (V+) vs P1.4 (V-)
    ADC_CTRL = 0;

    while(1) {
        ADC_CFG |= bADC_EN;
        CMP_START = 1;
        while (!CMP_IF);     // Wait for result
        CMP_IF = 0;
        printf("CMPO=%d\n", (UINT16)CMPO);
        mDelaymS(500);
    }
}
```

## Channel Selection Quick Reference

| Use Case | Positive | Negative | ADC_CTRL |
|---|---|---|---|
| P1.1 vs P1.4 | AIN0 | AIN1 | `0x00` |
| P1.5 vs P1.4 | AIN2 | AIN1 | `0x02` |
| P1.1 vs P3.2 | AIN0 | AIN3 | `0x08` |
| P1.5 vs P3.2 | AIN2 | AIN3 | `0x0A` |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Always reads 0 | ADC power off | Set `bADC_EN` in `ADC_CFG` |
| No interrupt | `IE_CMP` or `EA` not set | Enable both comparator and global interrupts |
| Wrong result | Channel mismatch | Verify positive and negative pin assignments |
| Pin stuck | Pin configured as output | Set pin to float input before use |
| CMP_IF not clearing | Wrote 1 instead of 0 | Write 0 to clear `CMP_IF` |
