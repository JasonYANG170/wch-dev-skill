# BLE RF 测试与非标射频

> **适用摘要**: 进行 RF 射频性能测试、非标准无线收发、跳频通信。

## 触发意图

- "RF 测试"
- "DTM 测试"
- "非标射频"
- "跳频"
- "无线收发"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/Direct_Test_Mode/`, `RF_PHY/`, `RF_PHY_Hop/` |
| 工具 | RF 测试仪器（如 Litepoint、CMW270） |

## 分步说明

### DTM（Direct Test Mode）

参考 `resources/EXAM/BLE/Direct_Test_Mode/`：

```c
// DTM 用于 BLE RF 射频认证测试
// 符合蓝牙 SIG DTM 规范
// 需要配合 RF 测试仪器使用

// DTM 测试命令格式：
// TX 测试: 0x1E | channel | length | payload_type
// RX 测试: 0x1D | channel | 0x00 | 0x00
// 测试结束: 0x1F | 0x00 | 0x00 | 0x00

// 使用方法：
// 1. 烧录 DTM 固件
// 2. 连接 UART（115200）到测试仪器
// 3. 通过仪器发送 DTM 命令
// 4. 仪器自动测量 TX Power、灵敏度等参数
```

### RF_PHY（非标射频收发）

参考 `resources/EXAM/BLE/RF_PHY/`：

```c
// 非标准 BLE 射频收发，不使用 BLE 协议栈
// 直接操作 RF 寄存器进行数据收发

// TX 发送
void RF_TxInit(void) {
    // 配置 RF 通道（0-39，对应 BLE 频段 2402-2480MHz）
    RF_SetChannel(20);  // 2440MHz

    // 配置发射功率
    RF_SetTxPower(LL_TX_POWEER_0_DBM);

    // 发送数据
    uint8_t payload[37];
    memset(payload, 0xAA, sizeof(payload));
    RF_TxData(payload, sizeof(payload));
}

// RX 接收
void RF_RxInit(void) {
    RF_SetChannel(20);
    RF_RxEnable(TRUE);

    // 等待接收完成
    while (!RF_RxDone());
    uint8_t *data = RF_GetRxData();
    uint8_t len = RF_GetRxLen();
    printf("Received %d bytes\n", len);
}
```

### RF_PHY_Hop（跳频收发）

参考 `resources/EXAM/BLE/RF_PHY_Hop/`：

```c
// 跳频通信，发送端和接收端按相同序列切换频率
// 提高抗干扰能力

// 跳频序列
static uint8_t hop_sequence[] = {2, 26, 10, 34, 18, 6, 30, 14, 38, 22};
static uint8_t hop_index = 0;

void RF_HopTx(void) {
    uint8_t channel = hop_sequence[hop_index];
    hop_index = (hop_index + 1) % sizeof(hop_sequence);

    RF_SetChannel(channel);
    RF_TxData(payload, len);
}

void RF_HopRx(void) {
    uint8_t channel = hop_sequence[hop_index];
    hop_index = (hop_index + 1) % sizeof(hop_sequence);

    RF_SetChannel(channel);
    RF_RxEnable(TRUE);
    // ... 等待接收 ...
}
```

## RF 通道���频率

| 通道 | 频率 (MHz) |
|------|-----------|
| 0 | 2402 |
| 1 | 2404 |
| ... | ... |
| 20 | 2440 |
| ... | ... |
| 39 | 2480 |

公式：`Frequency = 2402 + Channel * 2` MHz

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| DTM 测试失败 | UART 波特率不匹配 | 使用 115200 波特率 |
| 收发不通 | 通道不一致 | 确保 TX/RX 使用相同通道 |
| 跳频不同步 | 跳频序列不一致 | 发送端和接收端使用相同序列 |
| 功率不足 | 天线未匹配 | 检查 PCB 天线设计 |

## 参考项目

- `resources/EXAM/BLE/Direct_Test_Mode/` — DTM 射频测试
- `resources/EXAM/BLE/RF_PHY/` — 非标射频收发
- `resources/EXAM/BLE/RF_PHY_Hop/` — 跳频收发
