# USB Type-C Detection

> **Summary**: Configure USB Type-C DFP (host) and UFP (device) orientation detection and power capability negotiation on CH554/CH549 chips.

## Trigger Intent

- "USB Type-C"
- "Type-C detection"
- "DFP UFP"
- "Type-C orientation"
- "USB-C power"
- "CC pin detection"

## Type-C Registers

| Register | Address | Purpose |
|---|---|---|
| USB_C_CTRL | 0xA6 | Type-C control: Rp/Rd enable, CC pull configuration |

### USB_C_CTRL Bits (CH554)

| Bit | Name | Purpose |
|---|---|---|
| 7 | bUCC2_PU1_EN | UCC2 pull-up enable (high current) |
| 6 | bUCC2_PU0_EN | UCC2 pull-up enable (low current) |
| 5 | bUCC2_PD_EN | UCC2 pull-down enable (Rd) |
| 4 | bUCC1_PU1_EN | UCC1 pull-up enable (high current) |
| 3 | bUCC1_PU0_EN | UCC1 pull-up enable (low current) |
| 2 | bUCC1_PD_EN | UCC1 pull-down enable (Rd) |

### Pin Mapping (CH554)

| Pin | Function | ADC Channel |
|---|---|---|
| P1.0 | UCC1 | AIN1 (ADC_CHAN1=0, ADC_CHAN0=1) |
| P1.1 | UCC2 | AIN2 (ADC_CHAN1=1, ADC_CHAN0=0) |

### Power Capability (Rp Values)

| Mode | Rp on UCC1/UCC2 | Current |
|---|---|---|
| Disabled | No pull-up | 0mA |
| Default | PU0=1, PU1=0 | USB default (500mA) |
| 1.5A | PU0=0, PU1=1 | 1.5A |
| 3.0A | PU0=1, PU1=1 | 3.0A |

## Step-by-Step

### DFP Mode (Host)

#### 1. DFP Initialization

```c
void TypeC_DFP_Init(UINT8 power) {
    // Set UCC1, UCC2 to float input
    P1_MOD_OC &= ~(bUCC2 | bUCC1);
    P1_DIR_PU &= ~(bUCC2 | bUCC1);

    // Configure Rp (advertised current)
    switch (power) {
        case 0:  // Disabled
            USB_C_CTRL &= ~(bUCC1_PU1_EN | bUCC1_PU0_EN);
            USB_C_CTRL &= ~(bUCC2_PU1_EN | bUCC2_PU0_EN);
            break;
        case 1:  // Default USB current
            USB_C_CTRL = (USB_C_CTRL & ~bUCC1_PU1_EN) | bUCC1_PU0_EN;
            USB_C_CTRL = (USB_C_CTRL & ~bUCC2_PU1_EN) | bUCC2_PU0_EN;
            break;
        case 2:  // 1.5A
            USB_C_CTRL = (USB_C_CTRL | bUCC1_PU1_EN) & ~bUCC1_PU0_EN;
            USB_C_CTRL = (USB_C_CTRL | bUCC2_PU1_EN) & ~bUCC2_PU0_EN;
            break;
        case 3:  // 3.0A
            USB_C_CTRL |= (bUCC1_PU1_EN | bUCC1_PU0_EN);
            USB_C_CTRL |= (bUCC2_PU1_EN | bUCC2_PU0_EN);
            break;
    }

    // Enable ADC for CC voltage measurement
    ADC_CFG = (ADC_CFG & ~bADC_CLK) | bADC_EN;
    P1_DIR_PU &= ~(bAIN0 | bAIN1);
    mDelayuS(2);
}
```

#### 2. DFP Orientation Detection

```c
#define UCC_CONNECT_THRESHOLD  105

#define UCC_DISCONNECT  0x00
#define UCC1_CONNECT    0x01   // Normal orientation
#define UCC2_CONNECT    0x02   // Reversed orientation
#define UCC_BOTH        0x03   // Both CC connected

UINT8 TypeC_DFP_Detect(void) {
    UINT8 ucc1_val, ucc2_val;

    // Read UCC1 (AIN1: P1.0)
    ADC_CHAN1 = 0; ADC_CHAN0 = 1;
    mDelayuS(1);
    ADC_START = 1;
    while (ADC_START);
    ucc1_val = ADC_DATA;

    // Read UCC2 (AIN2: P1.1)
    ADC_CHAN1 = 1; ADC_CHAN0 = 0;
    mDelayuS(1);
    ADC_START = 1;
    while (ADC_START);
    ucc2_val = ADC_DATA;

    // Determine orientation based on Rd pull-down
    if (ucc1_val <= UCC_CONNECT_THRESHOLD && ucc2_val <= UCC_CONNECT_THRESHOLD)
        return UCC_BOTH;       // Both CC have Rd (unusual)
    if (ucc1_val <= UCC_CONNECT_THRESHOLD && ucc2_val > UCC_CONNECT_THRESHOLD)
        return UCC1_CONNECT;   // Normal: CC1 has Rd
    if (ucc1_val > UCC_CONNECT_THRESHOLD && ucc2_val <= UCC_CONNECT_THRESHOLD)
        return UCC2_CONNECT;   // Reversed: CC2 has Rd

    return UCC_DISCONNECT;
}
```

### UFP Mode (Device)

#### 1. UFP Initialization

```c
void TypeC_UFP_Init(void) {
    // Set UCC1, UCC2 to float input
    P1_MOD_OC &= ~(bUCC2 | bUCC1);
    P1_DIR_PU &= ~(bUCC2 | bUCC1);

    // Enable Rd (pull-down) on both CC pins
    USB_C_CTRL |= (bUCC1_PD_EN | bUCC2_PD_EN);

    // Enable ADC for CC voltage measurement
    ADC_CFG = (ADC_CFG & ~bADC_CLK) | bADC_EN;
    P1_DIR_PU &= ~(bAIN0 | bAIN1);
    mDelayuS(2);
}
```

#### 2. UFP Power Capability Detection

```c
// CC voltage thresholds (8-bit ADC values)
#define POWER_DEFAULT_MIN   13
#define POWER_DEFAULT_MAX   31
#define POWER_1A5_MIN       36
#define POWER_1A5_MAX       59
#define POWER_3A0_MIN       67
#define POWER_3A0_MAX      104

#define PD_DEFAULT   0x00   // 500mA
#define PD_1A5       0x01   // 1.5A
#define PD_3A0       0x02   // 3.0A
#define PD_DISCONNECT 0xFF

UINT8 TypeC_UFP_CheckPower(void) {
    UINT8 adc_val;

    // Check UCC1 (AIN1: P1.0)
    ADC_CHAN1 = 0; ADC_CHAN0 = 1;
    mDelayuS(2);
    ADC_START = 1;
    while (ADC_START);
    adc_val = ADC_DATA;

    if (adc_val >= POWER_3A0_MIN && adc_val <= POWER_3A0_MAX)
        return PD_3A0;
    if (adc_val >= POWER_1A5_MIN && adc_val <= POWER_1A5_MAX)
        return PD_1A5;
    if (adc_val >= POWER_DEFAULT_MIN && adc_val <= POWER_DEFAULT_MAX)
        return PD_DEFAULT;

    // Check UCC2 (AIN2: P1.1)
    ADC_CHAN1 = 1; ADC_CHAN0 = 0;
    mDelayuS(2);
    ADC_START = 1;
    while (ADC_START);
    adc_val = ADC_DATA;

    if (adc_val >= POWER_3A0_MIN && adc_val <= POWER_3A0_MAX)
        return PD_3A0;
    if (adc_val >= POWER_1A5_MIN && adc_val <= POWER_1A5_MAX)
        return PD_1A5;
    if (adc_val >= POWER_DEFAULT_MIN && adc_val <= POWER_DEFAULT_MAX)
        return PD_DEFAULT;

    return PD_DISCONNECT;
}
```

### 3. Full Example: DFP Host

```c
#include "..\Public\CH554.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

#define TYPE_C_DFP
#include "Type_C.H"

void main() {
    UINT8 result;

    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("Type-C DFP Demo\n");

    // Init as DFP with 3.0A capability
    TypeC_DFP_Init(3);

    while(1) {
        result = TypeC_DFP_Detect();
        switch (result) {
            case UCC1_CONNECT:   printf("Normal orientation\n"); break;
            case UCC2_CONNECT:   printf("Reversed orientation\n"); break;
            case UCC_BOTH:       printf("Both CC connected\n"); break;
            case UCC_DISCONNECT: printf("No device\n"); break;
        }
        mDelaymS(500);
    }
}
```

### 4. Full Example: UFP Device

```c
#include "..\Public\CH554.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

#define TYPE_C_UFP
#include "Type_C.H"

void main() {
    UINT8 power;

    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("Type-C UFP Demo\n");

    TypeC_UFP_Init();

    while(1) {
        power = TypeC_UFP_CheckPower();
        switch (power) {
            case PD_DEFAULT:    printf("Default 500mA\n"); break;
            case PD_1A5:        printf("1.5A available\n"); break;
            case PD_3A0:        printf("3.0A available\n"); break;
            case PD_DISCONNECT: printf("No charger\n"); break;
        }
        mDelaymS(500);
    }
}
```

## DFP vs UFP Summary

| Feature | DFP (Host) | UFP (Device) |
|---|---|---|
| CC pull | Rp (pull-up) | Rd (pull-down) |
| Detection | ADC reads Rd on CC | ADC reads Rp voltage on CC |
| Init macro | `#define TYPE_C_DFP` | `#define TYPE_C_UFP` |
| Function | Detect device orientation | Detect host current capability |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| No detection | ADC not enabled | Set `bADC_EN` in `ADC_CFG` |
| Wrong orientation | CC pins swapped | Verify P1.0=UCC1, P1.1=UCC2 |
| ADC reads 0 | Pin configured as output | Set UCC pins to float input |
| Always disconnect | Rp/Rd not configured | Check `USB_C_CTRL` register |
| Build error | Wrong `#define` | Use `TYPE_C_DFP` or `TYPE_C_UFP`, not both |
