# I2C 通信

> **适用摘要**: 配置 CH58x/CH59x I2C 主从通信

## 触发意图

- "I2C 通信"
- "I2C 读写 EEPROM"
- "I2C 主机初始化"
- "I2C 传感器通信"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| GPIO | I2C 引脚已配置 |

## 调用链

```
Step 1: 配置 GPIO 引脚 (开漏)
Step 2: 初始化 I2C (I2C_Init)
Step 3: 发送 START + 地址
Step 4: 读写数据
Step 5: 发送 STOP
```

## 分步说明

### Step 1: I2C 主机初始化

```c
// GPIO 配置为开漏
GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_OD_5mA);  // SCL
GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_OD_5mA);  // SDA

// I2C 初始化
I2C_Init(I2C_Mode_I2C, 400000, I2C_DutyCycle_16_9,
         I2C_Ack_Enable, I2C_AckAddr_7bit, 0x00);
I2C_Cmd(ENABLE);
```

### Step 2: 写数据

```c
I2C_GenerateSTART(ENABLE);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

I2C_Send7bitAddress(deviceAddr, I2C_Direction_Transmitter);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

I2C_SendData(regAddr);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

I2C_SendData(data);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

I2C_GenerateSTOP(ENABLE);
```

### Step 3: 读数据

```c
I2C_GenerateSTART(ENABLE);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

I2C_Send7bitAddress(deviceAddr, I2C_Direction_Receiver);
while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
uint8_t data = I2C_ReceiveData();

I2C_GenerateSTOP(ENABLE);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 无应答 | 设备地址错误 | 检查 7 位地址（不含 R/W 位） |
| 总线卡死 | SDA 被拉低 | 手动发送时钟脉冲释放总线 |
| 通信不稳定 | 上拉电阻缺失 | SCL/SDA 需要 4.7K 上拉 |

## 参考项目

- `resources/EXAM/I2C/`
