# Common Pitfalls and Solutions

## 1. Peripheral Clock Not Enabled

**Symptom**: Peripheral registers read as 0, or peripheral doesn't respond.

**Cause**: Forgetting to enable the peripheral clock before use.

**Solution**:
```c
// Always enable clock before configuring the peripheral
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIO
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART1
RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);   // I2C1
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);     // DMA
```

## 2. Wrong Alternate Function Number

**Symptom**: Peripheral pins don't work, signals not appearing on expected pins.

**Cause**: Using wrong AF number in `GPIO_PinAFConfig()`.

**Solution**: Check the datasheet for correct AF mapping:
- AF7 for USART1/2/3
- AF5 for SPI1/SPI2
- AF4 for I2C1/I2C2
- AF9 for CAN1/CAN2

## 3. Flash Write Without Erase

**Symptom**: Flash data corruption, write fails.

**Cause**: Flash must be erased (to 0xFF) before writing.

**Solution**:
```c
FLASH_Unlock();
FLASH_ErasePage(page_addr);  // Erase first
FLASH_ProgramWord(addr, data);  // Then write
FLASH_Lock();
```

## 4. Flash Sector Size Confusion

**Symptom**: Data loss when writing to flash.

**Cause**: CH32H417 flash erases in 4KB pages, not individual words.

**Solution**: Use read-modify-write pattern for partial page updates:
```c
// Read entire 4KB page to RAM
// Modify target bytes in RAM
// Erase 4KB page
// Write back entire 4KB from RAM
```

## 5. Bootloader Space Overwritten

**Symptom**: Chip won't boot, USB download stops working.

**Cause**: Application code written to 0x0000-0xFFFF (bootloader region).

**Solution**: Application must start at 0x10000. Ensure linker script has:
```
FLASH (rx) : ORIGIN = 0x00010000, LENGTH = 128K
```

## 6. GPIO Mode Not Set for Alternate Function

**Symptom**: Peripheral TX doesn't drive the pin, or RX doesn't receive.

**Cause**: GPIO mode not set to AF mode.

**Solution**:
```c
// For TX pins: AF push-pull
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;

// For RX pins: Input floating
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

// Don't forget AF selection
GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF7);
```

## 7. I2C Bus Hang

**Symptom**: I2C communication stuck, SDA held low.

**Cause**: Slave holding SDA low (NACK or busy), or missing stop condition.

**Solution**:
```c
// Check busy flag before starting
while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

// Generate clock pulses to release bus
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
GPIO_Init(GPIOB, &GPIO_InitStruct);
for(int i = 0; i < 9; i++)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    Delay_Us(5);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    Delay_Us(5);
}
// Reinitialize I2C
```

## 8. SPI Clock Polarity/Phase Mismatch

**Symptom**: SPI data corrupted, slave reads wrong values.

**Cause**: CPOL/CPHA mismatch between master and slave.

**Solution**: Ensure both sides use the same mode:
```c
// Mode 0: CPOL=0, CPHA=0 (most common)
SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;

// Mode 3: CPOL=1, CPHA=1
SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
```

## 9. DMA Buffer Alignment

**Symptom**: DMA transfer produces wrong data or crashes.

**Cause**: Buffer not aligned to data width.

**Solution**:
```c
// For 32-bit DMA transfers
uint32_t buffer[100] __attribute__((aligned(4)));

// For 16-bit DMA transfers
uint16_t buffer[100] __attribute__((aligned(2)));
```

## 10. LTDC Frame Buffer in Wrong Memory

**Symptom**: Display shows garbage or nothing.

**Cause**: Frame buffer placed in inaccessible memory region.

**Solution**: Ensure frame buffer is in DTCM RAM or external RAM:
```c
uint16_t fb[800*480] __attribute__((section(".ram_d1")));
// Or place at known address
uint16_t fb[800*480] __attribute__((aligned(4)));
```

## 11. Ethernet PHY Not Reset

**Symptom**: Ethernet link never comes up.

**Cause**: PHY needs hardware reset after power-up.

**Solution**:
```c
// Reset PHY via GPIO
GPIO_ResetBits(GPIOx, PHY_RESET_PIN);
Delay_Ms(10);
GPIO_SetBits(GPIOx, PHY_RESET_PIN);
Delay_Ms(50);

// Or software reset via register
ETH_WritePHYRegister(0x01, PHY_BCR, PHY_Reset);
```

## 12. CAN Filter Bank Split

**Symptom**: CAN2 or CAN3 doesn't receive messages.

**Cause**: Filter banks not properly split between CAN controllers.

**Solution**:
```c
// Split filter banks: CAN1=0-13, CAN2=14-20, CAN3=21-27
CAN_SlaveStartBank(14, 21);
```

## 13. USBPD CC Pin Configuration

**Symptom**: USB-PD communication fails, no PD messages.

**Cause**: CC pins not properly configured for source/sink role.

**Solution**:
```c
// For sink: enable pull-down
USBPD->PORT_CC2 = CC_PD | CC_CMP_45;

// For source: enable pull-up
USBPD->PORT_CC2 = CC_PU_330 | CC_CMP_66;
```

## 14. SerDes PLL Not Locked

**Symptom**: USB 3.0 not working.

**Cause**: SerDes PLL not locked before use.

**Solution**:
```c
// Wait for PLL lock
while(!(SDS_ReadCOMMAFlagBit(SDS1) & SDSIT_ST_PLLLock));

// Wait for PHY ready
while(!(SDS_ReadCOMMAFlagBit(SDS1) & SDSIT_ST_PhyReady));
```

## 15. HSADC DMA Buffer Overflow

**Symptom**: HSADC data corruption.

**Cause**: DMA buffer too small for burst transfer.

**Solution**: Ensure buffer size >= burst transfer length:
```c
#define HSADC_BURST_LEN 1024
uint16_t hsadc_buf[HSADC_BURST_LEN] __attribute__((aligned(4)));

HSADC_InitStruct.HSADC_BurstMode_TransferLen = HSADC_BURST_LEN;
HSADC_InitStruct.HSADC_DMA_TransferLen = HSADC_BURST_LEN;
```

## 16. Flash Access Clock Too Fast

**Symptom**: Flash read errors at high system clock.

**Cause**: Flash access clock not divided for high system clocks.

**Solution**:
```c
// At 120MHz, divide flash access clock by 2
FLASH_Access_Clock_Cfg(FLASH_CLK_HCLKDIV2);
```

## 17. Dual-Core Resource Conflict

**Symptom**: Unpredictable behavior when both cores access same peripheral.

**Cause**: Both cores trying to use the same peripheral without coordination.

**Solution**: Use hardware semaphores (HSEM) for resource sharing:
```c
// Acquire semaphore
while(HSEM->RLR[0] != 0x01);  // Wait for lock

// Use shared resource

// Release semaphore
HSEM->R[0] = 0x01;  // Release
```
