# RF PHY 射频物理层测试 (RF PHY)

> **适用摘要**: 实现 CH58x/CH59x 非标 2.4GHz 射频物理层直接收发测试，绕过 BLE 协议栈直接操作 RF 硬件，用于射频性能评估和信道测试

## 触发意图

- "RF 射频测试"
- "2.4G 直接收发"
- "RF PHY 测试"
- "非标射频"
- "信道频率测试"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `RF_PHY.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 4KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit) — RF 驱动依赖 BLE 初始化
Step 3: 初始化 HAL
Step 4: 注册 RF 任务 (TMOS_ProcessEventRegister)
Step 5: 配置 RF 参数 (accessAddress, CRC, 通道, 频率, LLE模式)
Step 6: 调用 RF_Config 配置射频
Step 7: 使用 RF_Tx/Rx 进行收发
Step 8: 在 RF_2G4StatusCallBack 中处理收发结果
Step 9: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "RF_PHY.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif

#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif

    CH58X_BLEInit();
    HAL_Init();
    RF_RoleInit();  // RF 角色初始化
    RF_Init();      // RF 参数配置
    Main_Circulation();
    return 0;
}
```

### Step 2: RF 参数配置

```c
void RF_Init(void) {
    rfConfig_t rfConfig;

    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    taskID = TMOS_ProcessEventRegister(RF_ProcessEvent);

    rfConfig.accessAddress = 0x71764129;  // 禁止使用 0x55555555 和 0xAAAAAAAA
    rfConfig.CRCInit = 0x555555;
    rfConfig.Channel = 8;                 // BLE 通道 (0-39)
    rfConfig.Frequency = 2480000;         // 频率 (kHz), 配合 LLE_MODE_EX_CHANNEL

    // LLE 模式选择
    rfConfig.LLEMode = LLE_MODE_BASIC | LLE_MODE_EX_CHANNEL;  // 基本模式 + 扩展通道
    // rfConfig.LLEMode = LLE_MODE_AUTO;  // 自动模式 (需要配对设备)

    rfConfig.rfStatusCB = RF_2G4StatusCallBack;  // 状态回调
    rfConfig.RxMaxlen = 251;                      // 最大接收长度

    uint8_t state = RF_Config(&rfConfig);
    PRINT("rf 2.4g init: %x\n", state);
}
```

### Step 3: 通道频率映射

```
频率(MHz)    通道
2402         37
2404         0
...          ...
f = 2404 + n*2M
...          ...
2424         10
2426         38
2428         11
...          ...
f = 2428 + (n-11)*2M
...          ...
2478         36
2480         39
```

### Step 4: 发送模式

```c
// 发送数据
void RF_Tx(uint8_t *pData, uint8_t len, uint8_t i, uint8_t cw);
// pData: 发送数据缓冲区 (需4字节对齐)
// len: 数据长度
// i, cw: 保留参数，填 0xFF

// 周期发送示例
case SBP_RF_PERIODIC_EVT:
    RF_Shut();  // 先关闭当前操作
    RF_Tx(TX_DATA, 10, 0xFF, 0xFF);
    tmos_start_task(taskID, SBP_RF_PERIODIC_EVT, 1000);  // 1秒周期
    break;
```

### Step 5: 接收模式

```c
// 启动接收
uint8_t state = RF_Rx(RxBuf, len, 0xFF, 0xFF);
// RxBuf: 接收缓冲区 (需4字节对齐, 大小>=264字节)
// len: 接收超时参数

// 接收完成后在回调中处理
case RX_MODE_RX_DATA:
    if(crc == 0) {
        PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
        PRINT("len:%d-", rxBuf[1]);
        for(i = 0; i < rxBuf[1]; i++)
            PRINT("%x ", rxBuf[i + 2]);
        PRINT("\n");
    }
    RF_Rx(TX_DATA, 10, 0xFF, 0xFF);  // 继续接收
    break;
```

### Step 6: 状态回调

```c
void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf) {
    switch(sta) {
        case TX_MODE_TX_FINISH:   // 发送完成
        case TX_MODE_TX_FAIL:     // 发送失败
        case TX_MODE_RX_DATA:     // 发送后收到应答
        case TX_MODE_RX_TIMEOUT:  // 发送后应答超时
        case RX_MODE_RX_DATA:     // 接收到数据
        case RX_MODE_TX_FINISH:   // 接收后应答发送完成
        case RX_MODE_TX_FAIL:     // 接收后应答发送失败
    }
}
```

### Step 7: 关闭 RF

```c
RF_Shut();  // 停止当前 RF 操作
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| RF_Config 失败 | accessAddress 不合法 | 避免使用 0x55555555 和 0xAAAAAAAA |
| CRC 错误 | 两端 CRCInit 不匹配 | 确保收发两端 `CRCInit` 一致 |
| 接收不到数据 | 通道/频率不匹配 | 检查 `Channel` 和 `Frequency` 配置 |
| 回调中直接调用 RF API | 不允许在回调中直接操作 | 使用 `tmos_set_event` 延迟处理 |

## 参考项目

- `resources/EXAM/BLE/RF_PHY/` — RF PHY 射频测试例程
- `resources/EXAM/BLE/RF_PHY_Hop/` — 跳频测试例程
