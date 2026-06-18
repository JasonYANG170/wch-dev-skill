# USB 设备模式

> **适用摘要**: 配置 CH57x 作为 USB 设备，实现 CDC 串口、HID 或自定义设备。

## 触发意图

- "USB 串口"
- "USB CDC 设备"
- "USB HID 设备"
- "USB 自定义设备"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/USB/Device/COM/`, `HID_CompliantDev/`, `VendorDefinedDev/` |
| 硬件 | USB D- (PA11), USB D+ (PA12) |

## 分步说明

### USB CDC 串口设备

参考 `resources/EXAM/USB/Device/COM/`：

```c
#include "CH57x_common.h"
#include "usbd_core.h"
#include "cdc_acm.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);  // USB 需要 48MHz

    // USB 引脚配置
    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);      // D-
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);      // D+

    // 初始化 USB 设备
    USB_DeviceInit();

    // CDC 初始化
    CDC_ACM_Init();

    while(1) {
        // CDC 数据收发
        uint8_t buf[64];
        uint16_t len = CDC_ACM_Recv(buf, sizeof(buf));
        if (len > 0) {
            // 回显
            CDC_ACM_Send(buf, len);
        }
    }
}
```

### USB HID 设备

参考 `resources/EXAM/USB/Device/HID_CompliantDev/`：

```c
#include "CH57x_common.h"
#include "usbd_core.h"
#include "hid_keyboard.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);

    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    USB_DeviceInit();
    HID_Keyboard_Init();

    while(1) {
        // 发送按键 'A'
        HID_Keyboard_SendKey(0x04);  // HID keycode for 'A'
        mDelaymS(100);
        HID_Keyboard_SendKey(0x00);  // Release
        mDelaymS(1000);
    }
}
```

### USB 自定义设备

参考 `resources/EXAM/USB/Device/VendorDefinedDev/`：

```c
// 自定义 USB 设备，8 个端点
// 上行 + 下行数据通道
#include "CH57x_common.h"
#include "usbd_core.h"
#include "vendor_device.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);

    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    USB_DeviceInit();
    VendorDev_Init();

    while(1) {
        // 自定义数据收发
        uint8_t buf[64];
        uint16_t len = VendorDev_Recv(buf, sizeof(buf));
        if (len > 0) {
            // 处理自定义命令
            ProcessCommand(buf, len);
        }
    }
}
```

## USB 时钟要求

USB 需要精确的 48MHz 时钟：

```c
// 必须使用 PLL 48MHz
SetSysClock(CLK_SOURCE_PLL_48MHz);
```

## USB 复合设备

参考 `resources/EXAM/USB/Device/CompoundDev/`：
- 同时支持多个 USB 功能
- 例如：CDC + HID 复合设备
- 支持 USB 供电检测和管理

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| USB 无法枚举 | 时钟不是 48MHz | 使用 CLK_SOURCE_PLL_48MHz |
| 设备描述符错误 | 描述符长度不匹配 | 检查描述符数组长度 |
| 数据传输失败 | 端点配置错误 | 检查端点类型和最大包长 |
| 枚举后断开 | 供电不足 | 检查 USB 供电或减少功耗 |

## 参考项目

- `resources/EXAM/USB/Device/COM/` — CDC 串口设备
- `resources/EXAM/USB/Device/HID_CompliantDev/` — HID 设备
- `resources/EXAM/USB/Device/VendorDefinedDev/` — 自定义设备
- `resources/EXAM/USB/Device/CompoundDev/` — 复合设备
