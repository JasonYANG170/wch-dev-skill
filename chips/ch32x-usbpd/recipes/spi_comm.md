# SPI Communication

> **Summary**: Configure SPI for master/slave communication on CH32X/CH6xx chips.

## Trigger Intent

- "SPI communication"
- "SPI master"
- "SPI slave"
- "SPI DMA transfer"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Clock | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)` |
| Reference | `CH32X035EVT/EVT/EXAM/SPI/` |

## Step-by-Step

### SPI1 Master Mode

```c
#include "ch32x035.h"
#include "debug.h"

void SPI1_Master_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    SPI_InitTypeDef SPI_InitStruct = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA5 = SCK (AF_PP), PA7 = MOSI (AF_PP), PA6 = MISO (floating input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA4 = NSS (software control, push-pull output)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);  // NSS high (deselect)

    // SPI configuration
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_TransferByte(uint8_t data) {
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    SPI1_Master_Init();
    printf("SPI1 Master ready\r\n");

    while(1) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);   // Select slave
        uint8_t rx = SPI1_TransferByte(0xA5);
        GPIO_SetBits(GPIOA, GPIO_Pin_4);     // Deselect slave
        printf("Sent:0xA5, Received:0x%02X\r\n", rx);
        Delay_Ms(500);
    }
}
```

### SPI1 Slave Mode

```c
void SPI1_Slave_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    SPI_InitTypeDef SPI_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA5 = SCK (input), PA6 = MISO (AF_PP), PA7 = MOSI (input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA4 = NSS (hardware input)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;  // Hardware NSS
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}
```

## SPI Baud Rate Prescaler Reference

| Prescaler | Constant | SYSCLK/Prescaler @ 48MHz |
|-----------|----------|--------------------------|
| 2 | `SPI_BaudRatePrescaler_2` | 24 MHz |
| 4 | `SPI_BaudRatePrescaler_4` | 12 MHz |
| 8 | `SPI_BaudRatePrescaler_8` | 6 MHz |
| 16 | `SPI_BaudRatePrescaler_16` | 3 MHz |
| 32 | `SPI_BaudRatePrescaler_32` | 1.5 MHz |
| 64 | `SPI_BaudRatePrescaler_64` | 750 kHz |
| 128 | `SPI_BaudRatePrescaler_128` | 375 kHz |
| 256 | `SPI_BaudRatePrescaler_256` | 187.5 kHz |

## Common Errors

- Using `SPI_FirstBit_LSB` in slave mode -- not supported
- Slave CPOL should be set to 1 when sending data
- Forgetting to toggle NSS (software) or configure hardware NSS
