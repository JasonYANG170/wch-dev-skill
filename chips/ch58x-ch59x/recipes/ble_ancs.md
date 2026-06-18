# BLE ANCS (Apple Notification Center Service)

> **适用摘要**: 实现 CH58x/CH59x BLE ANCS 客户端，连接 iPhone/iPad 后接收来电、短信、应用通知等 iOS 通知，并可执行确认/忽略等操作

## 触发意图

- "BLE ANCS"
- "Apple 通知中心"
- "iOS 通知"
- "ANCS 客户端"
- "iPhone 通知转发"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `ancs.h`, `ancs_client.h`, `devinfoservice.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 8KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |
| 配对要求 | 必须与 iPhone 配对绑定后才能使用 ANCS |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 ANCS 客户端 (bt_ancs_client_init)
Step 4: 注册 ANCS 属性缓冲区
Step 5: 注册 ANCS 订阅回调
Step 6: 初始化 GAP Peripheral Role (用于被 iPhone 发现)
Step 7: 配对绑定后发现 ANCS 服务
Step 8: 订阅 Notification Source 和 Data Source
Step 9: 接收并解析通知
Step 10: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "ancs.h"
#include "ancs_client.h"

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

    HalKeyConfig(button_changed);  // 按键配置
    ancs_init();                    // ANCS 客户端初始化
    ancs_subscribe_cb_register(&subscribe);  // 注册订阅回调

    GAPRole_PeripheralInit();
    peripheral_ancs_client_init();  // GAP Peripheral 初始化

    Main_Circulation();
    return 0;
}
```

### Step 2: ANCS 客户端初始化

```c
static int ancs_init(void) {
    int err;

    // 初始化 ANCS 客户端
    err = bt_ancs_client_init(&ancs_c);
    if(err) return err;

    // 注册通知属性缓冲区
    err = bt_ancs_register_attr(&ancs_c,
        BT_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
        attr_appid, ATTR_DATA_SIZE);

    err = bt_ancs_register_attr(&ancs_c,
        BT_ANCS_NOTIF_ATTR_ID_TITLE,
        attr_title, ATTR_DATA_SIZE);

    err = bt_ancs_register_attr(&ancs_c,
        BT_ANCS_NOTIF_ATTR_ID_MESSAGE,
        attr_message, ATTR_DATA_SIZE);

    // 注册应用属性
    err = bt_ancs_register_app_attr(&ancs_c,
        BT_ANCS_APP_ATTR_ID_DISPLAY_NAME,
        attr_disp_name, sizeof(attr_disp_name));

    return err;
}
```

### Step 3: 订阅回调注册

```c
static struct bt_ancs_subscribe_cb subscribe = {
    .data_source = bt_ancs_data_source_handler,
    .notification_source = bt_ancs_notification_source_handler,
    .write_response = bt_ancs_write_response_handler,
};

// 在 main 中注册
ancs_subscribe_cb_register(&subscribe);
```

### Step 4: 通知源处理

```c
static void bt_ancs_notification_source_handler(
    struct bt_ancs_client *ancs_c,
    int err,
    const struct bt_ancs_evt_notif *notif)
{
    if(!err) {
        notification_latest = *notif;
        // 打印通知信息
        PRINT("Event:       %s\n", lit_eventid[notif->evt_id]);
        PRINT("Category ID: %s\n", lit_catid[notif->category_id]);
        PRINT("Category Cnt:%u\n", notif->category_count);
        PRINT("UID:         %u\n", notif->notif_uid);
    }
}
```

### Step 5: 数据源处理

```c
static void bt_ancs_data_source_handler(
    struct bt_ancs_client *ancs_c,
    const struct bt_ancs_attr_response *response)
{
    switch(response->command_id) {
        case BT_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES:
            notif_attr_latest = response->attr;
            PRINT("%s: %s\n", lit_attrid[response->attr.attr_id],
                   (char *)response->attr.attr_data);
            break;

        case BT_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES:
            PRINT("%s: %s\n", lit_appid[response->attr.attr_id],
                   (char *)response->attr.attr_data);
            break;
    }
}
```

### Step 6: 配对后发现 ANCS 服务

```c
// 配对绑定成功后触发服务发现
static void AncsPairStateCB(uint16_t connHandle, uint8_t state, uint8_t status) {
    if(state == GAPBOND_PAIRING_STATE_BONDED && status == SUCCESS) {
        tmos_set_event(ancs_taskid, START_DISCOVER_EVT);
    }
}

// 服务发现流程: ANCS 服务 UUID -> 特征值 -> CCCD
static void AncsApp_discoverService(gattMsgEvent_t *pMsg) {
    // 1. 发现 ANCS 服务 (UUID: 7905F431-B5CE-4E99-A40F-4B1E122D00D0)
    GATT_DiscPrimaryServiceByUUID(ancs_c.conn, uuid, ATT_UUID_SIZE, ancs_taskid);

    // 2. 发现特征值 (Notification Source, Control Point, Data Source)
    GATT_DiscAllChars(connHandle, svcStartHdl, svcEndHdl, ancs_taskid);

    // 3. 发现 CCCD
    GATT_DiscAllCharDescs(connHandle, startHdl, endHdl, ancs_taskid);
}
```

### Step 7: 订阅通知源和数据源

```c
// 服务发现完成后订阅
tmos_set_event(ancs_taskid, START_SUBSCRIPE_NOTIDATA_EVT);
tmos_set_event(ancs_taskid, START_SUBSCRIPE_NOTISRC_EVT);

// 订阅通知源
bt_ancs_subscribe_notification_source(&ancs_c, bt_ancs_subscribe->notification_source);

// 订阅数据源
bt_ancs_subscribe_data_source(&ancs_c, bt_ancs_subscribe->data_source);
```

### Step 8: 请求通知属性和执行操作

```c
// 请求通知属性 (按键触发)
bt_ancs_request_attrs(&ancs_c, &notification_latest, bt_ancs_write_response_handler);

// 执行正面操作 (确认)
bt_ancs_notification_action(&ancs_c, notification_latest.notif_uid,
                             BT_ANCS_ACTION_ID_POSITIVE, bt_ancs_write_response_handler);

// 执行负面操作 (忽略)
bt_ancs_notification_action(&ancs_c, notification_latest.notif_uid,
                             BT_ANCS_ACTION_ID_NEGATIVE, bt_ancs_write_response_handler);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 服务发现失败 | 未配对绑定 | 确保与 iPhone 配对成功后再发现服务 |
| 通知不触发 | CCCD 未使能 | 检查 `bt_ancs_subscribe_notification_source` 是否成功 |
| 属性请求失败 | Control Point 写失败 | 检查 `handle_cp` 是否正确 |
| 堆内存不足 | ANCS 数据缓冲区大 | 增大 `BLE_MEMHEAP_SIZE` 到 8KB 以上 |
| 断连后重连失败 | 未重新初始化 ANCS | 断连回调中调用 `ancs_reinit` |

## 参考项目

- `resources/EXAM/BLE/peripheral_ancs_client/` — ANCS 客户端例程
