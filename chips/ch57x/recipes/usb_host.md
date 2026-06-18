# USB 主机模式

> **适用摘要**: 配置 CH57x 作为 USB 主机，实现设备枚举、AOA 协议和 U 盘文件系统。

## 触发意图

- "USB 主机"
- "读取 U 盘"
- "USB AOA 通信"
- "USB 设备枚举"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/USB/Host/HostEnum/`, `HostAOA/`, `U_DISK/` |
| 硬件 | USB D- (PA11), USB D+ (PA12), 需要 5V 供电电路 |

## 分步说明

### USB 设备枚举

参考 `resources/EXAM/USB/Host/HostEnum/`：

```c
#include "CH57x_common.h"
#include "usbhost_base.h"
#include "usbhost_class.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);

    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    // 初始化 USB 主机
    USB_HostInit();

    while(1) {
        // 检测设备连接
        uint8_t dev_status = USB_HostEnumDevice();
        if (dev_status == USB_INT_CONNECT) {
            // 设备已连接并枚举成功
            printf("USB device connected\n");

            // 读取设备描述符
            // 读取 HID 报告
            // ...
        }
    }
}
```

### U 盘文件系统

参考 `resources/EXAM/USB/Host/U_DISK/`：

```c
#include "CH57x_common.h"
#include "usbhost_base.h"
#include "CHRV3UFI.h"  // U 盘文件系统库

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);

    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    USB_HostInit();

    while(1) {
        uint8_t dev_status = USB_HostEnumDevice();
        if (dev_status == USB_INT_CONNECT) {
            // 初始化文件系统
            CHRV3_DiskReady();

            // 读取文件
            uint8_t buf[256];
            uint16_t len = CHRV3_FileOpen("TEST.TXT");
            if (len > 0) {
                CHRV3_FileRead(buf, sizeof(buf), &len);
                printf("Read %d bytes\n", len);
            }

            // 写入文件
            CHRV3_FileCreate("OUTPUT.TXT");
            CHRV3_FileWrite("Hello USB!", 10);
            CHRV3_FileClose();
        }
    }
}
```

### U 盘文件操作

```c
// 打开文件
uint16_t CHRV3_FileOpen(const char *filename);

// 读取文件
uint8_t CHRV3_FileRead(uint8_t *buf, uint16_t len, uint16_t *read_len);

// 写入文件
uint8_t CHRV3_FileWrite(const uint8_t *buf, uint16_t len);

// 关闭文件
uint8_t CHRV3_FileClose(void);

// 删除文件
uint8_t CHRV3_FileDelete(const char *filename);

// 枚举目录
uint8_t CHRV3_FileEnumDir(void);

// 创建文件
uint8_t CHRV3_FileCreate(const char *filename);
```

### AOA (Android Open Accessory)

参考 `resources/EXAM/USB/Host/HostAOA/`：

```c
// AOA 协议允许 CH57x 与 Android 设备通信
// 通过 USB 连接，使用 ADB 或自定义协议

#include "CH57x_common.h"
#include "usbhost_base.h"
#include "aoa.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_48MHz);

    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    USB_HostInit();

    while(1) {
        uint8_t dev_status = USB_HostEnumDevice();
        if (dev_status == USB_INT_CONNECT) {
            // 检查是否是 AOA 设备
            if (AOA_IsAccessoryDevice()) {
                // 进入 AOA 模式
                AOA_SendIdent("WCH", "CH57x", "CH57x Accessory",
                              "1.0", "http://wch.cn", "serial123");

                // 等待设备重新枚举为 AOA 模式
                mDelaymS(1000);

                // AOA 数据收发
                uint8_t buf[256];
                uint16_t len = AOA_Recv(buf, sizeof(buf));
                if (len > 0) {
                    AOA_Send(buf, len);  // 回显
                }
            }
        }
    }
}
```

## USB 主机库文件

```
resources/EXAM/USB/Host/USB_LIB/
├── CH573UFI.c       // U 盘文件系统实现
├── CHRV3UFI.h       // U 盘文件系统头文件
└── usbhost_base.h   // USB 主机基础定义
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 设备无法枚举 | 5V 供电不足 | 检查 VBUS 供电电路 |
| U 盘读取失败 | 文件系统未初始化 | 先调用 CHRV3_DiskReady() |
| AOA 无法识别 | Android 设备不支持 | 确认设备支持 AOA 协议 |
| 时钟错误 | 不是 48MHz | USB 主机必须使用 48MHz |

## 参考项目

- `resources/EXAM/USB/Host/HostEnum/` — HID 设备枚举
- `resources/EXAM/USB/Host/HostAOA/` — Android AOA 通信
- `resources/EXAM/USB/Host/U_DISK/` — U 盘文件系统操作
