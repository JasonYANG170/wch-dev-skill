**[English](README_EN.md)** | 中文

# wch-dev-skill

统一的 WCH（南京沁恒微电子）全系列微控制器固件开发 AI Skill。让 AI Agent 自动生成符合 WCH SDK 规范的嵌入式代码，覆盖 35+ 芯片家族、10 大架构分组。

支持芯片：CH572/CH579（BLE）、CH583/CH585/CH592/CH595（BLE+USB）、CH32V003/CH32V006/CH32L103（低成本）、CH32V103/CH32V20x/CH32V307/CH32V407（通用）、CH32F103/CH32F20x/CH32M030（ARM）、CH32X035/CH32X315/CH641/CH643（USB-PD）、CH32H417（高性能）、CH569（以太网+USB3.0）、CH561/CH563（ARM7TDMI）、CH543-CH559（8051）。

## 功能特性

- 统一路由：自动识别芯片型号，导航到正确的 `chips/<family>/` 目录
- 10 个芯片家族，每个家族包含完整的 recipes（场景指南）、API 速查、陷阱文档和 EXAM 例程
- 覆盖 BLE 全角色（Peripheral/Central/HID/Mesh/OTA/ANCS）、以太网、USB 3.0、USB-PD、LCD、NFC-A 等
- 每个场景提供完整的调用链、初始化序列和可复制的代码模板
- 包含各架构（RISC-V/ARM/ARM7TDMI/8051）的常见错误和陷阱

## 芯片家族覆盖

| 家族 | 目录 | 芯片 | 架构 | 关键特性 |
|------|------|------|------|----------|
| CH57x | `chips/ch57x/` | CH572, CH579 | RISC-V | BLE 5.0, LCD (CH579), NET (CH579) |
| CH58x/CH59x | `chips/ch58x-ch59x/` | CH583, CH585, CH592, CH595 | RISC-V | BLE+USB, LCD, NFCA, ENCODER, KEYSCAN |
| CH32V 通用 | `chips/ch32v-general/` | CH32V103, CH32V20x, CH32V307, CH32V407 | RISC-V | CAN, ETH, USB, BLE, LTDC, ARGB |
| CH32V 低成本 | `chips/ch32v-lowcost/` | CH32V003, CH32V006, CH32L103 | RISC-V | OPA, USB-PD (CH32L103), LPTIM |
| CH32F ARM | `chips/ch32f-arm/` | CH32F103, CH32F20x, CH32M030 | ARM Cortex-M3 | STM32 兼容, BLE, USB-PD |
| CH32X USB-PD | `chips/ch32x-usbpd/` | CH32X035, CH32X315, CH641, CH643 | RISC-V | USB-PD, PIOC |
| CH32H417 | `chips/ch32h-highperf/` | CH32H417 | RISC-V | USB 3.0, LTDC, SerDes, 双核 |
| CH569 | `chips/ch56x-ethernet/` | CH569 | RISC-V | ETH, USB 3.0, eMMC, ECDC |
| CH561/CH563 | `chips/ch561-ch563/` | CH561, CH563 | ARM7TDMI | ETH, USB 2.0 HS |
| CH5xx 8051 | `chips/ch5xx-8051/` | CH543-CH559 | 8051 | USB, TouchKey |

## 安装说明

### 1. 拉取仓库到 skill 目录

根据你使用的 AI Agent 文档，找到或创建存放 Skill 的目录：

```bash
git clone <repo-url> wch-dev-skill
```

例如：

> **Claude Code**
> **项目作用域**：位于项目根目录下的 `.claude/skills`
> **用户作用域**：位于 `~/.claude/skills`，对本机所有项目生效
> 进入到对应的 skills 文件夹下
> 在终端执行 `git clone <repo-url> wch-dev-skill` 即可

> **QwenCode**
> **项目作用域**：位于项目根目录下的 `.qwen/skills`
> **用户作用域**：位于 `~/.qwen/skills`，对本机所有项目生效

> **OpenCode**
> **项目作用域**：位于项目根目录下的 `.opencode/skills`
> **用户作用域**：位于 `~/.config/opencode/skills`，对本机所有项目生效

### 2. 使用指定 skill

在你的 AI Agent 中确认 Skill 已加载，可通过命令指定 skill。

例如：

> **Claude Code**
> 在终端中输入 `claude` 后回车，然后输入 `/wch-dev-skill` 并描述你的需求

> **QwenCode**
> 在终端中输入 `qwen` 后回车，输入 `/skills` 回车，选择 wch-dev-skill

> **OpenCode**
> 在终端中输入 `opencode` 后回车，输入 `/skills` 回车，选择 wch-dev-skill

## 工作原理

Skill 定义了一套工作流，AI Agent 在生成固件代码时会遵循：

| 步骤 | 名称 | 说明 |
|------|------|------|
| 1 | 识别芯片 | 确定具体芯片型号，导航到 `chips/<family>/` |
| 2 | 阅读方案 | 在 `recipes/` 中匹配场景，获取完整调用链 |
| 3 | 查询 API | 查阅 `resources/peripheral_api.md` 获取函数签名 |
| 4 | 检查陷阱 | 阅读 `resources/pitfalls.md` 避免常见错误 |
| 5 | 验证 | 确认 API 签名、头文件包含、引脚分配、时钟配置 |
| 6 | 确认 | 向用户展示实现方案（包含、引脚、初始化序列、主循环） |
| 7 | 执行 | 复制最接近的 EXAM 例程，然后修改适配 |
| 8 | 构建 | RISC-V/ARM: MounRiver Studio (.wvproj)。ARM7: Keil MDK。8051: Keil C51 |
| 9 | 调试 | UART 调试输出（115200 波特率）。BLE: nRF Connect。USB-PD: USB-C 分析仪 |

## 覆盖场景

### BLE 应用（CH57x / CH58x/CH59x / CH32V20x / CH32F20x）
- BLE 从机（Peripheral）— 广播、GATT 服务、通知
- BLE 主机（Central）— 扫描、连接、服务发现、读写
- BLE HID 设备 — 键盘、鼠标、消费者控制
- BLE Mesh — 配网、阿里智能灯模型、厂商模型
- BLE OTA — 双镜像备份升级、IAP 引导
- BLE 速度测试 — 吞吐量优化、LE 2M PHY
- BLE IoCHub — 自定义数据点协议、多设备组网
- BLE ANCS / 心率 / 透传 / USB 合用

### 有线通信
- 以太网 — TCP/UDP Socket（CH32V307/CH32F20x/CH569/CH561/CH563/CH32H417/CH579/CH595）
- USB 设备 — CDC、HID、自定义（全系列）
- USB 主机 — 设备枚举、AOA、U 盘
- USB 3.0 — 超高速设备（CH569/CH32H417/CH32X315）
- USB-PD — PD 协商、电压/电流配置（CH32X035/CH641/CH643/CH32L103/CH32M030/CH32H417）
- CAN — CAN 2.0B 收发（CH32V307/CH32F103/CH32L103/CH32H417）

### 显示与人机交互
- LCD 段码屏 — CH579/CH585/CH592/CH595
- FSMC/TFT LCD — CH32V307/CH32F20x
- LTDC 显示 — CH32V407/CH32H417
- ARGB LED — CH32V407/CH32X315
- TouchKey — 全系列
- KEYSCAN 键盘矩阵 — CH572/CH595
- ENCODER 编码器 — CH595
- NFCA NFC-A — CH585

### 外设驱动
- GPIO / UART / SPI / I2C / ADC / Timer / PWM — 全系列
- DAC / OPA / CMP — 多数系列
- Flash 读写擦除、DMA 传输、EXTI 中断
- RTC / IWDG / WWDG / 低功耗模式

### RTOS 支持
- FreeRTOS / RT-Thread / HarmonyOS / TencentOS — CH32V/CH32F/CH32X/CH58x/CH59x 系列
- UCOS — CH563

## 目录结构

```
wch-dev-skill/
  SKILL.md                        # 统一路由和指南
  README.md                       # 本文件（中文）
  README_EN.md                    # 英文说明
  CHANGELOG.md                    # 版本历史
  chips/
    ch57x/                        # CH572/CH579 BLE 5.0
      recipes/                    # 20 个场景指南
      resources/                  # API 速查、陷阱、配置、EXAM 例程
    ch58x-ch59x/                  # CH583/CH585/CH592/CH595 BLE+USB
      recipes/                    # 30 个场景指南
      resources/                  # API 速查、陷阱、配置、EXAM 例程
    ch32v-general/                # CH32V103/V20x/V307/V407
      recipes/                    # 21 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch32v-lowcost/                # CH32V003/V006/L103
      recipes/                    # 15 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch32f-arm/                    # CH32F103/F20x/M030 ARM Cortex-M3
      recipes/                    # 22 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch32x-usbpd/                  # CH32X035/X315/CH641/CH643 USB-PD
      recipes/                    # 14 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch32h-highperf/               # CH32H417 双核 USB3+LTDC
      recipes/                    # 38 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch56x-ethernet/               # CH569 ETH+USB3.0
      recipes/                    # 18 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch561-ch563/                  # CH561/CH563 ARM7TDMI ETH
      recipes/                    # 12 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
    ch5xx-8051/                   # CH543-CH559 8051
      recipes/                    # 12 个场景指南
      resources/                  # API 速查、陷阱、EXAM 例程
```

## 参考资源

- WCH 官网 → http://wch.cn
- MounRiver Studio → http://www.mounriver.com
- EVT SDK 源码 → 各芯片目录下的 `EVT/EXAM/`
- BLE 参考手册 → `chips/ch57x/resources/EXAM/BLE/`
- 每个家族的 API 速查 → `chips/<family>/resources/peripheral_api.md`
- 常见陷阱 → `chips/<family>/resources/pitfalls.md`
