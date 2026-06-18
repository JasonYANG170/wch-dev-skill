# I2C Communication

## Overview

CH32H417 has 2 I2C interfaces (I2C1, I2C2) supporting standard mode (100kHz), fast mode (400kHz), and SMBus. The I3C peripheral provides MIPI I3C bus support separately.

## Key API Functions

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
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DutyCycle);
```

## I2C Master Transmit Example

```c
#include "ch32h417.h"

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    I2C_InitTypeDef I2C_InitStruct = {0};

    // Enable clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // PB6 = I2C1_SCL, PB7 = I2C1_SDA (AF4)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF4);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF4);

    // I2C configuration
    I2C_InitStruct.I2C_ClockSpeed = 100000;  // 100kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

uint8_t I2C1_WriteBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    // Wait until I2C is not busy
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    // Generate START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send device address (write)
    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send register address
    I2C_SendData(I2C1, reg_addr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send data
    for(uint16_t i = 0; i < len; i++)
    {
        I2C_SendData(I2C1, data[i]);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    // Generate STOP
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

uint8_t I2C1_ReadBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    // Write register address first
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg_addr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Repeated START for read
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, dev_addr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    for(uint16_t i = 0; i < len; i++)
    {
        if(i == len - 1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
        data[i] = I2C_ReceiveData(I2C1);
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return 0;
}
```

## Key Events

| Event | Description |
|-------|-------------|
| `I2C_EVENT_MASTER_MODE_SELECT` | START condition sent |
| `I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED` | Address sent, ACK received (TX) |
| `I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED` | Address sent, ACK received (RX) |
| `I2C_EVENT_MASTER_BYTE_TRANSMITTED` | Byte transmitted, ACK received |
| `I2C_EVENT_MASTER_BYTE_RECEIVED` | Byte received |

## Fast Mode Configuration

```c
// 400kHz fast mode with duty cycle 2
I2C_InitStruct.I2C_ClockSpeed = 400000;
I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
```

## I3C Support

For MIPI I3C bus (higher speed, in-band interrupts), use the I3C peripheral:

```c
#include "ch32h417_i3c.h"

// I3C provides:
// - I2C backward compatibility
// - Push-pull mode for higher speeds
// - In-band interrupt (IBI) support
// - Hot-join capability
// - Dynamic address assignment
```
