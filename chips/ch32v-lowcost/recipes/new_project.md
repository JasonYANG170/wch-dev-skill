# Recipe: Create New Project

## Scenario
Create a new CH32V003/CH32V006/CH32L103 firmware project from scratch.

## Prerequisites
- MounRiver Studio installed
- WCH-LinkE debugger connected
- Target chip identified (CH32V003, CH32V006, or CH32L103)

## Step-by-Step

### 1. Select Base Example

Choose the closest example from the EVT package:

| Need | Example Path |
|------|-------------|
| Minimal blink | `CH32V003EVT/EVT/EXAM/GPIO/GPIO_Toggle/` |
| UART output | `CH32V003EVT/EVT/EXAM/USART/USART_Printf/` |
| ADC reading | `CH32V003EVT/EVT/EXAM/ADC/ADC_DMA/` |
| SPI comm | `CH32V003EVT/EVT/EXAM/SPI/2Lines_FullDuplex/` |
| Timer PWM | `CH32V003EVT/EVT/EXAM/TIM/PWM_Output/` |
| Flash storage | `CH32V003EVT/EVT/EXAM/FLASH/FLASH_Program/` |
| Power mgmt | `CH32V003EVT/EVT/EXAM/PWR/Sleep_Mode/` |

### 2. Copy Example Directory

```
cp -r CH32V003EVT/EVT/EXAM/GPIO/GPIO_Toggle/ MyProject/
```

This copies the complete structure:
```
MyProject/
├── User/
│   ├── main.c
│   ├── ch32v00x_it.c
│   └── system_ch32v00x.c
├── SRC/                    # Linked resources (shared)
├── Ld/Link.ld              # Linker script
├── .project
└── *.wvproj
```

### 3. Modify main.c

```c
#include "debug.h"

void MyPeripheral_Init(void)
{
    // Your initialization code here
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif

    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    MyPeripheral_Init();

    while(1)
    {
        // Your application logic
        Delay_Ms(1000);
    }
}
```

### 4. Configure Linker Script (if IAP)

For IAP applications, modify `Link.ld`:

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 12K  /* Offset by 4K for bootloader */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}
```

### 5. Build and Flash

1. Open `.wvproj` in MounRiver Studio
2. Project -> Build Project (Ctrl+B)
3. Run -> Debug (WCH-LinkE)

## Common Issues

| Issue | Solution |
|-------|----------|
| Build fails | Check SRC/ linked resources in .project file |
| No serial output | Verify USART_Printf_Init baud rate matches terminal |
| Flash write fails | Check FLASH_Unlock() before write, FLASH_Lock() after |
| Wrong chip selected | Verify project targets correct chip in MounRiver settings |
