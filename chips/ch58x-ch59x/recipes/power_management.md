# 电源管理

> **适用摘要**: 配置 CH58x/CH59x 低功耗模式（Idle、Halt、Sleep、Shutdown）

## 触发意图

- "低功耗模式"
- "睡眠唤醒"
- "降低功耗"
- "Shutdown 模式"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| config.h | HAL_SLEEP 配置正确 |

## 调用链

```
Step 1: 配置唤醒源 (GPIO / RTC / USB)
Step 2: 配置 DC/DC (可选)
Step 3: 关闭不需要的外设时钟
Step 4: 进入低功耗模式
```

## 分步说明

### Step 1: Idle 模式

```c
// 最浅睡眠，所有外设保持运行
LowPower_Idle();
```

### Step 2: Halt 模式

```c
// 较深睡眠，大部分外设关闭
LowPower_Halt();
```

### Step 3: Sleep 模式

```c
// 深度睡眠，RAM 保持
// 参数: 唤醒后是否保持 32K 时钟
LowPower_Sleep(RB_SLP_CLK_RAM_32K);
```

### Step 4: Shutdown 模式

```c
// 最深睡眠，仅 GPIO 唤醒
LowPower_Shutdown(0);
```

### Step 5: GPIO 唤醒配置

```c
// 配置 PA8 为唤醒引脚
PWR_PeriphWakeUpCfg(ENABLE, PA8, WakeUp_HighLevel);
```

### Step 6: RTC 唤醒配置

```c
// RTC 定时唤醒
RTC_InitTime(2024, 1, 1, 0, 0, 0);
RTC_TRIGFunCfg(32768);  // 1秒后唤醒
```

### Step 7: DC/DC 配置

```c
// 启用 DC/DC 降低功耗
PWR_DCDCCfg(ENABLE);
```

## 低功耗模式对比

| 模式 | 功耗 | RAM保持 | 唤醒源 |
|---|---|---|---|
| Idle | 最高 | 是 | 任意中断 |
| Halt | 中等 | 是 | GPIO/RTC/USB |
| Sleep | 较低 | 是 | GPIO/RTC |
| Shutdown | 最低 | 否 | GPIO |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 无法唤醒 | 唤醒源未配置 | 调用 PWR_PeriphWakeUpCfg |
| 唤醒后死机 | 时钟未恢复 | 唤醒后重新初始化外设 |
| 功耗偏高 | 外设时钟未关闭 | 调用 PWR_PeriphClkCfg(DISABLE, ...) |

## 参考项目

- `resources/EXAM/PM/Sleep/`
- `resources/EXAM/PM/Halt/`
- `resources/EXAM/PM/Shutdown/`
