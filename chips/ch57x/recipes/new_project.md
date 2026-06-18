# 创建新的 CH57x 项目

> **适用摘要**: 从零创建一个新的 CH57x 固件项目，包含正确的项目结构、链接脚本和初始化代码。

## 触发意图

- "创建一个新的 CH57x 项目"
- "初始化 CH57x 工程"
- "新建一个嵌入式项目"
- "从头开始写 CH57x 固件"

## 前置条件

| 条件 | 要求 |
|---|---|
| IDE | MounRiver Studio 已安装 |
| SDK | CH57xEVT SDK 已解压 |
| 调试器 | WCH-LinkE (可选) |

## 调用链

```
Step 1: 创建项目目录结构
Step 2: 复制共享源文件链接（SRC/）
Step 3: 创建 Link.ld 链接脚本
Step 4: 创建 config.h（如需 BLE）
Step 5: 编写 main.c 初始化代码
Step 6: 创建 .project 和 .wvproj 文件
Step 7: MounRiver Studio 中构建
```

## 分步说明

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

普通应用使用默认链接脚本 `SRC/Ld/Link.ld`：

```
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
```

IAP 应用需要修改起始地址为 `0x1000`（详见 `recipes/iap_ota.md`）。

### Step 4: config.h（BLE 项目需要）

如需 BLE 功能，创建 `APP/include/config.h`：

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

### Step 5: 编写 main.c

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

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 链接错误 undefined reference | 未链接 StdPeriphDriver | 检查 .project 中的 linked resources |
| Flash 写入失败 | 地址未对齐到 256 字节 | 确保写入地址和长度是 256 的倍数 |
| BLE 初始化崩溃 | BLE_MEMHEAP_SIZE 太小 | 增大到至少 4KB |
| 编译找不到头文件 | include path 缺少 StdPeriphDriver/inc | 在项目设置中添加 include path |

## 变体

- **带 BLE 功能**: 添加 `config.h`，参考 `recipes/ble_peripheral.md`
- **带 IAP**: 修改链接脚本起始地址，参考 `recipes/iap_ota.md`
- **低功耗项目**: 设置 `HAL_SLEEP=TRUE`，参考 `recipes/power_management.md`
