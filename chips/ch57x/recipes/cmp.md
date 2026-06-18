# Comparator (CMP)

> **Applicable summary**: Configure the on-chip comparator for analog voltage comparison, interrupt-driven detection, and timer capture integration.

## Trigger Intent

- "comparator"
- "CMP"
- "analog comparison"
- "voltage threshold detection"

## Prerequisites

| Condition | Requirement |
|---|---|
| Reference project | `CH572EVT/EVT/EXAM/CMP/` |

## Call Chain

```
Step 1: Configure UART debug output (optional)
Step 2: Initialize CMP with reference voltage
Step 3: Configure CMP interrupt
Step 4: (Optional) Route CMP output to timer capture
Step 5: Enable CMP
Step 6: Handle CMP interrupt and read output
```

## Step-by-Step Instructions

### CMP Pin Assignment

| Function | Pin | Description |
|---|---|---|
| CMP IN+ (positive input) | PA0 | External analog input |
| CMP IN- (negative input) | Internal | Configurable reference voltage |

### CMP Initialization

```c
#include "CH57x_common.h"

volatile uint8_t cmp_Itflag = 0;
const CMPOutSelTypeDef ITMode = cmp_out_sel_rise;

int main(void)
{
    SetSysClock(CLK_SOURCE_HSE_PLL_100MHz);

    // Initialize CMP: select input channel 3, internal reference 800mV
    CMP_Init(cmp_sw_3, cmp_nref_level_800);

    // Configure interrupt on rising edge
    CMP_INTCfg(ITMode, ENABLE);
    CMP_ClearITStatus();
    PFIC_EnableIRQ(CMP_IRQn);

    CMP_Enable();

    while (1) {
        if (cmp_Itflag) {
            DelayUs(1);
            if (CMP_ReadAPROut()) {
                PRINT("CMP_1\n");  // Input > reference
            } else {
                PRINT("CMP_0\n");  // Input < reference
            }
            cmp_Itflag = 1;
            CMP_INTCfg(ITMode, ENABLE);
        }
    }
}
```

### CMP Interrupt Handler

```c
__INTERRUPT
__HIGH_CODE
void CMP_IRQHandler(void)
{
    if (CMP_GetITStatus()) {
        CMP_ClearITStatus();
        CMP_INTCfg(ITMode, DISABLE);
        cmp_Itflag = 1;
    }
}
```

### CMP Output to Timer Capture

The comparator output can be routed to the timer capture input for precise timing measurements:

```c
// Route CMP output to timer capture
CMP_OutToTIMCAPCfg(ENABLE);
TMR_CAPTimeoutCfg(0xFFFFFFFF);  // Set capture timeout
R8_TMR_INT_FLAG |= RB_TMR_IF_DATA_ACT;
TMR_CapInit(RiseEdge_To_RiseEdge);

// In main loop, read capture value
if (R8_TMR_INT_FLAG & RB_TMR_IF_DATA_ACT) {
    R8_TMR_INT_FLAG |= RB_TMR_IF_DATA_ACT;
    PRINT("capture: %ld\n", R32_TMR_FIFO);
}
```

### Reference Voltage Levels

| Level | Voltage | Constant |
|---|---|---|
| 0 | 0V | `cmp_nref_level_0` |
| 1 | ~200mV | `cmp_nref_level_200` |
| 2 | ~400mV | `cmp_nref_level_400` |
| 3 | ~600mV | `cmp_nref_level_600` |
| 4 | ~800mV | `cmp_nref_level_800` |
| 5 | ~1000mV | `cmp_nref_level_1000` |
| 6 | ~1200mV | `cmp_nref_level_1200` |
| 7 | ~1400mV | `cmp_nref_level_1400` |

### CMP Input Channel Selection

The `cmp_sw_N` parameter selects which pin is used as the positive input:

| Channel | Constant | Pin |
|---|---|---|
| 0 | `cmp_sw_0` | PA0 |
| 1 | `cmp_sw_1` | PA1 |
| 2 | `cmp_sw_2` | PA2 |
| 3 | `cmp_sw_3` | PA3 |

### Interrupt Output Selection

| Mode | Constant | Description |
|---|---|---|
| Rising edge | `cmp_out_sel_rise` | Interrupt when input crosses above reference |
| Falling edge | `cmp_out_sel_fall` | Interrupt when input crosses below reference |
| Both edges | `cmp_out_sel_both` | Interrupt on any crossing |

## Common Errors

| Error | Cause | Solution |
|---|---|---|
| No interrupt fires | CMP not enabled | Call `CMP_Enable()` after configuration |
| Interrupt only fires once | Flag not cleared / interrupt not re-enabled | Clear flag with `CMP_ClearITStatus()` and re-enable with `CMP_INTCfg()` |
| Unstable output | Input noise | Add external filtering capacitor on input pin |
| Wrong comparison result | Wrong input channel | Verify `cmp_sw_N` matches your hardware connection |

## Reference Project

- `CH572EVT/EVT/EXAM/CMP/` -- Complete comparator example with interrupt and timer capture
