# CH32F Common Pitfalls and Solutions

## 1. Forgetting RCC Clock Enable

**Problem**: Peripheral does not respond, registers read as zero.

**Solution**: Always enable the peripheral clock before use.

```c
// WRONG - GPIOA clock not enabled
GPIO_Init(GPIOA, &GPIO_InitStructure);

// CORRECT - Enable clock first
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
GPIO_Init(GPIOA, &GPIO_InitStructure);
```

**Clock Bus Reference**:
- APB2: GPIOA-GPIOG, USART1, SPI1, ADC1, TIM1, TIM8, AFIO
- APB1: USART2/3, SPI2/3, I2C1/2, TIM2-TIM7, CAN1, DAC, PWR
- AHB: DMA1, DMA2, ETH, USB

---

## 2. Wrong Header Include

**Problem**: Compilation errors about undefined types or functions.

**Solution**: Use the correct header for your chip family.

```c
// For CH32F10x (CH32F103)
#include "ch32f10x.h"

// For CH32F20x (CH32F203/207/208)
#include "ch32f20x.h"

// Do NOT mix headers from different chip families
```

---

## 3. NVIC Priority Group Not Configured

**Problem**: Interrupts do not work or have unexpected priority behavior.

**Solution**: Call `NVIC_PriorityGroupConfig()` before configuring any interrupts.

```c
// WRONG - NVIC priority group not set
NVIC_Init(&NVIC_InitStructure);

// CORRECT - Set priority group first
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
NVIC_Init(&NVIC_InitStructure);
```

**Priority Group Options**:
| Group | Preemption Priority | Sub Priority |
|-------|-------------------|--------------|
| NVIC_PriorityGroup_0 | 0 bits (always 0) | 4 bits (0-15) |
| NVIC_PriorityGroup_1 | 1 bit (0-1) | 3 bits (0-7) |
| NVIC_PriorityGroup_2 | 2 bits (0-3) | 2 bits (0-3) |
| NVIC_PriorityGroup_3 | 3 bits (0-7) | 1 bit (0-1) |
| NVIC_PriorityGroup_4 | 4 bits (0-15) | 0 bits (always 0) |

---

## 4. Flash Erase Before Write

**Problem**: Flash write fails or data is corrupted.

**Solution**: Always erase Flash before writing. Flash can only change bits from 1 to 0 during write.

```c
// WRONG - Writing without erasing
FLASH_Unlock();
FLASH_ProgramWord(addr, data);  // May fail if bits need to go 0->1

// CORRECT - Erase then write
FLASH_Unlock();
FLASH_ErasePage(pageAddr);
FLASH_ProgramWord(addr, data);
FLASH_Lock();
```

**Flash Page Sizes**:
- CH32F10x: 1KB (1024 bytes)
- CH32F20x: 256 bytes (fast mode) or 1KB (standard mode)

---

## 5. GPIO Mode Mismatch for Alternate Functions

**Problem**: Peripheral does not work on expected pins.

**Solution**: Use the correct GPIO mode for alternate functions.

```c
// WRONG - Using push-pull for I2C (needs open-drain)
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

// CORRECT - I2C requires open-drain
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
```

**GPIO Mode Guide**:
| Peripheral | GPIO Mode |
|------------|-----------|
| USART TX | GPIO_Mode_AF_PP |
| USART RX | GPIO_Mode_IN_FLOATING |
| SPI SCK/MOSI | GPIO_Mode_AF_PP |
| SPI MISO | GPIO_Mode_IN_FLOATING |
| SPI NSS (HW) | GPIO_Mode_AF_PP |
| I2C SCL/SDA | GPIO_Mode_AF_OD |
| CAN TX | GPIO_Mode_AF_PP |
| CAN RX | GPIO_Mode_IPU or GPIO_Mode_IN_FLOATING |
| TIM PWM | GPIO_Mode_AF_PP |
| ADC | GPIO_Mode_AIN |
| DAC | GPIO_Mode_AIN |

---

## 6. USB Register API (Not STM32 OTG)

**Problem**: Trying to use STM32 USB OTG library with CH32F10x/CH32F20x.

**Solution**: CH32F10x/CH32F20x use a custom register-based USB API, not the STM32 USB OTG library.

```c
// WRONG - Using STM32 USB OTG functions
USB_OTG_Init(&USB_OTG_dev);

// CORRECT - Using CH32F USB functions
USB_DeviceInit();
```

Refer to `EVT/EXAM/USB/` examples for the correct USB implementation.

---

## 7. Interrupt Handler Name Must Match Vector Table

**Problem**: Interrupt handler is never called.

**Solution**: Use the exact handler name from the vector table.

```c
// WRONG - Typo in handler name
void USART1_IRQHandlr(void) { ... }  // Missing 'e'

// CORRECT - Exact name match
void USART1_IRQHandler(void) { ... }
```

**Common Handler Names**:
- `NMI_Handler`
- `HardFault_Handler`
- `MemManage_Handler`
- `BusFault_Handler`
- `UsageFault_Handler`
- `SVC_Handler`
- `DebugMon_Handler`
- `PendSV_Handler`
- `SysTick_Handler`
- `USART1_IRQHandler`
- `USART2_IRQHandler`
- `SPI1_IRQHandler`
- `I2C1_EV_IRQHandler`
- `I2C1_ER_IRQHandler`
- `TIM2_IRQHandler`
- `EXTI0_IRQHandler` through `EXTI15_10_IRQHandler`
- `DMA1_Channel1_IRQHandler` through `DMA1_Channel7_IRQHandler`
- `ADC1_2_IRQHandler`
- `USB_IRQHandler`

---

## 8. Blocking Delays in Interrupt Handlers

**Problem**: System hangs or watchdog resets.

**Solution**: Keep interrupt handlers short. Use flags to signal main loop.

```c
// WRONG - Delay in interrupt
void TIM2_IRQHandler(void)
{
    Delay_Ms(100);  // Blocks for 100ms!
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

// CORRECT - Set flag, handle in main loop
volatile uint8_t timer_flag = 0;

void TIM2_IRQHandler(void)
{
    timer_flag = 1;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

int main(void)
{
    while(1)
    {
        if(timer_flag)
        {
            timer_flag = 0;
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
        }
    }
}
```

---

## 9. Stack Overflow with Large Buffers

**Problem**: Hard fault or undefined behavior.

**Solution**: Use static or global allocation for large buffers.

```c
// WRONG - Large buffer on stack
void ProcessData(void)
{
    uint8_t buffer[1024];  // May overflow stack
    // ...
}

// CORRECT - Use static or global
static uint8_t buffer[1024];

void ProcessData(void)
{
    // Use static buffer
    // ...
}
```

---

## 10. Watchdog Timer Not Refreshed

**Problem**: System resets unexpectedly.

**Solution**: Refresh the watchdog timer periodically.

```c
// If IWDG is enabled, refresh it in main loop
while(1)
{
    IWDG_ReloadCounter();  // Refresh watchdog
    // ... application logic ...
}
```

---

## 11. CH32F20x BLE Initialization Order

**Problem**: BLE stack fails to initialize.

**Solution**: Follow the strict initialization sequence.

```c
// CORRECT BLE initialization order
int main(void)
{
    SystemCoreClockUpdate();
    // 1. Initialize BLE HAL
    HAL_Init();
    // 2. Initialize BLE library
    WCHBLE_Init();
    // 3. Initialize GAP
    GAP_Init();
    // 4. Initialize GATT
    GATT_Init();
    // 5. Initialize profiles
    Profile_Init();
    // 6. Start BLE
    GAP_Start();
    // 7. Main loop
    while(1) { BLE_Process(); }
}
```

---

## 12. GPIO Remap Required for Some Peripherals

**Problem**: Peripheral does not work on alternate pins.

**Solution**: Enable GPIO remap when using alternate pin assignments.

```c
// Example: Remap USART1 to PB6/PB7
GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

// Example: Remap CAN1 to PB8/PB9
GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

// Note: Some remaps require AFIO clock
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
```
