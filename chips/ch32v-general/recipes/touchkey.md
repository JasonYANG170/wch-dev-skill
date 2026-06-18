# Recipe: TouchKey (Capacitive Touch Sensing)

## When to Use
User wants to implement capacitive touch buttons/sensors using the built-in TouchKey peripheral, detect finger proximity, or replace mechanical buttons with touch sensing.

## Availability
- CH32V103: TouchKey on ADC channels (PA0-PA15)
- CH32V307: TouchKey on ADC channels
- CH32V407: TouchKey on ADC channels
- CH32V20x: No TouchKey peripheral

## API Reference

TouchKey uses the ADC peripheral with a special TouchKey mode enabled via register manipulation. There is no dedicated TouchKey driver library; it is controlled through ADC registers directly.

### Register Access Macros
```c
#define TKEY_CR    ADC1->CTLR1   // TouchKey control register
#define TKEY_CH    ADC1->RSQR3   // TouchKey channel selection
#define TKEY_SR    ADC1->RDATAR  // TouchKey status/data register
```

### Key Operations
```c
// Enable TouchKey mode
TKEY_CR |= 0x51000000;

// Select channel and start conversion
TKEY_CH = channel;  // ADC_Channel_0 to ADC_Channel_15

// Wait for conversion complete
while(!(TKEY_CR & 0x08000000));

// Read result
u16 value = (u16)TKEY_SR;

// Clear flag
TKEY_CR |= 0x08000000;
```

### TouchKey Register Bits
| Bit | Description |
|-----|-------------|
| CTLR1[28] | TouchKey enable (0x10000000) |
| CTLR1[25] | TouchKey start (0x02000000) |
| CTLR1[24] | TouchKey mode select (0x01000000) |
| CTLR1[27] | Conversion complete flag (0x08000000) |

### TouchKey Channels
Any ADC channel (0-15) can be used as a TouchKey input. Each channel corresponds to a GPIO pin:

| Channel | Pin |
|---------|-----|
| ADC_Channel_0 | PA0 |
| ADC_Channel_1 | PA1 |
| ADC_Channel_2 | PA2 |
| ADC_Channel_3 | PA3 |
| ... | ... |
| ADC_Channel_15 | PA15 (or remapped) |

## Example: TouchKey Single Channel

```c
#include "ch32v10x.h"  // or ch32v30x.h, ch32v4x7.h

#define TKEY_CR    ADC1->CTLR1
#define TKEY_CH    ADC1->RSQR3
#define TKEY_SR    ADC1->RDATAR

void TouchKey_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // Configure touch pin as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // PA2 = Channel 2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Enable ADC and TouchKey mode
    ADC_Cmd(ADC1, ENABLE);
    TKEY_CR |= 0x51000000;  // Enable TouchKey
}

u16 TouchKey_Read(u8 channel)
{
    u16 val;

    TKEY_CH = channel;

    // Wait for conversion complete
    while(!(TKEY_CR & 0x08000000));

    val = (u16)TKEY_SR;

    // Clear flag
    TKEY_CR |= 0x08000000;

    return val;
}

int main(void)
{
    u16 touch_val;
    u16 baseline;
    u8 touch_detected = 0;

    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    TouchKey_Init();

    // Calibrate baseline (average of several readings without touch)
    u32 sum = 0;
    for(int i = 0; i < 16; i++)
    {
        sum += TouchKey_Read(ADC_Channel_2);
        Delay_Ms(10);
    }
    baseline = sum / 16;
    printf("Baseline: %d\r\n", baseline);

    while(1)
    {
        touch_val = TouchKey_Read(ADC_Channel_2);

        // Detect touch: value drops when finger approaches
        // Threshold depends on electrode size and PCB design
        if(touch_val < (baseline - 100) && !touch_detected)
        {
            touch_detected = 1;
            printf("Touch detected! Value: %d\r\n", touch_val);
        }
        else if(touch_val >= (baseline - 50))
        {
            touch_detected = 0;
        }

        if(touch_val & 0x8000)
        {
            printf("Invalid reading (discarded)\r\n");
        }

        Delay_Ms(20);
    }
}
```

## Example: Multi-Channel TouchKey

```c
#define NUM_TOUCH_KEYS  4
const u8 touch_channels[NUM_TOUCH_KEYS] = {
    ADC_Channel_0,  // PA0
    ADC_Channel_1,  // PA1
    ADC_Channel_2,  // PA2
    ADC_Channel_3   // PA3
};

u16 baseline[NUM_TOUCH_KEYS];
u8 touch_state[NUM_TOUCH_KEYS];

void TouchKey_Calibrate(void)
{
    for(int k = 0; k < NUM_TOUCH_KEYS; k++)
    {
        u32 sum = 0;
        for(int i = 0; i < 16; i++)
        {
            sum += TouchKey_Read(touch_channels[k]);
            Delay_Ms(5);
        }
        baseline[k] = sum / 16;
    }
}

void TouchKey_Scan(void)
{
    for(int k = 0; k < NUM_TOUCH_KEYS; k++)
    {
        u16 val = TouchKey_Read(touch_channels[k]);
        if(val < (baseline[k] - 100))
        {
            if(!touch_state[k])
            {
                touch_state[k] = 1;
                printf("Key %d pressed\r\n", k);
            }
        }
        else
        {
            touch_state[k] = 0;
        }
    }
}
```

## Pitfalls
- **No dedicated driver** -- TouchKey is controlled via raw ADC register manipulation, not a standard peripheral library
- **Baseline calibration** -- Must calibrate baseline readings at startup with no finger present; values vary per PCB
- **Threshold tuning** -- Touch detection threshold depends on electrode geometry, overlay thickness, and parasitic capacitance
- **Invalid readings** -- Bit 15 of the result indicates an invalid reading; always check `(val & 0x8000)`
- **ADC conflicts** -- TouchKey uses ADC1; cannot use regular ADC conversions simultaneously
- **Channel sharing** -- TouchKey channels share ADC pins; cannot use same pin for both analog input and touch
- **ESD protection** -- Touch electrodes need proper ESD protection (TVS diodes or series resistors)
- **Debouncing** -- Implement software debouncing (require multiple consecutive touch detections) to avoid false triggers
