# Recipe: OPA (Operational Amplifier) Configuration

## Scenario
Configure the on-chip operational amplifier (OPA) for voltage follower, PGA (programmable-gain amplifier), or comparator mode, and read the output via ADC.

## Pin Mapping (CH32V003/CH32V006)

| OPA Signal | Pin | Notes |
|-----------|-----|-------|
| OPA1_CHP1 (non-inverting input) | PD7 | Also NRST pin -- must disable NRST function first |
| OPA1_CHN1 (inverting input) | PD0 | |
| OPA1_OUT (output) | PD4 | Can be sampled by ADC_Channel_7 |

> **Important:** PD7 shares the NRST (reset) function. To use it as OPA input, you must disable the NRST pin via option bytes. This is a one-time operation that requires a power cycle.

## API Quick Reference

### OPA Init Structure
```c
typedef struct
{
    uint8_t PSEL;  // Positive (non-inverting) input: CHP1, CHP2, ...
    uint8_t NSEL;  // Negative (inverting) input: CHN1, CHN2, ...
} OPA_InitTypeDef;
```

### Key Functions
```c
void OPA_Init(OPA_InitTypeDef *OPA_InitStruct);
void OPA_Cmd(FunctionalState NewState);
```

### Input Selection Constants
```c
// Positive input (PSEL)
CHP1   // OPA non-inverting input 1 (PD7 on CH32V003)
CHP2   // OPA non-inverting input 2

// Negative input (NSEL)
CHN1   // OPA inverting input 1 (PD0 on CH32V003)
CHN2   // OPA inverting input 2
```

## Complete Call Chains

### Disable NRST Pin (Required for PD7)
```c
// WARNING: This disables the hardware reset pin on PD7.
// After this, reset is only available via power cycle or SWD.
// Only call this once -- option bytes are non-volatile.

void Option_Byte_CFG(void)
{
    FLASH_Unlock();
    FLASH_EraseOptionBytes();
    FLASH_UserOptionByteConfig(
        OB_IWDG_SW,              // Software watchdog
        OB_STDBY_NoRST,          // No reset in standby
        OB_RST_NoEN,             // Disable NRST on PD7
        OB_PowerON_Start_Mode_BOOT
    );
    FLASH_Lock();
    // Power cycle the chip for option byte to take effect
}
```

### OPA Voltage Follower
```c
// Voltage follower: OPA_OUT = OPA_CHP (unity gain)
// Connect PD0 to PD4 externally (NSEL -> OUT feedback)
// Input signal on PD7 (CHP1)
// Read amplified output via ADC on PD4 (ADC_Channel_7)

void OPA1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    OPA_InitTypeDef OPA_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // OPA positive input (PD7) -- floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // OPA negative input (PD0) -- floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    OPA_InitStructure.PSEL = CHP1;   // Non-inverting = PD7
    OPA_InitStructure.NSEL = CHN1;   // Inverting = PD0
    OPA_Init(&OPA_InitStructure);
    OPA_Cmd(ENABLE);
}
```

### ADC Reading OPA Output
```c
// OPA output on PD4 = ADC_Channel_7
void ADC_OPA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // OPA output pin as analog input for ADC
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

u16 Get_OPA_Output(void)
{
    u16 val;
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);
    return val;
}
```

### Complete OPA Example (main)
```c
#include "debug.h"

int main(void)
{
    u16 adc_val;

    // Step 1: Disable NRST (one-time, needs power cycle)
    Option_Byte_CFG();

    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif

    printf("OPA Test\r\n");

    // Step 2: Init OPA
    OPA1_Init();

    // Step 3: Init ADC to read OPA output
    ADC_OPA_Init();

    while(1)
    {
        adc_val = Get_OPA_Output();
        printf("OPA_OUT: %04d\r\n", adc_val);
        Delay_Ms(500);
    }
}
```

### OPA Modes Reference

| Mode | PSEL Connection | NSEL Connection | Gain |
|------|----------------|-----------------|------|
| Voltage Follower | Signal input | Connect to OUT (PD4) | 1x |
| Non-inverting Amp | Signal input | External resistor divider | >1x |
| Comparator | Signal input | Reference voltage | N/A (digital output) |

## Common Errors

1. **PD7/NRST not disabled** -- Must call `Option_Byte_Config()` and power cycle before using PD7 as OPA input
2. **GPIO not floating** -- OPA input pins must be `GPIO_Mode_IN_FLOATING`, not push-pull or pull-up
3. **OPA not enabled** -- Must call `OPA_Cmd(ENABLE)` after `OPA_Init()`
4. **ADC pin conflict** -- PD4 is both OPA output and ADC_Channel_7; do not configure PD4 as GPIO when reading OPA
5. **Feedback path missing** -- For voltage follower, PD0 (CHN1) must be connected to PD4 (OPA_OUT) externally
