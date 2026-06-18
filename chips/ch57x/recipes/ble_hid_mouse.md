# BLE HID 鼠标（HID Mouse）

> **适用摘要**: 实现 BLE HID over GATT (HOGP) 鼠标设备，通过标准 HID 协议上报按键、移动和滚轮事件。

## 触发意图

- "BLE HID 鼠标"
- "HID Mouse"
- "HOGP 鼠标"
- "蓝牙鼠标"
- "BLE HID over GATT"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/HID_Mouse/` |
| 知识 | HID over GATT 协议、HID Report Descriptor |
| 工具 | 任意支持 BLE HID 的设备（手机、电脑） |

## 调用链

```
Step 1: 配置 BLE 参数
Step 2: 定义 HID Report Descriptor（鼠标描述符）
Step 3: 注册 GATT 服务（HID、Battery、DeviceInfo、ScanParam）
Step 4: 实现鼠标报告发送
Step 5: 实现 HID Dev 回调
Step 6: 编写主循环
```

## 分步说明

### Step 1: BLE 配置

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

### Step 2: HID Report Descriptor（鼠标描述符）

参考 `resources/EXAM/BLE/HID_Mouse/Profile/hidmouseservice.c`。

标准 HID 鼠标描述符定义 3 个按键 + X/Y/Wheel 轴：

```c
static CONST uint8 hidReportMap[] = {
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,        // USAGE (Mouse)
    0xa1, 0x01,        // COLLECTION (Application)
    0x09, 0x01,        //   USAGE (Pointer)
    0xa1, 0x00,        //   COLLECTION (Physical)
    // --- 按键部分 ---
    0x05, 0x09,        //     USAGE_PAGE (Button)
    0x19, 0x01,        //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,        //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,        //     LOGICAL_MINIMUM (0)
    0x25, 0x01,        //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //     REPORT_SIZE (1)
    0x95, 0x08,        //     REPORT_COUNT (8) — 3 按键 + 5 保留位
    0x81, 0x02,        //     INPUT (Data,Var,Abs)
    // --- 轴部分 ---
    0x05, 0x01,        //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,        //     USAGE (X)
    0x09, 0x31,        //     USAGE (Y)
    0x09, 0x38,        //     USAGE (Wheel)
    0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,        //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x03,        //     REPORT_COUNT (3) — X, Y, Wheel 各 1 字节
    0x81, 0x06,        //     INPUT (Data,Var,Rel)
    0xc0,              //     END_COLLECTION
    0xc0               // END_COLLECTION
};
```

**输入报告布局（4 字节）**:

| 字节 | 内容 | 说明 |
|---|---|---|
| buf[0] | Buttons | Bit0=左键, Bit1=右键, Bit2=中键 |
| buf[1] | X 轴 | 有符号相对位移 (-127..+127) |
| buf[2] | Y 轴 | 有符号相对位移 (-127..+127) |
| buf[3] | Wheel | 有符号滚轮值 (-127..+127) |

### Step 3: 注册 GATT 服务

HID 鼠标需要注册 4 个 GATT 服务：

```c
// HID 服务注册顺序
Hid_AddService();          // HID Service (0x1812) — 包含 Report Map、Report 特征值
Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8), &critical);
HidDev_Register(&hidEmuCfg, &hidEmuHidCBs);  // 注册 HID Dev 框架
```

服务列表：

| 服务 | UUID | 说明 |
|---|---|---|
| HID Service | 0x1812 | Report Map、Protocol Mode、Report 特征值 |
| Battery Service | 0x180F | 电池电量（通过 Include 引用） |
| Device Information | 0x180A | PnP ID、厂商信息 |
| Scan Parameters | 0x1813 | 扫描窗口参数 |

### Step 4: 鼠标报告发送

```c
#define HID_MOUSE_IN_RPT_LEN  4
#define MOUSE_BUTTON_NONE     0x00

// 发送鼠标报告
static void hidEmuSendMouseReport(uint8 buttons, uint8 X_data, uint8 Y_data) {
    uint8 buf[HID_MOUSE_IN_RPT_LEN];
    buf[0] = buttons;   // 按键状态
    buf[1] = X_data;    // X 轴相对位移
    buf[2] = Y_data;    // Y 轴相对位移
    buf[3] = 0;         // 滚轮

    HidDev_Report(HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                  HID_MOUSE_IN_RPT_LEN, buf);
}
```

定时发送示例（每秒移动一次）：

```c
if (events & START_REPORT_EVT) {
    hidEmuSendMouseReport(MOUSE_BUTTON_NONE, 2, 2);  // 向右下移动
    tmos_start_task(hidEmuTaskId, START_REPORT_EVT, 1000);
    return (events ^ START_REPORT_EVT);
}
```

### Step 5: HID Dev 回调

```c
// HID 报告回调（读写/通知使能）
static uint8 hidEmuRptCB(uint8 id, uint8 type, uint16 uuid,
                          uint8 oper, uint16 *pLen, uint8 *pData) {
    uint8 status = SUCCESS;
    if (oper == HID_DEV_OPER_WRITE) {
        status = Hid_SetParameter(id, type, uuid, *pLen, pData);
    } else if (oper == HID_DEV_OPER_READ) {
        status = Hid_GetParameter(id, type, uuid, pLen, pData);
    } else if (oper == HID_DEV_OPER_ENABLE) {
        // HID 通知使能后开始上报
        tmos_start_task(hidEmuTaskId, START_REPORT_EVT, 500);
    }
    return status;
}

// GAP 状态回调
static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t *pEvent) {
    switch (newState) {
        case GAPROLE_STARTED: {
            uint8 ownAddr[6];
            GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddr);
            GAP_ConfigDeviceAddr(ADDRTYPE_STATIC, ownAddr);
            PRINT("Initialized..\n");
            break;
        }
        case GAPROLE_CONNECTED: {
            gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;
            hidEmuConnHandle = event->connectionHandle;
            // 延迟请求连接参数更新
            tmos_start_task(hidEmuTaskId, START_PARAM_UPDATE_EVT, 12800);
            PRINT("Connected..\n");
            break;
        }
        case GAPROLE_WAITING:
            // 断开后重新广播
            {
                uint8 enable = TRUE;
                GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8), &enable);
            }
            break;
    }
}
```

### Step 6: 初始化与主循环

```c
// GAP 配置
static hidDevCfg_t hidEmuCfg = {
    60000,            // 空闲超时 60s
    HID_FEATURE_FLAGS // HID 特性标志
};

void HidEmu_Init(void) {
    hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);

    // 广播数据 —— 包含 HID 外观和 UUID
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);

    // Bond Manager —— 无 IO 能力，无需 MITM
    uint32 passkey = 0;
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = FALSE;
    uint8 ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32), &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8), &bonding);

    tmos_set_event(hidEmuTaskId, START_DEVICE_EVT);
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#ifdef DEBUG
    UART1_DefInit();
#endif
    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    HidDev_Init();
    HidEmu_Init();
    Main_Circulation();
    return 0;
}
```

## 广播数据配置

```c
static uint8 advertData[] = {
    0x02, GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    // HID 外观 —— 鼠标
    0x03, GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_MOUSE),
    HI_UINT16(GAP_APPEARE_HID_MOUSE),
};

static uint8 scanRspData[] = {
    0x0A, GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'H', 'I', 'D', ' ', 'M', 'o', 'u', 's', 'e',
    0x05, GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
    LO_UINT16(8), HI_UINT16(8),   // min 10ms
    LO_UINT16(8), HI_UINT16(8),   // max 10ms
    // 服务 UUID
    0x05, GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID), HI_UINT16(BATT_SERV_UUID),
    0x02, GAP_ADTYPE_POWER_LEVEL, 0,
};
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 设备无法被识别为鼠标 | 外观值错误 | 使用 `GAP_APPEARE_HID_MOUSE` |
| 配对失败 | IO 能力不匹配 | 设为 `NO_INPUT_NO_OUTPUT`，MITM=FALSE |
| 报告无法发送 | CCCD 未使能 | 等待 `HID_DEV_OPER_ENABLE` 回调后再发送 |
| 连接参数不兼容 | 间隔过短 | 部分主机要求 15ms 以上，使用 8（10ms） |
| Battery 服务报错 | 未先初始化 Batt | 调用 `Batt_SetParameter()` 设置 critical level |

## HID 初始化顺序

```
CH57X_BLEInit()  ->  HAL_Init()  ->  GAPRole_PeripheralInit()
->  HidDev_Init()  ->  HidEmu_Init()  ->  Main_Circulation()
```

注意：`HidDev_Init()` 必须在 `HidEmu_Init()` 之前调用。

## 参考项目

- `resources/EXAM/BLE/HID_Mouse/` — 完整 HID 鼠标示例
- `resources/EXAM/BLE/Peripheral/` — 基础从机示例
- `resources/ble_api.md` — BLE API 速查
