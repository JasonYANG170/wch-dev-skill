# LCD 显示 (CH592 only)

> **适用摘要**: 配置 CH592 片上 LCD 控制器驱动段码液晶屏

## 触发意图

- "LCD 显示"
- "段码液晶"
- "LCD 初始化"
- "CH592 LCD"

## 前置条件

| 条件 | 要求 |
|---|---|
| 芯片 | CH592 或 CH595 |
| 头文件 | `CH59x_common.h` |

## 调用链

```
Step 1: 初始化 LCD (LCD_Init)
Step 2: 配置显示参数
Step 3: 写入显示数据
```

## 分步说明

### Step 1: LCD 初始化

```c
#include "CH59x_common.h"

// 初始化 LCD: 1/4 duty, 1/3 bias
LCD_Init(LCD_Duty_1_4, LCD_Bias_1_3);
LCD_PowerOn();
```

### Step 2: 写入显示数据

```c
// 写入 LCD 显示缓冲区
LCD_WriteData0(0xFF);   // COM0 数据
LCD_WriteData1(0x01);   // COM1 数据
LCD_WriteData2(0x02);   // COM2 数据
LCD_WriteData3(0x04);   // COM3 数据
```

### Step 3: 配置扫描时钟

```c
LCD_ScanCLKCfg(0x04);  // 扫描频率配置
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 无显示 | LCD 未上电 | 调用 LCD_PowerOn() |
| 显示模糊 | duty/bias 不匹配 | 检查液晶屏规格 |
| 乱码 | 数据写错 COM | 确认 COM 引脚对应关系 |

## 参考项目

- `resources/EXAM/LCD/`
