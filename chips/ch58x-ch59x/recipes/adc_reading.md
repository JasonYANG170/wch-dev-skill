# ADC 采样

> **适用摘要**: 配置 CH58x/CH59x ADC 进行外部通道、温度、电池电压和触摸按键采样

## 触发意图

- "读取 ADC 值"
- "采集模拟电压"
- "读取芯片温度"
- "触摸按键检测"
- "电池电压检测"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| GPIO | ADC 引脚配置为模拟输入 |

## 调用链

```
Step 1: 配置 GPIO 为模拟输入
Step 2: 初始化 ADC (ExtSingleChSampInit)
Step 3: 选择通道
Step 4: 启动转换并读取结果
```

## 分步说明

### Step 1: 外部单端采样

```c
// GPIO 配置为模拟输入
GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_Floating);

// 初始化 ADC
ADC_ExtSingleChSampInit(ADC_SampClk_3_2, ADC_PGA_1_4);

// 选择通道
ADC_ChannelCfg(ADC_CH_0);  // PA4 = CH0

// 启动转换
ADC_StartUp();
while (!ADC_GetITStatus());
uint16_t value = ADC_ReadConverValue();
```

### Step 2: 温度传感器

```c
ADC_InterTSSampInit();
ADC_StartUp();
while (!ADC_GetITStatus());
uint16_t raw = ADC_ReadConverValue();
int temp = adc_to_temperature_celsius(raw);
```

### Step 3: 电池电压

```c
ADC_InterBATSampInit();
ADC_StartUp();
while (!ADC_GetITStatus());
uint16_t raw = ADC_ReadConverValue();
// 电压 = raw * 3.3 / 4096 * 分压比
```

### Step 4: 触摸按键

```c
TouchKey_ChSampInit();
uint16_t raw = TouchKey_ExcutSingleConver(chargeTime, dischargeTime);
// raw 越小表示有触摸
```

## ADC 通道

| 通道 | 引脚 |
|---|---|
| CH0 | PA4 |
| CH1 | PA5 |
| CH2 | PA12 |
| CH3 | PA13 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 读数为 0 | GPIO 未配置为模拟输入 | 调用 ModeCfg 设为 Floating |
| 读数不稳定 | 采样时钟太快 | 降低 ADC_SampClk |
| 温度偏差大 | 未校准 | 调用 ADC_DataCalib_Rough() |

## 参考项目

- `resources/EXAM/ADC/`
