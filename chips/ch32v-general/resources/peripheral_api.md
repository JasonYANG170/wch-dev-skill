# Peripheral API Quick Reference

Function signatures extracted from actual WCH driver headers. Each family has identical API patterns with different prefixes.

## Header-to-Family Mapping

| Family | Main Header | Driver Prefix | USB Header |
|--------|-------------|---------------|------------|
| CH32V103 | `ch32v10x.h` | `ch32v10x_` | `ch32v10x_usb.h` |
| CH32V20x | `ch32v20x.h` | `ch32v20x_` | `ch32v20x_usb.h` |
| CH32V307 | `ch32v30x.h` | `ch32v30x_` | `ch32v30x_usb.h` |
| CH32V407 | `ch32v4x7.h` | `ch32v4x7_` | `ch32v4x7_usbhs.h` |

---

## GPIO (ch32v*_gpio.h)

```c
void     GPIO_DeInit(GPIO_TypeDef *GPIOx);
void     GPIO_AFIODeInit(void);
void     GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void     GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);
uint8_t  GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
uint8_t  GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);
void     GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void     GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void     GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void     GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal);
void     GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void     GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void     GPIO_EventOutputCmd(FunctionalState NewState);
void     GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void     GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

### GPIO Ports
| Port | Base Address |
|------|-------------|
| GPIOA | 0x40010800 |
| GPIOB | 0x40010C00 |
| GPIOC | 0x40011000 |
| GPIOD | 0x40011400 |

---

## USART (ch32v*_usart.h)

```c
void       USART_DeInit(USART_TypeDef *USARTx);
void       USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct);
void       USART_StructInit(USART_InitTypeDef *USART_InitStruct);
void       USART_Cmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState);
void       USART_DMACmd(USART_TypeDef *USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
void       USART_SetAddress(USART_TypeDef *USARTx, uint8_t USART_Address);
void       USART_WakeUpConfig(USART_TypeDef *USARTx, uint16_t USART_WakeUp);
void       USART_ReceiverWakeUpCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_LINBreakDetectLengthConfig(USART_TypeDef *USARTx, uint16_t USART_LINBreakDetectLength);
void       USART_LINCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_SendData(USART_TypeDef *USARTx, uint16_t Data);
uint16_t   USART_ReceiveData(USART_TypeDef *USARTx);
void       USART_SendBreak(USART_TypeDef *USARTx);
void       USART_HalfDuplexCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_IrDAConfig(USART_TypeDef *USARTx, uint16_t USART_IrDAMode);
void       USART_IrDACmd(USART_TypeDef *USARTx, FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);
void       USART_ClearFlag(USART_TypeDef *USARTx, uint16_t USART_FLAG);
ITStatus   USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT);
void       USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT);
```

---

## SPI (ch32v*_spi.h)

```c
void       SPI_I2S_DeInit(SPI_TypeDef *SPIx);
void       SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void       I2S_Init(SPI_TypeDef *SPIx, I2S_InitTypeDef *I2S_InitStruct);
void       SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct);
void       I2S_StructInit(I2S_InitTypeDef *I2S_InitStruct);
void       SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       I2S_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_I2S_ITConfig(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void       SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void       SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t   SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
void       SPI_NSSInternalSoftwareConfig(SPI_TypeDef *SPIx, uint16_t SPI_NSSInternalSoft);
void       SPI_SSOutputCmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_DataSizeConfig(SPI_TypeDef *SPIx, uint16_t SPI_DataSize);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void       SPI_I2S_ClearFlag(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
ITStatus   SPI_I2S_GetITStatus(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
void       SPI_I2S_ClearITPendingBit(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
```

---

## I2C (ch32v*_i2c.h)

```c
void       I2C_DeInit(I2C_TypeDef *I2Cx);
void       I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);
void       I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct);
void       I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_OwnAddress2Config(I2C_TypeDef *I2Cx, uint8_t Address);
void       I2C_DualAddressCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GeneralCallCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState);
void       I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t    I2C_ReceiveData(I2C_TypeDef *I2Cx);
void       I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction);
ErrorStatus I2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
uint32_t   I2C_GetLastEvent(I2C_TypeDef *I2Cx);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
void       I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
ITStatus   I2C_GetITStatus(I2C_TypeDef *I2Cx, uint32_t I2C_IT);
void       I2C_ClearITPendingBit(I2C_TypeDef *I2Cx, uint32_t I2C_IT);
```

---

## ADC (ch32v*_adc.h)

```c
void       ADC_DeInit(ADC_TypeDef *ADCx);
void       ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct);
void       ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct);
void       ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ITConfig(ADC_TypeDef *ADCx, uint16_t ADC_IT, FunctionalState NewState);
void       ADC_ResetCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_StartCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx);
void       ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
uint16_t   ADC_GetConversionValue(ADC_TypeDef *ADCx);
uint32_t   ADC_GetDualModeConversionValue(void);
void       ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void       ADC_TempSensorVrefintCmd(FunctionalState NewState);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
void       ADC_ClearFlag(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
ITStatus   ADC_GetITStatus(ADC_TypeDef *ADCx, uint16_t ADC_IT);
void       ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint16_t ADC_IT);
s32        TempSensor_Volt_To_Temper(s32 Value);
void       ADC_BufferCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
int16_t    Get_CalibrationValue(ADC_TypeDef *ADCx);
```

---

## TIM (ch32v*_tim.h)

```c
void       TIM_DeInit(TIM_TypeDef *TIMx);
void       TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void       TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void       TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void       TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState);
void       TIM_GenerateEvent(TIM_TypeDef *TIMx, uint16_t TIM_EventSource);
void       TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1);
void       TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2);
void       TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3);
void       TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4);
uint16_t   TIM_GetCapture1(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture2(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture3(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture4(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCounter(TIM_TypeDef *TIMx);
void       TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter);
void       TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
void       TIM_ClearFlag(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
ITStatus   TIM_GetITStatus(TIM_TypeDef *TIMx, uint16_t TIM_IT);
void       TIM_ClearITPendingBit(TIM_TypeDef *TIMx, uint16_t TIM_IT);
```

---

## Flash (ch32v*_flash.h)

```c
void         FLASH_Unlock(void);
void         FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data);
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Sectors);
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
void         FLASH_Unlock_Fast(void);
void         FLASH_Lock_Fast(void);
void         FLASH_ErasePage_Fast(uint32_t Page_Address);
void         FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t *pbuf);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

---

## DMA (ch32v*_dma.h)

```c
void       DMA_DeInit(DMA_Channel_TypeDef *DMAy_Channelx);
void       DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, DMA_InitTypeDef *DMA_InitStruct);
void       DMA_StructInit(DMA_InitTypeDef *DMA_InitStruct);
void       DMA_Cmd(DMA_Channel_TypeDef *DMAy_Channelx, FunctionalState NewState);
void       DMA_ITConfig(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void       DMA_SetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx, uint16_t DataNumber);
uint16_t   DMA_GetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void       DMA_ClearFlag(uint32_t DMAy_FLAG);
ITStatus   DMA_GetITStatus(uint32_t DMAy_IT);
void       DMA_ClearITPendingBit(uint32_t DMAy_IT);
```

---

## CAN (ch32v20x_can.h, ch32v30x_can.h)

```c
void       CAN_DeInit(CAN_TypeDef *CANx);
uint8_t    CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct);
void       CAN_FilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct);
void       CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct);
uint8_t    CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage);
uint8_t    CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox);
void       CAN_CancelTransmit(CAN_TypeDef *CANx, uint8_t Mailbox);
void       CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage);
void       CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber);
uint8_t    CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber);
void       CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState);
FlagStatus CAN_GetFlagStatus(CAN_TypeDef *CANx, uint32_t CAN_FLAG);
void       CAN_ClearFlag(CAN_TypeDef *CANx, uint32_t CAN_FLAG);
ITStatus   CAN_GetITStatus(CAN_TypeDef *CANx, uint32_t CAN_IT);
void       CAN_ClearITPendingBit(CAN_TypeDef *CANx, uint32_t CAN_IT);
```

---

## PWR (ch32v*_pwr.h)

```c
void       PWR_DeInit(void);
void       PWR_BackupAccessCmd(FunctionalState NewState);
void       PWR_PVDCmd(FunctionalState NewState);
void       PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void       PWR_WakeUpPinCmd(FunctionalState NewState);
void       PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void       PWR_EnterSTANDBYMode(void);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void       PWR_ClearFlag(uint32_t PWR_FLAG);
```

---

## Ethernet (ch32v30x_eth.h, ch32v4x7_eth.h)

```c
void ETH_DeInit(void);
void ETH_StructInit(ETH_InitTypeDef* ETH_InitStruct);
void ETH_SoftwareReset(void);
FlagStatus ETH_GetlinkStaus(void);
void ETH_Start(void);
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength);
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);
uint32_t ETH_GetRxPktSize(void);
void ETH_DropRxPkt(void);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
void ETH_MACTransmissionCmd(FunctionalState NewState);
void ETH_MACReceptionCmd(FunctionalState NewState);
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_DMATransmissionCmd(FunctionalState NewState);
void ETH_DMAReceptionCmd(FunctionalState NewState);
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState);
```

---

## Additional Peripherals (CH32V307/V407 only)

### DAC (ch32v30x_dac.h)
```c
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef *DAC_InitStruct);
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
```

### FSMC (ch32v30x_fsmc.h)
- Flexible Static Memory Controller for SRAM, NOR Flash, NAND, PC Card

### SDIO (ch32v30x_sdio.h)
- SD card interface for data logging and storage

### RNG (ch32v30x_rng.h)
```c
void RNG_Cmd(FunctionalState NewState);
uint32_t RNG_GetRandomNumber(void);
```

### DVP (ch32v30x_dvp.h, ch32v4x7_dvp.h)
- Digital Video Port for camera interface

### LTDC (ch32v4x7_ltdc.h) -- CH32V407 only
- LCD-TFT Display Controller for RGB displays

### ARGB (ch32v4x7_argb.h) -- CH32V407 only
- Addressable RGB LED controller (WS2812, etc.)
