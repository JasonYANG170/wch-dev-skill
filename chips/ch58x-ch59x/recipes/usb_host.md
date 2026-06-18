# USB 主机

> **适用摘要**: 配置 CH58x/CH59x USB 主机模式，枚举设备、读写数据

## 触发意图

- "USB 主机"
- "读取 U 盘"
- "USB 设备枚举"
- "USB AOA"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| USB 库 | `USB_LIB/` 中的主机库文件 |

## 调用链

```
Step 1: 初始化 USB 主机 (USB_HostInit)
Step 2: 检测设备连接 (AnalyzeRootHub)
Step 3: 枚举设备 (InitRootDevice)
Step 4: 传输数据
```

## 分步说明

### Step 1: USB 主机初始化

```c
USB_HostInit();

while(1) {
    // 检测设备插入
    if (AnalyzeRootHub() == ERR_SUCCESS) {
        EnableRootHubPort();
        InitRootDevice();

        // 枚举成功，进行数据传输
        // ...
    }
}
```

### Step 2: 设备枚举

```c
// 获取设备描述符
CtrlGetDeviceDescr();

// 获取配置描述符
CtrlGetConfigDescr();

// 设置地址
CtrlSetUsbAddress(addr);

// 设置配置
CtrlSetUsbConfig(cfg);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 设备检测不到 | USB 供电不足 | 检查 VBUS 供电 |
| 枚举失败 | 描述符过大 | 增大接收缓冲区 |
| 传输超时 | 端点配置错误 | 检查端点类型和最大包长 |

## 参考项目

- `resources/EXAM/USB/Host/HostEnum/`
- `resources/EXAM/USB/Host/U_DISK/`
