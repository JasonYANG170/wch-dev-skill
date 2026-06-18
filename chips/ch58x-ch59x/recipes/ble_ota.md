# BLE OTA 固件升级

> **适用摘要**: 实现 CH58x/CH59x BLE 空中固件升级 (OTA)

## 触发意图

- "BLE OTA"
- "空中升级"
- "固件更新"
- "OTA 升级"

## 前置条件

| 条件 | 要求 |
|---|---|
| IAP | Bootloader 已烧录在 0x0000-0x0FFF |
| Link.ld | APP 起始地址 0x1000 |
| config.h | BLE 堆 ≥ 8KB |

## 调用链

```
Step 1: 烧录 IAP Bootloader
Step 2: 编译 OTA APP (地址 0x1000)
Step 3: 通过 BLE 连接
Step 4: 发送固件数据
Step 5: 校验并跳转
```

## 分步说明

### Step 1: IAP Bootloader

```c
// Bootloader 固定在 0x0000-0x0FFF
// 检查 OTA 标志，决定跳转 APP 或进入升级模式
if (ota_flag == OTA_REQUEST) {
    // 进入 OTA 模式
    OTA_Handle();
} else {
    // 跳转到 APP (0x1000)
    JumpToApp(0x1000);
}
```

### Step 2: OTA APP 链接脚本

```
MEMORY {
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 32K
}
```

### Step 3: OTA 升级流程

```
手机 APP → BLE 连接 → 发现 OTA 服务 → 发送固件数据 → 校验 → 重启
```

## OTA 方案对比

| 方案 | 说明 | 安全性 |
|---|---|---|
| BackupUpgrade | 备份升级，失败可回滚 | 高 |
| OnlyUpdateApp | 仅更新 APP，简单快速 | 中 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| OTA 失败 | Flash 地址错误 | 确认 APP 起始地址 0x1000 |
| 升级后无法启动 | 校验失败 | 检查固件完整性校验逻辑 |
| 回滚失败 | 备份区损坏 | 确保备份区不被意外擦除 |

## 参考项目

- `resources/EXAM/BLE/BackupUpgrade_OTA/`
- `resources/EXAM/BLE/OnlyUpdateApp_Peripheral/`
- `resources/EXAM/IAP/UART_IAP/`
