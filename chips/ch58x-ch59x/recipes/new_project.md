# 创建新项目

> **适用摘要**: 从零开始创建 CH58x/CH59x 固件项目，包含正确的项目结构和配置

## 触发意图

- "创建一个新的 CH583 项目"
- "帮我初始化一个 CH592 BLE 工程"
- "新建 CH58x 项目模板"
- "从头开始写 CH592 固件"

## 前置条件

| 条件 | 要求 |
|---|---|
| IDE | MounRiver Studio 已安装 |
| 芯片 | 确认目标芯片型号 (CH583/CH585/CH592/CH595) |
| 功能 | 确认是否需要 BLE |

## 调用链

```
Step 1: 确认芯片型号和功能需求
Step 2: 从 resources/EXAM/ 选择最接近的例程
Step 3: 复制例程目录结构
Step 4: 修改 config.h 配置（BLE项目）
Step 5: 修改 main.c 入口代码
Step 6: 配置 GPIO 和外设
Step 7: 编译验证
```

## 分步说明

### Step 1: 确认需求

确认以下信息：
- 芯片型号：CH583 / CH585 / CH592 / CH595
- 是否需要 BLE
- 需要哪些外设（UART, SPI, I2C, ADC, USB 等）

### Step 2: 选择基础例程

| 需求 | 推荐例程 |
|---|---|
| BLE 从机 | `EXAM/BLE/Peripheral/` |
| BLE 主机 | `EXAM/BLE/Central/` |
| BLE HID | `EXAM/BLE/HID_Keyboard/` |
| 串口通信 | `EXAM/UART1/` |
| GPIO 控制 | `EXAM/ADC/` (最简GPIO示例) |
| USB 设备 | `EXAM/USB/Device/COM/` |
| LCD 显示 | `EXAM/LCD/` (CH592 only) |

### Step 3: 复制项目结构

```
MyProject/
├── APP/
│   ├── include/
│   │   └── config.h          # BLE配置（非BLE项目可省略）
│   └── main.c                # 主程序
├── Ld/
│   └── Link.ld               # 链接脚本
├── Startup/
│   └── startup_CH583.S       # 启动文件
├── RVMSIS/
│   └── core_riscv.h          # RISC-V核心头文件
├── StdPeriphDriver/
│   ├── inc/                  # 驱动头文件
│   └── src/                  # 驱动源文件
├── .project                  # MounRiver项目文件
└── *.wvproj                  # 工作区项目文件
```

### Step 4: config.h 配置（BLE项目）

```c
// config.h 关键配置
#define BLE_MEMHEAP_SIZE    (1024*6)   // BLE堆大小
#define BLE_BUFF_MAX_LEN    27         // BLE缓冲区长度
#define BLE_TX_POWER        TX_POW_0_DBm  // 发射功率
#define HAL_SLEEP           FALSE      // 是否启用睡眠
```

### Step 5: main.c 模板（非BLE）

```c
#include "CH58x_common.h"  // CH58x
// 或 #include "CH59x_common.h"  // CH59x

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#ifdef DEBUG
    UART1_DefInit();
    printf("System init done\n");
#endif

    // 外设初始化
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);

    while(1) {
        // 主循环
    }
}
```

### Step 5b: main.c 模板（BLE）

```c
#include "CONFIG.h"
#include "HAL.h"
#include "CH58xBLE_LIB.h"  // 或 CH59xBLE_LIB.h
#include "peripheral.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif

#ifdef DEBUG
    UART1_DefInit();
    printf("BLE init...\n");
#endif

    CH58X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();
    Main_Circulation();

    return 0;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 编译找不到头文件 | 链接资源未配置 | 检查 .project 中的 linked resources |
| BLE 初始化失败 | config.h 配置错误 | 增大 BLE_MEMHEAP_SIZE |
| 程序卡死 | Main_Circulation 未调用 | 确保 main() 末尾调用 Main_Circulation() |
| 串口无输出 | GPIO 未配置 | 先配置 PA8/PA9 再初始化 UART1 |

## 参考项目

- BLE 从机: `resources/EXAM/BLE/Peripheral/`
- 串口: `resources/EXAM/UART1/`
- GPIO: `resources/EXAM/ADC/`
