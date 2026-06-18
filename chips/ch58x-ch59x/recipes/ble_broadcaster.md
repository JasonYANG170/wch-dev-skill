# BLE 广播者 (Broadcaster)

> **适用摘要**: 实现 CH58x/CH59x BLE 广播者角色，仅广播不可连接的非连接指示，用于 Beacon 或数据广播场景

## 触发意图

- "BLE 广播者"
- "BLE Beacon"
- "不可连接广播"
- "BLE Broadcaster"
- "NONCONN_IND 广播"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `broadcaster.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 4KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小、发射功率)
Step 2: 初始化 BLE 协议栈 (CH58X_BLEInit)
Step 3: 初始化 HAL (HAL_Init)
Step 4: 初始化 GAP Broadcaster Role (GAPRole_BroadcasterInit)
Step 5: 注册 TMOS 任务并配置广播参数
Step 6: 启动广播设备 (GAPRole_BroadcasterStartDevice)
Step 7: 运行 Main_Circulation
```

## 分步说明

### Step 1: config.h 配置

```c
#define BLE_MEMHEAP_SIZE    (1024*4)
#define BLE_BUFF_MAX_LEN    27
#define BLE_TX_POWER        TX_POW_0_DBm
```

### Step 2: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "broadcaster.h"

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
    GAPRole_BroadcasterInit();
    Broadcaster_Init();
    Main_Circulation();
    return 0;
}
```

### Step 3: 广播参数配置

```c
// 广播类型设为不可连接非定向广播
uint8_t initial_advertising_enable = TRUE;
uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;

GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);
GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

// 设置广播间隔 (单位 625us, 160=100ms)
GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, 160);
GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, 160);
```

### Step 4: 广播数据格式

```c
// 广播数据 (<=31字节)
static uint8_t advertData[] = {
    0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    0x04, GAP_ADTYPE_MANUFACTURER_SPECIFIC, 'b', 'l', 'e',
};

// 扫描响应数据 (<=31字节)
static uint8_t scanRspData[] = {
    0x0c, GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B','r','o','a','d','c','a','s','t','e','r',
    0x02, GAP_ADTYPE_POWER_LEVEL, 0,
};
```

### Step 5: 启动设备并处理状态回调

```c
static gapRolesBroadcasterCBs_t Broadcaster_BroadcasterCBs = {
    Broadcaster_StateNotificationCB,  // 状态变化回调
    NULL                              // 扫描请求回调 (可选)
};

// 在 TMOS 事件中启动
GAPRole_BroadcasterStartDevice(&Broadcaster_BroadcasterCBs);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 广播类型错误 | 使用了可连接广播类型 | 确保 `GAP_ADTYPE_ADV_NONCONN_IND` |
| 其他设备扫描不到 | 广播数据超长或格式错误 | 确保 <=31 字节，首字节为长度 |
| 堆内存不足 | `BLE_MEMHEAP_SIZE` 太小 | 增大到至少 4KB |

## 参考项目

- `resources/EXAM/BLE/Broadcaster/` — 基础广播者例程
