# Recipe: I2S Audio

## When to Use
User wants to implement I2S (Inter-IC Sound) audio data transfer, master/slave I2S communication, or I2S with DMA for continuous audio streaming.

## Availability
- CH32V307: I2S on SPI2 (PB12-PB15, PC6) and SPI3 (PA15, PB3-PB5, PC7)
- CH32V407: I2S on SPI2 (PB12-PB15, PC6) and SPI3 (PA15, PB3-PB5, PC7)
- CH32V103 / CH32V20x: No I2S peripheral

## API Reference (from ch32v*_spi.h)

### I2S Init Structure
```c
typedef struct {
    uint16_t I2S_Mode;        // I2S_Mode_SlaveTx, _SlaveRx, _MasterTx, _MasterRx
    uint16_t I2S_Standard;    // I2S_Standard_Phillips, _MSB, _LSB, _PCMShort, _PCMLong
    uint16_t I2S_DataFormat;  // I2S_DataFormat_16b, _16bextended, _24b, _32b
    uint16_t I2S_MCLKOutput;  // I2S_MCLKOutput_Enable/Disable
    uint16_t I2S_AudioFreq;   // I2S_AudioFreq_8k..192k, _Default
    uint16_t I2S_CPOL;        // I2S_CPOL_Low/High
} I2S_InitTypeDef;
```

### Key Functions
```c
void SPI_I2S_DeInit(SPI_TypeDef *SPIx);
void I2S_Init(SPI_TypeDef *SPIx, I2S_InitTypeDef *I2S_InitStruct);
void I2S_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void I2S_SlaveSyncWS(SPI_TypeDef *SPIx, FunctionalState NewState);  // Sync slave WS to master
```

### I2S Mode Constants
| Constant | Description |
|----------|-------------|
| `I2S_Mode_SlaveTx` | Slave transmit |
| `I2S_Mode_SlaveRx` | Slave receive |
| `I2S_Mode_MasterTx` | Master transmit |
| `I2S_Mode_MasterRx` | Master receive |

### I2S Standard Constants
| Constant | Description |
|----------|-------------|
| `I2S_Standard_Phillips` | Philips I2S standard |
| `I2S_Standard_MSB` | MSB-aligned standard |
| `I2S_Standard_LSB` | LSB-aligned standard |
| `I2S_Standard_PCMShort` | PCM short frame |
| `I2S_Standard_PCMLong` | PCM long frame |

### I2S Audio Frequency
| Constant | Frequency |
|----------|-----------|
| `I2S_AudioFreq_8k` | 8 kHz |
| `I2S_AudioFreq_16k` | 16 kHz |
| `I2S_AudioFreq_22k` | 22.05 kHz |
| `I2S_AudioFreq_44k` | 44.1 kHz |
| `I2S_AudioFreq_48k` | 48 kHz |
| `I2S_AudioFreq_96k` | 96 kHz |
| `I2S_AudioFreq_192k` | 192 kHz |

### I2S DMA Request
| Constant | Description |
|----------|-------------|
| `SPI_I2S_DMAReq_Tx` | TX DMA request |
| `SPI_I2S_DMAReq_Rx` | RX DMA request |

### I2S Pin Mapping
| Signal | SPI2 (I2S2) | SPI3 (I2S3) |
|--------|-------------|-------------|
| WS | PB12 | PA15 |
| CK | PB13 | PB3 |
| SD | PB15 (TX) / PB14 (RX) | PB5 (TX) / PB4 (RX) |
| MCK | PC6 | PC7 |

## Example: I2S Master TX with DMA

```c
#include "ch32v4x7.h"

#define I2S_BUF_LEN  256
s16 i2s_tx_buf[I2S_BUF_LEN];

void I2S2_MasterTx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2S_InitTypeDef I2S_InitStructure;

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_SPI2, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC, ENABLE);

    // WS - PB12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CK - PB13
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD (TX) - PB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MCK - PC6 (optional, for external codec)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);
}

void I2S2_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);  // SPI2 TX = DMA1 Channel 5
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)i2s_tx_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = I2S_BUF_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
}

void I2S2_Start(void)
{
    I2S_Cmd(SPI2, ENABLE);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
}
```

## Example: I2S Master-Slave with DMA

```c
// Master TX (SPI2) -> Slave RX (SPI3), synchronized WS
void I2S_MasterSlave_Init(void)
{
    // Init master (SPI2) as MasterTx
    I2S2_MasterTx_Init();

    // Init slave (SPI3) as SlaveRx
    I2S_InitTypeDef I2S_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_SPI3, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB, ENABLE);

    // WS - PA15 (slave, input with pull-up)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CK - PB3 (slave, input with pull-up)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD (RX) - PB5 (slave, input with pull-up)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI3, &I2S_InitStructure);

    // Synchronize slave WS to master
    I2S_SlaveSyncWS(SPI2, ENABLE);
}
```

## Pitfalls
- **I2S shares SPI peripherals** -- I2S2 uses SPI2, I2S3 uses SPI3; cannot use both SPI and I2S on the same peripheral
- **DeInit before reconfigure** -- Call `SPI_I2S_DeInit()` before switching between SPI and I2S mode
- **Slave pins are inputs** -- Slave WS/CK/SD must be configured as `GPIO_Mode_IPU` (input pull-up), not AF_PP
- **DMA channel mapping** -- SPI2 TX = DMA1_Channel5, SPI2 RX = DMA1_Channel4; SPI3 TX = DMA2_Channel2, SPI3 RX = DMA2_Channel1
- **16-bit data** -- I2S data is always 16-bit half-word; use `DMA_PeripheralDataSize_HalfWord`
- **MCK output** -- Enable `I2S_MCLKOutput_Enable` only if external codec needs master clock
