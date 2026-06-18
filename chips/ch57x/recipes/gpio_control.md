# GPIO 控制

> **适用摘要**: 配置 GPIO 引脚的输入、输出、中断和上下拉功能。

## 触发意图

- "配置 GPIO"
- "控制 LED"
- "读取按键"
- "GPIO 中断"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/PM/`（GPIO 唤醒）|

## 分步说明

### GPIO 输出（LED 控制）

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 配置 PA18 为推挽输出
    GPIOA_ModeCfg(GPIO_Pin_18, GPIO_ModeOut_PP_5mA);

    while(1) {
        GPIOA_InverseBits(GPIO_Pin_18);  // 翻转
        mDelaymS(500);
    }
}
```

### GPIO 输入（按键读取）

```c
// 配置 PA4 为上拉输入
GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);

// 读取按键状态（低电平有效）
uint8_t key_state = (GPIOA_ReadPort() & GPIO_Pin_4) ? 1 : 0;
if (key_state == 0) {
    // 按键按下
}
```

### GPIO 中断

```c
volatile uint8_t key_pressed = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void GPIOA_IRQHandler(void) {
    if (GPIOA_ReadITFlagBit(GPIO_Pin_4)) {
        GPIOA_ClearITFlagBit(GPIO_Pin_4);
        key_pressed = 1;
    }
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 配置 PA4 为下拉输入，下降沿中断
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PD);
    GPIOA_ITModeCfg(GPIO_Pin_4, GPIO_ITMode_FallEdge);

    PFIC_EnableIRQ(GPIOA_IRQn);

    while(1) {
        if (key_pressed) {
            key_pressed = 0;
            // 处理按键事件
            GPIOA_InverseBits(GPIO_Pin_18);
        }
        __WFI();
    }
}
```

### GPIO 模式一览

```c
// 输入模式
GPIO_ModeIN_Floating    // 浮空输入
GPIO_ModeIN_PU          // 上拉输入
GPIO_ModeIN_PD          // 下拉输入

// 输出模式
GPIO_ModeOut_PP_5mA     // 推挽输出 5mA
GPIO_ModeOut_PP_20mA    // 推挽输出 20mA
```

### GPIO 中断模式

```c
GPIO_ITMode_LowLevel    // 低电平触发
GPIO_ITMode_HighLevel   // 高电平触发
GPIO_ITMode_FallEdge    // 下降沿触发
GPIO_ITMode_RiseEdge    // 上升沿触发
```

### GPIOB 使用

```c
// GPIOB 与 GPIOA API 完全对称
GPIOB_ModeCfg(GPIO_Pin_0, GPIO_ModeOut_PP_5mA);
GPIOB_SetBits(GPIO_Pin_0);
GPIOB_ResetBits(GPIO_Pin_0);
GPIOB_InverseBits(GPIO_Pin_0);

// GPIOB 中断
void GPIOB_IRQHandler(void) { ... }
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 输出无反应 | 引脚被外设占用 | 检查引脚是否被 UART/SPI/USB 占用 |
| 中断不触发 | 中断未使能 | 调用 PFIC_EnableIRQ() |
| 按键抖动 | 未做消抖 | 软件延时 20ms 后再读取 |
| 浮空输入不稳定 | 未接上下拉 | 使用 GPIO_ModeIN_PU 或 GPIO_ModeIN_PD |

## 参考项目

- `resources/EXAM/PM/` — GPIO 唤醒示例
- `resources/EXAM/BLE/HAL/KEY.c` — 按键驱动
- `resources/EXAM/BLE/HAL/LED.c` — LED 驱动
