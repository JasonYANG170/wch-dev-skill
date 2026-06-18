# CH57x BLE HAL Layer API Reference

The HAL (Hardware Abstraction Layer) in `resources/EXAM/BLE/HAL/` provides board-level abstractions used by all BLE examples.

## HAL_Init

```c
// Header: HAL.h
#include "HAL.h"

// Initialize HAL (must be called after CH57X_BLEInit)
void HAL_Init(void);

// Initializes: RTC, keys, LEDs, power management
```

## KEY

```c
// Header: KEY.h

// Key definitions (active low, GPIO-based)
#define KEY_1    GPIO_Pin_4   // GPIOA_Pin_4
#define KEY_2    GPIO_Pin_5   // GPIOA_Pin_5

// Key initialization (called by HAL_Init)
void KEY_Init(void);

// Key value reading
uint8_t KEY_ReadPort(void);    // Read raw key GPIO state
uint8_t KEY_Value(void);       // Debounced key value

// Key interrupt
void KEY_ITModeCfg(GPIOITModeTpDef mode);  // Configure key interrupt mode
uint8_t KEY_ReadITFlagBit(uint32_t pin);   // Read interrupt flag
void KEY_ClearITFlagBit(uint32_t pin);     // Clear interrupt flag
```

## LED

```c
// Header: LED.h

// LED definitions (active low)
#define LED_1    GPIO_Pin_18  // GPIOA_Pin_18
#define LED_2    GPIO_Pin_19  // GPIOA_Pin_19

// LED control
void LED_Init(void);                     // Initialize LED GPIOs
#define LED_On(pin)    GPIOA_ResetBits(pin)  // Turn on (active low)
#define LED_Off(pin)   GPIOA_SetBits(pin)    // Turn off
#define LED_Toggle(pin) GPIOA_InverseBits(pin) // Toggle
```

## RTC

```c
// Header: RTC.h

// RTC initialization (called by HAL_Init)
void RTC_Init(void);

// RTC time operations
uint32_t RTC_GetCount(void);           // Get current RTC counter value
void RTC_SetCount(uint32_t count);     // Set RTC counter

// RTC interrupt
void RTC_SetCyc0Int(uint32_t cyc);     // Set cyclic interrupt period
void RTC_ClearCyc0Int(void);           // Clear cyclic interrupt flag

// RTC timing macros
#define US_TO_RTC(us)     ((uint32_t)((us) * 32768 / 1000000))
#define MS_TO_RTC(ms)     ((uint32_t)((ms) * 32768 / 1000))
#define RTC_TO_US(rtc)    ((uint32_t)((rtc) * 1000000 / 32768))
#define RTC_TO_MS(rtc)    ((uint32_t)((rtc) * 1000 / 32768))
#define RTC_TIMER_MAX_VALUE  0xA8C00000
```

## SLEEP

```c
// Header: SLEEP.h

// Low power sleep management
void SLEEP_Init(void);                  // Initialize sleep support

// Sleep mode entry (called automatically by BLE stack when HAL_SLEEP=TRUE)
void CH57X_LowPower(uint32_t time);    // BLE stack sleep callback

// Wakeup configuration
void SLEEP_WakeupCfg(uint8_t src);     // Configure wakeup sources

// Sleep status
uint8_t SLEEP_GetSleepFlag(void);       // Check if system was in sleep
void SLEEP_ClearSleepFlag(void);        // Clear sleep flag
```

## MCU

```c
// Header: MCU.h (in HAL/include/)

// System utilities
uint32_t HAL_GetInterTempValue(void);   // Get internal temperature ADC value (for BLE calibration)

// Delay
void mDelayuS(uint16_t n);             // Microsecond delay
void mDelaymS(uint16_t n);             // Millisecond delay
```

## config.h

```c
// Header: config.h (in HAL/include/)

// All BLE stack configuration macros
// See resources/config_reference.md for complete reference

// Key macros:
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*6)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      3
```

## Typical HAL Usage Flow

```c
#include "CONFIG.h"
#include "HAL.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (DCDC_ENABLE)
    PWR_DCDCCfg(ENABLE);
#endif

    CH57X_BLEInit();    // Initialize BLE stack
    HAL_Init();          // Initialize HAL (RTC, KEY, LED, SLEEP)

    // Application init...
    // GAPRole init...

    Main_Circulation();  // Runs TMOS event loop
}
```

## Modifying HAL for Custom Boards

The HAL files are shared across all BLE examples. To customize for your board:

1. **KEY**: Change `KEY_1`/`KEY_2` pin definitions in `KEY.h`, update `KEY_Init()` in `KEY.c`
2. **LED**: Change `LED_1`/`LED_2` pin definitions in `LED.h`, update `LED_Init()` in `LED.c`
3. **SLEEP**: Modify `SLEEP_Init()` for custom wakeup pin configuration
4. **config.h**: Copy to your project and modify for your requirements

> Do not modify the HAL files directly in `resources/EXAM/BLE/HAL/` — copy them to your project directory first.
