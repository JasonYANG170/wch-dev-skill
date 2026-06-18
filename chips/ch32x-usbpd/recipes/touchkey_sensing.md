# TouchKey Sensing

> **Summary**: Configure capacitive touch key detection on CH32X035/CH643 chips using the built-in TouchKey peripheral.

## Trigger Intent

- "Touch key"
- "Capacitive touch"
- "TouchKey detection"
- "Touch sensor"
- "Capacitive sensing"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE)` |
| Pins | Touch-capable GPIO pins (varies by chip package) |
| Reference | `CH32X035EVT/EVT/EXAM/TOUCHKEY/` |

## Step-by-Step

### TouchKey Initialization

```c
#include "ch32x035.h"
#include "debug.h"

// TouchKey channel to pin mapping (CH32X035)
// Channel 0: PA0    Channel 4: PA4
// Channel 1: PA1    Channel 5: PA5
// Channel 2: PA2    Channel 6: PA6
// Channel 3: PA3    Channel 7: PA7

#define TOUCH_NUM       4       // Number of touch keys
#define SAMPLE_TIMES    5       // Samples per measurement
#define THRESHOLD_DOWN  2000    // Press detection threshold
#define THRESHOLD_UP    500     // Release detection threshold

uint16_t Key_FreeBuf[TOUCH_NUM];   // Baseline (no touch) values
uint16_t Key_DataBuf[TOUCH_NUM];   // Current measurement values
uint8_t  Touch_State = 0;          // Bitmask of pressed keys
```

### TouchKey Channel Select

```c
void TouchKey_SelectChannel(uint8_t ch) {
    // TKEY_CTRL bits [2:0] select channel
    TKEY_CTRL = (TKEY_CTRL & 0xF8) | (ch + 1);
    // ch+1 because 0=no channel, 1=TIN0, 2=TIN1, ...
}
```

### TouchKey Baseline Calibration

```c
void TouchKey_Calibrate(void) {
    uint8_t i, j;
    uint16_t sum;
    uint16_t overtime;

    for (i = 0; i < TOUCH_NUM; i++) {
        sum = 0;
        for (j = 0; j < SAMPLE_TIMES; j++) {
            TouchKey_SelectChannel(i);
            overtime = 0;
            // Wait for measurement complete (bTKC_IF flag)
            while (!(TKEY_CTRL & bTKC_IF)) {
                if (++overtime == 0) return;  // Timeout
            }
            sum += TKEY_DAT;
        }
        Key_FreeBuf[i] = sum / SAMPLE_TIMES;
    }
}
```

### TouchKey Measurement (Polling)

```c
uint8_t TouchKey_Measure(void) {
    uint8_t i, j;
    uint16_t sum, diff;

    for (i = 0; i < TOUCH_NUM; i++) {
        sum = 0;
        for (j = 0; j < SAMPLE_TIMES; j++) {
            TouchKey_SelectChannel(i);
            while (!(TKEY_CTRL & bTKC_IF));
            sum += TKEY_DAT;
        }
        Key_DataBuf[i] = sum / SAMPLE_TIMES;

        // Calculate difference from baseline
        if (Key_FreeBuf[i] > Key_DataBuf[i])
            diff = Key_FreeBuf[i] - Key_DataBuf[i];
        else
            diff = Key_DataBuf[i] - Key_FreeBuf[i];

        // Detect press/release
        if (diff > THRESHOLD_DOWN) {
            Touch_State |= (1 << i);    // Key pressed
        } else if (diff < THRESHOLD_UP) {
            Touch_State &= ~(1 << i);   // Key released
        }
    }
    return Touch_State;
}
```

### TouchKey Interrupt Mode

```c
volatile uint8_t Touch_INT_Flag = 0;
volatile uint8_t Touch_INT_Channel = 0;

void TouchKey_InitInterrupt(void) {
    // Enable TouchKey interrupt
    IE_TKEY = 1;
    EA = 1;
}

// TouchKey interrupt service routine
void TouchKey_ISR(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TouchKey_ISR(void) {
    uint16_t key_val = TKEY_DAT;

    // Compare with baseline
    if (key_val < (Key_FreeBuf[Touch_INT_Channel] - THRESHOLD_DOWN)) {
        Touch_State |= (1 << Touch_INT_Channel);
    }

    // Move to next channel
    if (++Touch_INT_Channel >= TOUCH_NUM) {
        Touch_INT_Channel = 0;
    }
    TouchKey_SelectChannel(Touch_INT_Channel);
    Touch_INT_Flag = 1;
}
```

### GPIO Configuration for TouchKey Pins

```c
void TouchKey_PinInit(uint8_t pin_mask) {
    // Set touch pins to float input (required for capacitive sensing)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = pin_mask;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```

### Full Example

```c
#include "ch32x035.h"
#include "debug.h"

#define TOUCH_NUM       4
#define SAMPLE_TIMES    5
#define THRESHOLD_DOWN  2000

uint16_t Key_FreeBuf[TOUCH_NUM];
uint16_t Key_DataBuf[TOUCH_NUM];
uint8_t  Touch_State = 0;

int main(void) {
    uint8_t i;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    // Configure touch pins as floating input
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Calibrate baseline
    TouchKey_Calibrate();
    printf("Baseline calibrated\r\n");

    while(1) {
        TouchKey_Measure();
        for (i = 0; i < TOUCH_NUM; i++) {
            if (Touch_State & (1 << i)) {
                printf("Key %d pressed\r\n", i);
            }
        }
        Delay_Ms(50);
    }
}
```

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| No touch detection | Pin configured as output | Set pin to `GPIO_Mode_IN_FLOATING` |
| Unstable readings | Pin has pullup/pulldown | Remove all pull resistors, use floating mode |
| Always triggered | Threshold too low | Increase `THRESHOLD_DOWN` or recalibrate baseline |
| Never triggered | Threshold too high | Decrease `THRESHOLD_DOWN` |
| Timeout in calibration | TouchKey clock not enabled | Enable ADC clock: `RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE)` |
