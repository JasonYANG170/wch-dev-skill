# BLE 从机 (Peripheral)

> **适用摘要**: 实现 CH58x/CH59x BLE 从机角色，包含广播、GATT 服务、数据通知

## 触发意图

- "创建 BLE 从机"
- "BLE 外设广播"
- "实现 GATT 服务"
- "BLE 数据通知"
- "BLE 读写特征值"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `peripheral.h` |
| config.h | `BLE_MEMHEAP_SIZE` ≥ 6KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小、缓冲区、连接参数)
Step 2: 初始化 BLE 协议栈 (CH58X_BLEInit)
Step 3: 初始化 HAL (HAL_Init)
Step 4: 初始化 GAP Role (GAPRole_PeripheralInit)
Step 5: 注册 GATT 服务 (GGS + GATTServApp + DevInfo + 自定义)
Step 6: 配置广播参数
Step 7: 启动广播
Step 8: 处理连接事件和数据读写
Step 9: 运行 Main_Circulation
```

## 分步说明

### Step 1: config.h 配置

```c
#define BLE_MEMHEAP_SIZE    (1024*6)
#define BLE_BUFF_MAX_LEN    27
#define BLE_TX_POWER        TX_POW_0_DBm
#define PERIPHERAL_MAX_CONNECTION  1
```

### Step 2: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "CH58xBLE_LIB.h"
#include "peripheral.h"
#include "devinfoservice.h"
#include "gattprofile.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif

#ifdef DEBUG
    UART1_DefInit();
    printf("BLE Peripheral start\n");
#endif

    CH58X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();

    Main_Circulation();
    return 0;
}
```

### Step 3: GATT 服务注册顺序

```c
void Peripheral_Init(void) {
    // 1. GAP 服务
    GGS_AddService(GATT_ALL_SERVICES);

    // 2. GATT 服务
    GATTServApp_AddService(GATT_ALL_SERVICES);

    // 3. Device Information 服务
    DevInfo_AddService();

    // 4. 自定义服务（最后注册）
    MyCustomService_AddService(GATT_ALL_SERVICES);

    // 5. 注册回调
    GAPRole_PeripheralSetParameter(...);
    GATTServApp_RegisterForMsg(taskID);
}
```

### Step 4: 广播配置

```c
// 广播数据（≤31字节）
uint8_t advData[] = {
    0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    0x07, GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'C','H','5','8','x',
    0x03, GAP_ADTYPE_16BIT_MORE, LO_UINT16(0xFFF0), HI_UINT16(0xFFF0),
};

GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advData), advData);

// 广播参数
GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
```

### Step 5: 数据通知

```c
// 发送通知（需要有效连接句柄）
attHandleValueNoti_t noti;
noti.handle = myCharHandle;
noti.len = dataLen;
memcpy(noti.pValue, data, dataLen);
GATT_Notification(connHandle, &noti, FALSE);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| BLE 初始化失败 | 堆太小 | 增大 BLE_MEMHEAP_SIZE |
| 广播看不到 | 广播数据超长 | 确保 ≤31 字节 |
| 连接后断开 | GATT 服务注册顺序错误 | GAP → GATT → DevInfo → 自定义 |
| 通知发送失败 | 连接句柄无效 | 使用 GAP_LINK_ESTABLISHED_EVENT 中的句柄 |

## 参考项目

- `resources/EXAM/BLE/Peripheral/` — 基础从机例程
- `resources/EXAM/BLE/HeartRate/` — 心率 Profile
- `resources/EXAM/BLE/BLE_UART/` — BLE 转串口
