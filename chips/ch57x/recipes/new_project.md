# 创建新的 CH57x 项目

> **适用摘要**: 从零创建一个新的 CH57x 固件项目，包含正确的项目结构、链接脚本和初始化代码。
> Excludes: CH58x/CH59x BLE projects, CH32V/CH32F projects, and 8051 CH5xx projects.
> SDK/EVT: CH572 EVT (RISC-V, MounRiver Studio `.wvproj`) and CH579 EVT (ARM Cortex-M0, Keil MDK/uVision `.uvprojx`). Select the exact chip before copying project files.
> Evidence: CH572 `resources/EXAM/ADC/ADC.wvproj`; CH579 `resources/EXAM/CH579/ADC/CH57x.uvprojx`, `resources/EXAM/CH579/BLE/Peripheral/Project/BLE.uvprojx`, and the matching `SRC/` files.
> Validation: example-derived.

## 触发意图

- "创建一个新的 CH57x 项目"
- "初始化 CH57x 工程"
- "新建一个嵌入式项目"
- "从头开始写 CH57x 固件"

## 前置条件

| 条件 | 要求 |
|---|---|
| IDE | CH572：MounRiver Studio；CH579：Keil MDK/uVision，编译器版本与所选 EVT 工程及库匹配 |
| SDK | 与具体芯片匹配的 CH572 EVT 或 CH579 EVT 已解压 |
| 调试器 | 与芯片匹配的调试器及模式；CH579 使用 ARM/SWD 模式，按该芯片说明开启调试接口 |

## 先按芯片选择工程

| 芯片 | 内核 | 所捆绑 EVT 工程 | 启动与链接配置 |
|---|---|---|---|
| CH572 | RISC-V | MounRiver Studio `.wvproj`，例如 `resources/EXAM/ADC/ADC.wvproj` | `SRC/RVMSIS/`、`SRC/Startup/startup_CH572.S`、`SRC/Ld/Link.ld` |
| CH579 | ARM Cortex-M0 | Keil MDK/uVision `.uvprojx`，例如 `resources/EXAM/CH579/ADC/CH57x.uvprojx` | `SRC/CMSIS/`、`SRC/Startup/startup_ARMCM0.s`，以及原工程的链接设置或 `SRC/sct/CH57x.sct` scatter 文件 |

`CH57x_common.h` 和部分外设 API 名称相似，不能据此选择同一个内核或工具链。CH579 按下面的 Keil 流程建立工程。后续 RISC-V 分步仅展示 CH572 的工程结构；其中代码、引脚和地址是历史 CH57x 参考，未按具体芯片验证，不能直接套用。创建 CH572 工程时，从选定 CH572 EVT 示例复制实际配置，并按其源码核对 API、时钟、引脚和内存布局。

## CH579：从匹配的 Keil 示例创建工程

1. 从 `resources/EXAM/CH579/` 选择功能最接近的官方示例。普通外设可从 `ADC/CH57x.uvprojx` 开始；BLE 从机可从 `BLE/Peripheral/Project/BLE.uvprojx` 开始。LCD 和以太网分别选 `LCD/CH57x.uvprojx` 与 `NET/<示例>/CH57x.uvprojx`。
2. 复制所选示例及它引用的共享目录，保留或修正 `.uvprojx` 中的相对路径。外设工程通常引用 `SRC/`；BLE 工程还引用相应的 `HAL/`、`LIB/` 和 Profile 目录。先保持示例的项目结构和初始化代码，再修改应用逻辑。
3. 用 Keil MDK/uVision 打开复制后的 `.uvprojx`。安装该 EVT 提供的器件包，并核对设备型号、`Cortex-M0` 内核和 ARM 编译器设置。例如所捆绑 ADC 工程使用 `CH579F`、`ARM-ADS`、ARMCC 5.06 与 `Keil.WCH57x_DFP.1.1.0`；其他工程以自身配置为准。
4. 保留 ARM CMSIS 头文件 `SRC/CMSIS/Include/core_cm0.h` 和启动文件 `SRC/Startup/startup_ARMCM0.s`。保留原工程的 Flash/RAM 链接配置；scatter 文件以 `.uvprojx` 的实际设置为准。BLE 从机工程指定 `BLE.sct`，但本仓库未捆绑该文件，需从对应版本的完整 CH579 EVT 补齐；共享目录另有 `SRC/sct/CH57x.sct`，不能直接用它替代 BLE 的内存布局。不要替换成 CH572 的 RVMSIS、RISC-V 启动文件或 `Link.ld`。
5. BLE 库、`config.h`、中断入口和内存分配均从同一 CH579 示例保留并核对，参考 `resources/config_reference.md` 和 `resources/memory_layout.md` 的 CH579 部分。不要直接复制下面历史示例的地址、RISC-V 中断属性或 RISC-V 库。
6. 在 Keil 中 Build（F7），检查编译与链接结果，再按照 CH579 EVT 和调试器说明下载或调试。此文档的路由核对不等于新工程已经编译通过。

## CH572 调用链

```
Step 1: 创建项目目录结构
Step 2: 复制共享源文件链接（SRC/）
Step 3: 创建 Link.ld 链接脚本
Step 4: 创建 config.h（如需 BLE）
Step 5: 编写 main.c 初始化代码
Step 6: 创建 .project 和 .wvproj 文件
Step 7: MounRiver Studio 中构建
```

## CH572 工程结构示意（RISC-V / MounRiver Studio）

以下分步展示工程组织方式，API、代码、引脚和地址等历史示例以选定 CH572 EVT 的源码及项目设置为准。先复制并构建匹配的 `.wvproj`，再按应用需求修改。

### Step 1: 创建项目目录结构

```
MyProject/
├── APP/
│   ├── include/
│   │   └── config.h        # BLE 配置（如需 BLE）
│   └── Main.c              # 主程序
├── Ld/                     # → 链接到 SRC/Ld/
├── RVMSIS/                 # → 链接到 SRC/RVMSIS/
├── Startup/                # → 链接到 SRC/Startup/
└── StdPeriphDriver/        # → 链接到 SRC/StdPeriphDriver/
```

### Step 2: 链接共享源文件

在 `.project` 文件中配置 linked resources，将 `SRC/` 下的共享目录链接到项目中：

```xml
<linkedResources>
  <link>
    <name>Ld</name>
    <type>2</type>
    <locationURI>PARENT-1-PROJECT_LOC/SRC/Ld</locationURI>
  </link>
  <link>
    <name>RVMSIS</name>
    <type>2</type>
    <locationURI>PARENT-1-PROJECT_LOC/SRC/RVMSIS</locationURI>
  </link>
  <link>
    <name>Startup</name>
    <type>2</type>
    <locationURI>PARENT-1-PROJECT_LOC/SRC/Startup</locationURI>
  </link>
  <link>
    <name>StdPeriphDriver</name>
    <type>2</type>
    <locationURI>PARENT-1-PROJECT_LOC/SRC/StdPeriphDriver</locationURI>
  </link>
</linkedResources>
```

### Step 3: 链接脚本

普通应用从所选 CH572 示例保留对应的 `SRC/Ld/Link.ld`。下面仅保留历史布局示意，不表示当前 CH572 的有效容量或起始地址：

```
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
```

IAP 起始地址以所选 CH572 IAP 工程及链接脚本为准，不能直接采用历史示例中的偏移（另见 `recipes/iap_ota.md`）。

### Step 4: config.h（BLE 项目需要）

如需 BLE 功能，从匹配 CH572 BLE 示例保留其 `config.h`。下面是历史配置示意，宏、取值和文件位置须按所选工程核对：

```c
#ifndef __CONFIG_H
#define __CONFIG_H

#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*6)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0

#endif
```

详细配置选项参考 `resources/config_reference.md`。

### Step 5: 按所选示例编写 main.c

以下主循环是历史示意；例如 `CLK_SOURCE_PLL_60MHz`、`UART1_DefInit()` 等名称不能据此认定在 CH572 头文件中存在。初始化代码从选定 CH572 示例生成并核对，不直接复制下面的片段。

**非 BLE 项目**:

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    GPIOA_ModeCfg(GPIO_Pin_18, GPIO_ModeOut_PP_5mA);
    GPIOA_SetBits(GPIO_Pin_18);   // LED off (active low)

    while(1) {
        GPIOA_InverseBits(GPIO_Pin_18);
        mDelaymS(500);
    }
}
```

**BLE 项目**:

```c
#include "CONFIG.h"
#include "HAL.h"
#include "CH57xBLE_LIB.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (DCDC_ENABLE)
    PWR_DCDCCfg(ENABLE);
#endif

#ifdef DEBUG
    UART1_DefInit();
    PRINT("CH57x BLE starting...\n");
#endif

    CH57X_BLEInit();
    HAL_Init();
    // GAPRole_xxxInit();  // 根据角色
    // App_Init();          // 应用初始化

    Main_Circulation();
    return 0;
}
```

### Step 6: 创建 MounRiver 项目文件

参考 `resources/EXAM/ADC/.project` 创建 `.project` 文件，参考 `resources/EXAM/ADC/ADC.wvproj` 创建 `.wvproj` 文件。

### Step 7: 构建

1. 打开 MounRiver Studio
2. File → Import → Existing Projects into Workspace
3. 选择项目目录
4. Project → Build Project (Ctrl+B)
5. 使用 WCH-LinkE 烧录：Run → Debug

## 历史示例中的常见错误（按选定 CH572 工程核对）

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 链接错误 undefined reference | 未链接 StdPeriphDriver | 检查 .project 中的 linked resources |
| Flash 写入失败 | 地址未对齐到 256 字节 | 确保写入地址和长度是 256 的倍数 |
| BLE 初始化崩溃 | BLE_MEMHEAP_SIZE 太小 | 先按最近的 CH57x BLE 示例配置；常见模板使用 6KB，缩小时需要结合连接数/MTU 实测 |
| 编译找不到头文件 | include path 缺少 StdPeriphDriver/inc | 在项目设置中添加 include path |

## CH572 变体（配置以对应 EVT 工程为准）

- **带 BLE 功能**: 添加 `config.h`，参考 `recipes/ble_peripheral.md`
- **带 IAP**: 修改链接脚本起始地址，参考 `recipes/iap_ota.md`
- **低功耗项目**: 设置 `HAL_SLEEP=TRUE`，参考 `recipes/power_management.md`
