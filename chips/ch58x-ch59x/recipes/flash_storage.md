# Flash 存储

> **适用摘要**: CH58x/CH59x Flash 读写、擦除和数据存储

## 触发意图

- "Flash 读写"
- "保存数据到 Flash"
- "EEPROM 模拟"
- "数据持久化"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |

## 调用链

```
Step 1: 确定存储地址（避开程序区）
Step 2: 读取现有数据
Step 3: 修改数据
Step 4: 擦除扇区
Step 5: 写回数据
```

## 分步说明

### Step 1: 读 Flash

```c
uint8_t buf[256];
FLASH_ROM_READ(addr, buf, 256);
```

### Step 2: 写 Flash（必须先擦除）

```c
// 写入前必须擦除整个 256 字节扇区
FLASH_ROM_ERASE(addr, 256);
FLASH_ROM_WRITE(addr, buf, 256);
```

### Step 3: 读-改-写模式

```c
void flash_update_byte(uint32_t sector_addr, uint16_t offset, uint8_t value) {
    uint8_t buf[256];
    FLASH_ROM_READ(sector_addr, buf, 256);
    if (buf[offset] != value) {
        buf[offset] = value;
        FLASH_ROM_ERASE(sector_addr, 256);
        FLASH_ROM_WRITE(sector_addr, buf, 256);
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 写入失败 | 未先擦除 | 先调用 FLASH_ROM_ERASE |
| 数据损坏 | 写入地址在程序区 | 使用 Flash 末尾地址 |
| 写入不完整 | 未对齐 | 确保 4 字节对齐 |

## 参考项目

- `resources/EXAM/FLASH/`
