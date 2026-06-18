# Common Pitfalls and Fixes

Quick reference for errors frequently encountered in CH32V development. Each entry shows the wrong code, the symptom, and the correct fix.

---

## 1. Missing RCC Clock Enable

**Symptom**: Peripheral registers read as zero, peripheral does not respond, or hard fault.

**Wrong**:
```c
// Forgot to enable clock
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**Correct**:
```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**Rule**: Always enable the peripheral clock on the correct bus before any register access:
- APB2: GPIOA-D, USART1, SPI1, ADC1, TIM1, TIM8, AFIO
- APB1: USART2/3, SPI2, I2C1/2, TIM2-7, CAN1, DAC, PWR
- AHB: DMA1/2, ETH_MAC, USBFS/HS

---

## 2. Wrong GPIO Mode for Alternate Function

**Symptom**: Peripheral TX pin stuck low or high, no signal output.

**Wrong**:
```c
// USART1 TX as push-pull output (no signal)
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // WRONG
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**Correct**:
```c
// USART1 TX must be AF push-pull
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;   // Correct
GPIO_Init(GPIOA, &GPIO_InitStruct);

// USART1 RX must be floating input
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // Correct
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

**Rule**: Alternate function pins use:
- TX/MOSI/SCL/MISO outputs: `GPIO_Mode_AF_PP` (or `GPIO_Mode_AF_OD` for open-drain)
- RX/MISO inputs: `GPIO_Mode_IN_FLOATING`
- I2C SDA/SCL: `GPIO_Mode_AF_OD` (open-drain required by I2C spec)

---

## 3. Flash Write Without Erase

**Symptom**: Data written to Flash is corrupted; bits that need to go from 0 to 1 stay at 0.

**Wrong**:
```c
FLASH_Unlock();
FLASH_ProgramWord(0x0800F000, 0x12345678);  // May fail if not erased
FLASH_Lock();
```

**Correct**:
```c
FLASH_Unlock();
FLASH_ErasePage(0x0800F000);               // Erase 4KB sector first
FLASH_ProgramWord(0x0800F000, 0x12345678);  // Now program
FLASH_Lock();
```

**Rule**: Flash can only change bits from 1 to 0. To set bits back to 1, you must erase the entire page (4KB). For partial-page updates, read-modify-write:
```c
uint32_t page_buf[1024];  // 4KB = 1024 words
memcpy(page_buf, (void*)page_addr, 4096);   // Read
page_buf[offset/4] = new_value;              // Modify
FLASH_ErasePage(page_addr);                  // Erase
for(int i = 0; i < 1024; i++)
    FLASH_ProgramWord(page_addr + i*4, page_buf[i]);  // Write back
```

---

## 4. USART Printf Not Working

**Symptom**: `printf()` produces no output on UART.

**Wrong**:
```c
// USART init but no printf redirect
USART_InitTypeDef USART_InitStruct;
USART_InitStruct.USART_BaudRate = 115200;
// ... init code ...
USART_Cmd(USART1, ENABLE);
printf("Hello\n");  // No output
```

**Correct**:
```c
// Add printf redirect function
int _write(int fd, char *buf, int size)
{
    for(int i = 0; i < size; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, buf[i]);
    }
    return size;
}

// Use USART1_TX on PA9 (no remap) for printf
```

**Rule**: `printf()` requires `_write()` or `_putchar()` retarget. The function must poll `USART_FLAG_TXE` before each byte.

---

## 5. I2C Missing Event Check

**Symptom**: I2C communication hangs or produces garbage; SDA stays low.

**Wrong**:
```c
I2C_GenerateSTART(I2C1, ENABLE);
I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
I2C_SendData(I2C1, reg_addr);  // Sent immediately, may NACK
```

**Correct**:
```c
I2C_GenerateSTART(I2C1, ENABLE);
while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

I2C_SendData(I2C1, reg_addr);
while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
```

**Rule**: Every I2C step must wait for the corresponding event. The sequence is:
1. START -> wait `EVENT_MASTER_MODE_SELECT`
2. Address -> wait `EVENT_MASTER_TRANSMITTER_MODE_SELECTED` or `_RECEIVER_MODE_SELECTED`
3. Data TX -> wait `EVENT_MASTER_BYTE_TRANSMITTED`
4. Data RX -> wait `EVENT_MASTER_BYTE_RECEIVED`

---

## 6. CAN Filter Not Configured

**Symptom**: CAN transmit works but receive always returns zero messages.

**Wrong**:
```c
CAN_Init(CAN1, &CAN_InitStruct);
// No filter configured -> no messages received
```

**Correct**:
```c
CAN_Init(CAN1, &CAN_InitStruct);

CAN_FilterInitTypeDef CAN_FilterStruct;
CAN_FilterStruct.CAN_FilterNumber = 0;
CAN_FilterStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
CAN_FilterStruct.CAN_FilterScale = CAN_FilterScale_32bit;
CAN_FilterStruct.CAN_FilterIdHigh = 0x0000;
CAN_FilterStruct.CAN_FilterIdLow = 0x0000;
CAN_FilterStruct.CAN_FilterMaskIdHigh = 0x0000;
CAN_FilterStruct.CAN_FilterMaskIdLow = 0x0000;
CAN_FilterStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
CAN_FilterStruct.CAN_FilterActivation = ENABLE;
CAN_FilterInit(&CAN_FilterStruct);  // Accept all messages
```

**Rule**: CAN hardware filters must be configured before receiving. Without a filter, the hardware rejects all incoming messages.

---

## 7. ADC Calibration Skipped

**Symptom**: ADC readings are inaccurate or offset by a constant.

**Wrong**:
```c
ADC_Cmd(ADC1, ENABLE);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // No calibration
while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
uint16_t val = ADC_GetConversionValue(ADC1);  // May be inaccurate
```

**Correct**:
```c
ADC_Cmd(ADC1, ENABLE);

// Calibration sequence (mandatory for accuracy)
ADC_ResetCalibration(ADC1);
while(ADC_GetResetCalibrationStatus(ADC1));
ADC_StartCalibration(ADC1);
while(ADC_GetCalibrationStatus(ADC1));

ADC_SoftwareStartConvCmd(ADC1, ENABLE);
while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
uint16_t val = ADC_GetConversionValue(ADC1);
```

**Rule**: Always run the calibration sequence after enabling ADC and before the first conversion.

---

## 8. DMA Not Enabled on Peripheral Side

**Symptom**: DMA channel enabled but no data transfers; ADC/USART/SPI DMA never triggers.

**Wrong**:
```c
DMA_Cmd(DMA1_Channel1, ENABLE);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
// DMA never moves data because ADC DMA not enabled
```

**Correct**:
```c
DMA_Cmd(DMA1_Channel1, ENABLE);
ADC_DMACmd(ADC1, ENABLE);                  // Enable DMA on ADC side
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
```

**Rule**: DMA must be enabled on both sides:
- Memory side: `DMA_Cmd()`
- Peripheral side: `ADC_DMACmd()`, `USART_DMACmd()`, `SPI_I2S_DMACmd()`

---

## 9. Timer PWM Output Not Visible

**Symptom**: Timer counts but PWM pin stays low.

**Wrong**:
```c
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
TIM_OC1Init(TIM2, &TIM_OCInitStruct);
TIM_Cmd(TIM2, ENABLE);
// PWM pin has no output
```

**Correct**:
```c
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
TIM_OC1Init(TIM2, &TIM_OCInitStruct);
TIM_Cmd(TIM2, ENABLE);

// For TIM1 (advanced timer) only:
TIM_CtrlPWMOutputs(TIM1, ENABLE);  // Required for TIM1
```

**Rule**: TIM1/TIM8 (advanced timers) require `TIM_CtrlPWMOutputs(ENABLE)` after `TIM_Cmd()`. General-purpose timers (TIM2-5) do not. Also verify GPIO is configured as `GPIO_Mode_AF_PP`.

---

## 10. Stack Overflow in BLE/RTOS Projects

**Symptom**: Random hard faults, corrupted variables, system hangs.

**Wrong**:
```c
// Large buffer on stack
void ProcessData(void)
{
    uint8_t buf[2048];  // Stack is typically only 2-4KB
    // ...
}
```

**Correct**:
```c
// Use static or global buffer
static uint8_t buf[2048];

void ProcessData(void)
{
    // Use buf[]
}
```

**Rule**: Stack size is limited (typically 2-4KB in Link.ld). Large arrays should be `static` or global. In BLE projects, the BLE stack itself uses significant stack space.

---

## 11. GPIO Remap Not Applied

**Symptom**: Alternate function works on default pins but not on expected remap pins.

**Wrong**:
```c
// Want USART1 on PB6/PB7 (full remap)
GPIO_Init(...);  // Configured PB6/PB7 as AF
// USART1 still on PA9/PA10
```

**Correct**:
```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);  // Remap first

// Then configure PB6/PB7 as AF
GPIO_Init(...);
```

**Rule**: Pin remap must be enabled via `GPIO_PinRemapConfig()` before configuring the GPIO pins. The AFIO clock must be enabled.

---

## 12. Ethernet Link Status Not Checked

**Symptom**: ETH_HandleTxPkt returns error; no frames received.

**Wrong**:
```c
ETH_Start();
ETH_SendFrame(data, len);  // May fail if link not up
```

**Correct**:
```c
ETH_Start();

while(1)
{
    if(ETH_GetlinkStaus() == SET)  // Check link first
    {
        ETH_SendFrame(data, len);
    }
}
```

**Rule**: Always check `ETH_GetlinkStaus()` before sending or receiving. The PHY needs time to auto-negotiate after cable connection.

---

## 13. Wrong Peripheral Base Address for DMA

**Symptom**: DMA transfers garbage or wrong data.

**Wrong**:
```c
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)USART1;  // Wrong, points to base
```

**Correct**:
```c
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;  // Data register
```

**Rule**: DMA peripheral address must point to the specific data register, not the peripheral base:
- USART: `&USARTx->DATAR`
- SPI: `&SPIx->DATAR`
- ADC: `&ADCx->RDATAR`

---

## 14. IAP Application Not Relocated

**Symptom**: IAP bootloader works but application crashes on jump.

**Wrong**:
```c
// Application linker script still at 0x00000000
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K  // WRONG for IAP app
}
```

**Correct**:
```c
// Application linker script with offset
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 432K  // After 16KB bootloader
}
```

**Rule**: IAP application linker script must offset FLASH origin past the bootloader. Also set VTOR in the application:
```c
SCB->VTOR = 0x00004000;  // Vector table relocation
```
