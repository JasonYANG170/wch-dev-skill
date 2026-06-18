# TouchKey (Capacitive Touch Detection)

> **Summary**: Configure and use capacitive touch key detection on CH5xx 8051 chips -- channel selection, baseline calibration, polling and interrupt modes, press/release detection.

## Trigger Intent

- "Touch key detection"
- "Capacitive touch"
- "TouchKey setup"
- "Touch sensor"
- "Capacitive button"

## TouchKey Registers

| Register | Address | Purpose |
|---|---|---|
| TKEY_CTRL | 0xC2 | Control: channel select, enable, status flag |
| TKEY_DAT | 0xCC | (RO) TouchKey measurement result |

### TKEY_CTRL Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bTKC_2MS | Scan frequency: 0=1ms, 1=2ms |
| 6 | -- | Reserved |
| 5 | bTKC_CHAN2 | Channel select bit 2 |
| 4 | bTKC_CHAN1 | Channel select bit 1 |
| 3 | bTKC_CHAN0 | Channel select bit 0 |
| 2 | -- | Reserved |
| 1 | -- | Reserved |
| 0 | bTKC_IF | (RO) Measurement complete flag |

### Channel to Pin Mapping

| bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0 | Channel | Pin |
|---|---|---|---|---|
| 0 | 0 | 0 | No channel | -- |
| 0 | 0 | 1 | TIN0 | P1.0 |
| 0 | 1 | 0 | TIN1 | P1.1 |
| 0 | 1 | 1 | TIN2 | P1.4 |
| 1 | 0 | 0 | TIN3 | P1.5 |
| 1 | 0 | 1 | TIN4 | P1.6 |
| 1 | 1 | 0 | TIN5 | P1.7 |
| 1 | 1 | 1 | Enable core | No channel |

### Supported Chips

| Chip | Touch Channels | Pins |
|---|---|---|
| CH552/CH554 | 6 (TIN0-TIN5) | P1.0, P1.1, P1.4-P1.7 |
| CH543/CH549 | 4-6 | P1.x (varies) |
| CH555/CH557 | 6+ | P1.x, P2.x (varies) |

## Step-by-Step

### 1. Channel Select

```c
#define TOUCH_NUM  4

// Channel code table (matches bTKC_CHAN bits + 1)
UINT8 TK_Code[TOUCH_NUM] = {
    0x03,  // TIN2 (P1.4)
    0x04,  // TIN3 (P1.5)
    0x05,  // TIN4 (P1.6)
    0x06   // TIN5 (P1.7)
};

UINT8 TK_SelectChannel(UINT8 ch) {
    if (ch < TOUCH_NUM) {
        TKEY_CTRL = (TKEY_CTRL & 0xF8) | TK_Code[ch];
        return SUCCESS;
    }
    return FAIL;
}
```

### 2. Pin Configuration

```c
// Touch pins MUST be float input (no pullup, no output)
// Clear both Pn_MOD_OC and Pn_DIR_PU for touch pins
void TK_PinInit(UINT8 pin_mask) {
    P1_DIR_PU &= ~pin_mask;   // No pullup
    P1_MOD_OC &= ~pin_mask;   // Push-pull mode (but DIR=0 means float input)
}
```

### 3. Baseline Calibration

```c
#define SAMPLE_TIMES  5

UINT16 Key_FreeBuf[TOUCH_NUM];

UINT8 TK_Calibrate(void) {
    UINT8 i, j;
    UINT16 sum;
    UINT16 overtime;

    for (i = 0; i < TOUCH_NUM; i++) {
        sum = 0;
        j = SAMPLE_TIMES;
        TK_SelectChannel(i);
        while (j--) {
            overtime = 0;
            while (!(TKEY_CTRL & bTKC_IF)) {
                if (++overtime == 0) return FAIL;  // Timeout
            }
            sum += TKEY_DAT;
        }
        Key_FreeBuf[i] = sum / SAMPLE_TIMES;
    }
    return SUCCESS;
}
```

### 4. Polling Measurement

```c
#define THRESHOLD_DOWN  2000
#define THRESHOLD_UP    500

UINT16 Key_DataBuf[TOUCH_NUM];
UINT8  Touch_State = 0;

UINT8 TK_Measure(void) {
    UINT8 i, j;
    UINT16 sum, diff;

    for (i = 0; i < TOUCH_NUM; i++) {
        sum = 0;
        j = SAMPLE_TIMES;
        TK_SelectChannel(i);
        while (j--) {
            while (!(TKEY_CTRL & bTKC_IF));
            sum += TKEY_DAT;
        }
        Key_DataBuf[i] = sum / SAMPLE_TIMES;

        // Calculate absolute difference from baseline
        if (Key_FreeBuf[i] > Key_DataBuf[i])
            diff = Key_FreeBuf[i] - Key_DataBuf[i];
        else
            diff = Key_DataBuf[i] - Key_FreeBuf[i];

        // Hysteresis: different thresholds for press and release
        if (diff > THRESHOLD_DOWN) {
            if (!(Touch_State & (1 << i))) {
                printf("Key %d pressed, val=%d\n", (UINT16)i, Key_DataBuf[i]);
            }
            Touch_State |= (1 << i);
        } else if (diff < THRESHOLD_UP) {
            if (Touch_State & (1 << i)) {
                printf("Key %d released\n", (UINT16)i);
            }
            Touch_State &= ~(1 << i);
        }
    }
    return SUCCESS;
}
```

### 5. Interrupt Mode

```c
void TK_InitInterrupt(void) {
    IE_TKEY = 1;   // Enable TouchKey interrupt
    EA = 1;         // Enable global interrupts
}

// Interrupt service routine
void TK_ISR(void) interrupt INT_NO_TKEY using 1 {
    static UINT8 ch = 0;
    UINT16 key_val = TKEY_DAT;

    // Compare with baseline
    if (key_val < (Key_FreeBuf[ch] - THRESHOLD_DOWN)) {
        Touch_State |= (1 << ch);
    }

    // Advance to next channel
    if (++ch >= TOUCH_NUM) ch = 0;
    TK_SelectChannel(ch);
}
```

### 6. Full Example

```c
#include "..\Public\CH552.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

#define TOUCH_NUM       4
#define SAMPLE_TIMES    5
#define THRESHOLD_DOWN  2000
#define THRESHOLD_UP    500

UINT8  TK_Code[TOUCH_NUM] = {0x03, 0x04, 0x05, 0x06};
UINT16 Key_FreeBuf[TOUCH_NUM];
UINT16 Key_DataBuf[TOUCH_NUM];
UINT8  Touch_State = 0;

UINT8 TK_SelectChannel(UINT8 ch) {
    TKEY_CTRL = (TKEY_CTRL & 0xF8) | TK_Code[ch];
    return SUCCESS;
}

void main() {
    UINT8 i, j;
    UINT16 sum, diff, overtime;

    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("TouchKey Demo\n");

    // Set touch pins to float input
    P1_DIR_PU &= ~(bAIN0 | bAIN1 | bAIN2 | bAIN3);
    P1_MOD_OC &= ~(bAIN0 | bAIN1 | bAIN2 | bAIN3);

    // Calibrate baseline
    for (i = 0; i < TOUCH_NUM; i++) {
        sum = 0;
        for (j = 0; j < SAMPLE_TIMES; j++) {
            TK_SelectChannel(i);
            overtime = 0;
            while (!(TKEY_CTRL & bTKC_IF)) {
                if (++overtime == 0) break;
            }
            sum += TKEY_DAT;
        }
        Key_FreeBuf[i] = sum / SAMPLE_TIMES;
        printf("CH%d base=%d\n", (UINT16)i, Key_FreeBuf[i]);
    }

    while(1) {
        for (i = 0; i < TOUCH_NUM; i++) {
            sum = 0;
            for (j = 0; j < SAMPLE_TIMES; j++) {
                TK_SelectChannel(i);
                while (!(TKEY_CTRL & bTKC_IF));
                sum += TKEY_DAT;
            }
            Key_DataBuf[i] = sum / SAMPLE_TIMES;
            diff = (Key_FreeBuf[i] > Key_DataBuf[i]) ?
                   (Key_FreeBuf[i] - Key_DataBuf[i]) :
                   (Key_DataBuf[i] - Key_FreeBuf[i]);
            if (diff > THRESHOLD_DOWN) {
                Touch_State |= (1 << i);
            } else if (diff < THRESHOLD_UP) {
                Touch_State &= ~(1 << i);
            }
        }
        // Report state
        for (i = 0; i < TOUCH_NUM; i++) {
            if (Touch_State & (1 << i)) {
                printf("K%d ", (UINT16)i);
            }
        }
        printf("\n");
        mDelaymS(100);
    }
}
```

## Tuning Tips

| Parameter | Typical Value | Adjustment |
|---|---|---|
| SAMPLE_TIMES | 3-10 | Higher = more stable, slower |
| THRESHOLD_DOWN | 1000-3000 | Lower = more sensitive |
| THRESHOLD_UP | 300-800 | Must be < THRESHOLD_DOWN for hysteresis |
| Scan frequency | 1ms or 2ms | Set via `bTKC_2MS` bit |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| No detection | Pin configured as output | Set pin to float input (`Pn_MOD_OC=0, Pn_DIR_PU=0`) |
| Always triggered | Threshold too low | Increase `THRESHOLD_DOWN` |
| Never triggered | Threshold too high | Decrease `THRESHOLD_DOWN`, check wiring |
| Unstable readings | Pin has pullup | Remove pullup, use float input |
| Timeout | TouchKey clock not running | Ensure `CfgFsys()` called, check `CLOCK_CFG` |
| Wrong channel | Channel code mismatch | Verify `TK_Code[]` matches actual pin connections |
