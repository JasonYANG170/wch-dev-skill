# BLE+USB 组合 (BLE USB Combo)

> **适用摘要**: 实现 CH58x/CH59x BLE 与 USB 同时工作，BLE 收发数据通过 USB 转发，或 USB 数据通过 BLE 通知发送，使用环形缓冲区进行数据中转

## 触发意图

- "BLE 转 USB"
- "BLE USB 组合"
- "BLE USB 桥接"
- "BLE+USB 双接口"
- "USB BLE 数据转发"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `peripheral.h`, `gattprofile.h`, `ble_usb_service.h`, `app_usb.h`, `RingMem.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 8KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |

## 调用链

```
Step 1: 配置 config.h (堆大小需要更大，USB+BLE 共享)
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 GAP Peripheral Role
Step 4: 注册 GATT 服务 (GAP + GATT + DevInfo + SimpleProfile + ble_usb_service)
Step 5: 初始化 USB 设备 (USB_DeviceInit)
Step 6: 初始化环形缓冲区 (RingMem)
Step 7: BLE 数据写入 RingMemBLE -> USB 发送
Step 8: USB 数据写入 RingMemUSB -> BLE 通知发送
Step 9: 运行 Main_Circulation
```

## 分步说明

### Step 1: main.c 入口

```c
#include "CONFIG.h"
#include "HAL.h"
#include "gattprofile.h"
#include "peripheral.h"
#include "app_usb.h"

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
    app_usb_init();
    Main_Circulation();
    return 0;
}
```

### Step 2: GATT 服务注册 (包含 ble_usb_service)

```c
void Peripheral_Init(void) {
    Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);

    // 配置广播和连接参数
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &enable);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    // 注册 GATT 服务
    GGS_AddService(GATT_ALL_SERVICES);
    GATTServApp_AddService(GATT_ALL_SERVICES);
    DevInfo_AddService();
    SimpleProfile_AddService(GATT_ALL_SERVICES);
    ble_usb_add_service(ble_usb_ServiceEvt);  // BLE USB 服务

    // 初始化环形缓冲区
    RingMemInit(&RingMemUSB, RingMemUSBBuf, 2048, RingMem_Protection);
    RingMemInit(&RingMemBLE, RingMemBLEBuf, 2048, RingMem_Protection);

    tmos_set_event(Peripheral_TaskID, SBP_START_DEVICE_EVT);
}
```

### Step 3: USB 初始化

```c
void app_usb_init(void) {
    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;

    USB_DeviceInit();
    PFIC_EnableIRQ(USB_IRQn);
}
```

### Step 4: USB 数据转发到 BLE

```c
// USB 接收数据回调
void DevEP2_OUT_Deal(uint8_t l) {
    // USB 数据写入 RingMemUSB
    RingMemWrite(&RingMemUSB, pEP2_OUT_DataBuf, l);
    // 触发 BLE 发送事件
    tmos_start_task(Peripheral_TaskID, SBP_PROCESS_USBDATA_EVT, 32);
}

// 处理 USB 数据 -> BLE 通知
case SBP_PROCESS_USBDATA_EVT:
    while(RingMemUSB.CurrentLen) {
        uint8_t buf[peripheralMTU - 3];
        RingMemCopy(&RingMemUSB, buf, peripheralMTU - 3);
        app_usb_notify(buf, peripheralMTU - 3);  // BLE 通知发送
        RingMemDelete(&RingMemUSB, peripheralMTU - 3);
    }
    break;
```

### Step 5: BLE 数据转发到 USB

```c
// BLE 服务回调
void ble_usb_ServiceEvt(uint16_t connection_handle, ble_usb_evt_t *p_evt) {
    switch(p_evt->type) {
        case BLE_USB_EVT_BLE_DATA_RECIEVED:
            // BLE 数据写入 RingMemBLE
            RingMemWrite(&RingMemBLE, (uint8_t *)p_evt->data.p_data, p_evt->data.length);
            tmos_set_event(Peripheral_TaskID, SBP_PROCESS_BLEDATA_EVT);
            break;
    }
}

// 处理 BLE 数据 -> USB 发送
case SBP_PROCESS_BLEDATA_EVT:
    if(RingMemBLE.CurrentLen) {
        USBSendData();  // 从 RingMemBLE 读取并发送到 USB
        tmos_start_task(Peripheral_TaskID, SBP_PROCESS_BLEDATA_EVT, 5);
    }
    break;
```

### Step 6: PHY 更新 (可选，提高吞吐量)

```c
// 连接后请求 2M PHY
GAPRole_UpdatePHY(peripheralConnList.connHandle, 0,
                   GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, GAP_PHY_OPTIONS_NOPRE);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 堆内存不足 | USB+BLE 共享堆 | 增大 `BLE_MEMHEAP_SIZE` 到 8KB 以上 |
| 数据丢失 | RingMem 溢出 | 增大 RingMem 缓冲区 (2048+) |
| USB 中断阻塞 BLE | 中断处理时间过长 | 使用 RingMem 异步中转 |
| 通知长度超限 | 数据超过 MTU-3 | 检查 `peripheralMTU` 并分包发送 |

## 参考项目

- `resources/EXAM/BLE/BLE_USB/` — BLE+USB 组合例程
