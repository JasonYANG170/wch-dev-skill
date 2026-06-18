# IAP 引导程序与 BLE OTA 升级

> **适用摘要**: 实现 IAP (In-Application Programming) 引导程序和 BLE 空中固件升级。

## 触发意图

- "IAP 引导程序"
- "OTA 固件升级"
- "远程更新固件"
- "双镜像升级"
- "BackupUpgrade IAP"
- "JumpIAP 跳转"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/IAP/`, `resources/EXAM/BLE/BackupUpgrade_OTA/` |
| 知识 | Flash 地址布局、引导加载程序概念 |

## 分步说明

### 内存布局

```
0x00000000 - 0x00000FFF  Bootloader (4KB)
0x00001000 - 0x00038FFF  Image A (216KB) — 运行中的应用
0x00039000 - 0x0006AFFF  Image B (216KB) — OTA 备份
0x0006D000 - 0x0006FFFF  IAP Image (12KB) — 跳转用
0x00077E00 - 0x00077FFF  DataFlash — 镜像标志位
```

### 镜像标志位

```c
#define FLAG_IMAGE_A    0x01
#define FLAG_IMAGE_B    0x02
#define FLAG_IAP        0x03
#define FLAG_ADDR       0x77E00

uint8_t ReadImageFlag(void) {
    uint8_t flag;
    EEPROM_READ(FLAG_ADDR, &flag, 1);
    return flag;
}

void WriteImageFlag(uint8_t flag) {
    EEPROM_ERASE(FLAG_ADDR, 256);
    EEPROM_WRITE(FLAG_ADDR, &flag, 1);
}
```

### IAP 引导程序（UART）

参考 `resources/EXAM/IAP/UART_IAP/`：

```c
// IAP 链接脚本
// FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K

// IAP 通信协议
// 命令: INFO(0x84), ERASE(0x81), PROM(0x80), VERIFY(0x82), END(0x83)
// 帧格式: CMD | LEN | ADDR/DATA | CHECKSUM

void IAP_ProcessCommand(uint8_t cmd, uint8_t *data, uint16_t len) {
    switch (cmd) {
        case 0x84:  // INFO — 返回设备信息
            SendDeviceInfo();
            break;
        case 0x81:  // ERASE — 擦除 Flash
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            uint16_t blocks = BUILD_UINT16(data[4], data[5]);
            FLASH_ROM_ERASE(addr, blocks * 256);
            break;
        case 0x80:  // PROM — 写入数据
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            FLASH_ROM_WRITE(addr, &data[4], len - 4);
            break;
        case 0x82:  // VERIFY — 校验数据
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            FLASH_ROM_VERIFY(addr, &data[4], len - 4);
            break;
        case 0x83:  // END — 完成，跳转到应用
            WriteImageFlag(FLAG_IMAGE_A);
            NVIC_SystemReset();
            break;
    }
}
```

### IAP 跳转到应用

```c
// 在 IAP 中判断是否需要跳转
void JumpToApp(void) {
    uint8_t flag = ReadImageFlag();

    uint32_t app_addr;
    switch (flag) {
        case FLAG_IMAGE_A: app_addr = 0x00001000; break;
        case FLAG_IMAGE_B: app_addr = 0x00039000; break;
        default: app_addr = 0x00001000; break;
    }

    // 检查应用是否有效（栈指针在 RAM 范围内）
    uint32_t sp = *((uint32_t *)app_addr);
    if (sp >= 0x20003800 && sp <= 0x20008000) {
        // 设置栈指针
        __set_MSP(sp);
        // 跳转到复位向量
        void (*app_entry)(void) = (void (*)(void))(*((uint32_t *)(app_addr + 4)));
        app_entry();
    }
    // 无效应用，留在 IAP
}
```

### BLE OTA 升级流程

参考 `resources/EXAM/BLE/BackupUpgrade_OTA/`：

```c
// OTA Profile UUID
// 服务: 9ECA-DC24-0EE5-A9E0-93F3-A3B5-0200-406E
// 写入特征: ...0201-406E (OTA 数据写入)
// 通知特征: ...0202-406E (OTA 状态通知)

// OTA 命令处理
void OTA_ProcessCommand(uint8_t cmd, uint8_t *data, uint16_t len) {
    switch (cmd) {
        case OTA_CMD_INFO:    // 0x84 — 返回设备信息
            SendOTAInfo();
            break;

        case OTA_CMD_ERASE:   // 0x81 — 擦除目标镜像
            uint32_t addr = GetTargetImageAddr();
            FLASH_ROM_ERASE(addr, target_size);
            break;

        case OTA_CMD_PROGRAM: // 0x80 — 写入固件数据
            uint32_t addr = current_write_addr;
            FLASH_ROM_WRITE(addr, data, 256);
            current_write_addr += 256;
            break;

        case OTA_CMD_VERIFY:  // 0x82 — 校验
            VerifyFirmware();
            break;

        case OTA_CMD_END:     // 0x83 — 完成，切换镜像
            SwitchImage();
            NVIC_SystemReset();
            break;
    }
}

// 切换活动镜像
void SwitchImage(void) {
    uint8_t current = ReadImageFlag();
    uint8_t next = (current == FLAG_IMAGE_A) ? FLAG_IMAGE_B : FLAG_IMAGE_A;
    WriteImageFlag(next);
}
```

### 双镜像 IAP 应用链接脚本

```
// Image A 应用
MEMORY {
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 216K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}

// Image B 应用
MEMORY {
    FLASH (rx) : ORIGIN = 0x00039000, LENGTH = 216K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

### BLE BackupUpgrade IAP 引导程序

参考 `resources/EXAM/BLE/BackupUpgrade_IAP/`。

此项目实现一个基于 C 语言的 IAP 引导程序，用于配合 BLE OTA 升级流程。上电后引导程序检查 DataFlash 中的镜像标志，若 OTA 更新已完成（标志为 `IMAGE_IAP_FLAG`），则将备份镜像从 Image B 复制到 Image A，然后跳转到应用。

**Flash 布局（ota.h）**:

```
0x00000000 - 0x00000FFF  Bootloader 区（4KB）
0x00001000 - 0x0001AFFF  Image A（108KB）— 运行中的应用
0x0001B000 - 0x00033FFF  Image B（108KB）— OTA 备份
0x00034000 - 0x00036FFF  Image IAP（12KB）— IAP 跳转用
0x0003A000              DataFlash — 镜像标志位
```

**镜像标志定义**:

```c
#define IMAGE_A_FLAG    0x01    // 当前运行 Image A
#define IMAGE_B_FLAG    0x02    // 当前运行 Image B
#define IMAGE_IAP_FLAG  0x03    // OTA 更新完成，待提交

#define OTA_DATAFLASH_ADD  0x0003A000
```

**引导程序核心逻辑**:

```c
#include "CH57x_common.h"
#include "OTA.h"

unsigned char CurrImageFlag = 0xff;
#define jumpApp  ((void (*)(void))((int *)IMAGE_A_START_ADD))

// 读取镜像标志（DataFlash 为空时默认 Image A）
void ReadImageFlag(void) {
    OTADataFlashInfo_t p_image_flash;
    FLASH_ROM_READ(OTA_DATAFLASH_ADD, &p_image_flash, 4);
    CurrImageFlag = p_image_flash.ImageFlag;
    if ((CurrImageFlag != IMAGE_A_FLAG) &&
        (CurrImageFlag != IMAGE_B_FLAG) &&
        (CurrImageFlag != IMAGE_IAP_FLAG)) {
        CurrImageFlag = IMAGE_A_FLAG;
    }
}

// OTA 提交：复制备份镜像到运行区
void jump_APP(void) {
    if (CurrImageFlag == IMAGE_IAP_FLAG) {
        __attribute__((aligned(8))) uint8_t flash_Data[1024];
        // 擦除 Image A
        FLASH_ROM_ERASE(IMAGE_A_START_ADD, IMAGE_A_SIZE);
        // 逐块从 Image B 复制到 Image A
        for (uint8_t i = 0; i < IMAGE_A_SIZE / 1024; i++) {
            FLASH_ROM_READ(IMAGE_B_START_ADD + (i * 1024), flash_Data, 1024);
            FLASH_ROM_WRITE(IMAGE_A_START_ADD + (i * 1024), flash_Data, 1024);
        }
        SwitchImageFlag(IMAGE_A_FLAG);
        // 擦除备份区
        FLASH_ROM_ERASE(IMAGE_B_START_ADD, IMAGE_A_SIZE);
    }
    jumpApp();  // 跳转到 0x1000
}

int main(void) {
    SetSysClock(CLK_SOURCE_HSE_PLL_100MHz);
#ifdef DEBUG
    UART_DefInit();
#endif
    ReadImageFlag();
    jump_APP();
}
```

**使用场景**: 将此引导程序烧录到芯片的 0x0000 地址。BLE OTA 完成后，APP 将镜像标志写为 `IMAGE_IAP_FLAG`，下次复位时引导程序自动将新固件从备份区复制到运行区。

### BLE BackupUpgrade JumpIAP 跳转变体

参考 `resources/EXAM/BLE/BackupUpgrade_JumpIAP/`。

此项目是一个极简的跳转示例，`main()` 函数为空体。实际的跳转行为完全由启动汇编文件（`startup_CH57x.s`）中的 `j` 指令控制。

```c
// main.c — 空实现
// 跳转地址通过修改 startup 文件中的 j 指令来配置
int main(void) {
}
```

**使用场景**: 当不需要在 C 层做镜像复制逻辑，而是希望在汇编启动阶段直接跳转到目标地址时使用此变体。适用于跳转目标固定、无需运行时判断的简单场景。

### BackupUpgrade_IAP vs BackupUpgrade_JumpIAP 对比

| 特性 | BackupUpgrade_IAP | BackupUpgrade_JumpIAP |
|---|---|---|
| 跳转方式 | C 代码中函数指针跳转 | 启动文件汇编 `j` 指令 |
| OTA 提交逻辑 | 有（复制备份镜像 + 切换标志） | 无（需外部完成） |
| 镜像标志检查 | 有（DataFlash 读取） | 无 |
| 代码量 | ~136 行 C | ~29 行（空 main） |
| 适用场景 | BLE OTA 双镜像升级 | 固定地址跳转、无 OTA 需求 |

**建议**: BLE OTA 场景使用 `BackupUpgrade_IAP`；仅需简单跳转到固定地址（如自定义 bootloader）时使用 `BackupUpgrade_JumpIAP`。

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| OTA 后无法启动 | 镜像标志位未更新 | 确保 WriteImageFlag() 成功 |
| 写入数据错乱 | 地址未对齐 | OTA 数据按 256 字节对齐 |
| IAP 无法跳转 | 应用起始地址错误 | 检查链接脚本 ORIGIN 值 |
| 擦除超时 | Flash 擦除时间长 | 擦除大区域需等待 |

## 参考项目

- `resources/EXAM/IAP/UART_IAP/` — UART IAP 引导程序
- `resources/EXAM/IAP/USB_IAP/` — USB IAP 引导程序
- `resources/EXAM/BLE/BackupUpgrade_OTA/` — BLE OTA 双镜像升级
- `resources/EXAM/BLE/BackupUpgrade_IAP/` — IAP 引导程序（C 实现，OTA 提交逻辑）
- `resources/EXAM/BLE/BackupUpgrade_JumpIAP/` — 跳转变体（汇编启动跳转）
- `resources/EXAM/BLE/OnlyUpdateApp_Peripheral/` — 固定地址 OTA
