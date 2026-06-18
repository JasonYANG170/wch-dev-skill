# Recipe: SPI Communication

## When to Use
User wants to implement SPI master/slave communication, full-duplex data transfer, or SPI with DMA.

## API Reference (from ch32v20x_spi.h)

### Init Structure
```c
typedef struct {
    uint16_t SPI_Direction;           // SPI_Direction_2Lines_FullDuplex, _RxOnly, _1Line_Rx, _1Line_Tx
    uint16_t SPI_Mode;                // SPI_Mode_Master, SPI_Mode_Slave
    uint16_t SPI_DataSize;            // SPI_DataSize_8b, SPI_DataSize_16b
    uint16_t SPI_CPOL;                // SPI_CPOL_Low, SPI_CPOL_High
    uint16_t SPI_CPHA;                // SPI_CPHA_1Edge, SPI_CPHA_2Edge
    uint16_t SPI_NSS;                 // SPI_NSS_Soft, SPI_NSS_Hard
    uint16_t SPI_BaudRatePrescaler;   // SPI_BaudRatePrescaler_2 .. _256
    uint16_t SPI_FirstBit;            // SPI_FirstBit_MSB, SPI_FirstBit_LSB
    uint16_t SPI_CRCPolynomial;       // CRC polynomial value
} SPI_InitTypeDef;
```

### Key Functions
```c
void       SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void       SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t   SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void       SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
```

### SPI Flags
| Flag | Description |
|------|-------------|
| `SPI_I2S_FLAG_TXE` | TX buffer empty |
| `SPI_I2S_FLAG_RXNE` | RX buffer not empty |
| `SPI_I2S_FLAG_BSY` | Busy flag |
| `SPI_I2S_FLAG_OVR` | Overrun error |

## Example: SPI1 Master Full-Duplex

```c
#include "ch32v20x.h"

void SPI1_Init_Master(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // SCK - PA5 (AF push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // MOSI - PA7 (AF push-pull)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // MISO - PA6 (floating input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // NSS - PA4 (software controlled, GPIO output)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);  // Deselect slave

    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_TransferByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

// Usage with chip select
uint8_t SPI1_ReadRegister(uint8_t reg)
{
    uint8_t result;
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);   // CS low
    SPI1_TransferByte(reg);               // Send register address
    result = SPI1_TransferByte(0xFF);     // Read response
    GPIO_SetBits(GPIOA, GPIO_Pin_4);     // CS high
    return result;
}
```

## Example: SPI1 TX with DMA

```c
void SPI1_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA1 Channel 3 = SPI1 TX
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)tx_buffer;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = buffer_size;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
}
```

## SPI Instance Pins

| SPI | SCK | MOSI | MISO | NSS | APB |
|-----|-----|------|------|-----|-----|
| SPI1 | PA5 | PA7 | PA6 | PA4 | APB2 |
| SPI2 | PB13 | PB15 | PB14 | PB12 | APB1 |

## Pitfalls
- **SPI1 on APB2, SPI2 on APB1** -- different clock speeds
- **Must check TXE before sending** -- otherwise data overwritten
- **Must read RXNE to clear overrun** -- even if you don't need the data
- **NSS must be managed manually** when using `SPI_NSS_Soft` mode
