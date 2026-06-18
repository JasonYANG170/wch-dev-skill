# CH32X/CH6xx Peripheral Driver API Quick Reference

All function signatures are extracted from the actual WCH StdPeriphDriver headers.

## System / Clock (RCC)

```c
// Header: ch32x035_rcc.h (or chip-specific equivalent)

// Clock configuration
void RCC_DeInit(void);
void RCC_HSEConfig(uint32_t RCC_HSE);
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul);
void RCC_PLLCmd(FunctionalState NewState);
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t RCC_GetSYSCLKSource(void);
void RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void RCC_PCLK1Config(uint32_t RCC_HCLK);
void RCC_PCLK2Config(uint32_t RCC_HCLK);
void RCC_GetClocksFreq(RCC_ClocksTypeDef *RCC_Clocks);

// Peripheral clock enable/disable
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

// Peripheral reset
void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

// System core clock update
void SystemCoreClockUpdate(void);
```

### RCC Peripheral Macros

```c
// AHB peripherals
RCC_AHBPeriph_DMA1
RCC_AHBPeriph_SRAM
RCC_AHBPeriph_USBFS
RCC_AHBPeriph_IO2W
RCC_AHBPeriph_USBPD

// APB2 peripherals
RCC_APB2Periph_AFIO
RCC_APB2Periph_GPIOA
RCC_APB2Periph_GPIOB
RCC_APB2Periph_GPIOC
RCC_APB2Periph_ADC1
RCC_APB2Periph_TIM1
RCC_APB2Periph_SPI1
RCC_APB2Periph_USART1

// APB1 peripherals
RCC_APB1Periph_TIM2
RCC_APB1Periph_TIM3
RCC_APB1Periph_USART2
RCC_APB1Periph_USART3
RCC_APB1Periph_USART4
RCC_APB1Periph_I2C1
RCC_APB1Periph_PWR
```

## GPIO

```c
// Header: ch32x035_gpio.h

void GPIO_DeInit(GPIO_TypeDef *GPIOx);
void GPIO_AFIODeInit(void);
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
uint32_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
uint32_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef *GPIOx, uint32_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint16_t GPIO_PinSource);
void GPIO_IPD_Unused(void);
```

### GPIO Init Structure

```c
typedef struct {
    uint32_t GPIO_Pin;         // GPIO_Pin_0 through GPIO_Pin_23, GPIO_Pin_All
    GPIOSpeed_TypeDef GPIO_Speed;  // GPIO_Speed_50MHz
    GPIOMode_TypeDef GPIO_Mode;    // See mode table below
} GPIO_InitTypeDef;
```

### GPIO Modes

| Mode | Value | Description |
|------|-------|-------------|
| `GPIO_Mode_AIN` | 0x00 | Analog input |
| `GPIO_Mode_IN_FLOATING` | 0x04 | Floating input |
| `GPIO_Mode_IPD` | 0x28 | Input pull-down |
| `GPIO_Mode_IPU` | 0x48 | Input pull-up |
| `GPIO_Mode_Out_PP` | 0x10 | Push-pull output |
| `GPIO_Mode_AF_PP` | 0x18 | Alternate function push-pull |

### GPIO Pin Sources

```c
GPIO_PinSource0  through GPIO_PinSource23
GPIO_PortSourceGPIOA, GPIO_PortSourceGPIOB, GPIO_PortSourceGPIOC
```

### GPIO Remap Constants (partial list)

```c
GPIO_PartialRemap1_SPI1, GPIO_PartialRemap2_SPI1, GPIO_FullRemap_SPI1
GPIO_PartialRemap1_USART1, GPIO_PartialRemap2_USART1, GPIO_FullRemap_USART1
GPIO_PartialRemap1_TIM1, GPIO_FullRemap_TIM1
GPIO_Remap_PIOC
GPIO_Remap_SWJ_Disable
```

## USART

```c
// Header: ch32x035_usart.h

void USART_DeInit(USART_TypeDef *USARTx);
void USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct);
void USART_StructInit(USART_InitTypeDef *USART_InitStruct);
void USART_ClockInit(USART_TypeDef *USARTx, USART_ClockInitTypeDef *USART_ClockInitStruct);
void USART_ClockStructInit(USART_ClockInitTypeDef *USART_ClockInitStruct);
void USART_Cmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState);
void USART_DMACmd(USART_TypeDef *USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
void USART_SetAddress(USART_TypeDef *USARTx, uint8_t USART_Address);
void USART_WakeUpConfig(USART_TypeDef *USARTx, uint16_t USART_WakeUp);
void USART_ReceiverWakeUpCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_LINBreakDetectLengthConfig(USART_TypeDef *USARTx, uint16_t USART_LINBreakDetectLength);
void USART_LINCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_SendData(USART_TypeDef *USARTx, uint16_t Data);
uint16_t USART_ReceiveData(USART_TypeDef *USARTx);
void USART_SendBreak(USART_TypeDef *USARTx);
void USART_SetGuardTime(USART_TypeDef *USARTx, uint8_t USART_GuardTime);
void USART_SetPrescaler(USART_TypeDef *USARTx, uint8_t USART_Prescaler);
void USART_SmartCardCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_SmartCardNACKCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_HalfDuplexCmd(USART_TypeDef *USARTx, FunctionalState NewState);
void USART_IrDAConfig(USART_TypeDef *USARTx, uint16_t USART_IrDAMode);
void USART_IrDACmd(USART_TypeDef *USARTx, FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);
void USART_ClearFlag(USART_TypeDef *USARTx, uint16_t USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT);
void USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT);
```

### USART Init Structure

```c
typedef struct {
    uint32_t USART_BaudRate;
    uint16_t USART_WordLength;      // USART_WordLength_8b, USART_WordLength_9b
    uint16_t USART_StopBits;        // USART_StopBits_1, USART_StopBits_2
    uint16_t USART_Parity;          // USART_Parity_No, USART_Parity_Even, USART_Parity_Odd
    uint16_t USART_Mode;            // USART_Mode_Tx, USART_Mode_Rx (OR together)
    uint16_t USART_HardwareFlowControl;  // USART_HardwareFlowControl_None, _RTS, _CTS, _RTS_CTS
} USART_InitTypeDef;
```

### USART Flags

```c
USART_FLAG_TXE    // Transmit data register empty
USART_FLAG_TC     // Transmission complete
USART_FLAG_RXNE   // Read data register not empty
USART_FLAG_IDLE   // Idle line detected
USART_FLAG_ORE    // Overrun error
USART_FLAG_NE     // Noise error
USART_FLAG_FE     // Framing error
USART_FLAG_PE     // Parity error
```

### USART Interrupts

```c
USART_IT_TXE      // TX empty interrupt
USART_IT_TC       // TX complete interrupt
USART_IT_RXNE     // RX not empty interrupt
USART_IT_IDLE     // Idle line interrupt
USART_IT_PE       // Parity error interrupt
USART_IT_ERR      // Error interrupt
```

## SPI

```c
// Header: ch32x035_spi.h

void SPI_I2S_DeInit(SPI_TypeDef *SPIx);
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct);
void SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void SPI_I2S_ITConfig(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef *SPIx, uint16_t SPI_NSSInternalSoft);
void SPI_SSOutputCmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void SPI_DataSizeConfig(SPI_TypeDef *SPIx, uint16_t SPI_DataSize);
void SPI_TransmitCRC(SPI_TypeDef *SPIx);
void SPI_CalculateCRC(SPI_TypeDef *SPIx, FunctionalState NewState);
uint16_t SPI_GetCRC(SPI_TypeDef *SPIx, uint8_t SPI_CRC);
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef *SPIx);
void SPI_BiDirectionalLineConfig(SPI_TypeDef *SPIx, uint16_t SPI_Direction);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT);
```

### SPI Init Structure

```c
typedef struct {
    uint16_t SPI_Direction;          // SPI_Direction_2Lines_FullDuplex, _RxOnly, _1Line_Rx, _1Line_Tx
    uint16_t SPI_Mode;               // SPI_Mode_Master, SPI_Mode_Slave
    uint16_t SPI_DataSize;           // SPI_DataSize_8b, SPI_DataSize_16b
    uint16_t SPI_CPOL;               // SPI_CPOL_Low, SPI_CPOL_High
    uint16_t SPI_CPHA;               // SPI_CPHA_1Edge, SPI_CPHA_2Edge
    uint16_t SPI_NSS;                // SPI_NSS_Soft, SPI_NSS_Hard
    uint16_t SPI_BaudRatePrescaler;  // SPI_BaudRatePrescaler_2 through _256
    uint16_t SPI_FirstBit;           // SPI_FirstBit_MSB, SPI_FirstBit_LSB
    uint16_t SPI_CRCPolynomial;
} SPI_InitTypeDef;
```

## ADC

```c
// Header: ch32x035_adc.h

void ADC_DeInit(ADC_TypeDef *ADCx);
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_ITConfig(ADC_TypeDef *ADCx, uint16_t ADC_IT, FunctionalState NewState);
void ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *ADCx);
void ADC_DiscModeChannelCountConfig(ADC_TypeDef *ADCx, uint8_t Number);
void ADC_DiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_ExternalTrigConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef *ADCx);
void ADC_AutoInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_InjectedDiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef *ADCx, uint32_t ADC_ExternalTrigInjecConv);
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState);
void ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef *ADCx, uint8_t Length);
void ADC_SetInjectedOffset(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset);
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel);
void ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog);
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold, uint16_t LowThreshold);
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef *ADCx, uint8_t ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef *ADCx, uint16_t ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint16_t ADC_IT);
void ADC_CLKConfig(ADC_TypeDef *ADCx, uint32_t ADC_CLK_Div_x);
void ADC_ResetCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx);
void ADC_StartCalibration(ADC_TypeDef *ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx);
```

## Timer (TIM)

```c
// Header: ch32x035_tim.h

void TIM_DeInit(TIM_TypeDef *TIMx);
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct);
void TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void TIM_PWMIConfig(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct);
void TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct);
void TIM_OCStructInit(TIM_OCInitTypeDef *TIM_OCInitStruct);
void TIM_ICStructInit(TIM_ICInitTypeDef *TIM_ICInitStruct);
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState);
void TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState);
void TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState);
void TIM_GenerateEvent(TIM_TypeDef *TIMx, uint16_t TIM_EventSource);
void TIM_DMAConfig(TIM_TypeDef *TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);
void TIM_DMACmd(TIM_TypeDef *TIMx, uint16_t TIM_DMASource, FunctionalState NewState);
void TIM_InternalClockConfig(TIM_TypeDef *TIMx);
void TIM_PrescalerConfig(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);
void TIM_CounterModeConfig(TIM_TypeDef *TIMx, uint16_t TIM_CounterMode);
void TIM_SelectInputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource);
void TIM_EncoderInterfaceConfig(TIM_TypeDef *TIMx, uint16_t TIM_EncoderMode, uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
void TIM_ARRPreloadConfig(TIM_TypeDef *TIMx, FunctionalState NewState);
void TIM_OC1PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload);
void TIM_OC1PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void TIM_OC2PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void TIM_OC3PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void TIM_OC4PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity);
void TIM_CCxCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx);
void TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter);
void TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload);
void TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2);
void TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3);
void TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4);
uint16_t TIM_GetCapture1(TIM_TypeDef *TIMx);
uint16_t TIM_GetCapture2(TIM_TypeDef *TIMx);
uint16_t TIM_GetCapture3(TIM_TypeDef *TIMx);
uint16_t TIM_GetCapture4(TIM_TypeDef *TIMx);
uint16_t TIM_GetCounter(TIM_TypeDef *TIMx);
uint16_t TIM_GetPrescaler(TIM_TypeDef *TIMx);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
void TIM_ClearFlag(TIM_TypeDef *TIMx, uint16_t TIM_FLAG);
ITStatus TIM_GetITStatus(TIM_TypeDef *TIMx, uint16_t TIM_IT);
void TIM_ClearITPendingBit(TIM_TypeDef *TIMx, uint16_t TIM_IT);
```

## Flash

```c
// Header: ch32x035_flash.h

void FLASH_SetLatency(uint32_t FLASH_Latency);
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_EraseOptionBytes(void);
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Pages);
FLASH_Status FLASH_EnableReadOutProtection(void);
FLASH_Status FLASH_UserOptionByteConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY, uint8_t OB_RST);
uint32_t FLASH_GetUserOptionByte(void);
uint32_t FLASH_GetWriteProtectionOptionByte(void);
FlagStatus FLASH_GetReadOutProtectionStatus(void);
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_BufReset(void);
void FLASH_BufLoad(uint32_t Address, uint32_t Data0);
void FLASH_ErasePage_Fast(uint32_t Page_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address);
void SystemReset_StartMode(uint32_t Mode);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

## Power (PWR)

```c
// Header: ch32x035_pwr.h

void PWR_DeInit(void);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_EnterSTOPMode(uint8_t PWR_STOPEntry);
void PWR_EnterSTANDBYMode(void);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
PWR_VDD PWR_VDD_SupplyVoltage(void);
```

### PWR Constants

```c
// PVD levels
PWR_PVDLevel_2V1, PWR_PVDLevel_2V3, PWR_PVDLevel_3V0, PWR_PVDLevel_4V0

// STOP mode entry
PWR_STOPEntry_WFI  // Wait For Interrupt
PWR_STOPEntry_WFE  // Wait For Event

// Flags
PWR_FLAG_PVDO      // PVD output flag
PWR_FLAG_FLASH     // Flash flag
```

## NVIC / Interrupt (misc.h)

```c
// Header: ch32x035_misc.h

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct);
```

### NVIC Priority Groups

```c
// When interrupt nesting enabled (INTSYSCR_INEST_EN):
NVIC_PriorityGroup_1  // 1 bit preemption, 3 bits sub-priority

// When interrupt nesting disabled (INTSYSCR_INEST_NoEN):
NVIC_PriorityGroup_0  // 0 bit preemption, 4 bits sub-priority
```

## PIOC (Port I/O Controller)

```c
// Header: PIOC_SFR.h
// PIOC is a programmable I/O controller with its own instruction set
// Access via memory-mapped registers at PIOC_BASE

// Key registers (direct SFR access):
PIOC->D8_INDIR_ADDR   // Indirect address register
PIOC->D8_TMR0_COUNT   // Timer count
PIOC->D8_TMR0_CTRL    // Timer control (mode, frequency, GP bits)
PIOC->D8_TMR0_INIT    // Timer initial value
PIOC->D8_PORT_DIR     // IO port direction
PIOC->D8_PORT_IO      // IO port input/output
PIOC->D8_SYS_CFG      // System config (clock enable, reset, IO enable)
PIOC->D8_CTRL_RD      // Master read / PIOC write data
PIOC->D8_CTRL_WR      // Master write / PIOC read data
PIOC->D8_DATA_REG0    // Data buffer 0
// ... through DATA_REG31

// Enable PIOC clock and configure
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_PIOC, ENABLE);

// Enable PIOC
PIOC->D8_SYS_CFG = RB_MST_CLK_GATE;  // Enable clock
PIOC->D8_SYS_CFG |= RB_MST_RESET;    // Release reset
PIOC->D8_SYS_CFG |= RB_MST_IO_EN0 | RB_MST_IO_EN1;  // Enable IO0, IO1
```
