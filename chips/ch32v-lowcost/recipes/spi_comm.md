# Recipe: SPI Communication

## Scenario
Configure SPI for master/slave communication -- full-duplex, half-duplex, and DMA modes.

## API Quick Reference

### SPI Init Structure
```c
typedef struct
{
    uint16_t SPI_Direction;           // SPI_Direction_2Lines_FullDuplex, etc.
    uint16_t SPI_Mode;                // SPI_Mode_Master, SPI_Mode_Slave
    uint16_t SPI_DataSize;            // SPI_DataSize_8b, SPI_DataSize_16b
    uint16_t SPI_CPOL;                // SPI_CPOL_Low, SPI_CPOL_High
    uint16_t SPI_CPHA;                // SPI_CPHA_1Edge, SPI_CPHA_2Edge
    uint16_t SPI_NSS;                 // SPI_NSS_Soft, SPI_NSS_Hard
    uint16_t SPI_BaudRatePrescaler;   // SPI_BaudRatePrescaler_2..256
    uint16_t SPI_FirstBit;            // SPI_FirstBit_MSB, SPI_FirstBit_LSB
    uint16_t SPI_CRCPolynomial;
} SPI_InitTypeDef;
```

### SPI Modes (CPOL/CPHA)
| Mode | CPOL | CPHA | Description |
|------|------|------|-------------|
| Mode 0 | Low | 1Edge | Most common, SPI Flash default |
| Mode 1 | Low | 2Edge | |
| Mode 2 | High | 1Edge | |
| Mode 3 | High | 2Edge | Common for sensors |

### Key Functions
```c
void       SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void       SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState);
void       SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data);
uint16_t   SPI_I2S_ReceiveData(SPI_TypeDef *SPIx);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG);
void       SPI_NSSInternalSoftwareConfig(SPI_TypeDef *SPIx, uint16_t SPI_NSSInternalSoft);
void       SPI_DataSizeConfig(SPI_TypeDef *SPIx, uint16_t SPI_DataSize);
void       SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
```

### Common Flags
```c
SPI_I2S_FLAG_TXE   // TX buffer empty
SPI_I2S_FLAG_RXNE  // RX buffer not empty
SPI_I2S_FLAG_BSY   // SPI busy
SPI_I2S_FLAG_OVR   // Overrun error
```

### Default Pins (CH32V003)
| Signal | Pin |
|--------|-----|
| SPI1_SCK | PC5 |
| SPI1_MOSI | PC6 |
| SPI1_MISO | PC7 |
| SPI1_NSS | PC4 |

## Complete Call Chains

### Master Full-Duplex
```c
void SPI1_Master_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);

    // SCK - AF push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MOSI - AF push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MISO - floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SPI config
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

// Transfer one byte (simultaneous TX/RX)
uint8_t SPI1_TransferByte(uint8_t tx_data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, tx_data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}
```

### Slave Full-Duplex
```c
void SPI1_Slave_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);

    // SCK - floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MOSI - floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // MISO - AF push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SPI config
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  // Must be High for slave TX
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // Ignored in slave
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}
```

### Software NSS (Chip Select)
```c
// Assert CS (low)
SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Reset);
// or manually:
GPIO_ResetBits(GPIOC, GPIO_Pin_4);

// Transfer data
uint8_t rx = SPI1_TransferByte(tx);

// Deassert CS (high)
SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
// or manually:
GPIO_SetBits(GPIOC, GPIO_Pin_4);
```

## Common Errors

1. **Slave CPOL must be High** -- When using SPI slave mode to send data, CPOL must be set to 1
2. **MOSI/MISO swapped** -- Master MOSI connects to Slave MOSI (both are PC6)
3. **Baud rate too fast** -- Start with `SPI_BaudRatePrescaler_256` and increase
4. **B flag check wrong** -- Wait for `TXE` before sending, wait for `RXNE` after sending
