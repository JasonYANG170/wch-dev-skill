# Recipe: DAC Output

## When to Use
User wants to generate analog voltage output, use DAC with DMA for waveform generation, timer-triggered conversion, noise generation, or triangle wave generation.

## Availability
- CH32V307: DAC Channel 1 (PA4) and Channel 2 (PA5)
- CH32V407: DAC Channel 1 (PA4) and Channel 2 (PA5)
- CH32V103 / CH32V20x: No DAC peripheral

## API Reference (from ch32v*_dac.h)

### Init Structure
```c
typedef struct {
    uint32_t DAC_Trigger;                      // Trigger source (DAC_Trigger_None for software)
    uint32_t DAC_WaveGeneration;               // DAC_WaveGeneration_None, _Noise, _Triangle
    uint32_t DAC_LFSRUnmask_TriangleAmplitude; // LFSR mask or triangle amplitude
    uint32_t DAC_OutputBuffer;                 // DAC_OutputBuffer_Enable/Disable
} DAC_InitTypeDef;
```

### Key Functions
```c
void     DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef *DAC_InitStruct);
void     DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);
void     DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);
void     DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
void     DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void     DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
void     DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1);
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);
void     DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState);
```

### DAC Channel Selection
| Constant | Description |
|----------|-------------|
| `DAC_Channel_1` | PA4 output |
| `DAC_Channel_2` | PA5 output |

### DAC Trigger Sources
| Constant | Description |
|----------|-------------|
| `DAC_Trigger_None` | Automatic conversion (no trigger) |
| `DAC_Trigger_T6_TRGO` | Timer 6 TRGO |
| `DAC_Trigger_T7_TRGO` | Timer 7 TRGO |
| `DAC_Trigger_T8_TRGO` | Timer 8 TRGO |
| `DAC_Trigger_T5_TRGO` | Timer 5 TRGO |
| `DAC_Trigger_T2_TRGO` | Timer 2 TRGO |
| `DAC_Trigger_T4_TRGO` | Timer 4 TRGO |
| `DAC_Trigger_Ext_IT9` | EXTI Line 9 |
| `DAC_Trigger_Software` | Software trigger |

### DAC Data Alignment
| Constant | Description |
|----------|-------------|
| `DAC_Align_12b_R` | 12-bit right-aligned |
| `DAC_Align_12b_L` | 12-bit left-aligned |
| `DAC_Align_8b_R` | 8-bit right-aligned |

### DAC Register Access (CH32V307/V407)
| Register | Description |
|----------|-------------|
| `DAC->R12BDHR1` | Channel 1 12-bit right-aligned data holding |
| `DAC->R12BDHR2` | Channel 2 12-bit right-aligned data holding |
| `DAC->DOR1` | Channel 1 data output |
| `DAC->DOR2` | Channel 2 data output |

## Example: DAC Simple Output

```c
#include "ch32v4x7.h"  // or ch32v30x.h

void DAC_Simple_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_DAC, ENABLE);

    // PA4 as analog input (DAC Channel 1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

// Set output voltage (0-3.3V for 12-bit, 3.3V reference)
void DAC_SetVoltage(uint16_t mv)
{
    uint16_t dac_val = (uint16_t)((uint32_t)mv * 4095 / 3300);
    DAC_SetChannel1Data(DAC_Align_12b_R, dac_val);
}
```

## Example: DAC with DMA (Timer-Triggered Waveform)

```c
// DAC DMA channel mapping: CH32V407 uses DMA2_Channel3 for DAC1
u16 dac_buffer[8] = {0x123, 0x234, 0x345, 0x456, 0x567, 0x678, 0x789, 0x89A};

void DAC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA2, ENABLE);

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->R12BDHR1);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)dac_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);
}

void DAC_Trig_Init(void)
{
    DAC_InitTypeDef DAC_InitType;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_DAC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitType.DAC_Trigger = DAC_Trigger_T8_TRGO;  // Timer 8 trigger
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void TIM8_TRGO_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM8, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
    TIM_Cmd(TIM8, ENABLE);
}
```

## Pitfalls
- **GPIO must be analog mode** -- PA4/PA5 must be configured as `GPIO_Mode_AIN`, not floating or output
- **Output buffer** -- `DAC_OutputBuffer_Disable` gives full 0-VDDA range; Enable adds impedance but improves drive
- **DMA channel mapping** -- DAC1 uses DMA2_Channel3, DAC2 uses DMA2_Channel4 (CH32V307/V407)
- **12-bit range** -- DAC output = Data * VDDA / 4095; VDDA is typically 3.3V
- **Timer trigger** -- Must call `DAC_DMACmd(ENABLE)` separately from `DAC_Cmd(ENABLE)` when using DMA
