# Changelog

## 1.1.0

### 新增
- 新增 CH579 芯片支持（LCD 段码屏、SPI 以太网、双 SPI）
  - 导入 CH579EVT 完整 EXAM 例程（13 个外设类别，463 个文件）
  - 新增 CH579 专属 API 文档：LCD、NET(WCHNET)、SPI0/SPI1
  - 新增 CH579 内存布局、配置差异、陷阱文档
- 新增 CH585 芯片支持（NFC-A、LCD、LED）
  - 导入 CH585EVT 完整 EXAM 例程（19 个外设类别，1735 个文件）
  - 新增 NFCA(FCB/PICC/CRYPTO1)、LED 控制器 API 文档
- 新增 CH595 芯片支持（以太网、编码器、键盘扫描）
  - 导入 CH595EVT 完整 EXAM 例程（20 个外设类别，1327 个文件）
  - 新增 ENCODER、KEYSCAN、LED API 文档
- 新增 3 个 BLE 场景方案（ch57x）
  - `ble_speed_test.md` — BLE 吞吐量测试、LE 2M PHY 优化
  - `ble_iochub.md` — IoCHub 自定义数据点协议、多设备组网
  - `ble_hid_mouse.md` — HID 鼠标 HOGP、报告描述符
- 更新 `iap_ota.md` — 新增 BackupUpgrade_IAP 和 JumpIAP 变体说明

### 修复
- 修正 ch57x 芯片标签：CH573/CH571 → CH572/CH579（实际 EVT 包为 CH572EVT）
- 修正 ENCODER/KEYSCAN EXAM 路径归属：CH585 → CH595
- 修正 CH634 说明路径：PUB directory → EVT directory

### 补充（SKILL.md EXAM 表）
- ch32v-lowcost：新增 CH32V006 和 CH32L103 完整外设列表（CAN、USB-PD、LPTIM、FreeRTOS 等 12 项）
- ch32v-general：新增 CH32V407 独有外设（LTDC、ARGB、PSRAM、I3C、USBHS）和 CH32V20x BLE
- ch32x-usbpd：新增 CH32X315 和 CH643 完整外设列表
- ch5xx-8051：新增 CH543/CH545/CH549/CH555/CH557/CH559 EXAM 路径（原仅列 CH554）
- ch561-ch563：新增 ADC、WDOG、EEPROM、PARA、UCOS
- ch32h-highperf：新增 SerDes、UHSIF、SDMMC、SWPMI、Application

### 补充（Quick Reference 表）
- CAN：新增 CH32L103 覆盖
- USB-PD：新增 CH32L103 覆盖
- LPTIM：新增 CH32L103 覆盖
- 新增 ARGB LED、PSRAM、LTDC(CH32V407) 条目

### 文档更新
- 更新 ch57x 资源文档：example_list、peripheral_api、pitfalls、memory_layout、config_reference
- 更新 ch58x-ch59x 资源文档：example_list、peripheral_api、pitfalls、memory_layout、config_reference
- 修正 ch32x-usbpd example_list.md 中 CH634 路径描述
- 更新根目录 CLAUDE.md 路由表
- 更新 ch57x-dev-skill/SKILL.md 和 ch58x-ch59x-dev-skill/SKILL.md

## 1.0.0

### 新增
- wch-dev-skill 统一版本首次发布
- 合并 10 个独立芯片家族 skill 到统一的 `chips/` 目录结构
- 统一 SKILL.md 路由表，支持自动芯片识别和导航
- 覆盖 35+ 芯片家族：CH57x、CH58x/CH59x、CH32V 通用/低成本、CH32F ARM、CH32X USB-PD、CH32H417、CH569、CH561/CH563、CH5xx 8051
- 每个家族包含：场景方案（recipes/）、API 速查、陷阱文档、EXAM 例程
- 统一工作流：识别→方案→API→陷阱→验证→确认→执行→构建→调试
- 架构级陷阱文档：RISC-V、ARM Cortex-M3、ARM7TDMI、8051
