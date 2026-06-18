# GPIO 控制

> **适用摘要**: 配置 CH58x/CH59x GPIO 引脚的输入、输出、中断功能

## 触发意图

- "配置 GPIO 引脚"
- "设置 PA5 为输出"
- "GPIO 中断怎么配置"
- "读取 GPIO 输入电平"
- "控制 LED 闪烁"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| 时钟 | 系统时钟已初始化 |

## 调用链

```
Step 1: 选择 GPIO 端口 (GPIOA / GPIOB)
Step 2: 配置引脚模式 (ModeCfg)
Step 3: 读写引脚 (SetBits / ResetBits / ReadPortPin)
Step 4: (可选) 配置中断 (ITModeCfg)
```

## 分步说明

### Step 1: GPIO 输出

```c
#include "CH58x_common.h"

// 配置 PA9 为推挽输出
GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);

// 控制输出
GPIOA_SetBits(GPIO_Pin_9);    // 输出高
GPIOA_ResetBits(GPIO_Pin_9);  // 输出低
GPIOA_InverseBits(GPIO_Pin_9);// 翻转
```

### Step 2: GPIO 输入

```c
// 配置 PA8 为上拉输入
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);

// 读取引脚电平
if (GPIOA_ReadPortPin(GPIO_Pin_8)) {
    // 高电平
} else {
    // 低电平
}
```

### Step 3: GPIO 中断

```c
// 配置 PA8 下降沿中断
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
GPIOA_ITModeCfg(GPIO_Pin_8, GPIO_ITMode_FallEdge);

// 中断服务程序
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void GPIOA_IRQHandler(void) {
    if (GPIOA_ReadITFlagBit(GPIO_Pin_8)) {
        GPIOA_ClearITFlagBit(GPIO_Pin_8);
        // 处理中断
    }
}
```

## GPIO 模式

| 模式 | 说明 |
|---|---|
| `GPIO_ModeIN_Floating` | 浮空输入 |
| `GPIO_ModeIN_PU` | 上拉输入 |
| `GPIO_ModeIN_PD` | 下拉输入 |
| `GPIO_ModeOut_PP_5mA` | 推挽输出 5mA |
| `GPIO_ModeOut_PP_20mA` | 推挽输出 20mA |
| `GPIO_ModeOut_OD_5mA` | 开漏输出 5mA |

## 中断模式

| 模式 | 说明 |
|---|---|
| `GPIO_ITMode_FallEdge` | 下降沿触发 |
| `GPIO_ITMode_RiseEdge` | 上升沿触发 |
| `GPIO_ITMode_LowLevel` | 低电平触发 |
| `GPIO_ITMode_HighLevel` | 高电平触发 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 输出无效 | GPIO 模式未配置 | 先调用 ModeCfg 再操作引脚 |
| 中断不触发 | 中断模式未配置 | 调用 ITModeCfg 设置触发方式 |
| 引脚冲突 | 同一引脚被多个外设占用 | 检查引脚复用表 |

## 参考项目

- `resources/EXAM/ADC/` — GPIO 输入输出示例
- `resources/EXAM/PM/` — GPIO 唤醒示例
