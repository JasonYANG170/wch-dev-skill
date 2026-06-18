# BLE 观察者 (Observer)

> **适用摘要**: 实现 CH58x/CH59x BLE 观察者角色，仅扫描周围广播设备，不发起连接

## 触发意图

- "BLE 观察者"
- "BLE 扫描"
- "BLE Observer"
- "被动扫描广播"
- "扫描周围 BLE 设备"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `observer.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 4KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Observer Role (GAPRole_ObserverInit)
Step 4: 注册 TMOS 任务并配置扫描参数
Step 5: 启动扫描 (GAPRole_ObserverStartDiscovery)
Step 6: 处理扫描结果回调
Step 7: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "observer.h"

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
    GAPRole_ObserverInit();
    Observer_Init();
    Main_Circulation();
    return 0;
}
```

### Step 2: 扫描参数配置

```c
void Observer_Init(void) {
    ObserverTaskId = TMOS_ProcessEventRegister(Observer_ProcessEvent);

    // 最大扫描结果数
    uint8_t scanRes = DEFAULT_MAX_SCAN_RES; // 8
    GAPRole_SetParameter(GAPROLE_MAX_SCAN_RES, sizeof(uint8_t), &scanRes);

    // 扫描持续时间 (单位 625us, 4800=3s)
    GAP_SetParamValue(TGAP_DISC_SCAN, DEFAULT_SCAN_DURATION);

    tmos_set_event(ObserverTaskId, START_DEVICE_EVT);
}
```

### Step 3: 启动扫描设备

```c
static const gapRoleObserverCB_t ObserverRoleCB = {
    ObserverEventCB  // 事件回调
};

// 在 TMOS 事件中启动
GAPRole_ObserverStartDevice((gapRoleObserverCB_t *)&ObserverRoleCB);
```

### Step 4: 处理扫描事件

```c
static void ObserverEventCB(gapRoleEvent_t *pEvent) {
    switch(pEvent->gap.opcode) {
        case GAP_DEVICE_INIT_DONE_EVENT:
            // 设备初始化完成，开始扫描
            GAPRole_ObserverStartDiscovery(
                DEVDISC_MODE_ALL,       // 发现模式: 全部
                TRUE,                   // 主动扫描
                FALSE);                 // 不使用白名单
            break;

        case GAP_DEVICE_INFO_EVENT:
            // 收到广播包，记录设备地址
            ObserverAddDeviceInfo(pEvent->deviceInfo.addr,
                                  pEvent->deviceInfo.addrType);
            break;

        case GAP_DEVICE_DISCOVERY_EVENT:
            // 扫描结束，打印所有发现的设备
            for(int j = 0; j < pEvent->discCmpl.numDevs; j++) {
                PRINT("Device %d : ", j);
                for(int i = 0; i < 6; i++)
                    PRINT("%x ", pEvent->discCmpl.pDevList[j].addr[i]);
                PRINT("\n");
            }
            // 重新开始扫描
            GAPRole_ObserverStartDiscovery(DEVDISC_MODE_ALL, TRUE, FALSE);
            break;

        case GAP_EXT_ADV_DEVICE_INFO_EVENT:
            // 收到扩展广播包 (BLE 5.0)
            ObserverAddDeviceInfo(pEvent->deviceExtAdvInfo.addr,
                                  pEvent->deviceExtAdvInfo.addrType);
            break;
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 扫描不到设备 | 扫描时间太短 | 增大 `TGAP_DISC_SCAN` 值 |
| 扫描结果溢出 | `MAX_SCAN_RES` 太小 | 增大扫描结果缓冲区 |
| 堆内存不足 | `BLE_MEMHEAP_SIZE` 太小 | 增大到至少 4KB |

## 参考项目

- `resources/EXAM/BLE/Observer/` — 基础观察者例程
