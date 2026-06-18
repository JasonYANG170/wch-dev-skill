# BLE 主从一体 (CentPeri)

> **适用摘要**: 实现 CH58x/CH59x BLE 同时作为 Central 和 Peripheral 双角色运行，一边扫描连接其他设备，一边被其他设备连接

## 触发意图

- "BLE 主从一体"
- "Central + Peripheral"
- "双角色 BLE"
- "BLE CentPeri"
- "同时作为主机和从机"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `peripheral.h`, `central.h`, `gattprofile.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 8KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小需要更大)
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 同时初始化 Peripheral 和 Central Role
Step 4: 注册 GATT 服务 (Peripheral 端)
Step 5: 初始化 GATT Client (Central 端)
Step 6: Peripheral 开始广播，Central 开始扫描
Step 7: 处理双角色的连接/断连/数据事件
Step 8: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "peripheral.h"
#include "central.h"

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

    // 关键: 两个 Role 都要初始化
    GAPRole_PeripheralInit();
    GAPRole_CentralInit();

    Peripheral_Init();  // 从机初始化 (广播、GATT服务)
    Central_Init();     // 主机初始化 (扫描、GATT Client)

    Main_Circulation();
    return 0;
}
```

### Step 2: Peripheral 端初始化

```c
void Peripheral_Init(void) {
    Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);

    // 配置广播参数
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    // 注册 GATT 服务 (顺序重要)
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);
    DevInfo_AddService();
    SimpleProfile_AddService(GATT_ALL_SERVICES);

    tmos_set_event(Peripheral_TaskID, SBP_START_DEVICE_EVT);
}
```

### Step 3: Central 端初始化

```c
void Central_Init(void) {
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);

    GAP_SetParamValue(TGAP_DISC_SCAN, 2400);

    // 配对参数 (Central 端独立配置)
    GAPBondMgr_SetParameter(GAPBOND_CENT_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);

    GATT_InitClient();
    GATT_RegisterForInd(centralTaskId);

    // 注册 GAP 和 GATT 服务 (Central 端也需要)
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);

    tmos_set_event(centralTaskId, START_DEVICE_EVT);
}
```

### Step 4: 启动设备

```c
// Peripheral 端启动
GAPRole_PeripheralStartDevice(Peripheral_TaskID, &Peripheral_BondMgrCBs, &Peripheral_PeripheralCBs);

// Central 端启动 (注意传入 taskId 参数)
GAPRole_CentralStartDevice(centralTaskId, &centralBondCB, &centralRoleCB);
```

### Step 5: 数据交互

```c
// Peripheral 端: 发送通知
attHandleValueNoti_t noti;
noti.len = len;
noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle, ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
tmos_memcpy(noti.pValue, pValue, noti.len);
simpleProfile_Notify(peripheralConnList.connHandle, &noti);

// Central 端: 读写远端设备
GATT_WriteCharValue(centralConnHandle, &req, centralTaskId);
GATT_ReadCharValue(centralConnHandle, &req, centralTaskId);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 堆内存不足 | 双角色消耗更多堆 | 增大 `BLE_MEMHEAP_SIZE` 到 8KB 以上 |
| 配对参数冲突 | Central/Peripheral 使用相同参数名 | 使用 `GAPBOND_CENT_*` 和 `GAPBOND_PERI_*` 分别配置 |
| 广播和扫描冲突 | 时序资源竞争 | BLE 协议栈自动调度，但需确保堆足够 |
| 事件处理混乱 | 两个 Role 的事件混在一起 | 使用独立的 taskID 分别处理 |

## 参考项目

- `resources/EXAM/BLE/CentPeri/` — 主从一体例程
- `resources/EXAM/BLE/MultiCentPeri/` — 多连接主从一体例程
