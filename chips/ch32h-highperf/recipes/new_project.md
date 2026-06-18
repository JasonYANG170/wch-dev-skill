# Creating a New CH32H417 Project

## Prerequisites

- MounRiver Studio IDE
- CH32H417EVT SDK (this repository)

## Steps

### 1. Copy an Existing Example

The easiest way is to copy the closest existing example:

```bash
# Copy a simple GPIO example as a starting point
cp -r CH32H417EVT/EVT/EXAM/GPIO/GPIO_Toggle my_project
```

### 2. Directory Structure

A minimal CH32H417 project has this structure:

```
my_project/
  APP/
    main.c                  # Main application
  Ld/
    Link_v5f.ld             # V5F linker script (copy from SRC/Ld/V5F/)
    Link_v3f.ld             # V3F linker script (optional, from SRC/Ld/V3F/)
  .project                  # MounRiver Studio project file
  my_project.wvproj         # MounRiver Studio workspace project
```

### 3. Main Source File

```c
#include "ch32h417.h"

// System clock initialization
void SystemInit(void)
{
    // CH32H417 defaults to HSI (8MHz) after reset
    // Configure PLL for 120MHz (V5F) or 60MHz (V3F) as needed
}

// GPIO initialization example
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA5 as push-pull output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemInit();
    GPIO_Config();

    while(1)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        Delay_Ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        Delay_Ms(500);
    }
}
```

### 4. Linker Script Selection

- **V5F core** (high-performance): Use `SRC/Ld/V5F/Link_v5f.ld`
  - Flash origin: `0x00010000` (bootloader occupies 0x0000-0xFFFF)
  - RAM_CODE (ITCM): `0x200A0000`, 128KB
  - RAM (DTCM): `0x200C0000`, 256KB
- **V3F core** (low-power): Use `SRC/Ld/V3F/Link_v3f.ld`

### 5. Build

1. Open the `.wvproj` file in MounRiver Studio
2. Project -> Build Project (Ctrl+B)
3. Flash via WCH-LinkE: Run -> Debug

### 6. Clock Configuration

The CH32H417 system clock can be configured for different frequencies:

```c
// Example: Configure system clock to 120MHz using HSE + PLL
void SystemCoreClockUpdate(void)
{
    // Enable HSE
    RCC_HSEConfig(RCC_HSE_ON);
    while(!RCC_GetFlagStatus(RCC_FLAG_HSERDY));

    // Configure PLL: HSE / 1 * 15 = 120MHz (assuming 8MHz HSE)
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_15);
    RCC_PLLCmd(ENABLE);
    while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));

    // Set flash latency
    FLASH_Access_Clock_Cfg(FLASH_CLK_HCLKDIV2);

    // Switch system clock to PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);

    // Update SystemCoreClock variable
    SystemCoreClockUpdate();
}
```

## Important Notes

- Always enable peripheral clocks before using peripherals
- The CH32H417 has GPIOA through GPIOF (up to 112 pins)
- Use `GPIO_PinAFConfig()` to set alternate functions for peripheral pins
- The bootloader occupies Flash 0x0000-0xFFFF; application starts at 0x10000
