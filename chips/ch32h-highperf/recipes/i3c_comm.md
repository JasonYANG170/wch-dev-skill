# I3C Communication

## Overview

CH32H417 has an I3C (MIPI Improved Inter-Integrated Circuit) controller supporting both I2C legacy and I3C native protocols. I3C is backward-compatible with I2C and adds higher speed (up to 12.5 MHz), dynamic address assignment (DAA), in-band interrupt (IBI), and DMA support.

## Key API Functions

```c
void I3C_Ctrl_Init(I3C_Ctrl_BusTypeDef* I3C_Ctrl_BusInit);
void I3C_Ctrl_Config(I3C_CtrlConfTypeDef* I3C_CtrlConfInit);
void I3C_SetConfigFifo(I3C_FifoConfTypeDef* I3C_FifoConfInit);
void I3C_Cmd(FunctionalState NewState);
uint32_t I3C_ControllerHandleCCC(uint8_t CCC, uint8_t data, uint8_t stop);
uint32_t I3C_ControllerHandleMessage(uint8_t addr, uint16_t len, uint8_t dir,
                                      uint8_t mtype, uint8_t stop);
void I3C_WriteByte(uint8_t data);
uint8_t I3C_ReadByte(void);
FlagStatus I3C_GetFlagStatus(uint32_t flag);
void I3C_ClearFlag(uint32_t flag);
void I3C_DMAReq_TXCmd(FunctionalState NewState);
void I3C_DMAReq_RXCmd(FunctionalState NewState);
```

## I3C Controller Initialization

```c
#include "ch32h417.h"

void I3C_Config(void)
{
    I3C_Ctrl_BusTypeDef I3C_Ctrl_BusInit = {0};
    I3C_CtrlConfTypeDef I3C_CtrlConfInit = {0};
    I3C_FifoConfTypeDef I3C_FifoConfInit = {0};

    // Enable I3C clock
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_I3C, ENABLE);

    // Bus timing configuration
    I3C_Ctrl_BusInit.SDAHoldTime        = I3C_SDAHoldTime_1_5;
    I3C_Ctrl_BusInit.WaitTime           = I3C_WaitTime_State_2;
    I3C_Ctrl_BusInit.SCLPPLowDuration   = 0x89;
    I3C_Ctrl_BusInit.SCLI3CHighDuration = 0x88;
    I3C_Ctrl_BusInit.SCLODLowDuration   = 0x88;
    I3C_Ctrl_BusInit.SCLI2CHighDuration = 0x88;
    I3C_Ctrl_BusInit.BusFreeDuration    = 0x77;
    I3C_Ctrl_BusInit.BusIdleDuration    = 0x8e;

    // Controller configuration
    I3C_CtrlConfInit.DynamicAddr       = 0x22;
    I3C_CtrlConfInit.StallTime         = 0x10;
    I3C_CtrlConfInit.HotJoinAllowed_EN = DISABLE;
    I3C_CtrlConfInit.ACKStallState     = DISABLE;
    I3C_CtrlConfInit.CCCStallState     = DISABLE;
    I3C_CtrlConfInit.TxStallState      = DISABLE;
    I3C_CtrlConfInit.RxStallState      = DISABLE;

    // FIFO configuration
    I3C_FifoConfInit.RxFifoThreshold = I3C_RXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.TxFifoThreshold = I3C_TXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.ControlFifo     = I3C_CONTROLFIFO_DISABLE;
    I3C_FifoConfInit.StatusFifo      = I3C_STATUSFIFO_DISABLE;

    I3C_Ctrl_Init(&I3C_Ctrl_BusInit);
    I3C_Ctrl_Config(&I3C_CtrlConfInit);
    I3C_SetConfigFifo(&I3C_FifoConfInit);
    I3C_Cmd(ENABLE);
}
```

## GPIO Configuration (PE14=SCL, PE15=SDA)

```c
void I3C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOE, ENABLE);

    // SCL PE14(AF3)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // SDA PE15(AF3)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}
```

## Dynamic Address Assignment (DAA)

```c
uint32_t I3C_RSTDAA(void)
{
    // Reset Dynamic Address Assignment (CCC 0x06)
    I3C_ControllerHandleCCC(0x06, 0, I3C_GENERATE_STOP);

    while (I3C_GetFlagStatus(I3C_EVR_FCF) == RESET &&
           I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET);

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
        I3C_ClearFlag(I3C_FLAG_FCF);

    return I3C_GetFlagStatus(I3C_EVR_ERRF) ? 1 : 0;
}

uint32_t I3C_ENTDAA(uint8_t target_daa)
{
    // Enter Dynamic Address Assignment (CCC 0x07)
    I3C_ControllerHandleCCC(0x07, 0, I3C_GENERATE_STOP);

    // Read device payloads and assign addresses
    uint64_t payload = 0;
    do
    {
        if (I3C_GetFlagStatus(I3C_EVR_TXFNFF))
        {
            for (uint32_t i = 0; i < 8; i++)
            {
                while (I3C_GetFlagStatus(I3C_EVR_RXFNEF) == RESET);
                payload |= (uint64_t)((uint64_t)I3C_ReadByte() << (i * 8));
            }
            I3C_WriteByte(target_daa++);
        }
    } while (I3C_GetFlagStatus(I3C_EVR_FCF) == RESET &&
             I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET);

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
        I3C_ClearFlag(I3C_FLAG_FCF);

    return I3C_GetFlagStatus(I3C_EVR_ERRF) ? 1 : 0;
}
```

## I3C DMA Transmit/Receive

```c
uint32_t I3C_DMA_Transmit(uint8_t addr, uint8_t *send_buf, uint16_t send_len)
{
    // Configure DMA TX on DMA1 Channel 4
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(I3C->TDBR));
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)send_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = send_len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    // DMA MUX: I3C_TX = 83
    DMA_MuxChannelConfig(DMA_MuxChannel4, 83);
    I3C_DMAReq_TXCmd(ENABLE);

    I3C_ControllerHandleMessage(addr, send_len, I3C_Direction_WR,
                                I3C_CONTROLLER_MTYPE_PRIVATE, I3C_GENERATE_STOP);
    DMA_Cmd(DMA1_Channel4, ENABLE);

    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC4) == RESET);

    // Wait for I3C completion
    while (I3C_GetFlagStatus(I3C_EVR_FCF) == RESET &&
           I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET);

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
        I3C_ClearFlag(I3C_FLAG_FCF);

    return I3C_GetFlagStatus(I3C_EVR_ERRF) ? 1 : 0;
}
```

## I3C Message Types

| Macro | Description |
|-------|-------------|
| `I3C_CONTROLLER_MTYPE_PRIVATE` | Private I3C message |
| `I3C_CONTROLLER_MTYPE_LEGACY_I2C` | Legacy I2C message |
| `I3C_CONTROLLER_MTYPE_CCC` | Common Command Code |

## I3C Direction

| Macro | Description |
|-------|-------------|
| `I3C_Direction_WR` | Write to target |
| `I3C_Direction_RD` | Read from target |

## DMA MUX Values for I3C

| Peripheral | MUX Value |
|-----------|-----------|
| I3C_TX | 83 |
| I3C_RX | 84 |
