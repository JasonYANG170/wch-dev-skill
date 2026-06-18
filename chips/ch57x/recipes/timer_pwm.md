# Timer 和 PWM 使用

> **适用摘要**: 配置定时器进行定时中断、PWM 输出和输入捕获。

## 触发意图

- "配置定时器"
- "PWM 输出"
- "定时中断"
- "脉冲计数"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/TMR/`, `resources/EXAM/PWMX/` |

## 分步说明

### Timer 定时中断

```c
#include "CH57x_common.h"

volatile uint8_t tmr_flag = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR0_IRQHandler(void) {
    if (TMR0_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END);
        tmr_flag = 1;
    }
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 初始化 TMR0，定时周期 = period 个系统时钟周期
    // 60MHz 时钟，1ms 定时：period = 60000
    TMR0_TimerInit(60000);

    // 使能中断
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR0_IRQn);

    // 启动定时器
    TMR0_Enable();

    while(1) {
        if (tmr_flag) {
            tmr_flag = 0;
            GPIOA_InverseBits(GPIO_Pin_18);  // 1ms 翻转 LED
        }
    }
}
```

### PWM 输出

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 配置 PWM 引脚（PA12 = PWM4）
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);

    // PWM 时钟分频
    PWMX_CLKCfg(4);  // 分频系数

    // PWM 周期配置
    PWMX_CycleCfg(PWMX_Cycle_256);  // 256 级占空比

    // 输出 PWM：通道 4，占空比 128/256 = 50%，极性高有效
    PWMX_ACTOUT(CH_PWM4, 128, High_Level, ENABLE);

    while(1);
}
```

### 改变 PWM 占空比

```c
// 运行时改变占空比（0-255）
void PWM_SetDuty(uint8_t channel, uint8_t duty) {
    switch (channel) {
        case 4:  PWM4_ActDataWidth(duty); break;
        case 5:  PWM5_ActDataWidth(duty); break;
        case 6:  PWM6_ActDataWidth(duty); break;
        case 7:  PWM7_ActDataWidth(duty); break;
        case 8:  PWM8_ActDataWidth(duty); break;
        case 9:  PWM9_ActDataWidth(duty); break;
        case 10: PWM10_ActDataWidth(duty); break;
        case 11: PWM11_ActDataWidth(duty); break;
    }
}

// 使用：呼吸灯效果
uint8_t duty = 0;
int8_t dir = 1;
while(1) {
    PWM_SetDuty(4, duty);
    duty += dir;
    if (duty == 255 || duty == 0) dir = -dir;
    mDelaymS(10);
}
```

### 输入捕获

```c
volatile uint32_t capture_val = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR1_IRQHandler(void) {
    if (TMR1_GetITFlag(TMR0_3_IT_DATA_ACT)) {
        TMR1_ClearITFlag(TMR0_3_IT_DATA_ACT);
        capture_val = TMR1_CAPGetData();
    }
}

void Capture_Init(void) {
    // 配置 TMR1 为输入捕获模式
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);  // 捕获输入引脚
    TMR1_CapInit(RiseEdge_To_RiseEdge);          // 上升沿到上升沿
    TMR1_ITCfg(ENABLE, TMR0_3_IT_DATA_ACT);
    PFIC_EnableIRQ(TMR1_IRQn);
    TMR1_Enable();
}
```

### PWM 通道引脚

| 通道 | 引脚 |
|------|------|
| PWM4 | PA12 |
| PWM5 | PA13 |
| PWM6 | PA14 |
| PWM7 | PA15 |
| PWM8 | PA16 |
| PWM9 | PA17 |
| PWM10 | PA18 |
| PWM11 | PA19 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| PWM 无输出 | 引脚未配置为输出 | 先调用 GPIOA_ModeCfg() |
| 定时不准 | 时钟频率假设错误 | 使用 GetSysClock() 确认实际频率 |
| 中断不触发 | PFIC 未使能 | 调用 PFIC_EnableIRQ() |
| PWM 频率不对 | 分频系数错误 | 检查 PWMX_CLKCfg() 参数 |

## 参考项目

- `resources/EXAM/TMR/` — 定时器示例
- `resources/EXAM/PWMX/` — PWM 输出示例
