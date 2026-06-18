# BLE 多连接主机 (MultiCentral)

> **适用摘要**: 实现 CH58x/CH59x BLE 主机同时连接多个从机设备，支持独立的服务发现和数据读写

## 触发意图

- "BLE 多连接"
- "多主机"
- "同时连接多个设备"
- "BLE MultiCentral"
- "多从机管理"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `multiCentral.h`, `gattprofile.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 8KB, `CENTRAL_MAX_CONNECTION` 需定义 |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小、最大连接数)
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Central Role (GAPRole_CentralInit)
Step 4: 初始化连接项列表 (每个连接独立 taskID)
Step 5: 初始化 GATT Client 并注册通知
Step 6: 启动扫描，发现目标设备后建立连接
Step 7: 为每个连接独立执行服务发现和数据读写
Step 8: 运行 Main_Circulation
```

## 分步说明

### Step 1: 连接项数据结构

```c
// 每个连接维护独立的状态
typedef struct {
    uint8_t  taskID;               // 独立的 TMOS 任务 ID
    uint16_t connHandle;           // 连接句柄
    uint8_t  state;                // BLE_STATE_IDLE/CONNECTED
    uint8_t  discState;            // 服务发现状态
    uint16_t charHdl;              // 特征值句柄
    uint16_t svcStartHdl, svcEndHdl;
    uint16_t cccHdl;               // CCCD 句柄
    uint8_t  procedureInProgress;
} centralConnItem_t;

// 目标设备地址列表
static peerAddrDefItem_t PeerAddrDef[CENTRAL_MAX_CONNECTION] = {
    {0x02, 0x02, 0x03, 0xE4, 0xC2, 0x84},
    {0x03, 0x02, 0x03, 0xE4, 0xC2, 0x84},
    {0x04, 0x02, 0x03, 0xE4, 0xC2, 0x84}
};
```

### Step 2: 初始化连接项

```c
static void centralInitConnItem(uint8_t task_id, centralConnItem_t *list) {
    for(uint8_t i = 0; i < CENTRAL_MAX_CONNECTION; i++) {
        // 每个连接注册独立的 TMOS 事件处理函数
        list[i].taskID = TMOS_ProcessEventRegister(Central_ProcessEvent);
        list[i].connHandle = GAP_CONNHANDLE_INIT;
        list[i].state = BLE_STATE_IDLE;
        list[i].discState = BLE_DISC_STATE_IDLE;
        list[i].procedureInProgress = FALSE;
        list[i].charHdl = 0;
    }
}
```

### Step 3: GATT Client 初始化

```c
void Central_Init(void) {
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);

    GAP_SetParamValue(TGAP_DISC_SCAN, 2400);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, 20);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, 100);

    // 配对参数
    GAPBondMgr_SetParameter(GAPBOND_CENT_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);
    GAPBondMgr_SetParameter(GAPBOND_CENT_PAIRING_MODE, sizeof(uint8_t), &pairMode);

    centralInitConnItem(centralTaskId, centralConnList);
    GATT_InitClient();
    GATT_RegisterForInd(centralTaskId);

    tmos_set_event(centralTaskId, START_DEVICE_EVT);
}
```

### Step 4: 连接建立和多连接管理

```c
// 发现目标设备后建立连接
GAPRole_CentralEstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE,
                              DEFAULT_LINK_WHITE_LIST,
                              centralDevList[i].addrType,
                              centralDevList[i].addr);

// 在 GAP_LINK_ESTABLISHED_EVENT 中分配连接项
for(connItem = 0; connItem < CENTRAL_MAX_CONNECTION; connItem++) {
    if(centralConnList[connItem].connHandle == GAP_CONNHANDLE_INIT)
        break;
}
centralConnList[connItem].state = BLE_STATE_CONNECTED;
centralConnList[connItem].connHandle = pEvent->linkCmpl.connectionHandle;

// 如果还有空闲连接槽，继续扫描
for(connItem = 0; connItem < CENTRAL_MAX_CONNECTION; connItem++) {
    if(centralConnList[connItem].connHandle == GAP_CONNHANDLE_INIT)
        break;
}
if(connItem < CENTRAL_MAX_CONNECTION) {
    GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                   DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                   DEFAULT_DISCOVERY_WHITE_LIST);
}
```

### Step 5: 独立的服务发现和数据读写

```c
// 每个连接的事件由其独立的 taskID 处理
if(task_id == centralConnList[CONNECT0_ITEM].taskID) {
    return connect0_ProcessEvent(task_id, events);
} else if(task_id == centralConnList[CONNECT1_ITEM].taskID) {
    // 连接1的事件处理
}

// 写操作示例
attWriteReq_t req;
req.cmd = FALSE;
req.sig = FALSE;
req.handle = centralConnList[connItem].charHdl;
req.len = 1;
req.pValue = GATT_bm_alloc(centralConnList[connItem].connHandle, ATT_WRITE_REQ, req.len, NULL, 0);
*req.pValue = centralCharVal;
GATT_WriteCharValue(centralConnList[connItem].connHandle, &req, centralTaskId);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 连接数超限 | 未检查空闲连接槽 | 连接前检查 `connHandle == GAP_CONNHANDLE_INIT` |
| 事件混淆 | 所有连接共用 taskID | 每个连接注册独立的 `TMOS_ProcessEventRegister` |
| 堆内存不足 | 多连接消耗更多堆 | 增大 `BLE_MEMHEAP_SIZE` 到 8KB 以上 |
| 断连后不重连 | 未重置连接项状态 | 断连回调中重置 `connHandle = GAP_CONNHANDLE_INIT` 并重启扫描 |

## 参考项目

- `resources/EXAM/BLE/MultiCentral/` — 多连接主机例程 (默认连接3个从机)
