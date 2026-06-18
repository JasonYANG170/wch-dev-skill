# BLE 心率 Profile (Heart Rate)

> **适用摘要**: 实现 CH58x/CH59x BLE 心率传感器 Profile，遵循蓝牙 SIG Heart Rate Service 规范，支持心率测量通知、电池服务和设备信息服务

## 触发意图

- "BLE 心率"
- "Heart Rate Profile"
- "心率传感器"
- "HRS 服务"
- "BLE 健康设备"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `heartrate.h`, `heartrateservice.h`, `battservice.h`, `devinfoservice.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 6KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Peripheral Role
Step 4: 注册 GATT 服务 (GAP + GATT + HeartRate + DevInfo + Battery)
Step 5: 配置心率特征值 (传感器位置等)
Step 6: 等待客户端使能通知后，定时发送心率测量
Step 7: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "heartrate.h"

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
    GAPRole_PeripheralInit();
    HeartRate_Init();
    Main_Circulation();
    return 0;
}
```

### Step 2: HeartRate 服务初始化

```c
void HeartRate_Init(void) {
    heartRate_TaskID = TMOS_ProcessEventRegister(HeartRate_ProcessEvent);

    // 配置广播数据 (包含心率和电池服务 UUID)
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    // 配对参数
    GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);

    // 设置传感器位置 (手腕)
    uint8_t sensLoc = HEARTRATE_SENS_LOC_WRIST;
    HeartRate_SetParameter(HEARTRATE_SENS_LOC, sizeof(uint8_t), &sensLoc);

    // 设置电池临界水平
    uint8_t critical = 6;
    Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);

    // 注册 GATT 服务 (顺序: GAP -> GATT -> HeartRate -> DevInfo -> Battery)
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);
    HeartRate_AddService(GATT_ALL_SERVICES);
    DevInfo_AddService();
    Batt_AddService();

    // 注册回调
    HeartRate_Register(heartRateCB);
    Batt_Register(heartRateBattCB);

    tmos_set_event(heartRate_TaskID, START_DEVICE_EVT);
}
```

### Step 3: 心率测量通知

```c
static void heartRateMeasNotify(void) {
    heartRateMeas.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI,
                                           HEARTRATE_MEAS_LEN, NULL, 0);
    if(heartRateMeas.pValue != NULL) {
        uint8_t *p = heartRateMeas.pValue;
        uint8_t flags = heartRateFlags[heartRateFlagsIdx];

        *p++ = flags;
        *p++ = heartRateBpm;

        if(flags & HEARTRATE_FLAGS_FORMAT_UINT16)
            *p++ = 0;  // 16位格式额外字节

        if(flags & HEARTRATE_FLAGS_ENERGY_EXP) {
            *p++ = LO_UINT16(heartRateEnergy);
            *p++ = HI_UINT16(heartRateEnergy);
        }

        if(flags & HEARTRATE_FLAGS_RR) {
            *p++ = LO_UINT16(heartRateRrInterval1);
            *p++ = HI_UINT16(heartRateRrInterval1);
            *p++ = LO_UINT16(heartRateRrInterval2);
            *p++ = HI_UINT16(heartRateRrInterval2);
        }

        heartRateMeas.len = (uint8_t)(p - heartRateMeas.pValue);
        HeartRate_MeasNotify(gapConnHandle, &heartRateMeas);
    }
}
```

### Step 4: 通知使能回调

```c
static void heartRateCB(uint8_t event) {
    if(event == HEARTRATE_MEAS_NOTI_ENABLED) {
        // 客户端使能了通知，开始定时测量
        if(gapProfileState == GAPROLE_CONNECTED) {
            tmos_start_task(heartRate_TaskID, HEART_PERIODIC_EVT, DEFAULT_HEARTRATE_PERIOD);
        }
    } else if(event == HEARTRATE_MEAS_NOTI_DISABLED) {
        tmos_stop_task(heartRate_TaskID, HEART_PERIODIC_EVT);
    } else if(event == HEARTRATE_COMMAND_SET) {
        heartRateEnergy = 0;  // 重置能量消耗
    }
}
```

### Step 5: 广播策略 (快慢切换)

```c
// 断连后使用快广播
GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL); // 32=20ms
GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL);

// 快广播超时后切换到慢广播
GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL); // 1600=1s
GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 通知未发送 | 客户端未使能 CCCD | 等待 `HEARTRATE_MEAS_NOTI_ENABLED` 事件 |
| 服务注册失败 | 顺序错误 | GAP -> GATT -> HeartRate -> DevInfo -> Battery |
| 心率数据格式错误 | flags 与数据不匹配 | 根据 flags 字段正确填充数据 |
| 连接后断开 | 配对失败 | 检查配对参数和 I/O 能力配置 |

## 参考项目

- `resources/EXAM/BLE/HeartRate/` — 心率 Profile 例程
- `resources/EXAM/BLE/CyclingSensor/` — 骑行传感器 Profile
- `resources/EXAM/BLE/RunningSensor/` — 跑步传感器 Profile
