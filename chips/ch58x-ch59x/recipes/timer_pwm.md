# 定时器与 PWM

> **适用摘要**: 配置 CH58x/CH59x 定时器中断、PWM 输出和输入捕获

## 触发意图

- "定时器中断"
- "PWM 输出"
- "输入捕获"
- "周期性任务"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| 时钟 | 系统时钟已初始化 |

## 调用链

```
Step 1: 选择定时器 (TMR0-TMR3)
Step 2: 初始化 (TimerInit / PWMInit / CapInit)
Step 3: 配置中断
Step 4: 启动定时器
```

## 分步说明

### Step 1: 定时器中断

```c
// 初始化 TMR0，周期 1ms (60MHz / 60000 = 1kHz)
TMR0_TimerInit(60000);

// 配置中断
TMR0_ITCfg(ENABLE, TMR_IT_CYC_END);

// 中断服务程序
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void TMR0_IRQHandler(void) {
    TMR0_ClearITFlag(TMR_IT_CYC_END);
    // 周期性任务
}
```

### Step 2: PWM 输出

```c
// TMR0 作为 PWM
GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_5mA);

TMR0_PWMInit(Low_Level, PWM_Times_1);

// 设置周期和占空比
TMR0_PWMCycleCfg(1000);      // 周期 1000 个时钟
TMR0_PWMActDataWidth(500);   // 占空比 50%
TMR0_PWMEnable();
TMR0_Enable();
```

### Step 3: 扩展 PWM (PWM4-11)

```c
// PWM4 输出
PWMX_CycleCfg(PWMX_Cycle_256);
PWMX_ACTOUT(4, 128, Low_Level, ENABLE);  // 50% 占空比
```

### Step 4: 输入捕获

```c
TMR0_CapInit(Cap_RiseEdge);

// 读取捕获值
uint32_t capture = TMR0_CAPGetData();
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 中断不触发 | 未使能中断 | 调用 TMR0_ITCfg(ENABLE, ...) |
| PWM 无输出 | GPIO 未配置 | 配置对应引脚为输出 |
| 频率不对 | 时钟分频计算错误 | 检查系统时钟和分频值 |

## 参考项目

- `resources/EXAM/TMR/Timer/`
- `resources/EXAM/TMR/PWM/`
- `resources/EXAM/PWMX/`
