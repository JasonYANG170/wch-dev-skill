# BLE 串口透传 (BLE UART Bridge)

> **适用摘要**: 实现 CH58x/CH59x BLE 转串口透明传输，通过 BLE 连接收发 UART 数据，支持 FIFO 缓冲和 MTU 自适应

## 触发意图

- "BLE 转串口"
- "BLE 透传"
- "BLE UART 透传"
- "BLE 串口桥接"
- "BLE Serial Bridge"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `peripheral.h`, `ble_uart_service.h`, `app_uart.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 6KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小、连接参数)
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Peripheral Role
Step 4: 注册 GATT 服务 (GAP + GATT + DevInfo + BLE UART Service)
Step 5: 初始化 UART 硬件和 FIFO 缓冲
Step 6: 在 Main_Circulation 中轮询 UART 数据并转发到 BLE
Step 7: BLE 收到数据后转发到 UART
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "peripheral.h"
#include "app_uart.h"

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
    Peripheral_Init();
    app_uart_init();
    Main_Circulation();
    return 0;
}
```

### Step 2: GATT 服务注册

```c
void Peripheral_Init(void) {
    Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);

    // 配置广播参数
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    // 连接参数 (低延迟)
    uint16 desired_min_interval = 6;
    uint16 desired_max_interval = 1000;
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16), &desired_min_interval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16), &desired_max_interval);

    // 注册 GATT 服务
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);
    DevInfo_AddService();
    ble_uart_add_service(on_bleuartServiceEvt);  // BLE UART 服务

    tmos_set_event(Peripheral_TaskID, SBP_START_DEVICE_EVT);
}
```

### Step 3: Main_Circulation 中轮询 UART

```c
__HIGH_CODE
void Main_Circulation() {
    while(1) {
        TMOS_SystemProcess();
        app_uart_process();  // 轮询 UART FIFO
    }
}
```

### Step 4: UART 数据转发到 BLE

```c
// UART -> BLE: 从 FIFO 读取数据，通过通知发送
case UART_TO_BLE_SEND_EVT:
    // 检查通知是否使能
    if(!ble_uart_notify_is_ready(peripheralConnList.connHandle))
        break;

    // 读取长度为 MTU-3
    read_length = ATT_GetMTU(peripheralConnList.connHandle) - 3;

    if(app_drv_fifo_length(&app_uart_rx_fifo) >= read_length) {
        app_drv_fifo_read(&app_uart_rx_fifo, to_test_buffer, &read_length);

        noti.len = read_length;
        noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle,
                                     ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
        tmos_memcpy(noti.pValue, to_test_buffer, noti.len);
        ble_uart_notify(peripheralConnList.connHandle, &noti, 0);
    }
    break;
```

### Step 5: BLE 数据转发到 UART

```c
// BLE -> UART: 在 BLE UART 服务回调中接收数据
void on_bleuartServiceEvt(uint16_t conn_handle, ble_uart_evt_t *p_evt) {
    switch(p_evt->type) {
        case BLE_UART_EVT_BLE_DATA_RECIEVED:
            // 将收到的数据写入 UART TX FIFO
            app_drv_fifo_write(&app_uart_tx_fifo,
                               p_evt->data.p_data,
                               p_evt->data.length);
            break;
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 数据丢失 | FIFO 溢出 | 增大 FIFO 缓冲区，或加快处理速度 |
| 通知发送失败 | MTU 限制 | 使用 `ATT_GetMTU() - 3` 计算最大长度 |
| 连接间隔太大 | 延迟高 | 设置较小的 `MIN_CONN_INTERVAL` |
| UART 数据不完整 | FIFO 读取长度不匹配 | 确保读取长度不超过 FIFO 中的数据量 |

## 参考项目

- `resources/EXAM/BLE/BLE_UART/` — BLE 转串口透传例程
