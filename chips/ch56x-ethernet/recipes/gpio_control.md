# Recipe: GPIO Control

## Overview

Configure GPIO pins for digital input, output, interrupts, and peripheral pin remapping
on CH56x (CH569).

## GPIO Ports

- **GPIOA**: PA0-PA15 (16 pins)
- **GPIOB**: PB0-PB23 (24 pins)

## Pin Mode Configuration

```c
#include "CH56x_common.h"

// Output mode - push-pull 8mA
GPIOA_ModeCfg(GPIO_Pin_5, GPIO_Slowascent_PP_8mA);

// Output mode - push-pull 16mA (high drive)
GPIOB_ModeCfg(GPIO_Pin_0, GPIO_Highspeed_PP_16mA);

// Input mode - floating
GPIOA_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_Floating);

// Input mode - pull-up with Schmitt trigger
GPIOB_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU_SMT);

// Input mode - pull-down without Schmitt trigger
GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_PD_NSMT);

// Open-drain output 8mA
GPIOB_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_OP_8mA);
```

### Available Modes (GPIOModeTypeDef)

| Mode | Description |
|------|-------------|
| `GPIO_ModeIN_Floating` | Floating input |
| `GPIO_ModeIN_PU_NSMT` | Pull-up, no Schmitt trigger |
| `GPIO_ModeIN_PD_NSMT` | Pull-down, no Schmitt trigger |
| `GPIO_ModeIN_PU_SMT` | Pull-up, with Schmitt trigger |
| `GPIO_ModeIN_PD_SMT` | Pull-down, with Schmitt trigger |
| `GPIO_Slowascent_PP_8mA` | Slow-slew push-pull, 8mA |
| `GPIO_Slowascent_PP_16mA` | Slow-slew push-pull, 16mA |
| `GPIO_Highspeed_PP_8mA` | High-speed push-pull, 8mA |
| `GPIO_Highspeed_PP_16mA` | High-speed push-pull, 16mA |
| `GPIO_ModeOut_OP_8mA` | Open-drain, 8mA |
| `GPIO_ModeOut_OP_16mA` | Open-drain, 16mA |

## Digital Output

```c
// Set pin high
GPIOA_SetBits(GPIO_Pin_5);
GPIOB_SetBits(GPIO_Pin_0);

// Set pin low
GPIOA_ResetBits(GPIO_Pin_5);
GPIOB_ResetBits(GPIO_Pin_0);

// Toggle pin
GPIOA_InverseBits(GPIO_Pin_5);
GPIOB_InverseBits(GPIO_Pin_0);
```

## Digital Input

```c
// Read single pin (returns 0 or non-zero)
UINT32 val = GPIOA_ReadPortPin(GPIO_Pin_3);
if (val) {
    // Pin is high
} else {
    // Pin is low
}

// Read entire port (32-bit, lower bits valid)
UINT32 portA = GPIOA_ReadPort();  // Lower 16 bits valid
UINT32 portB = GPIOB_ReadPort();  // Lower 24 bits valid
```

## GPIO Interrupts

Only specific pins support interrupts:
- GPIOA: PA2, PA3, PA4
- GPIOB: PB3, PB4, PB11, PB12, PB15

```c
// Configure interrupt mode
GPIOA_ITModeCfg(GPIO_Pin_2, GPIO_ITMode_FallEdge);  // Falling edge
GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_RiseEdge);  // Rising edge

// Available interrupt modes:
// GPIO_ITMode_LowLevel   - Low level trigger
// GPIO_ITMode_HighLevel  - High level trigger
// GPIO_ITMode_FallEdge   - Falling edge trigger
// GPIO_ITMode_RiseEdge   - Rising edge trigger

// In interrupt handler:
void TMR0_IRQHandler(void)
{
    // Check and clear GPIO interrupt flags
    if (GPIOA_2_ReadITFlagBit()) {
        GPIOA_2_ClearITFlagBit();
        // Handle PA2 interrupt
    }
    if (GPIOB_3_ReadITFlagBit()) {
        GPIOB_3_ClearITFlagBit();
        // Handle PB3 interrupt
    }
}
```

## Peripheral Pin Remapping

```c
// Remap peripheral function to alternate pins
// s: ENABLE or DISABLE
// perph: peripheral pin remap constant
GPIOPinRemap(ENABLE, xx_PERIPHERAL_REMAP);

// MCO (Microcontroller Clock Output)
GPIOMco(ENABLE, MCO_125);   // Output 125MHz clock on MCO pin
GPIOMco(ENABLE, MCO_25);    // Output 25MHz clock
GPIOMco(ENABLE, MCO_2d5);   // Output 2.5MHz clock
```

## Complete Example: LED Blink

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    // Configure PA5 as output
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_Slowascent_PP_8mA);

    printf("GPIO LED blink example\n");

    while(1)
    {
        GPIOA_InverseBits(GPIO_Pin_5);
        mDelaymS(500);
    }
}
```

## Pin Assignment Notes

- PA0/PA1: Often used for crystal oscillator
- Check the schematic for your specific evaluation board
- Some pins have alternate functions (UART TX/RX, SPI, ETH MDC/MDIO)
- Use `GPIOPinRemap()` to switch between default and alternate pin functions
