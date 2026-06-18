# 创建 BLE HID 设备

> **适用摘要**: 创建 BLE HID 设备（键盘、鼠标、消费者控制、触摸），实现 HID 服务和报告发送。

## 触发意图

- "做一个 BLE 键盘"
- "BLE HID 设备"
- "BLE 鼠标"
- "蓝牙遥控器"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/HID_Keyboard/`, `HID_Mouse/`, `HID_Consumer/`, `HID_Touch/` |
| 知识 | USB HID 报告描述符概念 |

## 调用链

```
Step 1: 配置 config.h
Step 2: 定义 HID 报告描述符
Step 3: 注册 HID 服务
Step 4: 实现 HID 初始化
Step 5: 发送 HID 报告（键盘按键/鼠标移动/消费者控制）
```

## 分步说明

### Step 1: config.h

```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             TRUE
#define HAL_SLEEP               TRUE      // HID 设备通常需要低功耗
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*6)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### Step 2: HID 报告描述符

**键盘报告描述符**（参考 `HID_Keyboard/Profile/hidkbdservice.c`）:

```c
static const uint8_t hidReportMap[] = {
    // 键盘
    0x05, 0x01,     // Usage Page (Generic Desktop)
    0x09, 0x06,     // Usage (Keyboard)
    0xA1, 0x01,     // Collection (Application)
    0x85, 0x01,     //   Report ID (1)
    0x05, 0x07,     //   Usage Page (Key Codes)
    0x19, 0xE0,     //   Usage Minimum (224)
    0x29, 0xE7,     //   Usage Maximum (231)
    0x15, 0x00,     //   Logical Minimum (0)
    0x25, 0x01,     //   Logical Maximum (1)
    0x75, 0x01,     //   Report Size (1)
    0x95, 0x08,     //   Report Count (8)
    0x81, 0x02,     //   Input (Data, Variable, Absolute)
    0x95, 0x01,     //   Report Count (1)
    0x75, 0x08,     //   Report Size (8)
    0x81, 0x01,     //   Input (Constant)
    0x95, 0x06,     //   Report Count (6)
    0x75, 0x08,     //   Report Size (8)
    0x15, 0x00,     //   Logical Minimum (0)
    0x25, 0x65,     //   Logical Maximum (101)
    0x05, 0x07,     //   Usage Page (Key Codes)
    0x19, 0x00,     //   Usage Minimum (0)
    0x29, 0x65,     //   Usage Maximum (101)
    0x81, 0x00,     //   Input (Data, Array)
    0xC0,           // End Collection
};
```

**鼠标报告描述符**:

```c
static const uint8_t hidReportMap[] = {
    0x05, 0x01,     // Usage Page (Generic Desktop)
    0x09, 0x02,     // Usage (Mouse)
    0xA1, 0x01,     // Collection (Application)
    0x85, 0x01,     //   Report ID (1)
    0x09, 0x01,     //   Usage (Pointer)
    0xA1, 0x00,     //   Collection (Physical)
    0x05, 0x09,     //     Usage Page (Buttons)
    0x19, 0x01,     //     Usage Minimum (1)
    0x29, 0x03,     //     Usage Maximum (3)
    0x15, 0x00,     //     Logical Minimum (0)
    0x25, 0x01,     //     Logical Maximum (1)
    0x75, 0x01,     //     Report Size (1)
    0x95, 0x03,     //     Report Count (3)
    0x81, 0x02,     //     Input (Data, Variable, Absolute)
    0x75, 0x05,     //     Report Size (5)
    0x95, 0x01,     //     Report Count (1)
    0x81, 0x01,     //     Input (Constant)
    0x05, 0x01,     //     Usage Page (Generic Desktop)
    0x09, 0x30,     //     Usage (X)
    0x09, 0x31,     //     Usage (Y)
    0x15, 0x81,     //     Logical Minimum (-127)
    0x25, 0x7F,     //     Logical Maximum (127)
    0x75, 0x08,     //     Report Size (8)
    0x95, 0x02,     //     Report Count (2)
    0x81, 0x06,     //     Input (Data, Variable, Relative)
    0xC0,           //   End Collection
    0xC0,           // End Collection
};
```

### Step 3: 注册 HID 服务

```c
void HidEmu_Init(void) {
    uint8_t taskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);

    // 注册标准服务
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);
    DevInfo_AddService();
    Batt_AddService();       // 电池服务（可选）
    Hid_AddService();        // HID 服务
    ScanParam_AddService();  // 扫描参数服务（可选）

    // 设置 HID 参数
    Hid_SetParameter(HID_RPT_ID_MAP_KEY_IN_SIZE, sizeof(keyReportId));
    Hid_SetParameter(HID_RPT_ID_MAP_KEY_OUT_SIZE, sizeof(ledReportId));
}
```

### Step 4: 发送 HID 报告

**键盘按键**:

```c
// 报告格式: [modifier, reserved, key1, key2, key3, key4, key5, key6]
static uint8_t keyboardReport[8] = {0};

void HID_SendKey(uint8_t modifier, uint8_t keycode) {
    keyboardReport[0] = modifier;
    keyboardReport[2] = keycode;

    Hid_SetParameter(HID_RPT_ID_MAP_KEY_IN, sizeof(keyboardReport), keyboardReport);
}

// 按键释放
void HID_ReleaseKey(void) {
    osal_memset(keyboardReport, 0, sizeof(keyboardReport));
    Hid_SetParameter(HID_RPT_ID_MAP_KEY_IN, sizeof(keyboardReport), keyboardReport);
}

// 使用示例: 发送 'A' 键
HID_SendKey(0, 0x04);     // 按下 'A'
HID_ReleaseKey();          // 释放
```

**鼠标移动**:

```c
// 报告格式: [buttons, x, y]
static uint8_t mouseReport[3] = {0};

void HID_SendMouse(int8_t x, int8_t y, uint8_t buttons) {
    mouseReport[0] = buttons;
    mouseReport[1] = x;
    mouseReport[2] = y;
    Hid_SetParameter(HID_RPT_ID_MAP_KEY_IN, sizeof(mouseReport), mouseReport);
}
```

**消费者控制**（音量、媒体键）:

```c
// 报告格式: [key_lo, key_hi]
static uint8_t consumerReport[2] = {0};

void HID_SendConsumer(uint16_t key) {
    consumerReport[0] = LO_UINT16(key);
    consumerReport[1] = HI_UINT16(key);
    Hid_SetParameter(HID_RPT_ID_MAP_KEY_IN, sizeof(consumerReport), consumerReport);
}

// 常用消费者控制键
#define HID_CONSUMER_VOLUME_UP      0x00E9
#define HID_CONSUMER_VOLUME_DOWN    0x00EA
#define HID_CONSUMER_MUTE           0x00E2
#define HID_CONSUMER_PLAY_PAUSE     0x00CD
#define HID_CONSUMER_SCAN_NEXT      0x00B5
#define HID_CONSUMER_SCAN_PREV      0x00B6
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 设备无法配对 | HID 服务未正确注册 | 确保 Hid_AddService() 在 GAP 之后调用 |
| 按键无响应 | 报告描述符与实际报告不匹配 | 确保报告长度和格式与描述符一致 |
| 连接后断开 | 未实现电池服务 | 添加 Batt_AddService() 或在 config 中禁用 |
| 高延迟 | 连接间隔太大 | 设置较小的连接间隔（15-30ms） |

## 参考项目

- `resources/EXAM/BLE/HID_Keyboard/` — 键盘示例
- `resources/EXAM/BLE/HID_Mouse/` — 鼠标示例
- `resources/EXAM/BLE/HID_Consumer/` — 消费者控制示例
- `resources/EXAM/BLE/HID_Touch/` — 触摸板示例
