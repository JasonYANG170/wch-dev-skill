# BLE 速度测试（SpeedTest Peripheral）

> **适用摘要**: 实现 BLE 吞吐量测试，通过连接事件回调连续发送通知，测量实际传输速率。

## 触发意图

- "BLE 速度测试"
- "BLE 吞吐量"
- "BLE throughput test"
- "SpeedTest Peripheral"
- "BLE 传输速率测量"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/SpeedTest_Peripheral/` |
| 知识 | BLE 连接事件、MTU 协商、PHY 切换 |
| 工具 | nRF Connect 手机 App（观察通知数据速率） |

## 调用链

```
Step 1: 配置 config.h（BLE 缓冲区、连接参数）
Step 2: 定义 GATT 服务（SimpleProfile，Char4 用于通知）
Step 3: 实现 BLE 初始化序列（连接参数、PHY 更新）
Step 4: 实现连接事件回调（每个连接事件批量发通知）
Step 5: 实现速度统计定时器
Step 6: 编写主循环
```

## 分步说明

### Step 1: config.h

```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*6)
#define BLE_BUFF_MAX_LEN        244
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### Step 2: 定义 GATT 服务

参考 `resources/EXAM/BLE/SpeedTest_Peripheral/Profile/gattprofile.c`。

使用 SimpleProfile（UUID 0xFFE0），其中 Char4（0xFFE4）用于通知发送：

```c
// 服务 UUID
#define SIMPLEPROFILE_SERV_UUID  0xFFE0

// Char4 用于速度测试通知
#define SIMPLEPROFILE_CHAR4      3
#define SIMPLEPROFILE_CHAR4_LEN  1
```

注册服务：

```c
GGS_AddService(GATT_ALL_SERVICES);           // GAP
GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT
SimpleProfile_AddService(GATT_ALL_SERVICES); // SimpleProfile
```

### Step 3: BLE 初始化与连接参数

```c
static uint8_t Peripheral_TaskID;
static uint16_t peripheralMTU = 23;
static uint8_t speed_test_enable = FALSE;
static uint8_t speed_test_buf[244] = {0};
static uint32_t peripheralTxDataLen = 0;

void Peripheral_Init(void) {
    Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);

    // 广播间隔 50ms（80 * 625us）
    uint16_t advInt = 80;
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

    // 连接间隔 7.5ms（6 * 1.25ms）—— 最小值以最大化吞吐
    uint16_t minConnInterval = 6;
    uint16_t maxConnInterval = 6;
    uint16_t slaveLatency = 0;
    uint16_t timeout = 100;  // 1s
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t), &minConnInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t), &maxConnInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t), &slaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t), &timeout);

    // 注册连接事件回调
    LL_ConnectEventRegister(peripheralConnectEventCB);

    GAPRole_PeripheralStartDevice(Peripheral_TaskID, &gapBondCBs, &peripheralRoleCBs);
}
```

### Step 4: 连接事件回调（核心速度测试逻辑）

每个 BLE 连接事件触发时，连续发送通知直到缓冲区满：

```c
static void peripheralConnectEventCB(uint32_t timeUs) {
    if (speed_test_enable) {
        // 在每个连接事件中持续发送通知
        while (1) {
            if (peripheralChar4Notify(speed_test_buf, peripheralMTU - 3)) {
                break;  // 发送失败（缓冲区满），退出
            } else {
                peripheralTxDataLen += peripheralMTU - 3;
            }
        }
    }
}
```

通知发送辅助函数：

```c
static uint8_t peripheralChar4Notify(uint8_t *pValue, uint16_t len) {
    attHandleValueNoti_t noti;
    uint8_t state = SUCCESS;
    if (len > (peripheralMTU - 3)) {
        return bleInvalidRange;
    }
    noti.len = len;
    noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle,
                                 ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
    if (noti.pValue) {
        tmos_memcpy(noti.pValue, pValue, noti.len);
        state = simpleProfile_Notify(peripheralConnList.connHandle, &noti);
        if (state != SUCCESS) {
            GATT_bm_free((gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI);
        }
    } else {
        return bleMemAllocError;
    }
    return state;
}
```

### Step 5: 速度统计与 PHY/参数更新

```c
#define SBP_SPEED_TEST_EVT_PERIOD   3200   // 1.6s 统计周期
#define SBP_PARAM_UPDATE_DELAY      2800   // 连接后延迟请求参数更新
#define SBP_PHY_UPDATE_DELAY        2400   // 连接后延迟请求 PHY 更新

uint16_t Peripheral_ProcessEvent(uint8_t task_id, uint16_t events) {
    // ... SYS_EVENT_MSG, SBP_START_DEVICE_EVT 处理 ...

    if (events & SBP_SPEED_TEST_EVT) {
        if (speed_test_enable) {
            // 打印本周期吞吐量
            PRINT("Tx: %dB/s\n", peripheralTxDataLen);
            peripheralTxDataLen = 0;
        } else {
            // 首次触发，开始测试
            PRINT("start speed test..\n");
            speed_test_enable = TRUE;
        }
        tmos_start_task(Peripheral_TaskID, SBP_SPEED_TEST_EVT, 1600);
        return (events ^ SBP_SPEED_TEST_EVT);
    }

    if (events & SBP_PARAM_UPDATE_EVT) {
        // 请求连接参数更新
        GAPRole_PeripheralConnParamUpdateReq(peripheralConnList.connHandle,
            6, 6, 0, 100, Peripheral_TaskID);
        return (events ^ SBP_PARAM_UPDATE_EVT);
    }

    if (events & SBP_PHY_UPDATE_EVT) {
        // 请求 2M PHY 以提升吞吐量
        GAPRole_UpdatePHY(peripheralConnList.connHandle, 0,
            GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, 0);
        return (events ^ SBP_PHY_UPDATE_EVT);
    }

    return 0;
}
```

连接建立后启动定时器：

```c
static void Peripheral_LinkEstablished(gapRoleEvent_t *pEvent) {
    gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;
    peripheralConnList.connHandle = event->connectionHandle;
    peripheralMTU = 23;  // 重置为默认 MTU

    // 启动速度测试定时器
    tmos_start_task(Peripheral_TaskID, SBP_SPEED_TEST_EVT, SBP_SPEED_TEST_EVT_PERIOD);
    // 延迟请求连接参数更新
    tmos_start_task(Peripheral_TaskID, SBP_PARAM_UPDATE_EVT, SBP_PARAM_UPDATE_DELAY);
    // 延迟请求 PHY 更新（2M）
    tmos_start_task(Peripheral_TaskID, SBP_PHY_UPDATE_EVT, SBP_PHY_UPDATE_DELAY);
}
```

### Step 6: MTU 交换与主循环

```c
// 在 TMOS 消息处理中跟踪 MTU 变化
static void Peripheral_ProcessTMOSMsg(tmos_event_hdr_t *pMsg) {
    switch (pMsg->event) {
        case GATT_MSG_EVENT: {
            gattMsgEvent_t *pMsgEvent = (gattMsgEvent_t *)pMsg;
            if (pMsgEvent->method == ATT_MTU_UPDATED_EVENT) {
                peripheralMTU = pMsgEvent->msg.exchangeMTUReq.clientRxMTU;
                PRINT("mtu exchange: %d\n", peripheralMTU);
            }
            break;
        }
    }
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
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
| 吞吐量远低于预期 | 连接间隔过大 | 设为最小值 6（7.5ms），关闭 slave latency |
| 通知发送失败 | GATT_bm_alloc 返回 NULL | 检查 BLE_MEMHEAP_SIZE 和 BLE_BUFF_NUM |
| PHY 更新失败 | 对端不支持 2M PHY | 确认手机支持 BLE 5.0 |
| MTU 未协商 | 未处理 ATT_MTU_UPDATED_EVENT | 在 GATT_MSG_EVENT 中更新 peripheralMTU |
| 连接后立即断开 | supervision timeout 过短 | 设 timeout 为 100（1s）或更大 |

## 速度优化要点

| 参数 | 推荐值 | 说明 |
|---|---|---|
| 连接间隔 | 6（7.5ms） | BLE 规范最小值 |
| Slave Latency | 0 | 不跳过连接事件 |
| MTU | 247（协商后） | 最大通知载荷 = MTU - 3 |
| PHY | LE 2M | 速率翻倍（需双端支持） |
| BLE_BUFF_NUM | 10+ | 确保发送队列不空 |
| 连接事件内发送 | while 循环连续发 | 利用每个连接事件的最大容量 |

## 参考项目

- `resources/EXAM/BLE/SpeedTest_Peripheral/` — 完整速度测试示例
- `resources/EXAM/BLE/Peripheral/` — 基础从机示例（对比参考）
- `resources/ble_api.md` — BLE API 速查
