# BLE IoCHub 网络（IoCHub_NET）

> **适用摘要**: 实现 BLE IoCHub 数据透传网络，通过自定义帧协议实现设备与手机之间的双向数据交互。

## 触发意图

- "BLE IoCHub"
- "IoCHub 网络"
- "BLE 数据透传"
- "IoCHub_NET"
- "BLE 数据点控制"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/IoCHub_NET/` |
| 知识 | BLE GATT 通知、FIFO 缓冲、帧协议 |
| 工具 | nRF Connect 手机 App 或 IoCHub 配套 App |

## 调用链

```
Step 1: 配置 BLE 参数
Step 2: 定义 IoCHub GATT 服务
Step 3: 实现帧协议（编解码）
Step 4: 实现 FIFO 缓冲桥接
Step 5: 实现数据点处理
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

### Step 2: 定义 IoCHub GATT 服务

参考 `resources/EXAM/BLE/IoCHub_NET/APP/ble_iochub_service/`。

IoCHub 服务使用 UUID 0xFFF3，包含 TX（通知）和 RX（写入）两个特征值：

```c
#define BLE_IOCHUB_SERVICE_UUID  0xFFF3
#define BLE_IOCHUB_TXCHAR_UUID   0xFFF4   // 通知特征（设备 -> 手机）
#define BLE_IOCHUB_RXCHAR_UUID   0xFFF5   // 写入特征（手机 -> 设备）
```

服务初始化（传入事件回调）：

```c
GGS_AddService(GATT_ALL_SERVICES);
GATTServApp_AddService(GATT_ALL_SERVICES);
DevInfo_AddService();
ble_iochub_add_service(on_bleiochubServiceEvt);
```

### Step 3: 帧协议编解码

IoCHub 使用自定义 TLV 帧协议，帧头标识方向：

```c
// 帧头标识
#define BLE_REC_FST    0xBB    // 接收帧第一字节
#define BLE_REC_SEC    0x44    // 接收帧第二字节
#define BLE_SEND_FST   0x33    // 发送帧第一字节
#define BLE_SEND_SEC   0xCC    // 发送帧第二字节

// 帧结构长度
#define BLE_HEAD_LEN   2       // 帧头
#define BLE_FOMRAT_LEN 4       // dp_id + dp_type + data_len(2)
#define BLE_TAIL_LEN   1       // 校验和

// 数据点 ID
#define DP_ID_LIGHT    0x01    // 灯控
#define DP_ID_SWITCH   0x02    // 开关

// 数据点类型
#define DP_TYPE_RAW    0x00
#define DP_TYPE_BOOL   0x01
#define DP_TYPE_VALUE  0x02
#define DP_TYPE_STRING 0x03
#define DP_TYPE_ENUM   0x04
```

帧格式：`[帧头(2)] [dp_id(1)] [dp_type(1)] [data_len(2)] [data(N)] [checksum(1)]`

发送帧：

```c
uint8_t ble_send_frame(uint8_t frame_cmd, uint8_t frame_type,
                        uint8_t *frame_buf, uint8_t frame_dlc) {
    if (frame_buf == NULL) return 0x01;

    uint16_t send_len = frame_dlc + BLE_HEAD_LEN + BLE_FOMRAT_LEN + BLE_TAIL_LEN;
    uint8_t *send_value = (uint8_t *)tmos_msg_allocate(send_len);
    uint8_t *p = send_value;

    *p++ = BLE_SEND_FST;       // 0x33
    *p++ = BLE_SEND_SEC;       // 0xCC
    *p++ = frame_cmd;          // dp_id
    *p++ = frame_type;         // dp_type
    *p++ = frame_dlc >> 8;     // data_len high
    *p++ = frame_dlc & 0xFF;   // data_len low
    tmos_memcpy(p, frame_buf, frame_dlc);
    p += frame_dlc;
    *p++ = check_sum_8(send_value, (uint8_t)(p - send_value));

    // 写入 TX FIFO
    uint16_t to_write_length = (uint8_t)(p - send_value);
    app_drv_fifo_write(&app_ble_tx_fifo, send_value, &to_write_length);
    tmos_start_task(Peripheral_TaskID, IOCHUB_TO_BLE_SEND_EVT, 2);

    tmos_msg_deallocate(send_value);
    return 0;
}
```

接收帧解析：

```c
static uint8_t ble_recv_frame(uint8_t *frame_buf, uint8_t frame_dlc) {
    uint8_t *p = frame_buf;
    if (*p++ != BLE_REC_FST) return 0xFF;
    if (*p++ != BLE_REC_SEC) return 0xFF;

    uint8_t dp_id = *p++;
    uint8_t dp_type = *p++;
    uint16_t data_len = (*p << 8) | *(p + 1);
    p += 2;

    // 校验和验证
    uint8_t cs = check_sum_8(frame_buf, frame_dlc - 1);
    if (cs != frame_buf[frame_dlc - 1]) return 0xFF;

    switch (dp_id) {
        case DP_ID_LIGHT: {
            uint8_t light_sta = DP_LIGHT_CTRL(p);
            ble_send_frame(DP_ID_LIGHT, DP_TYPE_BOOL, &light_sta, 1);
            break;
        }
        default:
            break;
    }
    return 0x00;
}
```

### Step 4: FIFO 缓冲桥接

使用环形 FIFO 在 BLE 层和应用层之间缓冲数据：

```c
#include "app_drv_fifo.h"

#define APP_BLE_TX_BUFFER_LENGTH  512U
#define APP_BLE_RX_BUFFER_LENGTH  512U

static uint8_t app_ble_tx_buffer[APP_BLE_TX_BUFFER_LENGTH];
static uint8_t app_ble_rx_buffer[APP_BLE_RX_BUFFER_LENGTH];
static app_drv_fifo_t app_ble_tx_fifo;
static app_drv_fifo_t app_ble_rx_fifo;

void Peripheral_Init(void) {
    // FIFO 长度必须是 2 的幂
    app_drv_fifo_init(&app_ble_tx_fifo, app_ble_tx_buffer, APP_BLE_TX_BUFFER_LENGTH);
    app_drv_fifo_init(&app_ble_rx_fifo, app_ble_rx_buffer, APP_BLE_RX_BUFFER_LENGTH);
    // ... 其余 GAP/GATT 初始化 ...
}
```

BLE 服务回调 —— 将接收到的数据写入 RX FIFO：

```c
void on_bleiochubServiceEvt(uint16_t connection_handle, ble_iochub_evt_t *p_evt) {
    switch (p_evt->type) {
        case BLE_IOCHUB_EVT_TX_NOTI_DISABLED:
            tmos_stop_task(Peripheral_TaskID, IOCHUB_ALL_DP_UPLOAD_EVT);
            break;
        case BLE_IOCHUB_EVT_TX_NOTI_ENABLED:
            tmos_start_task(Peripheral_TaskID, IOCHUB_ALL_DP_UPLOAD_EVT,
                            MS1_TO_SYSTEM_TIME(1000));
            break;
        case BLE_IOCHUB_EVT_BLE_DATA_RECIEVED: {
            uint16_t to_write_length = p_evt->data.length;
            app_drv_fifo_write(&app_ble_rx_fifo,
                               (uint8_t *)p_evt->data.p_data, &to_write_length);
            tmos_start_task(Peripheral_TaskID, IOCHUB_DATA_PROCESS_EVT, 2);
            break;
        }
    }
}
```

### Step 5: 数据点处理

从 RX FIFO 读取数据并解析帧：

```c
#define CMD_BUF_LEN  512
static uint8_t _cmdl_buff[CMD_BUF_LEN];
static buffer_t cmdl_buff = { .data = _cmdl_buff, .length = 0, .capacity = CMD_BUF_LEN };

// IOCHUB_DATA_PROCESS_EVT 处理
if (events & IOCHUB_DATA_PROCESS_EVT) {
    while (app_drv_fifo_length(&app_ble_rx_fifo)) {
        if (cmdl_buff.length < CMD_BUF_LEN) {
            cmdl_buff.data[cmdl_buff.length] = app_drv_fifo_pop(&app_ble_rx_fifo);
            cmdl_buff.length++;
            if (cmdl_buff.length >= 7) {
                if (0x00 == ble_recv_frame(cmdl_buff.data, cmdl_buff.length)) {
                    cmdl_buff.length = 0;
                }
            }
        } else {
            app_drv_fifo_flush(&app_ble_rx_fifo);
            cmdl_buff.length = 0;
        }
    }
    cmdl_buff.length = 0;
    return (events ^ IOCHUB_DATA_PROCESS_EVT);
}
```

定时上报所有数据点：

```c
if (events & IOCHUB_ALL_DP_UPLOAD_EVT) {
    uint8_t light_sta = DP_LIGHT_CTRL(NULL);
    ble_send_frame(DP_ID_LIGHT, DP_TYPE_BOOL, &light_sta, 1);
    ble_send_frame(DP_ID_SWITCH, DP_TYPE_BOOL, (uint8_t *)&switch_sta, 1);
    tmos_start_task(Peripheral_TaskID, IOCHUB_ALL_DP_UPLOAD_EVT, MS1_TO_SYSTEM_TIME(3000));
    return (events ^ IOCHUB_ALL_DP_UPLOAD_EVT);
}
```

TX FIFO 发送（通过通知）：

```c
if (events & IOCHUB_TO_BLE_SEND_EVT) {
    uint16_t read_length = ATT_GetMTU(peripheralConnList.connHandle) - 3;
    if (app_drv_fifo_length(&app_ble_tx_fifo) >= read_length) {
        app_drv_fifo_read(&app_ble_tx_fifo, to_test_buffer, &read_length);
        attHandleValueNoti_t noti;
        noti.len = read_length;
        noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle,
                                     ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
        if (noti.pValue) {
            tmos_memcpy(noti.pValue, to_test_buffer, noti.len);
            ble_iochub_notify(peripheralConnList.connHandle, &noti, 0);
        }
    }
    tmos_start_task(Peripheral_TaskID, IOCHUB_TO_BLE_SEND_EVT, 2);
    return (events ^ IOCHUB_TO_BLE_SEND_EVT);
}
```

### Step 6: GPIO 控制与主循环

```c
#define LED_LIGHT_PIN   GPIO_Pin_9
#define KEY_SWITCH_PIN  GPIO_Pin_1

uint8_t DP_LIGHT_CTRL(uint8_t *light_sta) {
    if (light_sta != NULL) {
        if (*light_sta) GPIOA_ResetBits(LED_LIGHT_PIN);  // 亮
        else            GPIOA_SetBits(LED_LIGHT_PIN);    // 灭
    }
    return (0 == GPIOA_ReadPortPin(LED_LIGHT_PIN));
}

// GPIO 中断处理开关状态变化
void GPIOA_IRQHandler(void) {
    if (GPIOA_ReadITFlagBit(KEY_SWITCH_PIN)) {
        GPIOA_ClearITFlagBit(KEY_SWITCH_PIN);
        switch_sta = !switch_sta;
        tmos_set_event(Peripheral_TaskID, IOCHUB_SWITCH_CHANGE_EVT);
    }
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    GPIOA_SetBits(LED_LIGHT_PIN);
    GPIOA_ModeCfg(LED_LIGHT_PIN, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(KEY_SWITCH_PIN, GPIO_ModeIN_PD);
    GPIOA_ITModeCfg(KEY_SWITCH_PIN, GPIO_ITMode_FallEdge);
    PFIC_EnableIRQ(GPIOA_IRQn);
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
| 数据丢失 | FIFO 溢出 | 增大 FIFO 缓冲区或提高消费速率 |
| 帧校验失败 | 帧格式不匹配 | 检查帧头（0xBB/0x44）和 checksum 计算 |
| 通知发送失败 | CCCD 未使能 | 检查 `ble_iochub_notify_is_ready()` |
| FIFO 初始化失败 | 长度非 2 的幂 | 确保 FIFO 缓冲区长度为 256、512 等 |

## GATT 服务构建变体

示例提供 4 种 IoCHub 服务实现，编译时选择其一：

| 文件 | UUID 类型 | 特征值布局 |
|---|---|---|
| `ble_iochub_service.c` | 128-bit UUID | 独立 RX/TX 特征值（默认） |
| `ble_iochub_service_16bit.c` | 16-bit UUID（0xFFF3/4/5） | 独立 RX/TX 特征值 |
| `ble_iochub_service_same_char.c` | 128-bit UUID | 单一 RX/TX 特征值 |
| `ble_iochub_service_same_16bit_char.c` | 16-bit UUID | 单一 RX/TX 特征值 |

所有变体暴露相同 API：`ble_iochub_add_service()`、`ble_iochub_notify_is_ready()`、`ble_iochub_notify()`。

## 参考项目

- `resources/EXAM/BLE/IoCHub_NET/` — 完整 IoCHub 网络示例
- `resources/EXAM/BLE/Peripheral/` — 基础从机示例
- `resources/ble_api.md` — BLE API 速查
