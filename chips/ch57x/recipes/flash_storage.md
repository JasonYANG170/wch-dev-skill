# Flash / EEPROM 数据存储

> **适用摘要**: 使用片上 Flash 或 DataFlash (EEPROM) 进行数据读写存储。

## 触发意图

- "Flash 读写"
- "存储数据到 Flash"
- "EEPROM 操作"
- "掉电保存数据"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/FLASH/` |

## 分步说明

### Flash (Code Area) 读写

```c
#include "CH57x_common.h"

// Flash 读取（任意地址、任意长度）
void Flash_Read(uint32_t addr, uint8_t *buf, uint32_t len) {
    FLASH_ROM_READ(addr, buf, len);
}

// Flash 写入（必须 256 字节对齐）
uint8_t Flash_Write(uint32_t addr, uint8_t *buf, uint32_t len) {
    // addr 和 len 必须是 256 的倍数
    return FLASH_ROM_WRITE(addr, buf, len);
}

// Flash 擦除（必须 256 字节对齐）
uint8_t Flash_Erase(uint32_t addr, uint32_t len) {
    return FLASH_ROM_ERASE(addr, len);
}

// Flash 校验
uint8_t Flash_Verify(uint32_t addr, uint8_t *buf, uint32_t len) {
    return FLASH_ROM_VERIFY(addr, buf, len);
}
```

### 读-改-写模式

```c
// 更新 Flash 中单个字节（不丢失其他数据）
void Flash_UpdateByte(uint32_t sector_addr, uint16_t offset, uint8_t value) {
    uint8_t buf[256];
    FLASH_ROM_READ(sector_addr, buf, 256);  // 读取整个扇区
    buf[offset] = value;                     // 修改目标字节
    FLASH_ROM_ERASE(sector_addr, 256);       // 擦除扇区
    FLASH_ROM_WRITE(sector_addr, buf, 256);  // 写回
}
```

### DataFlash (EEPROM) 操作

```c
// DataFlash 地址范围：0x77E00 - 0x77FFF（512 字节）
// 用于存储配置数据、BLE bonding 信息等

// 读取
uint8_t EEPROM_Read(uint32_t addr, void *buf, uint32_t len) {
    return EEPROM_READ(addr, buf, len);
}

// 写入
uint8_t EEPROM_Write(uint32_t addr, void *buf, uint32_t len) {
    return EEPROM_WRITE(addr, buf, len);
}

// 擦除
uint8_t EEPROM_Erase(uint32_t addr, uint32_t len) {
    return EEPROM_ERASE(addr, buf, len);
}
```

### 存储用户配置示例

```c
// 用户配置结构体
typedef struct {
    uint32_t magic;       // 0x12345678
    uint8_t brightness;
    uint8_t color_temp;
    uint16_t checksum;
} user_config_t;

#define CONFIG_ADDR    0x77E00  // DataFlash 起始地址

void Config_Save(user_config_t *cfg) {
    cfg->checksum = CalcChecksum(cfg);
    EEPROM_ERASE(CONFIG_ADDR, 256);
    EEPROM_WRITE(CONFIG_ADDR, cfg, sizeof(user_config_t));
}

uint8_t Config_Load(user_config_t *cfg) {
    EEPROM_READ(CONFIG_ADDR, cfg, sizeof(user_config_t));
    if (cfg->magic != 0x12345678) return 0;  // 无效
    if (cfg->checksum != CalcChecksum(cfg)) return 0;  // 校验失败
    return 1;  // 有效
}
```

### 获取芯片唯一 ID

```c
uint8_t uid[8];
GET_UNIQUE_ID(uid);
printf("Chip UID: %02X%02X%02X%02X%02X%02X%02X%02X\n",
       uid[0], uid[1], uid[2], uid[3],
       uid[4], uid[5], uid[6], uid[7]);
```

## Flash 地址布局

| 区域 | 起始地址 | 结束地址 | 大小 | 用途 |
|------|----------|----------|------|------|
| 系统引导 | 0x00000000 | 0x00000FFF | 4KB | ROM bootloader |
| 应用代码 | 0x00001000 | 0x0006FFFF | 444KB | 用户程序 |
| DataFlash | 0x00077E00 | 0x00077FFF | 512B | NV 存储 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 写入后数据错乱 | 未擦除就写入 | 先擦除再写入 |
| 部分数据丢失 | 只修改了部分扇区 | 读-改-写整个扇区 |
| 地址对齐错误 | 地址不是 256 倍数 | 确保地址和长度都是 256 的倍数 |
| 覆盖了应用代码 | 写入地址在代码区 | 确保写入地址不与代码冲突 |

## 参考项目

- `resources/EXAM/FLASH/` — Flash 读写示例
