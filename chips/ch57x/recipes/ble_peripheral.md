# 创建 BLE 从机（Peripheral）应用

> **适用摘要**: 创建 BLE 从机角色，实现广播、GATT 服务、特征值读写和通知功能。

## 触发意图

- "创建 BLE 从机"
- "做一个 BLE 外设"
- "BLE peripheral 应用"
- "实现 GATT 服务"
- "BLE 广播和通知"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/Peripheral/` |
| 知识 | BLE GAP/GATT 基础概念 |
| 工具 | nRF Connect 手机 App（用于测试） |

## 调用链

```
Step 1: 配置 config.h（内存、连接数、TX 功率）
Step 2: 定义 GATT 服务和特征值（Profile/）
Step 3: 实现 BLE 初始化序列
Step 4: 实现广播数据配置
Step 5: 实现状态回调和事件处理
Step 6: 实现特征值读写回调
Step 7: 实现通知发送
Step 8: 编写主循环
```

## 分步说明

### Step 1: config.h

```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*6)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### Step 2: 定义 GATT 服务

参考 `resources/EXAM/BLE/Peripheral/Profile/gattprofile.c`。

**属性表定义**:

```c
// 128-bit 自定义服务 UUID（小端序）
static uint8_t simpleProfileServiceUUID[ATT_UUID_SIZE] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

// 特征值属性表
static gattAttribute_t simpleProfileAttrTbl[] = {
    // 服务声明
    { {ATT_BT_UUID_SIZE, primaryServiceUUID},
      GATT_PERMIT_READ, 0, (uint8_t *)&simpleProfileService },

    // 特征值 1 声明
    { {ATT_BT_UUID_SIZE, characterUUID},
      GATT_PERMIT_READ, 0, &simpleProfileChar1Props },

    // 特征值 1 值
    { {ATT_UUID_SIZE, simpleProfilechar1UUID},
      GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, simpleProfileChar1 },

    // 特征值 1 通知描述符
    { {ATT_BT_UUID_SIZE, clientCharCfgUUID},
      GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, simpleProfileChar1Config },
};
```

**注册服务**:

```c
void SimpleProfile_AddService(uint32_t services) {
    GATTServApp_RegisterService(simpleProfileAttrTbl,
                                 GATT_NUM_ATTRS(simpleProfileAttrTbl),
                                 GATT_MAX_ENCRYPT_KEY_SIZE,
                                 &simpleProfileCBs);
}
```

### Step 3: BLE 初始化序列

```c
static uint8_t simpleProfile_TaskID;

void Peripheral_Init(void) {
    // 1. 注册 TMOS 事件处理函数
    simpleProfile_TaskID = TMOS_ProcessEventRegister(SimpleProfile_ProcessEvent);

    // 2. 初始化 GAP Peripheral 角色
    GAPRole_PeripheralInit();

    // 3. 设置 GAP 参数
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, 160);  // 100ms
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, 160);

    // 4. 设置连接参数
    uint16_t minConnInterval = 80;   // 100ms
    uint16_t maxConnInterval = 160;  // 200ms
    uint16_t slaveLatency = 0;
    uint16_t timeout = 600;          // 6s
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t), &minConnInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t), &maxConnInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t), &slaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t), &timeout);

    // 5. 设置广播数据
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);

    // 6. 启动设备
    GAPRole_PeripheralStartDevice(simpleProfile_TaskID, &gapBondCBs, &simpleProfileRoleCBs);
}
```

### Step 4: 广播数据配置

```c
// 广播数据（≤31 字节）
static uint8_t advertData[] = {
    0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_GENERAL_DISCOVERABLE | GAP_ADTYPE_BREDR_NOT_SUPPORTED,
    0x07, GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'C', 'H', '5', '7', '3', 'x',
};

// 扫描响应数据
static uint8_t scanRspData[] = {
    0x05, GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
    LO_UINT16(80), HI_UINT16(80),   // min
    LO_UINT16(160), HI_UINT16(160), // max
};
```

### Step 5: 状态回调

```c
static void peripheralStateNotificationCB(gapRole_States_t newState, gapRoleEvent_t *pEvent) {
    switch (newState) {
        case GAPROLE_STARTED:
            PRINT("BLE started\n");
            break;
        case GAPROLE_ADVERTISING:
            PRINT("Advertising...\n");
            break;
        case GAPROLE_CONNECTED:
            connHandle = pEvent->linkCmpl.connectionHandle;
            PRINT("Connected, handle=%d\n", connHandle);
            break;
        case GAPROLE_DISCONNECTED:
            connHandle = INVALID_CONNHANDLE;
            PRINT("Disconnected\n");
            // 重新开始广播
            {
                uint8_t enable = TRUE;
                GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
            }
            break;
        default:
            break;
    }
}
```

### Step 6: 特征值读写回调

```c
static uint8_t simpleProfile_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen) {
    if (pAttr->type.len == ATT_BT_UUID_SIZE) {
        // 标准 UUID 处理
    } else {
        // 128-bit UUID 处理
        if (osal_memcmp(pAttr->type.uuid, simpleProfilechar1UUID, ATT_UUID_SIZE)) {
            *pLen = 1;
            pValue[0] = *pAttr->pValue;
            return SUCCESS;
        }
    }
    return ATT_ERR_ATTR_NOT_FOUND;
}

static uint8_t simpleProfile_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t len, uint16_t offset) {
    if (osal_memcmp(pAttr->type.uuid, simpleProfilechar1UUID, ATT_UUID_SIZE)) {
        if (offset + len > 1) return ATT_ERR_INVALID_VALUE_SIZE;
        osal_memcpy(pAttr->pValue, pValue, len);
        // 通知应用层数据已更新
        tmos_set_event(simpleProfile_TaskID, SIMPLE_PROFILE_CHAR1_EVT);
        return SUCCESS;
    }
    // CCCD 处理（通知开关）
    if (osal_memcmp(pAttr->type.uuid, clientCharCfgUUID, ATT_BT_UUID_SIZE)) {
        GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset,
                                        GATT_CLIENT_CFG_NOTIFY);
        return SUCCESS;
    }
    return ATT_ERR_ATTR_NOT_FOUND;
}
```

### Step 7: 发送通知

```c
// 检查通知是否启用
static uint8_t simpleProfileChar1Config[GATT_MAX_NUM_CONN];

void SimpleProfile_Notify(uint8_t value) {
    if (connHandle == INVALID_CONNHANDLE) return;

    // 检查 CCCD
    if (simpleProfileChar1Config[0] & GATT_CLIENT_CFG_NOTIFY) {
        attHandleValueNoti_t noti;
        noti.handle = simpleProfileAttrTbl[2].handle;
        noti.len = 1;
        noti.pValue = &value;
        GATT_Notification(connHandle, &noti, FALSE);
    }
}
```

### Step 8: 主循环

```c
int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#if (DCDC_ENABLE)
    PWR_DCDCCfg(ENABLE);
#endif
#ifdef DEBUG
    UART1_DefInit();
#endif
    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();
    Main_Circulation();
    return 0;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 广播无法被手机发现 | 广播数据超过 31 字节 | 减少广播数据，长名放扫描响应 |
| 连接后立即断开 | 连接参数不合理 | 使用推荐参数（100-200ms 间隔） |
| 通知发送失败 | CCCD 未检查 | 发送前检查 `simpleProfileChar1Config` |
| GATT 写入无响应 | 回调返回错误码 | 检查 UUID 匹配和 offset/len |

## 参考项目

- `resources/EXAM/BLE/Peripheral/` — 完整从机示例
- `resources/EXAM/BLE/HeartRate/` — 心率服务示例（标准 Profile）
- `resources/ble_api.md` — BLE API 速查
