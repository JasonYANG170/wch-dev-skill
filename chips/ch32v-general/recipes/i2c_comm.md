# Recipe: I2C Communication

## When to Use
User wants to implement I2C master/slave communication, read/write EEPROM, or interface I2C sensors.

## API Reference (from ch32v20x_i2c.h)

### Init Structure
```c
typedef struct {
    uint32_t I2C_ClockSpeed;          // Clock speed in Hz (max 400kHz)
    uint16_t I2C_Mode;                // I2C_Mode_I2C, I2C_Mode_SMBusDevice, I2C_Mode_SMBusHost
    uint16_t I2C_DutyCycle;           // I2C_DutyCycle_2, I2C_DutyCycle_16_9
    uint16_t I2C_OwnAddress1;         // Device own address (7-bit or 10-bit)
    uint16_t I2C_Ack;                 // I2C_Ack_Enable, I2C_Ack_Disable
    uint16_t I2C_AcknowledgedAddress; // I2C_AcknowledgedAddress_7bit, _10bit
} I2C_InitTypeDef;
```

### Key Functions
```c
void       I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);
void       I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction);
void       I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t    I2C_ReceiveData(I2C_TypeDef *I2Cx);
void       I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState);
ErrorStatus I2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
void       I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
```

### I2C Master Events
| Event | Description |
|-------|-------------|
| `I2C_EVENT_MASTER_MODE_SELECT` | START condition sent, EV5 |
| `I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED` | Address+Write ACKed, EV6 |
| `I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED` | Address+Read ACKed, EV6 |
| `I2C_EVENT_MASTER_BYTE_TRANSMITTED` | Byte transmitted, EV8_2 |
| `I2C_EVENT_MASTER_BYTE_RECEIVED` | Byte received, EV7 |

### I2C Direction
| Constant | Value | Description |
|----------|-------|-------------|
| `I2C_Direction_Transmitter` | 0x00 | Write to slave |
| `I2C_Direction_Receiver` | 0x01 | Read from slave |

## Example: I2C1 Master Read EEPROM

```c
#include "ch32v20x.h"

#define EEPROM_ADDR  0xA0  // 7-bit address 0x50, left-shifted for R/W bit

void I2C1_Init_Master(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // SCL - PB6 (AF open-drain)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // SDA - PB7 (AF open-drain)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_InitStruct.I2C_ClockSpeed = 100000;  // 100kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

void I2C1_EEPROM_WriteByte(uint16_t addr, uint8_t data)
{
    // Wait for bus free
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    // Generate START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send device address + Write
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send memory address high byte
    I2C_SendData(I2C1, (uint8_t)(addr >> 8));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send memory address low byte
    I2C_SendData(I2C1, (uint8_t)(addr & 0xFF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send data
    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Generate STOP
    I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t I2C1_EEPROM_ReadByte(uint16_t addr)
{
    uint8_t data;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    // Phase 1: Set memory address (write)
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, (uint8_t)(addr >> 8));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, (uint8_t)(addr & 0xFF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Phase 2: Read data
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return data;
}
```

## I2C Instance Pins

| I2C | SCL | SDA | APB |
|-----|-----|-----|-----|
| I2C1 | PB6 | PB7 | APB1 |
| I2C2 | PB10 | PB11 | APB1 |

## Pitfalls
- **GPIO must be AF_OD (open-drain)** -- I2C requires open-drain with external pull-ups
- **Always check events** -- don't send data before previous transfer completes
- **I2C address is 7-bit** -- the library handles the R/W bit; pass `0x50` not `0xA0` for typical EEPROM
- **STOP condition after read** -- must disable ACK before reading last byte, then generate STOP
- **Bus busy flag** -- check before generating START to avoid bus collision
