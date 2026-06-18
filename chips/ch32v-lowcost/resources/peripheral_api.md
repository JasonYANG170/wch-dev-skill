# Peripheral API Quick Reference

All function signatures extracted from actual CH32V003/CH32V006/CH32L103 driver headers.

## GPIO (ch32v00x_gpio.h)

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

## USART (ch32v00x_usart.h)

```c
void       USART_DeInit(USART_TypeDef *USARTx);
void       USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct);
void       USART_StructInit(USART_InitTypeDef *USART_InitStruct);
void       USART_ClockInit(USART_TypeDef *USARTx, USART_ClockInitTypeDef *USART_ClockInitStruct);
void       USART_ClockStructInit(USART_ClockInitTypeDef *USART_ClockInitStruct);
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
void       USART_SetGuardTime(USART_TypeDef *USARTx, uint8_t USART_GuardTime);
void       USART_SetPrescaler(USART_TypeDef *USARTx, uint8_t USART_Prescaler);
void       USART_SmartCardCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_SmartCardNACKCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_HalfDuplexCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_OverSampling8Cmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_OneBitMethodCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void       USART_IrDAConfig(USART_TypeDef *USARTx, uint16_t USART_IrDAMode);
void       USART_IrDACmd(USART_TypeDef *USARTx, FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);
void       USART_ClearFlag(USART_TypeDef *USARTx, uint16_t USART_FLAG);
ITStatus   USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT);
void       USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT);
```

## SPI (ch32v00x_spi.h)

```c
void       SPI_I2S_DeInit(SPI_TypeDef *SPIx);
void       SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void       SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct);
void       SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_I2S_ITConfig(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void       SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void       SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t   SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
void       SPI_NSSInternalSoftwareConfig(SPI_TypeDef *SPIx, uint16_t SPI_NSSInternalSoft);
void       SPI_SSOutputCmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_DataSizeConfig(SPI_TypeDef *SPIx, uint16_t SPI_DataSize);
void       SPI_TransmitCRC(SPI_TypeDef *SPIx);
void       SPI_CalculateCRC(SPI_TypeDef *SPIx, FunctionalState NewState);
uint16_t   SPI_GetCRC(SPI_TypeDef *SPIx, uint8_t SPI_CRC);
uint16_t   SPI_GetCRCPolynomial(SPI_TypeDef *SPIx);
void       SPI_BiDirectionalLineConfig(SPI_TypeDef *SPIx, uint16_t SPI_Direction);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void       SPI_I2S_ClearFlag(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
ITStatus   SPI_I2S_GetITStatus(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
void       SPI_I2S_ClearITPendingBit(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
```

## ADC (ch32v00x_adc.h)

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
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *ADCx);
void       ADC_DiscModeChannelCountConfig(ADC_TypeDef *ADCx, uint8_t Number);
void       ADC_DiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_ExternalTrigConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
uint16_t   ADC_GetConversionValue(ADC_TypeDef *ADCx);
void       ADC_AutoInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_InjectedDiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef *ADCx, uint32_t ADC_ExternalTrigInjecConv);
void       ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void       ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef *ADCx);
void       ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void       ADC_InjectedSequencerLengthConfig(ADC_TypeDef *ADCx, uint8_t Length);
void       ADC_SetInjectedOffset(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset);
uint16_t   ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel);
void       ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void       ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void       ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
void       ADC_ClearFlag(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
ITStatus   ADC_GetITStatus(ADC_TypeDef *ADCx, uint16_t ADC_IT);
void       ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint16_t ADC_IT);
void       ADC_Calibration_Vol(ADC_TypeDef *ADCx, uint32_t ADC_CALVOL);
void       ADC_ExternalTrig_DLY(ADC_TypeDef *ADCx, uint32_t channel, uint16_t DelayTim);
```

## TIM (ch32v00x_tim.h)

```c
void       TIM_DeInit(TIM_TypeDef *TIMx);
void       TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void       TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void       TIM_PWMIConfig(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void       TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void       TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void       TIM_OCStructInit(TIM_OCInitTypeDef *TIM_OCInitStruct);
void       TIM_ICStructInit(TIM_ICInitTypeDef *TIM_ICInitStruct);
void       TIM_BDTRStructInit(TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void       TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState);
void       TIM_GenerateEvent(TIM_TypeDef *TIMx, uint16_t TIM_EventSource);
void       TIM_DMAConfig(TIM_TypeDef *TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);
void       TIM_DMACmd(TIM_TypeDef *TIMx, uint16_t TIM_DMASource, FunctionalState NewState);
void       TIM_InternalClockConfig(TIM_TypeDef *TIMx);
void       TIM_ITRxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource);
void       TIM_TIxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_TIxExternalCLKSource,
                                      uint16_t TIM_ICPolarity, uint16_t ICFilter);
void       TIM_ETRClockMode1Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                                   uint16_t ExtTRGFilter);
void       TIM_ETRClockMode2Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler,
                                   uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);
void       TIM_ETRConfig(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                         uint16_t ExtTRGFilter);
void       TIM_PrescalerConfig(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);
void       TIM_CounterModeConfig(TIM_TypeDef *TIMx, uint16_t TIM_CounterMode);
void       TIM_SelectInputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource);
void       TIM_EncoderInterfaceConfig(TIM_TypeDef *TIMx, uint16_t TIM_EncoderMode,
                                      uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
void       TIM_ForcedOC1Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction);
void       TIM_ForcedOC2Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction);
void       TIM_ForcedOC3Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction);
void       TIM_ForcedOC4Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction);
void       TIM_ARRPreloadConfig(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_SelectCOM(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_SelectCCDMA(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_CCPreloadControl(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_OC1PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void       TIM_OC2PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void       TIM_OC3PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void       TIM_OC4PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void       TIM_OC1FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast);
void       TIM_OC2FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast);
void       TIM_OC3FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast);
void       TIM_OC4FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast);
void       TIM_ClearOC1Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear);
void       TIM_ClearOC2Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear);
void       TIM_ClearOC3Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear);
void       TIM_ClearOC4Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear);
void       TIM_OC1PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void       TIM_OC1NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity);
void       TIM_OC2PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void       TIM_OC2NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity);
void       TIM_OC3PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void       TIM_OC3NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity);
void       TIM_OC4PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void       TIM_CCxCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx);
void       TIM_CCxNCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN);
void       TIM_SelectOCxM(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode);
void       TIM_UpdateDisableConfig(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_UpdateRequestConfig(TIM_TypeDef *TIMx, uint16_t TIM_UpdateSource);
void       TIM_SelectHallSensor(TIM_TypeDef *TIMx, FunctionalState NewState);
void       TIM_SelectOnePulseMode(TIM_TypeDef *TIMx, uint16_t TIM_OPMode);
void       TIM_SelectOutputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_TRGOSource);
void       TIM_SelectSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_SlaveMode);
void       TIM_SelectMasterSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_MasterSlaveMode);
void       TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter);
void       TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload);
void       TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1);
void       TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2);
void       TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3);
void       TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4);
void       TIM_SetIC1Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC);
void       TIM_SetIC2Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC);
void       TIM_SetIC3Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC);
void       TIM_SetIC4Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC);
void       TIM_SetClockDivision(TIM_TypeDef *TIMx, uint16_t TIM_CKD);
uint16_t   TIM_GetCapture1(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture2(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture3(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCapture4(TIM_TypeDef *TIMx);
uint16_t   TIM_GetCounter(TIM_TypeDef *TIMx);
uint16_t   TIM_GetPrescaler(TIM_TypeDef *TIMx);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
void       TIM_ClearFlag(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
ITStatus   TIM_GetITStatus(TIM_TypeDef *TIMx, uint16_t TIM_IT);
void       TIM_ClearITPendingBit(TIM_TypeDef *TIMx, uint16_t TIM_IT);
```

## FLASH (ch32v00x_flash.h)

```c
void         FLASH_SetLatency(uint32_t FLASH_Latency);
void         FLASH_Unlock(void);
void         FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_EraseOptionBytes(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data);
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Pages);
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState);
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STDBY, uint16_t OB_RST, uint16_t OB_PowerON_Start_Mode);
uint32_t     FLASH_GetUserOptionByte(void);
uint32_t     FLASH_GetWriteProtectionOptionByte(void);
FlagStatus   FLASH_GetReadOutProtectionStatus(void);
void         FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void         FLASH_Unlock_Fast(void);
void         FLASH_Lock_Fast(void);
void         FLASH_BufReset(void);
void         FLASH_BufLoad(uint32_t Address, uint32_t Data0);
void         FLASH_ErasePage_Fast(uint32_t Page_Address);
void         FLASH_ProgramPage_Fast(uint32_t Page_Address);
void         SystemReset_StartMode(uint32_t Mode);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

## PWR (ch32v00x_pwr.h)

```c
void       PWR_DeInit(void);
void       PWR_PVDCmd(FunctionalState NewState);
void       PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void       PWR_AutoWakeUpCmd(FunctionalState NewState);
void       PWR_AWU_SetPrescaler(uint32_t AWU_Prescaler);
void       PWR_AWU_SetWindowValue(uint8_t WindowValue);
void       PWR_EnterSTANDBYMode(uint8_t PWR_STANDBYEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
```

## RCC (ch32v00x_rcc.h)

```c
void        RCC_DeInit(void);
void        RCC_HSEConfig(uint32_t RCC_HSE);
ErrorStatus RCC_WaitForHSEStartUp(void);
void        RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue);
void        RCC_HSICmd(FunctionalState NewState);
void        RCC_PLLConfig(uint32_t RCC_PLLSource);
void        RCC_PLLCmd(FunctionalState NewState);
void        RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t     RCC_GetSYSCLKSource(void);
void        RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void        RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState);
void        RCC_ADCCLKConfig(uint32_t RCC_PCLK2);
void        RCC_LSICmd(FunctionalState NewState);
void        RCC_GetClocksFreq(RCC_ClocksTypeDef *RCC_Clocks);
void        RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void        RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void        RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void        RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void        RCC_ClockSecuritySystemCmd(FunctionalState NewState);
void        RCC_MCOConfig(uint8_t RCC_MCO);
FlagStatus  RCC_GetFlagStatus(uint8_t RCC_FLAG);
void        RCC_ClearFlag(void);
ITStatus    RCC_GetITStatus(uint8_t RCC_IT);
void        RCC_ClearITPendingBit(uint8_t RCC_IT);
```

## EXTI (ch32v00x_exti.h)

```c
void       EXTI_DeInit(void);
void       EXTI_Init(EXTI_InitTypeDef *EXTI_InitStruct);
void       EXTI_StructInit(EXTI_InitTypeDef *EXTI_InitStruct);
void       EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
void       EXTI_ClearFlag(uint32_t EXTI_Line);
ITStatus   EXTI_GetITStatus(uint32_t EXTI_Line);
void       EXTI_ClearITPendingBit(uint32_t EXTI_Line);
```

## IWDG (ch32v00x_iwdg.h)

```c
void       IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);
void       IWDG_SetPrescaler(uint8_t IWDG_Prescaler);
void       IWDG_SetReload(uint16_t Reload);
void       IWDG_ReloadCounter(void);
void       IWDG_Enable(void);
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);
```

## I2C (ch32v00x_i2c.h)

```c
void       I2C_DeInit(I2C_TypeDef *I2Cx);
void       I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);
void       I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct);
void       I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_DMALastTransferCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_OwnAddress2Config(I2C_TypeDef *I2Cx, uint8_t Address);
void       I2C_DualAddressCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GeneralCallCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState);
void       I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction);
void       I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t    I2C_ReceiveData(I2C_TypeDef *I2Cx);
void       I2C_TransmitPEC(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_PECPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_PECPosition);
void       I2C_CalculatePEC(I2C_TypeDef *I2Cx, FunctionalState NewState);
uint8_t    I2C_GetPEC(I2C_TypeDef *I2Cx);
void       I2C_ARPCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_StretchClockCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_FastModeDutyCycleConfig(I2C_TypeDef *I2Cx, uint16_t I2C_DutyCycle);
uint32_t   I2C_ReadRegister(I2C_TypeDef *I2Cx, uint8_t I2C_Register);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
void       I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
ITStatus   I2C_GetITStatus(I2C_TypeDef *I2Cx, uint32_t I2C_IT);
void       I2C_ClearITPendingBit(I2C_TypeDef *I2Cx, uint32_t I2C_IT);
```

## NVIC (ch32v00x_misc.h)

```c
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct);
```

## CH32L103 Additional Peripherals

CH32L103 includes all CH32V003 peripherals plus:

### USB (ch32l103_usb.h)
- USB device/host support
- Endpoint configuration, transfers, interrupts

### CAN (ch32l103_can.h)
- CAN 2.0B protocol
- Message filtering, transmit, receive

### RTC (ch32l103_rtc.h)
- Real-time clock with calendar
- Alarm, wakeup, tamper detection

### BKP (ch32l103_bkp.h)
- Backup registers (42 x 16-bit)
- Tamper detection

### LPTIM (ch32l103_lptim.h)
- Low-power timer
- Ultra-low-power periodic wakeup

### CRC (ch32l103_crc.h)
- Hardware CRC calculation
- CRC-32, CRC-16

### USBPD (ch32l103_usbpd.h)
- USB Power Delivery protocol
- PD message handling, power negotiation

### OPA (ch32l103_opa.h)
- Operational amplifier
- PGA mode, comparator mode
