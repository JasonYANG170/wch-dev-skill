# BLE 速率测试 (Speed Test)

> **适用摘要**: 实现 CH58x/CH59x BLE 吞吐量测试，包含 Central 端和 Peripheral 端，通过通知持续发送数据并统计接收速率

## 触发意图

- "BLE 速率测试"
- "BLE 吞吐量"
- "BLE 速度测试"
- "BLE SpeedTest"
- "BLE 带宽测试"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `central.h` 或 `peripheral.h`, `gattprofile.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 8KB, `BLE_BUFF_MAX_LEN` 建议 251 |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |
| 配对设备 | 需要一对设备: SpeedTest_Central + SpeedTest_Peripheral |

## 调用链

```
Step 1: 配置 config.h (增大 MTU 和缓冲区)
Step 2: Central 端: 初始化 BLE, 扫描, 连接, 使能通知
Step 3: Peripheral 端: 初始化 BLE, 广播, 注册 GATT 服务
Step 4: Central 端: 请求 2M PHY, 写 CCCD 使能通知
Step 5: Peripheral 端: 定时通过通知发送数据
Step 6: Central 端: 统计每秒接收字节数并打印
```

## 分步说明

### Step 1: config.h 配置 (两端相同)

```c
#define BLE_MEMHEAP_SIZE    (1024*8)
#define BLE_BUFF_MAX_LEN    251     // 最大数据长度
#define BLE_TX_POWER        TX_POW_0_DBm
```

### Step 2: Central 端初始化

```c
void Central_Init(void) {
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);

    GAP_SetParamValue(TGAP_DISC_SCAN, 2400);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, 10);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, 10);  // 最小连接间隔

    // 配对参数
    GAPBondMgr_SetParameter(GAPBOND_CENT_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);

    GATT_InitClient();
    GATT_RegisterForInd(centralTaskId);

    tmos_set_event(centralTaskId, START_DEVICE_EVT);
}
```

### Step 3: Central 端连接后操作

```c
case GAP_LINK_ESTABLISHED_EVENT:
    centralConnHandle = pEvent->linkCmpl.connectionHandle;

    // 请求 MTU 交换
    attExchangeMTUReq_t req = { .clientRxMTU = BLE_BUFF_MAX_LEN - 4 };
    GATT_ExchangeMTU(centralConnHandle, &req, centralTaskId);

    // 服务发现
    tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, 1600);

    // 请求 2M PHY
    tmos_start_task(centralTaskId, START_PHY_UPDATE_EVT, 2400);

    // 启动速率统计定时器
    tmos_start_task(centralTaskId, START_SPEED_TEST_EVT, 1600);
    break;
```

### Step 4: Central 端速率统计

```c
// 收到通知时累加字节数
case ATT_HANDLE_VALUE_NOTI:
    centralRxDataLen += pMsg->msg.handleValueNoti.len;
    break;

// 定时打印速率
case START_SPEED_TEST_EVT:
    PRINT("Rx: %dB/s\n", centralRxDataLen);
    centralRxDataLen = 0;
    tmos_start_task(centralTaskId, START_SPEED_TEST_EVT, 1600);
    break;
```

### Step 5: Central 端写 CCCD 使能通知

```c
case START_WRITE_CCCD_EVT:
    attWriteReq_t req;
    req.cmd = FALSE;
    req.sig = FALSE;
    req.handle = centralCCCDHdl;
    req.len = 2;
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0);
    req.pValue[0] = 1;  // 使能通知
    req.pValue[1] = 0;
    GATT_WriteCharValue(centralConnHandle, &req, centralTaskId);
    break;
```

### Step 6: Peripheral 端持续发送通知

```c
// 定时发送通知
case SBP_SPEED_TEST_EVT:
    if(gapProfileState == GAPROLE_CONNECTED) {
        // 构造最大长度的通知数据
        attHandleValueNoti_t noti;
        noti.len = peripheralMTU - 3;
        noti.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
        if(noti.pValue) {
            tmos_memset(noti.pValue, 0x55, noti.len);
            SimpleProfile_Notify(gapConnHandle, &noti);
        }
        tmos_start_task(Peripheral_TaskID, SBP_SPEED_TEST_EVT, SBP_SPEED_TEST_EVT_PERIOD);
    }
    break;
```

### Step 7: PHY 更新

```c
case START_PHY_UPDATE_EVT:
    GAPRole_UpdatePHY(centralConnHandle, 0,
                       GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M,
                       GAP_PHY_OPTIONS_NOPRE);
    break;
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 速率很低 | 连接间隔太大 | 设置最小连接间隔 10 (12.5ms) |
| 数据丢失 | 通知队列满 | 控制发送频率，等待上一次完成 |
| MTU 未生效 | 未执行 MTU 交换 | 连接后立即调用 `GATT_ExchangeMTU` |
| PHY 未切换 | 不支持 2M PHY | 确认芯片和对端设备支持 BLE 5.0 |

## 参考项目

- `resources/EXAM/BLE/SpeedTest_Central/` — 速率测试主机端
- `resources/EXAM/BLE/SpeedTest_Peripheral/` — 速率测试从机端
