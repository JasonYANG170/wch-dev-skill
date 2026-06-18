# SPI 通信

> **适用摘要**: 配置 CH58x/CH59x SPI 主从通信，包含 FIFO 和 DMA 传输

## 触发意图

- "SPI 主机发送数据"
- "配置 SPI 从机"
- "SPI DMA 传输"
- "SPI 读写 Flash"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| GPIO | SPI 引脚已配置 |

## 调用链

```
Step 1: 配置 GPIO 引脚
Step 2: 初始化 SPI (MasterDefInit / SlaveInit)
Step 3: 配置时钟和数据模式
Step 4: 发送/接收数据
```

## 分步说明

### Step 1: SPI0 主机

```c
// GPIO 配置
GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);   // SCK
GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);   // MOSI
GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeIN_PU);         // MISO
GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);   // CS

// 初始化
SPI0_MasterDefInit();
SPI0_CLKCfg(4);  // 时钟分频

// 发送接收
SPI0_MasterSendByte(0xAA);
uint8_t rx = SPI0_MasterRecvByte();

// FIFO 批量传输
SPI0_MasterTrans(txBuf, len);
SPI0_MasterRecv(rxBuf, len);
```

### Step 2: SPI0 从机

```c
GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);         // SCK
GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);   // MISO
GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);         // CS

SPI0_SlaveInit();

// 从机接收
SPI0_SlaveRecv(rxBuf, len);

// 从机发送
SPI0_SlaveTrans(txBuf, len);
```

### Step 3: DMA 传输

```c
// 主机 DMA 发送
SPI0_MasterDMATrans(txBuf, len);

// 主机 DMA 接收
SPI0_MasterDMARecv(rxBuf, len);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 数据全为 0xFF | GPIO 未配置为输出 | 检查 SCK/MOSI 引脚模式 |
| 数据错位 | 时钟极性/相位不匹配 | 调用 SPI0_DataMode() |
| DMA 传输失败 | 地址未对齐 | 确保缓冲区 4 字节对齐 |

## 参考项目

- `resources/EXAM/SPI/SPI0_Master/`
- `resources/EXAM/SPI/SPI0_Slave/`
