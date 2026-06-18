# SPI 通信

> **适用摘要**: 配置 SPI 进行主从模式通信，包括 FIFO 和 DMA 传输。

## 触发意图

- "配置 SPI"
- "SPI 主从通信"
- "SPI 读写外设"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/SPI0/` |

## 分步说明

### SPI 引脚

| 功能 | 引脚 |
|------|------|
| SCK | PA12 |
| MOSI | PA13 |
| MISO | PA14 |
| CS | PA15 |

### SPI 主机模式

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 配置 SPI 引脚
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);  // SCK
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);  // MOSI
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeIN_PU);        // MISO
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // CS

    // 初始化 SPI 主机（Mode 0, MSB first, 8MHz）
    SPI0_MasterDefInit();

    // 片选使能
    GPIOA_ResetBits(GPIO_Pin_15);

    // 发送单字节
    SPI0_MasterSendByte(0xAA);

    // 接收单字节
    uint8_t rx = SPI0_MasterRecvByte();

    // 片选释放
    GPIOA_SetBits(GPIO_Pin_15);

    while(1);
}
```

### SPI 数据模式

```c
// Mode 0, LSB first
SPI0_DataMode(Mode0_LowBitINFront);

// Mode 0, MSB first（默认）
SPI0_DataMode(Mode0_HighBitINFront);

// Mode 3, LSB first
SPI0_DataMode(Mode3_LowBitINFront);

// Mode 3, MSB first
SPI0_DataMode(Mode3_HighBitINFront);
```

### SPI 时钟配置

```c
// 默认 8MHz，修改分频系数
SPI0_CLKCfg(2);  // 分频系数越大，时钟越慢
```

### FIFO 批量传输

```c
uint8_t tx_buf[] = {0x01, 0x02, 0x03, 0x04};
uint8_t rx_buf[4];

// 发送多字节
GPIOA_ResetBits(GPIO_Pin_15);  // CS 使能
SPI0_MasterTrans(tx_buf, sizeof(tx_buf));
GPIOA_SetBits(GPIO_Pin_15);    // CS 释放

// 接收多字节
GPIOA_ResetBits(GPIO_Pin_15);
SPI0_MasterRecv(rx_buf, sizeof(rx_buf));
GPIOA_SetBits(GPIO_Pin_15);
```

### DMA 传输

```c
uint8_t dma_buf[256];

// DMA 发送
GPIOA_ResetBits(GPIO_Pin_15);
SPI0_MasterDMATrans(dma_buf, sizeof(dma_buf));
GPIOA_SetBits(GPIO_Pin_15);

// DMA 接收
GPIOA_ResetBits(GPIO_Pin_15);
SPI0_MasterDMARecv(dma_buf, sizeof(dma_buf));
GPIOA_SetBits(GPIO_Pin_15);
```

### SPI 从机模式

```c
// 初始化从机
SPI0_SlaveInit();

// 从机接收
uint8_t rx = SPI0_SlaveRecvByte();

// 从机发送
SPI0_SlaveSendByte(0x55);
```

### SPI Flash 读写示例

```c
// 读取 SPI Flash ID
uint32_t SPI_FlashReadID(void) {
    uint32_t id;
    GPIOA_ResetBits(GPIO_Pin_15);
    SPI0_MasterSendByte(0x9F);  // JEDEC ID 命令
    id = SPI0_MasterRecvByte();  // Manufacturer ID
    id <<= 8;
    id |= SPI0_MasterRecvByte(); // Memory Type
    id <<= 8;
    id |= SPI0_MasterRecvByte(); // Capacity
    GPIOA_SetBits(GPIO_Pin_15);
    return id;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 通信失败 | GPIO 模式未配置 | 先配置所有 SPI 引脚 |
| 数据错误 | 时钟极性不匹配 | 尝试 Mode0 或 Mode3 |
| 从机无响应 | CS 信号错误 | 检查 CS 片选逻辑 |
| 速度太慢 | 分频系数太大 | 减小 SPI0_CLKCfg() 参数 |

## 参考项目

- `resources/EXAM/SPI0/` — SPI 主从通信示例
