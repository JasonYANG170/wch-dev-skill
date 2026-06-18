# ADC Reading

> **Summary**: Configure and use the ADC on CH5xx 8051 chips -- channel selection, clock configuration, polling and interrupt modes, comparator mode.

## Trigger Intent

- "Read ADC value"
- "ADC channel select"
- "Analog to digital conversion"
- "Voltage measurement"
- "ADC interrupt mode"
- "Comparator mode"

## ADC Registers

| Register | Address | Purpose |
|---|---|---|
| ADC_CTRL | 0x80 | Control (start, channel select, flags) |
| ADC_CFG | 0x9A | Config (enable, clock speed) |
| ADC_DATA | 0x9F | (RO) ADC result data |

### ADC_CTRL Bits (sbit)

| Bit | Name | Purpose |
|---|---|---|
| 7 | CMPO | (RO) Comparator output |
| 6 | CMP_IF | Comparator interrupt flag |
| 5 | ADC_IF | ADC finished interrupt flag |
| 4 | ADC_START | Start ADC (auto-clear when done) |
| 3 | CMP_CHAN | Comparator IN- channel (0=AIN1, 1=AIN3) |
| 1 | ADC_CHAN1 | ADC channel select high bit |
| 0 | ADC_CHAN0 | ADC channel select low bit |

### ADC Channel Selection

| ADC_CHAN1 | ADC_CHAN0 | Channel | Pin |
|---|---|---|---|
| 0 | 0 | AIN0 | P1.1 |
| 0 | 1 | AIN1 | P1.4 |
| 1 | 0 | AIN2 | P1.5 |
| 1 | 1 | AIN3 | P3.2 |

### ADC_CFG Bits

| Bit | Name | Purpose |
|---|---|---|
| 3 | bADC_EN | ADC power enable |
| 2 | bCMP_EN | Comparator power enable |
| 0 | bADC_CLK | Clock: 0=slow (384 cycles), 1=fast (96 cycles) |

## ADC Resolution by Chip

| Chip | Resolution | Voltage Range |
|---|---|---|
| CH552/CH551 | 8-bit (0-255) | 0 to VCC (typically 3.3V or 5V) |
| CH554 | 8-bit | 0 to VCC |
| CH555 | 12-bit (0-4095) | 0 to VCC |
| CH559 | 12-bit | 0 to VCC |
| CH543/CH549 | 8-bit or 12-bit | 0 to VCC |

## Step-by-Step

### 1. ADC Init (Polling Mode)

```c
void ADC_Init(void)
{
    ADC_CFG = (ADC_CFG & ~bADC_CLK) | 0x01;  // Fast clock (96 cycles)
    ADC_CFG |= bADC_EN;                       // Enable ADC power
}
```

### 2. ADC Init (Interrupt Mode)

```c
void ADC_InitInt(void)
{
    ADC_CFG = (ADC_CFG & ~bADC_CLK) | 0x01;
    ADC_CFG |= bADC_EN;
    ADC_IF = 0;                // Clear ADC interrupt flag
    IE_ADC = 1;                // Enable ADC interrupt
    EA = 1;                    // Enable global interrupts
}
```

### 3. Select ADC Channel

```c
void ADC_SelectChannel(UINT8 ch)
{
    switch(ch) {
        case 0:  // AIN0 (P1.1)
            ADC_CHAN1 = 0; ADC_CHAN0 = 0;
            P1_DIR_PU &= ~bAIN0;    // Set P1.1 as float input
            break;
        case 1:  // AIN1 (P1.4)
            ADC_CHAN1 = 0; ADC_CHAN0 = 1;
            P1_DIR_PU &= ~bAIN1;
            break;
        case 2:  // AIN2 (P1.5)
            ADC_CHAN1 = 1; ADC_CHAN0 = 0;
            P1_DIR_PU &= ~bAIN2;
            break;
        case 3:  // AIN3 (P3.2)
            ADC_CHAN1 = 1; ADC_CHAN0 = 1;
            P3_DIR_PU &= ~bAIN3;
            break;
    }
}
```

### 4. Read ADC (Polling)

```c
UINT8 ADC_ReadPolling(void)
{
    ADC_IF = 0;                // Clear flag
    ADC_START = 1;             // Start conversion
    while(ADC_START == 0 && ADC_IF == 0);  // Wait for done
    ADC_IF = 0;
    return ADC_DATA;           // Return 8-bit result
}
```

### 5. Read ADC (Interrupt)

```c
volatile UINT8 adcResult;

void ADC_ISR(void) interrupt INT_NO_ADC using 1
{
    if (ADC_IF) {
        adcResult = ADC_DATA;  // Read result
        ADC_IF = 0;            // Clear flag
    }
    if (CMP_IF) {
        // Comparator result changed
        CMP_IF = 0;
    }
}
```

### 6. Full Example: Read All 4 Channels

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

void main()
{
    UINT8 ch;
    UINT8 val;

    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();

    ADC_Init();

    while(1) {
        for(ch = 0; ch < 4; ch++) {
            ADC_SelectChannel(ch);
            val = ADC_ReadPolling();
            printf("CH%d=%d ", (UINT16)ch, (UINT16)val);
        }
        printf("\n");
        mDelaymS(500);
    }
}
```

### 7. Comparator Mode

```c
void Comparator_Init(UINT8 pos_ch, UINT8 neg_ch)
{
    ADC_CFG |= bCMP_EN;       // Enable comparator power

    // Set positive input channel
    switch(pos_ch) {
        case 0: ADC_CHAN1=0; ADC_CHAN0=0; break;
        case 1: ADC_CHAN1=0; ADC_CHAN0=1; break;
        case 2: ADC_CHAN1=1; ADC_CHAN0=0; break;
        case 3: ADC_CHAN1=1; ADC_CHAN0=1; break;
    }

    // Set negative input (AIN1 or AIN3)
    if (neg_ch == 1) CMP_CHAN = 0;    // AIN1
    else if (neg_ch == 3) CMP_CHAN = 1; // AIN3

    CMP_IF = 0;               // Clear comparator flag
    IE_ADC = 1;               // Enable ADC/comparator interrupt
}
```

### 8. Voltage Calculation

```c
// For 8-bit ADC (CH552/CH554):
// Voltage = ADC_DATA * Vref / 255
// Example: ADC_DATA = 128, Vref = 3.3V -> 1.65V

// For 12-bit ADC (CH555/CH559):
// Voltage = ADC_DATA * Vref / 4095
```

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| ADC reads 0 | Pin configured as output | Set pin to float input (Pn_MOD_OC=0, Pn_DIR_PU=0) |
| ADC reads max | Pin floating/no connection | Ensure analog source is connected |
| Noisy readings | Fast clock + high impedance source | Use slow clock (bADC_CLK=0) or add buffer |
| Interrupt not firing | IE_ADC=0 or EA=0 | Enable both ADC and global interrupts |
