# ADC 采样

> **适用摘要**: 配置 ADC 进行模拟信号采集，包括外部通道、温度传感器、电池电压和触摸按键。

## 触发意图

- "读取 ADC"
- "采集模拟信号"
- "测量温度"
- "触摸按键检测"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/ADC/` |

## 分步说明

### ADC 通道分配

| 通道 | 引脚 | 说明 |
|------|------|------|
| CH0 | PA4 | 外部模拟输入 0 |
| CH1 | PA5 | 外部模拟输入 1 |
| CH2 | PA12 | 外部模拟输入 2 |
| CH3 | PA13 | 外部模拟输入 3 |
| CH4-CH11 | 其他引脚 | 更多外部通道 |
| CH12 | 内部 | 温度传感器 |
| CH13 | 内部 | 电池电压 (VDD) |
| CH14 | 触摸 | TouchKey |

### 外部单通道采样

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // 初始化 ADC（单端采样）
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);

    // 选择通道 0 (PA4)
    ADC_ChannelCfg(0);

    // 执行单次转换
    uint16_t adc_val = ADC_ExcutSingleConver();

    // 转换为电压（参考电压 3.3V，12-bit ADC）
    float voltage = (float)adc_val / 4096 * 3.3f;
    printf("ADC CH0: %d, %.3fV\n", adc_val, voltage);

    while(1);
}
```

### 外部差分采样

```c
// 差分采样（两引脚间电压差）
ADC_ExtDiffChSampInit(SampleFreq_3_2, ADC_PGA_0);
uint16_t diff_val = ADC_ExcutSingleConver();
// 差分值 = (IN+ - IN-) * PGA gain
```

### 温度传感器

```c
// 初始化内部温度传感器
ADC_InterTSSampInit();

// 读取温度 ADC 值
uint16_t ts_val = ADC_ExcutSingleConver();

// 转换为摄氏度（CH57x 公式）
int temperature = adc_to_temperature_celsius(ts_val);
printf("Temperature: %d°C\n", temperature);
```

### 电池电压

```c
// 初始化电池电压采样
ADC_InterBATSampInit();

// 读取 VDD 电压 ADC 值
uint16_t bat_val = ADC_ExcutSingleConver();

// 转换为电压
float vdd = (float)bat_val / 4096 * 3.3f * 3;  // 内部分压 1/3
printf("VDD: %.2fV\n", vdd);
```

### TouchKey

```c
// 初始化触摸按键
TouchKey_ChSampInit();

// 选择触摸通道
ADC_ChannelCfg(14);  // TouchKey 通道

// 读取触摸值
uint16_t touch_val = ADC_ExcutSingleConver();

// 基线校准（需要在无触摸时采集基准值）
static uint16_t baseline = 0;
if (baseline == 0) {
    baseline = touch_val;
}

// 判断触摸（值减小表示触摸）
if (touch_val < baseline - 100) {
    printf("Touch detected!\n");
}
```

### DMA 连续采样

```c
#define ADC_DMA_BUF_SIZE  128
static uint16_t adc_dma_buf[ADC_DMA_BUF_SIZE];

void ADC_DMA_Init(void) {
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
    ADC_ChannelCfg(0);

    // 配置 DMA
    ADC_DMACfg(ENABLE, (uint16_t)adc_dma_buf,
               (uint16_t)(adc_dma_buf + ADC_DMA_BUF_SIZE),
               ADC_DMAMode_LOOP);

    // 启动 DMA 连续采样
    ADC_StartDMA();
}

// DMA 完成后直接读取 adc_dma_buf 中的数据
```

### 校准

```c
// 粗略校准（提高精度）
signed short calib_val = ADC_DataCalib_Rough();
// 校准后的值更接近实际值
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| ADC 值全为 0 | 通道选择错误 | 检查 ADC_ChannelCfg() 参数 |
| 值不稳定 | 采样频率太高 | 降低采样频率或增加滤波 |
| 温度不准 | 未校准 | 使用 ADC_DataCalib_Rough() |
| 触摸不灵敏 | 基线漂移 | 定期更新基线值 |

## 参考项目

- `resources/EXAM/ADC/` — 完整 ADC 示例（温度、单通道、差分、TouchKey）
