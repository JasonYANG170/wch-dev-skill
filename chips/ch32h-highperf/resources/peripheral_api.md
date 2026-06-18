# CH32H417 Peripheral API Reference

## GPIO

```c
void GPIO_DeInit(GPIO_TypeDef *GPIOx);
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, uint8_t GPIO_PinSource, uint8_t GPIO_AF);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_IPD_Unused(void);
```

## USART/UART

```c
void USART_DeInit(USART_TypeDef* USARTx);
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
void USART_StructInit(USART_InitTypeDef* USART_InitStruct);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);
void USART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp);
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength);
void USART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
void USART_SendBreak(USART_TypeDef* USARTx);
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode);
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
```

## SPI/I2S

```c
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft);
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, uint16_t SPI_DataSize);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_HighSpeedMode_Config(SPI_TypeDef *SPIx, uint8_t SPI_SPEED_MODE, FunctionalState NewState);
```

## I2C

```c
void I2C_DeInit(I2C_TypeDef* I2Cx);
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
```

## ADC

```c
void ADC_DeInit(ADC_TypeDef* ADCx);
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
uint32_t ADC_GetDualModeConversionValue(void);
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog);
void ADC_TempSensorVrefintCmd(FunctionalState NewState);
void ADC_BufferCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_LowPowerModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
s32 TempSensor_Volt_To_Temper(s32 Value);
```

## HSADC (High-Speed ADC)

```c
void HSADC_DeInit(void);
void HSADC_Init(HSADC_InitTypeDef *HSADC_InitStruct);
void HSADC_Cmd(FunctionalState NewState);
void HSADC_DMACmd(FunctionalState NewState);
void HSADC_DualBufferCmd(FunctionalState NewState);
void HSADC_BurstModeCmd(FunctionalState NewState);
void HSADC_ChannelConfig(uint8_t HSADC_Channel);
uint16_t HSADC_GetConversionValue(void);
```

## Timer

```c
void TIM_DeInit(TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare1);
uint16_t TIM_GetCapture1(TIM_TypeDef* TIMx);
uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);

// TIM9-12 (32-bit timer) functions
void TIM9_12_TimeBaseInit(TIM_TypeDef *TIMx, TIM9_12_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void TIM9_12_OC1Init(TIM_TypeDef *TIMx, TIM9_12_OCInitTypeDef *TIM_OCInitStruct);
void TIM9_12_SetCompare1(TIM_TypeDef *TIMx, uint32_t Compare1);
void TIM9_12_SetCounter(TIM_TypeDef *TIMx, uint32_t Counter);
uint32_t TIM9_12_GetCounter(TIM_TypeDef *TIMx);
```

## DMA

```c
void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx);
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber);
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(DMA_TypeDef* DMAx, uint32_t DMAy_FLAG);
void DMA_ClearFlag(DMA_TypeDef* DMAx, uint32_t DMAy_FLAG);
void DMA_MuxChannelConfig(uint8_t DMA_MuxChannelx, uint32_t DMA_Requestx);
```

## Flash

```c
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_EraseBlock_Fast(uint32_t Block_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t* pbuf);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
FLASHCapacity FLASH_GetCapacity(void);
```

## CAN

```c
void CAN_DeInit(CAN_TypeDef *CANx);
uint8_t CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct);
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);
uint8_t CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage);
uint8_t CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox);
void CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage);
void CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber);
uint8_t CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber);
void CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState);
void CAN_SlaveStartBank(uint8_t CAN2_BankNumber, uint8_t CAN3_BankNumber);
```

## Ethernet

```c
void ETH_DeInit(void);
void ETH_Start(void);
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength);
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
```

## LTDC

```c
void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct);
void LTDC_Cmd(FunctionalState NewState);
void LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct);
void LTDC_LayerCmd(LTDC_Layer_TypeDef* LTDC_Layerx, FunctionalState NewState);
void LTDC_LayerPosition(LTDC_Layer_TypeDef* LTDC_Layerx, uint16_t OffsetX, uint16_t OffsetY);
void LTDC_LayerAlpha(LTDC_Layer_TypeDef* LTDC_Layerx, uint8_t ConstantAlpha);
void LTDC_LayerAddress(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Address);
void LTDC_LayerSize(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t Width, uint32_t Height);
void LTDC_LayerPixelFormat(LTDC_Layer_TypeDef* LTDC_Layerx, uint32_t PixelFormat);
void LTDC_ReloadConfig(uint32_t LTDC_Reload);
```

## PWR

```c
void PWR_DeInit(void);
void PWR_BackupAccessCmd(FunctionalState NewState);
void PWR_PVDCmd(FunctionalState NewState);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void PWR_VIO18ModeCfg(uint32_t PWR_VIO18CfgMode);
void PWR_VIO18LevelCfg(uint16_t VIO18Level);
```

## SerDes

```c
void SDS_Config(SDS_TypeDef *SDSx, SDS_CFG_TypeDef *sds_cfg);
void SDS_DMA_Tx_CFG(SDS_TypeDef *SDSx, uint32_t DMAaddr, uint32_t DataLen, uint32_t CustomWord);
ErrorStatus SDS_DMA_Rx_CFG(SDS_TypeDef *SDSx, uint32_t DMAaddr0, uint32_t DMAaddr1);
uint32_t SDS_GetFirstWord(SDS_TypeDef *SDSx, uint8_t buffnum);
void SDS_RTX_Ctrl(SDS_TypeDef *SDSx, SDSRTXCtrl_TypeDef *ctrl);
SDSIT_FLAG_TypeDef SDS_ReadIT(SDS_TypeDef *SDSx);
void SDS_ConfigIT(SDS_TypeDef *SDSx, SDSIT_EN_TypeDef sdsit_en, FunctionalState en);
SDSIT_ST_TypeDef SDS_ReadCOMMAFlagBit(SDS_TypeDef *SDSx);
```

## QSPI

```c
void QSPI_Init(QSPI_TypeDef *QSPIx, QSPI_InitTypeDef *QSPI_InitStruct);
void QSPI_ComConfig_Init(QSPI_TypeDef *QSPIx, QSPI_ComConfig_InitTypeDef *QSPI_ComConfig_InitStruct);
void QSPI_Cmd(QSPI_TypeDef *QSPIx, FunctionalState NewState);
void QSPI_AutoPollingMode_Config(QSPI_TypeDef *QSPIx, ...);
void QSPI_MemoryMappedMode_Config(QSPI_TypeDef *QSPIx, ...);
uint32_t QSPI_ReceiveData(QSPI_TypeDef *QSPIx);
```

## SAI (Serial Audio Interface)

```c
void SAI_Init(SAI_Block_TypeDef* SAI_Block_x, SAI_InitTypeDef* SAI_InitStruct);
void SAI_FrameInit(SAI_Block_TypeDef* SAI_Block_x, SAI_FrameInitTypeDef* SAI_FrameInitStruct);
void SAI_SlotInit(SAI_Block_TypeDef* SAI_Block_x, SAI_SlotInitTypeDef* SAI_SlotInitStruct);
void SAI_Cmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);
void SAI_DMACmd(SAI_Block_TypeDef* SAI_Block_x, FunctionalState NewState);
```

## I3C

```c
void I3C_Ctrl_Init(I3C_TypeDef *I3Cx, I3C_Ctrl_BusTypeDef *I3C_InitStruct);
void I3C_Ctrl_Conf_Init(I3C_TypeDef *I3Cx, I3C_CtrlConfTypeDef *I3C_InitStruct);
void I3C_Cmd(I3C_TypeDef *I3Cx, FunctionalState NewState);
void I3C_Ctrl_SendCCC_Cmd(I3C_TypeDef *I3Cx, ...);
```

## DFSDM

```c
void DFSDM_ChannelInit(DFSDM_Channel_TypeDef *DFSDM_Channelx, DFSDM_ChannelInitTypeDef *DFSDM_ChannelInitStruct);
void DFSDM_FilterInit(DFSDM_Filter_TypeDef *DFSDM_Filterx, DFSDM_FilterInitTypeDef *DFSDM_FilterInitStruct);
void DFSDM_Cmd(DFSDM_Filter_TypeDef *DFSDM_Filterx, FunctionalState NewState);
```

## FMC (External Memory Controller)

```c
void FMC_NORSRAMInit(FMC_NORSRAMTimingInitTypeDef *FMC_NORSRAMInitStruct);
void FMC_NORSRAMCmd(uint32_t FMC_Bank, FunctionalState NewState);
void FMC_NANDInit(...);
void FMC_SDRAMInit(...);
```
