# Recipe: OPA and Comparator Configuration

## Overview

Configure the on-chip Operational Amplifier (OPA) and Comparator (CMP) peripherals on CH32F20x and CH32M030. These analog peripherals can be used for signal conditioning, voltage following, programmable gain amplification, and voltage comparison without external components.

## Key API Functions

### CH32F20x OPA

```c
// OPA initialization (standard peripheral library)
void OPA_Init(OPA_InitTypeDef* OPA_InitStruct);
void OPA_Cmd(uint32_t OPA_NUM, FunctionalState NewState);
```

### CH32M030 OPA (ISP mode with PGA)

```c
// OPA ISP initialization (CH32M030)
void OPA_ISP_Init(uint32_t OPA_ISP_NUM, OPA_ISP_InitTypeDef* OPA_ISP_InitStruct);
void OPA_ISP_Cmd(uint32_t OPA_ISP_NUM, FunctionalState NewState);
```

### CH32M030 Comparator

```c
// Comparator initialization (CH32M030)
void CMP3_Init(CMP3_InitTypeDef* CMP3_InitStruct);
void CMP3_Cmd(FunctionalState NewState);
```

## CH32F20x OPA Init Structure

```c
typedef struct {
    uint32_t OPA_NUM;   // OPA1, OPA2 (channel number)
    uint32_t PSEL;      // Positive input: CHP0, CHP1
    uint32_t NSEL;      // Negative input: CHN0, CHN1
    uint32_t Mode;      // Output mode: OUT_IO_OUT0, OUT_IO_OUT1, OUT_TO_ADC
} OPA_InitTypeDef;
```

## CH32M030 OPA ISP Init Structure (PGA mode)

```c
typedef struct {
    uint32_t OPA_ISP_GAIN;        // Gain: OPA_ISP_GAIN_4, OPA_ISP_GAIN_8,
                                  //       OPA_ISP_GAIN_16, OPA_ISP_GAIN_32
    uint32_t OPA_ISP_VFBIAS;     // Bias voltage: OPA_ISP_VFBIAS_0_55V, etc.
    uint32_t OPA_ISP_NESL;       // Negative input: OPA_ISP_NESL_VSS (single-ended),
                                  //                  OPA_ISP_NESL_ISN (differential)
    uint32_t OPA_ISP_SEL_IO;     // ADC connection: OPA_ISP_SEL_TO_ADC_ON/OFF
    uint32_t OPA_ISP_QDET;       // Quick detect: OPA_ISP_QDET_EN_ON/OFF
    uint32_t OPA_ISP_QDET_PD30K; // 30K pull-down: OPA_ISP_QDET_PD30K_ON/OFF
    uint32_t OPA_ISP_QDET_VBSEL; // Voltage select: OPA_ISP_QDET_VBSEL_ON/OFF
} OPA_ISP_InitTypeDef;
```

## CH32M030 Comparator Init Structure

```c
typedef struct {
    uint32_t CMP3_INT_EN;        // Interrupt enable: CMP3_INT_EN_ON/OFF
    uint32_t CMP3_CAP;           // Timer capture: CMP3_CAP_EN_T2_Channels_ON/OFF
    uint32_t CMP3_PSEL;          // Positive input: CMP3_PSEL_PB3, etc.
    uint32_t CMP3_NSEL;          // Negative input: CMP3_NSEL_PA6, etc.
    uint32_t CMP3_HYS;           // Hysteresis: CMP3_HYS_0mv, CMP3_HYS_10mv, etc.
    uint32_t CMP3_RMID;          // Reference midpoint: CMP3_RMID_EN_ON/OFF
    uint32_t CMP3_AT_IO;         // Analog output: CMP3_AT_IO_ON/OFF
    uint32_t CMP3_PT_IO;         // Port output: CMP3_PT_IO_ON/OFF
    uint32_t CMP3_CH_SW_NUM;     // Channel switch: CMP3_CH_SW_NUM_0, etc.
    uint32_t CMP3_PT_IO_SEL;     // Output pin: CMP3_PT_IO_PB4, etc.
    uint32_t CMP3_TRG_GATE;      // Trigger gate: CMP3_TRG_GATE_ON/OFF
    uint32_t CMP3_BK_EN;         // Break enable: CMP3_BK_EN_TIM1BKIN_ON/OFF
    uint32_t CMP3_COM_EN;        // Common mode: CMP3_COM_MODE_ON/OFF
} CMP3_InitTypeDef;
```

## CH32F20x OPA Pin Mapping

| OPA | Positive Input | Pin | Negative Input | Pin | Output | Pin |
|-----|---------------|-----|---------------|-----|--------|-----|
| OPA1 | CHP0 | PA2 | CHN0 | PA0 | OUT_IO_OUT0 | PA3 |
| OPA1 | CHP1 | PB0 | CHN1 | PA6 | OUT_IO_OUT0 | PA3 |
| OPA2 | CHP0 | PA7 | CHN0 | PA4 | OUT_IO_OUT1 | PA1 |

## Example: CH32F20x OPA Voltage Follower

Connect the positive input to the external voltage, and tie the output back to the negative input for unity-gain buffering:

```c
#include "debug.h"

// Pin connections:
// OPA1_CHP1 = PB0 (positive input, external voltage)
// OPA1_CHN1 = PA6 (negative input, tied to output)
// OPA1_OUT  = PA3 (output, connected to PA6 externally)

void OPA1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    OPA_InitTypeDef OPA_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // PA6 = negative input (floating)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PB0 = positive input (floating)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PA3 = output (configured by OPA peripheral automatically)

    OPA_InitStructure.OPA_NUM = OPA1;
    OPA_InitStructure.PSEL = CHP1;       // Positive input = PB0
    OPA_InitStructure.NSEL = CHN1;       // Negative input = PA6
    OPA_InitStructure.Mode = OUT_IO_OUT0; // Output on PA3
    OPA_Init(&OPA_InitStructure);

    OPA_Cmd(OPA1, ENABLE);
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("OPA Voltage Follower Test\r\n");

    OPA1_Init();

    // Read OPA output via ADC on PA3 (ADC_Channel_3)
    ADC_Channel3_Init();

    while(1) {
        u16 adc_val = Get_ADC_Average(ADC_Channel_3, 10);
        printf("OPA_OUT: %04d\r\n", adc_val);
        Delay_Ms(500);
    }
}
```

## Example: CH32M030 OPA PGA (Programmable Gain Amplifier)

```c
#include "debug.h"

// Pin connections:
// OPA1_CHP0 = PA10 (positive input)
// OPA1_OUT  = PA15 (output, 4x gain)

void OPA4_PGA_Init(void)
{
    OPA_ISP_InitTypeDef OPA_ISP_InitStruct = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_OPCM, ENABLE);

    // PA10 = positive input, PA15 = output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    OPA_ISP_InitStruct.OPA_ISP_GAIN = OPA_ISP_GAIN_4;       // 4x gain
    OPA_ISP_InitStruct.OPA_ISP_VFBIAS = OPA_ISP_VFBIAS_0_55V; // 0.55V bias
    OPA_ISP_InitStruct.OPA_ISP_NESL = OPA_ISP_NESL_VSS;     // Single-ended (VSS ref)
    OPA_ISP_InitStruct.OPA_ISP_SEL_IO = OPA_ISP_SEL_TO_ADC_ON;  // Connect to ADC
    OPA_ISP_InitStruct.OPA_ISP_QDET = OPA_ISP_QDET_EN_ON;
    OPA_ISP_InitStruct.OPA_ISP_QDET_PD30K = OPA_ISP_QDET_PD30K_OFF;
    OPA_ISP_InitStruct.OPA_ISP_QDET_VBSEL = OPA_ISP_QDET_VBSEL_OFF;

    OPA_ISP_Init(OPA4_ISP2, &OPA_ISP_InitStruct);
    OPA_ISP_Cmd(OPA4_ISP2, ENABLE);
}

// For differential mode, also configure PA11 as negative input:
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
// GPIO_Init(GPIOA, &GPIO_InitStructure);
// OPA_ISP_InitStruct.OPA_ISP_NESL = OPA_ISP_NESL_ISN;  // Differential input
```

## Example: CH32M030 Comparator with TIM1 Break

```c
#include "debug.h"

// Pin connections:
// CMP1_P1 = PB3 (positive input)
// CMP1_N0 = PA6 (negative input)
// CMP1_OUT = PB4 (comparator output)

void CMP_Init(void)
{
    CMP3_InitTypeDef CMP3_InitStruct = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOA, ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_OPCM, ENABLE);

    // PB3 = positive input (analog)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB4 = comparator output (AF push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PA6 = negative input (analog)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CMP3_InitStruct.CMP3_INT_EN = CMP3_INT_EN_OFF;
    CMP3_InitStruct.CMP3_PSEL = CMP3_PSEL_PB3;
    CMP3_InitStruct.CMP3_NSEL = CMP3_NSEL_PA6;
    CMP3_InitStruct.CMP3_HYS = CMP3_HYS_0mv;
    CMP3_InitStruct.CMP3_AT_IO = CMP3_AT_IO_ON;
    CMP3_InitStruct.CMP3_PT_IO = CMP3_PT_IO_ON;
    CMP3_InitStruct.CMP3_PT_IO_SEL = CMP3_PT_IO_PB4;

    // Optional: Enable TIM1 break input for PWM shutdown
    CMP3_InitStruct.CMP3_BK_EN = CMP3_BK_EN_TIM1BKIN_ON;
    CMP3_InitStruct.CMP3_COM_EN = CMP3_COM_MODE_ON;

    CMP3_Init(&CMP3_InitStruct);
    CMP3_Cmd(ENABLE);
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("CMP Test\r\n");

    CMP_Init();

    // When PB3 voltage > PA6 voltage, PB4 outputs high
    // When PB3 voltage < PA6 voltage, PB4 outputs low
    while(1);
}
```

## Pitfalls

- **CH32F20x vs CH32M030 API**: The OPA/CMP APIs are completely different between chip families. CH32F20x uses `OPA_InitTypeDef` with `OPA_NUM/PSEL/NSEL/Mode`. CH32M030 uses `OPA_ISP_InitTypeDef` for PGA mode and `CMP3_InitTypeDef` for comparators.
- **Clock enable**: CH32M030 uses `RCC_HBPeriphClockCmd(RCC_HBPeriph_OPCM, ENABLE)` for OPA/CMP clock. CH32F20x does not need a separate OPA clock.
- **GPIO mode**: OPA/CMP input pins must be configured as `GPIO_Mode_AIN` (analog input). Output pins use `GPIO_Mode_AF_PP`.
- **Voltage follower wiring**: For CH32F20x voltage follower mode, the output pin must be physically connected (wired) to the negative input pin.
- **PGA bias voltage**: CH32M030 PGA requires a bias voltage setting (`OPA_ISP_VFBIAS`). This affects the output DC offset.
- **ADC buffer**: When sampling OPA output with ADC, you may need to enable the ADC input buffer: `ADC1->CTLR1 |= (1 << 26);`

## Related Examples

- `CH32F20xEVT/EVT/EXAM/OPA/OPA_Voltage_Follower` - CH32F20x OPA voltage follower
- `CH32M030EVT/EVT/EXAM/OPA/OPA_PGA` - CH32M030 PGA with 4x gain
- `CH32M030EVT/EVT/EXAM/OPA/CMP` - CH32M030 comparator with TIM1 break
- `CH32M030EVT/EVT/EXAM/OPA/CMP_IRQ` - CH32M030 comparator with interrupt
