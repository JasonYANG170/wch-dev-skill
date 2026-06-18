# CH58x/CH59x config.h 配置参考

> **芯片覆盖**: CH583, CH585, CH592, CH595。标注 **CH585** 或 **CH595** 的为芯片特有配置。

## MAC 地址

```c
#define BLE_MAC_ADDR    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}
// 自定义 MAC 地址（小端序）
```

## 电源管理

```c
#define DCDC_ENABLE     TRUE
// TRUE: 启用 DC/DC 转换器，降低功耗
// FALSE: 使用 LDO

#define HAL_SLEEP       FALSE
// TRUE: BLE 空闲时自动进入睡眠
// FALSE: 不自动睡眠（调试时建议关闭）
```

## 内存配置

```c
#define BLE_MEMHEAP_SIZE    (1024*6)
// BLE 协议栈堆大小（字节）
// 最小 4KB，推荐 6KB
// MESH 需要 10KB+
// 多连接需要 8KB+

#define BLE_BUFF_MAX_LEN    27
// BLE 缓冲区最大长度
// = ATT_MTU + 4
// 默认 27 (MTU=23)
// 最大 251 (MTU=247)
```

## 发射功率

```c
#define BLE_TX_POWER    TX_POW_0_DBm
// TX_POW_N20_DBm   -20dBm
// TX_POW_N12_DBm   -12dBm
// TX_POW_N6_DBm    -6dBm
// TX_POW_0_DBm      0dBm（默认）
// TX_POW_2_DBm      2dBm
// TX_POW_4_DBm      4dBm
// TX_POW_6_DBm      6dBm
```

## 连接参数

```c
#define PERIPHERAL_MAX_CONNECTION    1
// 最大外设连接数（0-3）

#define CENTRAL_MAX_CONNECTION       3
// 最大中心连接数（0-15）

// 连接参数编码
cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
```

## 广播参数

```c
#define DEFAULT_ADVERTISING_INTERVAL    160
// 广播间隔（单位 0.625ms）
// 160 = 100ms

#define B_MAX_ADV_LEN    31
// 广播数据最大长度（字节）
```

## 连接参数

```c
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL    80
// 最小连接间隔（单位 1.25ms）
// 80 = 100ms

#define DEFAULT_DESIRED_MAX_CONN_INTERVAL    800
// 最大连接间隔
// 800 = 1000ms

#define DEFAULT_DESIRED_SLAVE_LATENCY        0
// 从机延迟（跳过的连接事件数）

#define DEFAULT_DESIRED_CONN_TIMEOUT         1000
// 连接超时（单位 10ms）
// 1000 = 10s
```

## 安全配置

```c
#define BLE_SECURITY    FALSE
// TRUE: 启用配对和加密
// FALSE: 不启用安全连接

#define GAP_BONDING     FALSE
// TRUE: 启用绑定（保存配对信息）
// FALSE: 不绑定
```

## RTC 时钟源

```c
#define RTC_CLK_SRC    LSI_CLK_32K_Quart
// LSI_CLK_32K_Quart    — 内部 RC 32K
// LSI_CLK_32K_External — 外部 32K 晶体
```

## 典型配置

### 简单外设（低功耗）

```c
#define BLE_MEMHEAP_SIZE         (1024*6)
#define BLE_BUFF_MAX_LEN         27
#define BLE_TX_POWER             TX_POW_0_DBm
#define DCDC_ENABLE              TRUE
#define HAL_SLEEP                TRUE
#define PERIPHERAL_MAX_CONNECTION 1
#define CENTRAL_MAX_CONNECTION    0
```

### 主从一体

```c
#define BLE_MEMHEAP_SIZE         (1024*8)
#define BLE_BUFF_MAX_LEN         27
#define BLE_TX_POWER             TX_POW_0_DBm
#define DCDC_ENABLE              TRUE
#define HAL_SLEEP                TRUE
#define PERIPHERAL_MAX_CONNECTION 1
#define CENTRAL_MAX_CONNECTION    3
```

### BLE Mesh

```c
#define BLE_MEMHEAP_SIZE         (1024*12)
#define BLE_BUFF_MAX_LEN         27
#define BLE_TX_POWER             TX_POW_0_DBm
#define DCDC_ENABLE              TRUE
#define HAL_SLEEP                FALSE
```

### 高吞吐量 (BLE 5.0 DLE)

```c
#define BLE_MEMHEAP_SIZE         (1024*8)
#define BLE_BUFF_MAX_LEN         251
#define BLE_TX_POWER             TX_POW_6_DBm
#define DCDC_ENABLE              TRUE
#define HAL_SLEEP                FALSE
```

## CH585 vs CH595 BLE 配置差异

CH585 和 CH595 的 CONFIG.h 格式相同，但有以下关键差异：

### 芯片标识和头文件

```c
// CH585
#define CHIP_ID    ID_CH585
#include "CH58xBLE_LIB.h"    // BLE协议栈库
#include "CH58x_common.h"

// CH595
#define CHIP_ID    ID_CH595
#include "CH59xBLE_LIB.h"    // BLE协议栈库（不同芯片使用不同库）
#include "CH59x_common.h"
```

### SNV (存储) 地址

```c
// CH585 — 动态计算（基于Flash大小）
#define BLE_SNV_ADDR    0x77000 - FLASH_ROM_MAX_SIZE

// CH595 — 固定地址（Flash较小，240KB）
#define BLE_SNV_ADDR    0x3B000    // 注意：固定地址时只预留4KB Flash给SNV
```

**重要**: CH595 Flash 只有 240KB，SNV 地址必须在 Flash 末尾预留空间。修改 SNV 地址时注意不要覆盖应用程序代码。

### 唤醒等待时间

```c
// CH585 — 1600us（128KB RAM，唤醒时间较长）
#define WAKE_UP_RTC_MAX_TIME    US_TO_RTC(1600)

// CH595 — 1100us（32KB RAM，唤醒较快）
#define WAKE_UP_RTC_MAX_TIME    US_TO_RTC(1100)
```

**原因**: CH585 有 128KB RAM，从睡眠唤醒时需要更长时间等待 32MHz 晶振稳定和 RAM 恢复。

### 发射功率枚举名称

```c
// CH585
#define BLE_TX_POWER    LL_TX_POWEER_0_DBM   // 注意拼写: POWEER

// CH595
#define BLE_TX_POWER    LL_TX_PWR_0_DBM      // 注意拼写: PWR
```

**注意**: 发射功率枚举名称在两个芯片的 BLE 库中不同。使用错误的名称会导致编译错误。

### RAM 对 BLE 堆的影响

CH585 有 128KB RAM，可以分配更大的 BLE 堆：

```c
// CH585 — 可以使用更大的堆（MESH/多连接场景）
#define BLE_MEMHEAP_SIZE    (1024*12)   // 12KB，MESH推荐

// CH595 — RAM 有限（32KB），堆大小需谨慎
#define BLE_MEMHEAP_SIZE    (1024*6)    // 6KB，一般场景
// CH595 多连接最大建议 8KB，MESH 建议 10KB
```
