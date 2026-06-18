# Recipe: I2S Audio Interface

## Overview

Configure the I2S (Inter-IC Sound) peripheral on CH32F20x for digital audio data transfer. I2S is built on top of the SPI peripheral (SPI2/SPI3) and supports master/slave modes with 16-bit or 32-bit data formats.

**Availability**: CH32F20x only (not available on CH32F10x or CH32M030).

## Key API Functions

```c
// I2S Initialization (uses SPI peripheral)
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

// SPI/I2S shared functions
void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState);
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
```

## I2S Init Structure

```c
typedef struct {
    uint16_t I2S_Mode;          // I2S_Mode_SlaveTx, I2S_Mode_SlaveRx,
                                // I2S_Mode_MasterTx, I2S_Mode_MasterRx
    uint16_t I2S_Standard;      // I2S_Standard_Phillips, I2S_Standard_MSB,
                                // I2S_Standard_LSB, I2S_Standard_PCMShort,
                                // I2S_Standard_PCMLong
    uint16_t I2S_DataFormat;    // I2S_DataFormat_16b, I2S_DataFormat_16bextended,
                                // I2S_DataFormat_24b, I2S_DataFormat_32b
    uint16_t I2S_MCLKOutput;    // I2S_MCLKOutput_Enable, I2S_MCLKOutput_Disable
    uint32_t I2S_AudioFreq;     // I2S_AudioFreq_8k, _16k, _22k, _44k, _48k, _96k, _192k,
                                // or any uint32_t value
    uint16_t I2S_CPOL;          // I2S_CPOL_Low, I2S_CPOL_High
} I2S_InitTypeDef;
```

## I2S Available Instances

| Instance | SPI Peripheral | WS Pin | CK Pin | SD Pin | MCK Pin |
|----------|---------------|--------|--------|--------|---------|
| I2S2 | SPI2 | PB12 | PB13 | PB15 | PC6 |
| I2S3 | SPI3 | PA15 | PB3 | PB5 | PC7 |

## I2S Interrupt Flags

| Flag | Description |
|------|-------------|
| SPI_I2S_IT_TXE | Transmit buffer empty |
| SPI_I2S_IT_RXNE | Receive buffer not empty |
| SPI_I2S_IT_ERR | Error interrupt |

## Example: I2S Master Transmit (Interrupt)

```c
#include "debug.h"

#define DATA_LEN  10
u32 I2S2_Tx[DATA_LEN];

void I2S2_MasterTx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    // Configure I2S2 pins: WS(PB12), CK(PB13), SD(PB15) - AF push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MCK(PC6) - AF push-pull (if MCLK output enabled)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // I2S configuration
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);

    // Enable TXE interrupt
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);

    I2S_Cmd(SPI2, ENABLE);
}

volatile u8 tx_index = 0;

void SPI2_IRQHandler(void)
{
    if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET) {
        SPI_I2S_SendData(SPI2, I2S2_Tx[tx_index++]);
    }
}

int main(void)
{
    u32 i;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    printf("I2S Master Tx Test\r\n");

    // Fill test data
    for(i = 0; i < DATA_LEN; i++) {
        I2S2_Tx[i] = 0x5AA1 + i;
    }

    SPI_I2S_DeInit(SPI2);
    I2S2_MasterTx_Init();

    while(1);
}
```

## Example: I2S Slave Receive (Interrupt)

```c
#define DATA_LEN  10
u32 I2S3_Rx[DATA_LEN];
volatile u8 rx_index = 0;
volatile u8 rx_complete = 0;

void I2S3_SlaveRx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // Slave RX pins: WS(PA15), CK(PB3), SD(PB5) - floating input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI3, &I2S_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_RXNE);
    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);

    I2S_Cmd(SPI3, ENABLE);
}

void SPI3_IRQHandler(void)
{
    if(SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET) {
        I2S3_Rx[rx_index++] = SPI_I2S_ReceiveData(SPI3);
        if(rx_index >= DATA_LEN) {
            rx_complete = 1;
            SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
        }
    }
}
```

## Example: 32-bit Data Mode

For data formats larger than 16 bits, the I2S peripheral requires two 16-bit transfers per sample:

```c
// Transmit 32-bit data
void SPI2_IRQHandler_32bit(void)
{
    static u8 flag = 0;

    if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET) {
        if(flag == 0) {
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[tx_index] >> 16));  // High word first
            flag = 1;
        } else {
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[tx_index++]));     // Low word
            flag = 0;
        }
    }
}

// Receive 32-bit data
void SPI3_IRQHandler_32bit(void)
{
    static u8 flag = 0;

    if(SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET) {
        if(flag == 0) {
            I2S3_Rx[rx_index] = SPI_I2S_ReceiveData(SPI3);  // High word
            flag = 1;
        } else {
            I2S3_Rx[rx_index++] |= (u32)SPI_I2S_ReceiveData(SPI3) << 16;  // Low word
            flag = 0;
        }
    }
}
```

## Pitfalls

- **I2S uses SPI peripheral**: I2S2 is on SPI2, I2S3 is on SPI3. You cannot use both SPI and I2S on the same peripheral simultaneously.
- **Clock domain**: SPI2/SPI3 are on APB1. Enable with `RCC_APB1PeriphClockCmd()`.
- **GPIO remap**: I2S3 pins (PA15, PB3, PB5) may need JTAG remap. Use `GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE)` to free PA15/PB3/PB5.
- **32-bit mode**: Data values > 16 bits require two SPI_I2S_SendData() calls per sample (high word first, then low word).
- **Slave input mode**: In slave receive mode, GPIO pins must be configured as `GPIO_Mode_IN_FLOATING`, not AF_PP.
- **MCLK output**: Enable `I2S_MCLKOutput_Enable` only if your audio codec requires a master clock. This output is on PC6 (I2S2) or PC7 (I2S3).
- **Audio frequency precision**: The actual audio frequency depends on the I2S clock source. Use `I2S_AudioFreq_48k` etc. for standard rates.

## Related Examples

- `CH32F20xEVT/EVT/EXAM/I2S/HostRx_SlaveTx` - Master TX / Slave RX pair
- `CH32F20xEVT/EVT/EXAM/I2S/I2S_Interupt` - Interrupt-driven I2S
- `CH32F20xEVT/EVT/EXAM/I2S/I2S_DMA` - DMA-driven I2S
