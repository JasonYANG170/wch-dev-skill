# Common Pitfalls and Fixes

## 1. Peripheral Clock Not Enabled

**Symptom:** Peripheral init appears to succeed but peripheral doesn't work.

**Wrong:**
```c
GPIO_Init(GPIOD, &GPIO_InitStructure);  // Does nothing without clock
```

**Correct:**
```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
GPIO_Init(GPIOD, &GPIO_InitStructure);
```

**Rule:** Always enable the peripheral clock AND the GPIO port clock before initialization.

---

## 2. Wrong Header Include

**Symptom:** Compilation errors -- undefined types, missing functions.

**Wrong:**
```c
#include "CH57x_common.h"  // This is for CH57x BLE chips
```

**Correct:**
```c
#include "debug.h"  // For CH32V003: includes ch32v00x.h
                    // For CH32V006: includes ch32v00X.h
                    // For CH32L103: includes ch32l103.h
```

---

## 3. Flash Page Size Confusion

**Symptom:** Flash erase fails, data corruption, or only partial erase.

**Wrong:**
```c
// Assuming 1KB pages on CH32V003
FLASH_ErasePage(0x08003000);  // CH32V003 has 64-byte pages!
```

**Correct:**
```c
// CH32V003: 64 bytes per page
// CH32V006: 1024 bytes per page
// CH32L103: 1024 bytes per page
FLASH_ErasePage(0x08003000);  // Works, but erases only 64 bytes on CH32V003
```

---

## 4. GPIO Mode for Alternate Functions

**Symptom:** Peripheral output doesn't work, signal stuck low or high.

**Wrong:**
```c
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // Won't drive SPI/UART signal
```

**Correct:**
```c
// For SPI SCK, MOSI, UART TX, I2C SCL/SDA:
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // Alternate function push-pull

// For SPI MISO, UART RX:
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // Floating input

// For I2C (open-drain bus):
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;   // Alternate function open-drain
```

---

## 5. ADC Pin Not Set to Analog Input

**Symptom:** ADC reads 0 or incorrect values.

**Wrong:**
```c
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Pull-up, not analog
```

**Correct:**
```c
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // Analog input mode
```

---

## 6. Missing Interrupt Attribute

**Symptom:** System crashes or hangs when interrupt fires.

**Wrong:**
```c
void EXTI7_0_IRQHandler(void) { ... }
```

**Correct:**
```c
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)
{
    // ...
}
```

---

## 7. SystemCoreClockUpdate Not Called

**Symptom:** Delay timing is wrong, UART baud rate is off.

**Wrong:**
```c
int main(void) {
    USART_Printf_Init(115200);  // May use wrong clock value
}
```

**Correct:**
```c
int main(void) {
    SystemCoreClockUpdate();  // Update SystemCoreClock variable
    Delay_Init();             // Initialize delay functions
    USART_Printf_Init(115200);
}
```

---

## 8. SPI Slave CPOL Must Be High

**Symptom:** SPI slave cannot send data to master.

**Wrong:**
```c
SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;  // Slave can't drive MISO properly
```

**Correct:**
```c
SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  // Required for slave TX
```

---

## 9. TIM1 Needs PWM Output Enable

**Symptom:** TIM1 PWM output stays low, no signal on pin.

**Wrong:**
```c
TIM_OC1Init(TIM1, &TIM_OCInitStructure);
TIM_Cmd(TIM1, ENABLE);
// Missing: TIM_CtrlPWMOutputs
```

**Correct:**
```c
TIM_OC1Init(TIM1, &TIM_OCInitStructure);
TIM_CtrlPWMOutputs(TIM1, ENABLE);  // Required for TIM1 (advanced timer)
TIM_Cmd(TIM1, ENABLE);
```

---

## 10. Flash Not Unlocked Before Write

**Symptom:** Flash write fails silently, data not programmed.

**Wrong:**
```c
FLASH_ProgramHalfWord(addr, data);  // Fails, flash is locked
```

**Correct:**
```c
FLASH_Unlock();
FLASH_ProgramHalfWord(addr, data);
FLASH_Lock();
```

---

## 11. EXTI Line Not Connected to GPIO

**Symptom:** External interrupt never fires.

**Wrong:**
```c
EXTI_InitStructure.EXTI_Line = EXTI_Line0;
EXTI_Init(&EXTI_InitStructure);
// Missing: GPIO_EXTILineConfig
```

**Correct:**
```c
GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);  // Connect GPIO to EXTI
EXTI_InitStructure.EXTI_Line = EXTI_Line0;
EXTI_Init(&EXTI_InitStructure);
```

---

## 12. NVIC Priority Group Not Configured

**Symptom:** Interrupts behave unexpectedly, priority not working.

**Wrong:**
```c
int main(void) {
    // NVIC priority group not set
    NVIC_Init(&NVIC_InitStructure);
}
```

**Correct:**
```c
int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_Init(&NVIC_InitStructure);
}
```

---

## 13. SDI Pin Conflict with GPIO

**Symptom:** Cannot use PD7 as GPIO output.

**Wrong:**
```c
// Trying to use PD7 as GPIO without disabling SDI
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
GPIO_Init(GPIOD, &GPIO_InitStructure);  // May not work, SDI uses PD7
```

**Correct:**
```c
// Option 1: Disable SDI to use PD7 as GPIO
GPIO_PinRemapConfig(GPIO_Remap_SDI_Disable, ENABLE);

// Option 2: Use SDI print instead of PD7 GPIO
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#endif
```

---

## 14. ADC Clock Too Fast

**Symptom:** ADC readings are noisy or inaccurate.

**Wrong:**
```c
RCC_ADCCLKConfig(RCC_PCLK2_Div2);  // ADC clock too fast
```

**Correct:**
```c
RCC_ADCCLKConfig(RCC_PCLK2_Div8);  // Slower, more accurate
```

---

## 15. Timer Prescaler Off-by-One

**Symptom:** Timer frequency is slightly wrong.

**Wrong:**
```c
// Trying to divide by 24
TIM_TimeBaseStructure.TIM_Prescaler = 24;  // Actually divides by 25
```

**Correct:**
```c
// Prescaler = divider - 1
TIM_TimeBaseStructure.TIM_Prescaler = 24 - 1;  // Divides by 24
```

---

## 16. IAP Application Not Offset

**Symptom:** Application doesn't boot, bootloader gets overwritten.

**Wrong:**
```c
// Application linker script
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K  // Overwrites bootloader!
```

**Correct:**
```c
// Application linker script (offset by bootloader size)
FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 12K  // After 4K bootloader
```

---

## 17. Overrun Error Not Cleared

**Symptom:** UART stops receiving after high data rate.

**Wrong:**
```c
void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        data = USART_ReceiveData(USART1);
    }
    // ORE flag not cleared!
}
```

**Correct:**
```c
void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        data = USART_ReceiveData(USART1);
    }
    if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
}
```

---

## 18. DMA Channel Wrong for Peripheral

**Symptom:** DMA transfer never completes, wrong data.

**Wrong:**
```c
DMA_DeInit(DMA1_Channel1);  // Wrong channel for USART1_TX
```

**Correct:**
```c
// Check datasheet for correct DMA channel mapping:
// USART1_TX = DMA1_Channel4
// USART1_RX = DMA1_Channel5
// SPI1_TX   = DMA1_Channel3
// SPI1_RX   = DMA1_Channel2
// ADC1      = DMA1_Channel1
DMA_DeInit(DMA1_Channel4);  // Correct for USART1_TX
```

---

## 19. GPIO Pin Source Mismatch

**Symptom:** EXTI interrupt fires on wrong pin or not at all.

**Wrong:**
```c
GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);  // PC0
// But configured EXTI for PD0
```

**Correct:**
```c
GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);  // PD0 matches EXTI_Line0
```

---

## 20. Watchdog Not Fed

**Symptom:** System resets periodically.

**Wrong:**
```c
IWDG_SetReload(0xFF);
IWDG_Enable();
while(1) {
    // Long operation, watchdog not reloaded
    LongOperation();
}
```

**Correct:**
```c
IWDG_SetReload(0xFF);
IWDG_Enable();
while(1) {
    LongOperation();
    IWDG_ReloadCounter();  // Feed the watchdog
}
```
