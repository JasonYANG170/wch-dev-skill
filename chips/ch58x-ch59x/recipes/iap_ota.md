# IAP 在线编程

> **适用摘要**: CH58x/CH59x IAP 引导程序和固件升级方案

## 触发意图

- "IAP 引导程序"
- "在线升级"
- "串口 IAP"
- "USB IAP"

## 前置条件

| 条件 | 要求 |
|---|---|
| Flash | Bootloader 在 0x0000-0x0FFF |
| Link.ld | APP 地址偏移到 0x1000 |

## 调用链

```
Step 1: 编译 IAP Bootloader
Step 2: 编译 APP (地址 0x1000)
Step 3: 烧录 Bootloader
Step 4: 通过 UART/USB/BLE 传输 APP 固件
Step 5: Bootloader 写入 Flash
Step 6: 跳转执行 APP
```

## 分步说明

### Step 1: IAP 内存布局

```
0x0000 ┌─────────────────┐
       │  Bootloader     │  4KB
0x1000 ├─────────────────┤
       │  Application    │  444KB
       │                 │
0x7000 └─────────────────┘
```

### Step 2: APP 链接脚本

```
MEMORY {
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}
```

### Step 3: UART IAP 传输协议

```
Host → MCU: [CMD] [ADDR] [LEN] [DATA...] [CRC]
MCU → Host: [ACK/NAK]
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| APP 不启动 | 地址未偏移 | 检查 Link.ld ORIGIN = 0x1000 |
| 覆盖 Bootloader | APP 地址错误 | 确保 APP 不写入 0x0000-0x0FFF |
| 传输失败 | CRC 校验错误 | 检查通信链路和 CRC 算法 |

## 参考项目

- `resources/EXAM/IAP/UART_IAP/`
- `resources/EXAM/IAP/USB_IAP/`
- `resources/EXAM/IAP/APP/`
