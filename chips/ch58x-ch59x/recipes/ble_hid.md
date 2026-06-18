# BLE HID 设备

> **适用摘要**: 实现 CH58x/CH59x BLE HID 键盘、鼠标、消费者控制

## 触发意图

- "BLE 键盘"
- "BLE 鼠标"
- "HID 设备"
- "BLE HID"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | BLE 标准头文件 + HID 相关头文件 |
| config.h | BLE 堆 ≥ 8KB |

## 调用链

```
Step 1: 配置 HID 描述符
Step 2: 初始化 BLE 协议栈
Step 3: 注册 HID 服务
Step 4: 发送 HID 报告
```

## 分步说明

### Step 1: HID 键盘报告

```c
// HID 报告描述符（键盘）
const uint8_t hidReportMap[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    // ...
};

// 发送按键
uint8_t report[8] = {0};
report[2] = 0x04;  // 'A' 键
HID_SendReport(report, 8);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 主机不识别 | 报告描述符错误 | 检查 HID 描述符格式 |
| 按键不释放 | 未发送松开报告 | 发送全 0 报告 |

## 参考项目

- `resources/EXAM/BLE/HID_Keyboard/`
- `resources/EXAM/BLE/HID_Mouse/`
- `resources/EXAM/BLE/HID_Consumer/`
