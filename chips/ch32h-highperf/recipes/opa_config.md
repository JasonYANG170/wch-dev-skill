# OPA Operational Amplifier

## Overview

CH32H417 has built-in OPA (Operational Amplifier) peripherals for analog signal conditioning. Supports PGA (Programmable Gain Amplifier) mode with configurable gain, differential/single-ended input, and internal feedback. Useful for sensor amplification without external op-amp components.

## Key API Functions

```c
void OPA_DeInit(OPA_TypeDef* OPAx);
void OPA_Init(OPA_TypeDef* OPAx, OPA_InitTypeDef* OPA_InitStruct);
void OPA_Cmd(OPA_TypeDef* OPAx, FunctionalState NewState);
void OPA_StructInit(OPA_InitTypeDef* OPA_InitStruct);
```

## OPA Initialization Example (PGA Mode)

```c
#include "ch32h417.h"

void OPA3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    OPA_InitTypeDef OPA_InitStructure = {0};

    // Enable clocks
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOC |
                          RCC_HB2Periph_OPCM, ENABLE);

    // PA0 as analog input (positive input)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PC2 as analog input (negative input for PGA)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // OPA configuration
    OPA_InitStructure.PSEL = CHP0;          // Positive input: channel 0
    OPA_InitStructure.NSEL = CHN_PGA_8xIN;  // Negative input: PGA 8x
    OPA_InitStructure.Mode = OUT_IO_OUT0;   // Output to IO pin
    OPA_InitStructure.PGADIF = DIF_OFF;     // Single-ended mode
    OPA_InitStructure.FB = FB_ON;           // Internal feedback enabled
    OPA_InitStructure.HS = HS_ON;           // High-speed mode
    OPA_Init(OPA3, &OPA_InitStructure);
    OPA_Cmd(OPA3, ENABLE);
}
```

## OPA Input Selection

### Positive Input (PSEL)

| Macro | Description |
|-------|-------------|
| `CHP0` | Positive input channel 0 |
| `CHP1` | Positive input channel 1 |
| `CHP2` | Positive input channel 2 |
| `CHP3` | Positive input channel 3 |

### Negative Input (NSEL)

| Macro | Description |
|-------|-------------|
| `CHN_PGA_2xIN` | PGA 2x gain |
| `CHN_PGA_4xIN` | PGA 4x gain |
| `CHN_PGA_8xIN` | PGA 8x gain |
| `CHN_PGA_16xIN` | PGA 16x gain |
| `CHN_IO` | External IO pin |

### Output Mode (Mode)

| Macro | Description |
|-------|-------------|
| `OUT_IO_OUT0` | Output to IO pin |
| `OUT_IO_NONE` | No external output |

### Other Options

| Field | Values | Description |
|-------|--------|-------------|
| `PGADIF` | `DIF_ON` / `DIF_OFF` | Differential mode |
| `FB` | `FB_ON` / `FB_OFF` | Internal feedback |
| `HS` | `HS_ON` / `HS_OFF` | High-speed mode |

## Important Notes

- Enable OPCM clock: `RCC_HB2PeriphClockCmd(RCC_HB2Periph_OPCM, ENABLE)`
- Positive and negative inputs must be configured as analog (`GPIO_Mode_AIN`)
- PGA gain is set via the negative input selection
- High-speed mode (`HS_ON`) increases bandwidth but also power consumption
