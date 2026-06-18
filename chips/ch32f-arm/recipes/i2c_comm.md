# Recipe: I2C Communication

## Overview

Configure I2C for master/slave communication on CH32F10x/CH32F20x.

## Key API Functions

```c
// Initialization
void I2C_DeInit(I2C_TypeDef* I2Cx);
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

// Master operations
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);

// State monitoring
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);

// DMA
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
```

## I2C Init Structure

```c
typedef struct {
    uint32_t I2C_ClockSpeed;          // Clock frequency in Hz (max 400kHz)
    uint16_t I2C_Mode;                // I2C_Mode_I2C, I2C_Mode_SMBusDevice, I2C_Mode_SMBusHost
    uint16_t I2C_DutyCycle;           // I2C_DutyCycle_2 or I2C_DutyCycle_16_9 (fast mode)
    uint16_t I2C_OwnAddress1;         // Device own address (7-bit or 10-bit)
    uint16_t I2C_Ack;                 // I2C_Ack_Enable or I2C_Ack_Disable
    uint16_t I2C_AcknowledgedAddress; // I2C_AcknowledgedAddress_7bit or _10bit
} I2C_InitTypeDef;
```

## I2C Direction

| Constant | Value | Description |
|----------|-------|-------------|
| I2C_Direction_Transmitter | 0x00 | Write to slave |
| I2C_Direction_Receiver | 0x01 | Read from slave |

## Example: I2C1 Master Read/Write

```c
#include "debug.h"

#define I2C_TIMEOUT 10000

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // PB6 = I2C1_SCL (AF open-drain)
    // PB7 = I2C1_SDA (AF open-drain)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure I2C1
    I2C_InitStructure.I2C_ClockSpeed = 100000;  // 100kHz
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}

uint8_t I2C1_WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len)
{
    uint32_t timeout;

    // Wait until I2C is not busy
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if(--timeout == 0) return 1;
    }

    // Generate START
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 2;
    }

    // Send device address (write)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(--timeout == 0) return 3;
    }

    // Send register address
    I2C_SendData(I2C1, regAddr);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(--timeout == 0) return 4;
    }

    // Send data bytes
    for(uint16_t i = 0; i < len; i++)
    {
        I2C_SendData(I2C1, data[i]);
        timeout = I2C_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(--timeout == 0) return 5;
        }
    }

    // Generate STOP
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

uint8_t I2C1_ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len)
{
    uint32_t timeout;

    // Wait until I2C is not busy
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if(--timeout == 0) return 1;
    }

    // Generate START
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 2;
    }

    // Send device address (write) to set register pointer
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(--timeout == 0) return 3;
    }

    // Send register address
    I2C_SendData(I2C1, regAddr);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(--timeout == 0) return 4;
    }

    // Repeated START
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 5;
    }

    // Send device address (read)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(--timeout == 0) return 6;
    }

    // Read data bytes
    for(uint16_t i = 0; i < len; i++)
    {
        if(i == len - 1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);  // NACK on last byte
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        timeout = I2C_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if(--timeout == 0) return 7;
        }
        data[i] = I2C_ReceiveData(I2C1);
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);  // Re-enable ACK
    return 0;
}
```

## Available I2C Instances

| Instance | Bus | SCL | SDA |
|----------|-----|-----|-----|
| I2C1 | APB1 | PB6 | PB7 |
| I2C2 | APB1 | PB10 | PB11 |
