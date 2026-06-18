# 电源管理与睡眠模式

> **适用摘要**: 配置 CH57x 的低功耗模式，包括 Idle、Halt、Sleep、Shutdown 以及唤醒配置。

## 触发意图

- "低功耗模式"
- "睡眠唤醒"
- "省电配置"
- "GPIO 唤醒"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/PM/` |

## 分步说明

### 低功耗模式一览

| 模式 | 功耗 | 唤醒源 | RAM 保持 |
|------|------|--------|----------|
| Idle | 最高 | 任何中断 | 全部 |
| Halt | 低 | GPIO/RTC/USB | 全部 |
| Sleep | 很低 | GPIO/RTC | 可选 |
| Shutdown | 最低 | 复位 | 无 |

### Idle 模式

```c
// CPU 停止，外设继续运行
// 任何中断可唤醒
LowPower_Idle();
// 唤醒后从这里继续执行
```

### Halt 模式

```c
// CPU 和大部分外设停止
// GPIO/RTC/USB 可唤醒
LowPower_Halt();
// 唤醒后从这里继续执行
```

### Sleep 模式

```c
// 最低功耗（除 Shutdown）
// 只有 GPIO 和 RTC 可唤醒
// 可选择保持多少 RAM

// 保持 2KB RAM
LowPower_Sleep(RB_PWR_RAM2K);

// 保持 16KB RAM
LowPower_Sleep(RB_PWR_RAM16K);

// 保持扩展 RAM
LowPower_Sleep(RB_PWR_EXTEND);
```

### Shutdown 模式

```c
// 最低功耗，只能复位唤醒
// RAM 不保持
LowPower_Shutdown(0);
// 唤醒后相当于复位重启
```

### GPIO 唤醒配置

```c
// 配置 PA5 为唤醒引脚（低电平唤醒）
void GPIO_Wakeup_Init(void) {
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_PU);

    // 配置唤醒源
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, WakUP_Mode_LEVEL);
}

// 在进入睡眠前调用
int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    GPIO_Wakeup_Init();

    while(1) {
        // 正常工作...

        // 进入睡眠
        LowPower_Sleep(RB_PWR_RAM2K);

        // 唤醒后继续
        // ...
    }
}
```

### RTC 唤醒配置

```c
void RTC_Wakeup_Init(void) {
    // 配置 RTC 唤醒（5 秒后唤醒）
    uint32_t rtc_wakeup = MS_TO_RTC(5000);
    RTC_SetCyc0Int(rtc_wakeup);
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_RTC_WAKE, WakUP_Mode_TIMER);
}
```

### DC/DC 配置

```c
// 启用 DC/DC 转换器（降低功耗）
// 需要外部电感硬件支持
PWR_DCDCCfg(ENABLE);
```

### 外设时钟门控

```c
// 关闭不用的外设时钟（降低功耗）
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_USB);    // 关闭 USB 时钟
PWR_PeriphClkCfg(DISABLE, BIT_SLP_CLK_SPI0);   // 关闭 SPI0 时钟

// 需要使用时再打开
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_SPI0);
```

### BLE 低功耗配置

```c
// config.h 中配置
#define DCDC_ENABLE     TRUE   // 启用 DC/DC
#define HAL_SLEEP       TRUE   // 启用 BLE 自动睡眠

// BLE 协议栈会在空闲时自动进入低功耗模式
// 通过 RTC 唤醒处理 BLE 事件
```

### PM 示例完整流程

```c
// 参考 resources/EXAM/PM/
int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 配置唤醒引脚
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_PU);
    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, WakUP_Mode_LEVEL);

    // LED 指示
    GPIOA_ModeCfg(GPIO_Pin_18, GPIO_ModeOut_PP_5mA);

    while(1) {
        // LED 闪烁指示唤醒
        for (int i = 0; i < 3; i++) {
            GPIOA_InverseBits(GPIO_Pin_18);
            mDelaymS(200);
        }

        // 进入 Sleep，等待 PA5 低电平唤醒
        LowPower_Sleep(RB_PWR_RAM2K);

        // 唤醒后继续
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 唤醒后复位 | RAM 保持不足 | 使用 RB_PWR_RAM16K 保持更多 RAM |
| 无法唤醒 | 唤醒源未配置 | 调用 PWR_PeriphWakeUpCfg() |
| 功耗没降低 | DC/DC 未启用 | 设置 DCDC_ENABLE=TRUE |
| BLE 断连 | 睡眠时间太长 | 调整 SLEEP_RTC_MAX_TIME |

## 参考项目

- `resources/EXAM/PM/` — 电源管理示例（4 种唤醒优先级）
