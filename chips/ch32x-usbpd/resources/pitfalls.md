# CH32X/CH6xx Development Common Pitfalls

Detailed explanations of common errors with wrong/correct code examples.

## 1. RCC Clock Enable Before Peripheral Access

Every peripheral requires its clock to be enabled before any register access. Forgetting this causes silent failures or hard faults.

```c
// WRONG -- accessing USART1 without enabling its clock
USART_InitTypeDef USART_InitStruct = {0};
USART_InitStruct.USART_BaudRate = 115200;
USART_Init(USART1, &USART_InitStruct);  // Hard fault or no effect

// CORRECT -- enable clock first
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
USART_Init(USART1, &USART_InitStruct);
```

**Why**: Peripheral registers are clocked by their bus clock. Without the clock, the register bus is inactive and reads return undefined values.

## 2. GPIO Alternate Function Mode

Peripheral pins (UART TX, SPI SCK, TIM CH, etc.) must use `GPIO_Mode_AF_PP`, not `GPIO_Mode_Out_PP`.

```c
// WRONG -- using output mode for USART TX
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO output, not peripheral
GPIO_Init(GPIOA, &GPIO_InitStruct);

// CORRECT -- alternate function for peripheral pins
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**Why**: `GPIO_Mode_Out_PP` connects the pin to the GPIO output register. `GPIO_Mode_AF_PP` connects the pin to the peripheral's output signal.

## 3. Flash Unlock/Lock Sequence

Flash write/erase operations require explicit unlock. Forgetting to unlock causes silent failures.

```c
// WRONG -- erase without unlock
FLASH_ErasePage(0x08001000);  // Fails silently

// CORRECT -- unlock, operation, lock
FLASH_Unlock();
FLASH_Status status = FLASH_ErasePage(0x08001000);
if(status != FLASH_COMPLETE) {
    printf("Erase failed: %d\r\n", status);
}
FLASH_Lock();
```

**Why**: Flash is locked by default to prevent accidental writes. The unlock sequence writes a specific key pattern to the FLASH_KEYR register.

## 4. Flash Page Size and Alignment

Flash operates on 256-byte pages. All erase and program operations must be page-aligned.

```c
// WRONG -- non-aligned address
FLASH_ErasePage(0x08001001);  // Not 256-byte aligned!

// WRONG -- program without erase (Flash can only clear bits)
FLASH_Unlock();
FLASH_ProgramWord(0x08001000, 0x12345678);  // May corrupt data
FLASH_Lock();

// CORRECT -- erase first, then program
FLASH_Unlock();
FLASH_ErasePage(0x08001000);  // Page-aligned address
FLASH_ProgramWord(0x08001000, 0x12345678);
FLASH_Lock();
```

**Why**: Flash memory can only change bits from 1 to 0 during programming. To set bits from 0 to 1, an erase operation (which sets all bits to 1) is required first.

## 5. USART TX Flag Check

Sending data without checking the TXE flag can overwrite the previous data.

```c
// WRONG -- sending without checking TXE
for(int i = 0; i < 100; i++) {
    USART_SendData(USART1, buffer[i]);  // Overwrites previous data!
}

// CORRECT -- wait for TX empty before each send
for(int i = 0; i < 100; i++) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, buffer[i]);
}
```

**Why**: The USART transmit data register (TDR) is double-buffered. TXE indicates the TDR is empty and ready for new data.

## 6. ADC Calibration

ADC must be calibrated after enable and before first conversion for accurate results.

```c
// WRONG -- starting conversion without calibration
ADC_Cmd(ADC1, ENABLE);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // Inaccurate!

// CORRECT -- calibrate first
ADC_Cmd(ADC1, ENABLE);
ADC_ResetCalibration(ADC1);
while(ADC_GetResetCalibrationStatus(ADC1));
ADC_StartCalibration(ADC1);
while(ADC_GetCalibrationStatus(ADC1));
// Now safe to convert
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
```

**Why**: ADC calibration compensates for internal offset and gain errors. Without calibration, the conversion results have systematic errors.

## 7. TIM1 PWM Output Enable

TIM1 is an advanced timer with complementary outputs. PWM output requires enabling the BDTR MOE (Main Output Enable) bit.

```c
// WRONG -- enabling TIM1 without MOE
TIM_OC1Init(TIM1, &TIM_OCInitStruct);
TIM_Cmd(TIM1, ENABLE);
// No output on pin!

// CORRECT -- enable main output
TIM_CtrlPWMOutputs(TIM1, ENABLE);  // Sets MOE bit
TIM_Cmd(TIM1, ENABLE);
```

**Why**: Advanced timers (TIM1) have a break and dead-time register (BDTR) with MOE bit that gates all outputs. This is a safety feature for motor control applications.

## 8. USB-PD CC Pin Comparator Configuration

USB-PD requires proper CC pin comparator thresholds for BMC communication.

```c
// WRONG -- only pull-down, no comparator
USBPD->PORT_CC1 = CC_PD;  // Can't detect PD signals

// CORRECT -- pull-down + comparator threshold
USBPD->PORT_CC1 = CC_PD | CC_CMP_22;  // 0.22V threshold for sink
USBPD->PORT_CC2 = CC_PD | CC_CMP_22;

// For source role with pull-up:
USBPD->PORT_CC1 = CC_PU_330 | CC_CMP_66;  // 330uA pull-up + 0.66V
```

**Why**: The CC comparator detects BMC (Biphase Mark Coding) signals from the PD partner. Without proper thresholds, the PD PHY cannot decode incoming messages.

## 9. USB-PD PHY Voltage Selection

The `USBPD_PHY_V33` bit must match the actual VDD voltage to prevent PHY damage.

```c
// WRONG -- VDD=5V but using direct mode (no LDO limit)
USBPD->PORT_CC1 |= USBPD_PHY_V33;  // PHY sees 5V, may damage!

// CORRECT -- for VDD > 4V, use LDO limit
USBPD->PORT_CC1 &= ~USBPD_PHY_V33;  // LDO limits to ~3.3V

// CORRECT -- for VDD = 3.3V, use direct mode
USBPD->PORT_CC1 |= USBPD_PHY_V33;
```

**Why**: The USBPD PHY has an internal LDO that limits the voltage to 3.3V. When VDD is already 3.3V, the LDO is bypassed for better signal integrity.

## 10. Pin Remap AFIO Clock

GPIO pin remapping requires the AFIO clock to be enabled.

```c
// WRONG -- remap without AFIO clock
GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);  // No effect

// CORRECT -- enable AFIO clock first
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);
```

**Why**: The AFIO (Alternate Function I/O) register block controls pin remapping. Its clock must be enabled for register access.

## 11. NVIC Priority Configuration

The priority group must be configured before setting individual interrupt priorities.

```c
// WRONG -- setting priorities without group config
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
NVIC_Init(&NVIC_InitStruct);  // May use wrong priority split

// CORRECT -- configure group first
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 2+2 bit split
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
NVIC_Init(&NVIC_InitStruct);
```

**Why**: The priority group configuration determines how the 4-bit priority field is split between preemption priority and sub-priority.

## 12. System Clock After STOP Mode Wakeup

After wakeup from STOP mode, the system uses HSI (internal RC) clock, not the PLL.

```c
// After wakeup from STOP mode:
// System clock is now HSI (~8MHz), not PLL (48MHz)

// WRONG -- USART baud rate is now wrong
printf("Woke up!\r\n");  // Garbled output

// CORRECT -- reconfigure system clock
SystemCoreClockUpdate();
USART_Printf_Init(115200);  // Re-init with correct clock
printf("Woke up!\r\n");
```

**Why**: STOP mode disables the PLL to save power. On wakeup, the system automatically switches to HSI until the PLL is re-enabled.
