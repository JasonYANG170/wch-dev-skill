# RTC Real-Time Clock

## Overview

CH32H417 has an RTC (Real-Time Clock) peripheral for keeping wall-clock time. It runs from LSI (internal ~40kHz) or LSE (external 32.768kHz) and continues operating in low-power modes. Supports seconds interrupt, alarm, and calibration.

## Key API Functions

```c
void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState);
void RTC_EnterConfigMode(void);
void RTC_ExitConfigMode(void);
uint32_t RTC_GetCounter(void);
void RTC_SetCounter(uint32_t CounterValue);
void RTC_SetPrescaler(uint32_t PrescalerValue);
void RTC_SetAlarm(uint32_t AlarmValue);
uint32_t RTC_GetDivider(void);
void RTC_WaitForLastTask(void);
void RTC_WaitForSynchro(void);
FlagStatus RTC_GetFlagStatus(uint32_t RTC_FLAG);
ITStatus RTC_GetITStatus(uint32_t RTC_IT);
void RTC_ClearITPendingBit(uint32_t RTC_IT);
```

## RTC Initialization with LSI

```c
#include "ch32h417.h"

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t week;
} calendar_t;

calendar_t calendar;

u8 RTC_Init(void)
{
    uint8_t temp = 0;

    // Enable backup domain access
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ClearITPendingBit(RTC_IT_SEC);

    // Enable LSI
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
        temp++;
        Delay_Ms(10);
        if (temp >= 250) return 1;  // Timeout
    }

    // Configure RTC clock source
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);

    RTC_WaitForLastTask();
    RTC_WaitForSynchro();

    // Enable seconds interrupt
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();

    // Set prescaler: LSI ~40kHz / 40000 = 1Hz
    RTC_EnterConfigMode();
    RTC_SetPrescaler(40000);
    RTC_WaitForLastTask();

    // Set initial time
    RTC_SetCounter(Convert_To_Counter(2025, 1, 1, 0, 0, 0));
    RTC_WaitForLastTask();

    RTC_ExitConfigMode();

    return 0;
}
```

## Time Conversion Functions

```c
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint8_t Is_Leap_Year(uint16_t year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
            return (year % 400 == 0) ? 1 : 0;
        return 1;
    }
    return 0;
}

uint32_t Convert_To_Counter(uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;
    uint16_t t;

    for (t = 1970; t < year; t++)
        seccount += Is_Leap_Year(t) ? 31622400 : 31536000;

    month -= 1;
    for (t = 0; t < month; t++)
    {
        seccount += (uint32_t)mon_table[t] * 86400;
        if (Is_Leap_Year(year) && t == 1)
            seccount += 86400;
    }

    seccount += (uint32_t)(day - 1) * 86400;
    seccount += (uint32_t)hour * 3600;
    seccount += (uint32_t)min * 60;
    seccount += sec;

    return seccount;
}
```

## RTC Interrupt Handler

```c
void RTC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        // Update calendar from counter
        uint32_t timecount = RTC_GetCounter();
        // Convert timecount to year/month/day/hour/min/sec...
    }

    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        // Alarm triggered
    }

    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);
    RTC_WaitForLastTask();
}
```

## Alarm Configuration

```c
void RTC_Alarm_Set(uint32_t alarm_time)
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetAlarm(alarm_time);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
}
```

## RTC Interrupt Flags

| Flag | Description |
|------|-------------|
| `RTC_IT_SEC` | Seconds interrupt |
| `RTC_IT_ALR` | Alarm interrupt |
| `RTC_IT_OW` | Overflow interrupt |

## Important Notes

- Must enable backup domain access: `PWR_BackupAccessCmd(ENABLE)`
- LSI frequency varies (~40kHz), prescaler needs calibration
- RTC counter is a 32-bit seconds counter (epoch time from 1970)
- After Stop mode wakeup, may need to re-enable LSI
- For better accuracy, use an external 32.768kHz crystal (LSE)
