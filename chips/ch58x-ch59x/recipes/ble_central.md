# BLE 主机 (Central)

> **适用摘要**: 实现 CH58x/CH59x BLE 主机角色，扫描、连接、服务发现、读写

## 触发意图

- "BLE 主机"
- "BLE 扫描连接"
- "读写 BLE 特征值"
- "BLE Central"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `central.h` |
| config.h | `BLE_MEMHEAP_SIZE` ≥ 6KB |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Central Role
Step 4: 启动扫描
Step 5: 连接目标设备
Step 6: 服务发现
Step 7: 读写特征值
```

## 分步说明

### Step 1: main.c

```c
#include "CONFIG.h"
#include "HAL.h"
#include "CH58xBLE_LIB.h"
#include "central.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    UART1_DefInit();

    CH58X_BLEInit();
    HAL_Init();
    GAPRole_CentralInit();
    Central_Init();

    Main_Circulation();
    return 0;
}
```

### Step 2: 扫描和连接

```c
// 启动扫描
uint8_t enable = TRUE;
GAPRole_CentralSetParameter(GAPROLE_CENTRAL_SCAN_ENABLED, sizeof(uint8_t), &enable);

// 扫描回调中处理设备发现
// 连接目标设备
GAPRole_CentralEstablishLink(...);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 扫描不到设备 | 广播类型不匹配 | 确认设备在广播中 |
| 连接失败 | 地址类型错误 | 检查 public/random 地址 |
| 服务发现失败 | 连接参数不当 | 调整连接间隔 |

## 参考项目

- `resources/EXAM/BLE/Central/`
- `resources/EXAM/BLE/MultiCentral/`
