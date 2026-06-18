# Recipe: RTC Configuration

## When to Use
User wants to configure the real-time clock (RTC), maintain calendar time, set alarms, use backup registers, or implement timekeeping across power cycles.

## Availability
All CH32V families: CH32V103, CH32V20x, CH32V307, CH32V407

## API Reference (from ch32v*_rtc.h)

### Key Functions
```c
void       RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState);
void       RTC_EnterConfigMode(void);
void       RTC_ExitConfigMode(void);
uint32_t   RTC_GetCounter(void);
void       RTC_SetCounter(uint32_t CounterValue);
void       RTC_SetPrescaler(uint32_t PrescalerValue);
void       RTC_SetAlarm(uint32_t AlarmValue);
uint32_t   RTC_GetDivider(void);
void       RTC_WaitForLastTask(void);
void       RTC_WaitForSynchro(void);
FlagStatus RTC_GetFlagStatus(uint16_t RTC_FLAG);
void       RTC_ClearFlag(uint16_t RTC_FLAG);
ITStatus   RTC_GetITStatus(uint16_t RTC_IT);
void       RTC_ClearITPendingBit(uint16_t RTC_IT);
```

### Backup Register Functions
```c
void     PWR_BackupAccessCmd(FunctionalState NewState);
void     BKP_DeInit(void);
void     BKP_WriteBackupRegister(uint16_t BKP_DR, uint16_t Data);
uint16_t BKP_ReadBackupRegister(uint16_t BKP_DR);
```

### RTC Interrupt Flags
| Constant | Description |
|----------|-------------|
| `RTC_IT_SEC` | Second interrupt |
| `RTC_IT_ALR` | Alarm interrupt |
| `RTC_IT_OW` | Overflow interrupt |

### RTC Flags
| Constant | Description |
|----------|-------------|
| `RTC_FLAG_RSF` | Registers synchronized |
| `RTC_FLAG_RTOFF` | RTC operation finished |
| `RTC_FLAG_SEC` | Second flag |
| `RTC_FLAG_ALR` | Alarm flag |
| `RTC_FLAG_OW` | Overflow flag |

### RCC RTC Functions
```c
void RCC_LSEConfig(uint32_t RCC_LSE);         // RCC_LSE_ON, RCC_LSE_OFF, RCC_LSE_Bypass
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource); // RCC_RTCCLKSource_LSE, _LSI, _HSE_Div128
void RCC_RTCCLKCmd(FunctionalState NewState);
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG); // RCC_FLAG_LSERDY, RCC_FLAG_LSIRDY
```

## Example: RTC Calendar Initialization

```c
#include "ch32v30x.h"  // or ch32v10x.h, ch32v20x.h, ch32v4x7.h

// Calendar structure
typedef struct {
    vu8 hour, min, sec;
    vu16 w_year;
    vu8 w_month, w_date, week;
} calendar_obj;

calendar_obj calendar;

u8 RTC_Calendar_Init(void)
{
    u8 timeout = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    // Clear pending interrupts
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ClearITPendingBit(RTC_IT_SEC);

    // Check if RTC already configured (backup register marker)
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA1A1)
    {
        BKP_DeInit();

        // Enable LSE (32.768 kHz crystal)
        RCC_LSEConfig(RCC_LSE_ON);
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && timeout < 250)
        {
            timeout++;
            Delay_Ms(20);
        }
        if(timeout >= 250) return 1;  // LSE failed

        // Select LSE as RTC clock source
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();

        // Set prescaler: 32768 Hz / (32767+1) = 1 Hz
        RTC_EnterConfigMode();
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();

        // Set initial time: 2024-01-01 12:00:00
        RTC_SetCounter(RTC_DateToSeconds(2024, 1, 1, 12, 0, 0));
        RTC_WaitForLastTask();
        RTC_ExitConfigMode();

        BKP_WriteBackupRegister(BKP_DR1, 0xA1A1);
    }
    else
    {
        // RTC already running, just wait for sync
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
    }

    return 0;
}

// Convert date to seconds since 1970-01-01
u32 RTC_DateToSeconds(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0;
    const u8 mon_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    for(t = 1970; t < year; t++)
    {
        if((t%4==0 && t%100!=0) || t%400==0)
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    month -= 1;
    for(t = 0; t < month; t++)
    {
        seccount += (u32)mon_table[t] * 86400;
        if(((year%4==0 && year%100!=0) || year%400==0) && t==1)
            seccount += 86400;
    }
    seccount += (u32)(day-1) * 86400;
    seccount += (u32)hour * 3600;
    seccount += (u32)min * 60;
    seccount += sec;
    return seccount;
}

// RTC second interrupt handler
void RTC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTC_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_WaitForLastTask();
        // Update calendar from RTC counter
    }
}
```

## Example: RTC Alarm

```c
void RTC_SetAlarmTime(u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    u32 alarm_sec = RTC_DateToSeconds(year, month, day, hour, min, sec);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    RTC_SetAlarm(alarm_sec);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    RTC_WaitForLastTask();
}
```

## Pitfalls
- **Backup domain access** -- Must call `PWR_BackupAccessCmd(ENABLE)` before any RTC/BKP register write
- **LSE startup time** -- 32.768 kHz crystal takes ~1-2 seconds to stabilize; always timeout-check `RCC_FLAG_LSERDY`
- **WaitForLastTask** -- Must call `RTC_WaitForLastTask()` after every RTC write operation
- **Config mode** -- Must call `RTC_EnterConfigMode()` before setting prescaler/counter, and `RTC_ExitConfigMode()` after
- **Backup register marker** -- Use `BKP_DR1` as a flag to detect cold start vs warm reset
- **Counter is 32-bit** -- RTC counter wraps at ~136 years from epoch (1970); sufficient for most applications
- **Clock source** -- LSE (32.768 kHz crystal) is recommended for accurate timekeeping; LSI (internal ~40 kHz) is less accurate
