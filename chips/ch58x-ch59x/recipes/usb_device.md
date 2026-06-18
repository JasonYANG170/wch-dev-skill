# USB 设备

> **适用摘要**: 配置 CH58x/CH59x USB 设备模式（CDC、HID、自定义）

## 触发意图

- "USB 虚拟串口"
- "USB HID 设备"
- "USB 自定义设备"
- "USB CDC"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| USB 库 | `USB_LIB/` 目录中的 USB 库文件 |

## 调用链

```
Step 1: 配置 GPIO (PA11/PA12)
Step 2: 初始化 USB 设备 (USB_DeviceInit)
Step 3: 处理 USB 事务 (USB_DevTransProcess)
Step 4: 实现端点回调
```

## 分步说明

### Step 1: USB CDC 虚拟串口

```c
#include "CH58x_common.h"
#include "usbcfg.h"
#include "usbdesc.h"
#include "usbprop.c"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    UART1_DefInit();

    // USB 初始化
    USB_DeviceInit();

    while(1) {
        USB_DevTransProcess();
    }
}

// 端点回调
void DevEP1_OUT_Deal(uint8_t l) {
    // 接收数据处理
}

void DevEP2_IN_Deal(uint8_t l) {
    // 发送完成处理
}
```

### Step 2: CH58x 双 USB

```c
// USB1
USB_DeviceInit();
USB_DevTransProcess();

// USB2 (CH583/CH585 only)
USB2_DeviceInit();
USB2_DevTransProcess();
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| USB 枚举失败 | 描述符错误 | 检查设备/配置/字符串描述符 |
| 数据丢失 | 端点缓冲区溢出 | 及时读取 OUT 端点数据 |
| USB2 不工作 | 使用了 USB1 函数 | 使用 USB2_* 系列函数 |

## 参考项目

- `resources/EXAM/USB/Device/COM/`
- `resources/EXAM/USB/Device/HID/`
- `resources/EXAM/USB/Device/Vendor/`
