# CH32F Peripheral API Reference

## GPIO (ch32f10x_gpio.h / ch32f20x_gpio.h)

### Types
```c
typedef enum { GPIO_Speed_10MHz = 1, GPIO_Speed_2MHz, GPIO_Speed_50MHz } GPIOSpeed_TypeDef;
typedef enum { GPIO_Mode_AIN = 0x0, GPIO_Mode_IN_FLOATING = 0x04, GPIO_Mode_IPD = 0x28,
               GPIO_Mode_IPU = 0x48, GPIO_Mode_Out_OD = 0x14, GPIO_Mode_Out_PP = 0x10,
               GPIO_Mode_AF_OD = 0x1C, GPIO_Mode_AF_PP = 0x18 } GPIOMode_TypeDef;
typedef struct { uint16_t GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
typedef enum { Bit_RESET = 0, Bit_SET } BitAction;
```

### Functions
```c
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_AFIODeInit(void);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_EventOutputCmd(FunctionalState NewState);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

---

## USART (ch32f10x_usart.h / ch32f20x_usart.h)

### Types
```c
typedef struct {
    uint32_t USART_BaudRate;
    uint16_t USART_WordLength;    // USART_WordLength_8b, USART_WordLength_9b
    uint16_t USART_StopBits;      // USART_StopBits_1, _0_5, _2, _1_5
    uint16_t USART_Parity;        // USART_Parity_No, _Even, _Odd
    uint16_t USART_Mode;          // USART_Mode_Tx | USART_Mode_Rx
    uint16_t USART_HardwareFlowControl; // _None, _RTS, _CTS, _RTS_CTS
} USART_InitTypeDef;
```

### Functions
```c
void USART_DeInit(USART_TypeDef* USARTx);
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
void USART_StructInit(USART_InitTypeDef* USART_InitStruct);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
```

---

## SPI (ch32f10x_spi.h / ch32f20x_spi.h)

### Types
```c
typedef struct {
    uint16_t SPI_Direction;           // SPI_Direction_2Lines_FullDuplex, _2Lines_RxOnly, _1Line_Rx, _1Line_Tx
    uint16_t SPI_Mode;                // SPI_Mode_Master, SPI_Mode_Slave
    uint16_t SPI_DataSize;            // SPI_DataSize_8b, SPI_DataSize_16b
    uint16_t SPI_CPOL;                // SPI_CPOL_Low, SPI_CPOL_High
    uint16_t SPI_CPHA;                // SPI_CPHA_1Edge, SPI_CPHA_2Edge
    uint16_t SPI_NSS;                 // SPI_NSS_Soft, SPI_NSS_Hard
    uint16_t SPI_BaudRatePrescaler;   // SPI_BaudRatePrescaler_2 to _256
    uint16_t SPI_FirstBit;            // SPI_FirstBit_MSB, SPI_FirstBit_LSB
    uint16_t SPI_CRCPolynomial;
} SPI_InitTypeDef;
```

### Functions
```c
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
```

---

## I2C (ch32f10x_i2c.h / ch32f20x_i2c.h)

### Types
```c
typedef struct {
    uint32_t I2C_ClockSpeed;          // Max 400kHz
    uint16_t I2C_Mode;                // I2C_Mode_I2C, _SMBusDevice, _SMBusHost
    uint16_t I2C_DutyCycle;           // I2C_DutyCycle_2, _16_9
    uint16_t I2C_OwnAddress1;
    uint16_t I2C_Ack;                 // I2C_Ack_Enable, _Disable
    uint16_t I2C_AcknowledgedAddress; // I2C_AcknowledgedAddress_7bit, _10bit
} I2C_InitTypeDef;
```

### Functions
```c
void I2C_DeInit(I2C_TypeDef* I2Cx);
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
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
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
```

---

## ADC (ch32f10x_adc.h / ch32f20x_adc.h)

### Types
```c
typedef struct {
    uint32_t ADC_Mode;                      // ADC_Mode_Independent
    FunctionalState ADC_ScanConvMode;
    FunctionalState ADC_ContinuousConvMode;
    uint32_t ADC_ExternalTrigConv;          // ADC_ExternalTrigConv_None
    uint32_t ADC_DataAlign;                 // ADC_DataAlign_Right, _Left
    uint8_t ADC_NbrOfChannel;
} ADC_InitTypeDef;
```

### Functions
```c
void ADC_DeInit(ADC_TypeDef* ADCx);
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState);
void ADC_ResetCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx);
void ADC_StartCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx);
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
void ADC_TempSensorVrefintCmd(FunctionalState NewState);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT);
s32 TempSensor_Volt_To_Temper(s32 Value);
int16_t Get_CalibrationValue(ADC_TypeDef* ADCx);
```

---

## TIM (ch32f10x_tim.h / ch32f20x_tim.h)

### Types
```c
typedef struct {
    uint16_t TIM_Prescaler;
    uint16_t TIM_CounterMode;       // TIM_CounterMode_Up, _Down, _CenterAligned1/2/3
    uint16_t TIM_Period;
    uint16_t TIM_ClockDivision;     // TIM_CKD_DIV1, _DIV2, _DIV4
    uint8_t TIM_RepetitionCounter;
} TIM_TimeBaseInitTypeDef;

typedef struct {
    uint16_t TIM_OCMode;            // TIM_OCMode_Timing, _Active, _Inactive, _Toggle, _PWM1, _PWM2
    uint16_t TIM_OutputState;       // TIM_OutputState_Enable, _Disable
    uint16_t TIM_OutputNState;
    uint16_t TIM_Pulse;
    uint16_t TIM_OCPolarity;        // TIM_OCPolarity_High, _Low
    uint16_t TIM_OCNPolarity;
    uint16_t TIM_OCIdleState;
    uint16_t TIM_OCNIdleState;
} TIM_OCInitTypeDef;
```

### Functions
```c
void TIM_DeInit(TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4);
uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);
void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter);
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload);
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);
```

---

## CAN (ch32f10x_can.h / ch32f20x_can.h)

### Types
```c
typedef struct {
    uint16_t CAN_Prescaler;
    uint8_t CAN_Mode;               // CAN_Mode_Normal, _LoopBack, _Silent, _Silent_LoopBack
    uint8_t CAN_SJW;                // CAN_SJW_1tq to _4tq
    uint8_t CAN_BS1;                // CAN_BS1_1tq to _16tq
    uint8_t CAN_BS2;                // CAN_BS2_1tq to _8tq
    FunctionalState CAN_TTCM, CAN_ABOM, CAN_AWUM, CAN_NART, CAN_RFLM, CAN_TXFP;
} CAN_InitTypeDef;

typedef struct {
    uint32_t StdId, ExtId;
    uint8_t IDE, RTR, DLC, Data[8];
} CanTxMsg;

typedef struct {
    uint32_t StdId, ExtId;
    uint8_t IDE, RTR, DLC, Data[8], FMI;
} CanRxMsg;
```

### Functions
```c
void CAN_DeInit(CAN_TypeDef* CANx);
uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct);
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct);
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage);
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox);
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage);
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber);
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber);
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState);
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG);
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG);
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT);
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT);
```

---

## DAC (ch32f10x_dac.h / ch32f20x_dac.h)

### Types
```c
typedef struct {
    uint32_t DAC_Trigger;                      // DAC_Trigger_None, _T6_TRGO, _Software, etc.
    uint32_t DAC_WaveGeneration;               // DAC_WaveGeneration_None, _Noise, _Triangle
    uint32_t DAC_LFSRUnmask_TriangleAmplitude;
    uint32_t DAC_OutputBuffer;                 // DAC_OutputBuffer_Enable, _Disable
} DAC_InitTypeDef;
```

### Functions
```c
void DAC_DeInit(void);
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct);
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState);
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);
```

---

## DMA (ch32f10x_dma.h / ch32f20x_dma.h)

### Types
```c
typedef struct {
    uint32_t DMA_PeripheralBaseAddr;
    uint32_t DMA_MemoryBaseAddr;
    uint32_t DMA_DIR;                // DMA_DIR_PeripheralSRC, _PeripheralDST
    uint32_t DMA_BufferSize;
    uint32_t DMA_PeripheralInc;      // DMA_PeripheralInc_Enable, _Disable
    uint32_t DMA_MemoryInc;          // DMA_MemoryInc_Enable, _Disable
    uint32_t DMA_PeripheralDataSize; // _Byte, _HalfWord, _Word
    uint32_t DMA_MemoryDataSize;     // _Byte, _HalfWord, _Word
    uint32_t DMA_Mode;               // DMA_Mode_Normal, _Circular
    uint32_t DMA_Priority;           // _Low, _Medium, _High, _VeryHigh
    uint32_t DMA_M2M;                // DMA_M2M_Enable, _Disable
} DMA_InitTypeDef;
```

### Functions
```c
void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx);
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber);
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void DMA_ClearFlag(uint32_t DMAy_FLAG);
ITStatus DMA_GetITStatus(uint32_t DMAy_IT);
void DMA_ClearITPendingBit(uint32_t DMAy_IT);
```

---

## Flash (ch32f10x_flash.h / ch32f20x_flash.h)

### Types
```c
typedef enum {
    FLASH_BUSY = 1, FLASH_ERROR_PG, FLASH_ERROR_WRP, FLASH_COMPLETE,
    FLASH_TIMEOUT, FLASH_OP_RANGE_ERROR = 0xFD, FLASH_ALIGN_ERROR = 0xFE, FLASH_ADR_RANGE_ERROR = 0xFF
} FLASH_Status;
```

### Functions
```c
void FLASH_SetLatency(uint32_t FLASH_Latency);
void FLASH_PrefetchBufferCmd(uint32_t FLASH_PrefetchBuffer);
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
// Fast mode (CH32F10x)
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_BufReset(void);
void FLASH_BufLoad(uint32_t Address, uint32_t Data0, uint32_t Data1, uint32_t Data2, uint32_t Data3);
void FLASH_ErasePage_Fast(uint32_t Page_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address);
// ROM operations (CH32F20x)
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

---

## PWR (ch32f10x_pwr.h / ch32f20x_pwr.h)

### Functions
```c
void PWR_DeInit(void);
void PWR_BackupAccessCmd(FunctionalState NewState);
void PWR_PVDCmd(FunctionalState NewState);
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel);
void PWR_WakeUpPinCmd(FunctionalState NewState);
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterSTANDBYMode(void);
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG);
void PWR_ClearFlag(uint32_t PWR_FLAG);
PWR_VDD PWR_VDD_SupplyVoltage(void);
```

---

## RCC (ch32f10x_rcc.h / ch32f20x_rcc.h)

### Key Functions
```c
void RCC_DeInit(void);
void RCC_HSEConfig(uint32_t RCC_HSE);
void RCC_HSICmd(FunctionalState NewState);
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul);
void RCC_PLLCmd(FunctionalState NewState);
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t RCC_GetSYSCLKSource(void);
void RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void RCC_PCLK1Config(uint32_t RCC_HCLK);
void RCC_PCLK2Config(uint32_t RCC_HCLK);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);
```
