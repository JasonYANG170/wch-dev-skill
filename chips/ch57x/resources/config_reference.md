# CH57x config.h Configuration Reference

Each BLE project has a `config.h` file (typically in `HAL/include/` or `APP/include/`) that controls the BLE stack behavior. All macros below are `#define` directives. CH572 (RISC-V) and CH579 (ARM Cortex-M0) share the same config.h format with a few defaults that differ.

## MAC Address

```c
#define BLE_MAC                 FALSE
// TRUE: use custom MAC address from NV storage
// FALSE: use default chip MAC address
```

## Power Management

```c
#define DCDC_ENABLE             FALSE
// TRUE: enable DC/DC converter (lower power consumption)
// FALSE: use LDO (simpler, higher consumption)

#define HAL_SLEEP               FALSE
// TRUE: enable BLE sleep mode (requires RTC wakeup)
// FALSE: no sleep (simpler, always active)

#define SLEEP_RTC_MIN_TIME      US_TO_RTC(1000)
// Minimum RTC time before entering sleep (1000us = 1ms)

#define SLEEP_RTC_MAX_TIME      MS_TO_RTC(RTC_TO_MS(RTC_TIMER_MAX_VALUE) - 1000*60*60)
// Maximum sleep duration before forced wakeup

#define WAKE_UP_RTC_MAX_TIME    US_TO_RTC(1400)
// Maximum time allowed for wakeup stabilization (1400us)
```

## Calibration

```c
#define TEM_SAMPLE              TRUE
// TRUE: enable temperature sampling for RC calibration
// FALSE: disable temperature sampling

#define BLE_CALIBRATION_ENABLE  TRUE
// TRUE: enable periodic RC oscillator calibration
// FALSE: disable calibration

#define BLE_CALIBRATION_PERIOD  120000
// Calibration period in milliseconds (120000 = 2 minutes)
```

## NV Storage

```c
#define BLE_SNV                 TRUE
// TRUE: enable Simple NV storage for BLE bonding data
// FALSE: disable NV storage (no bonding persistence)

#define BLE_SNV_ADDR            0x77E00 - FLASH_ROM_MAX_SIZE
// Flash address for SNV storage (in DataFlash area)
// Actual address depends on FLASH_ROM_MAX_SIZE
//
// CH572 default: 0x77E00 - FLASH_ROM_MAX_SIZE
// CH579 default: 0x3EC00 (DataFlash at 0x3E800, SNV offset within it)
```

## RTC Clock Source

```c
#define CLK_OSC32K              1
// 0: External 32.768kHz crystal (most accurate, needs hardware)
// 1: Internal 32kHz RC oscillator (default, no external crystal)
// 2: Internal 32.768kHz RC oscillator (better accuracy than option 1)
```

## Memory Configuration

```c
#define BLE_MEMHEAP_SIZE        (1024*6)
// BLE stack memory heap size in bytes
// CH572 default: (1024*6) = 6KB
// CH579 default: (1024*8) = 8KB (more RAM available)
// Minimum: 4KB for simple peripheral
// Recommended: 6KB for most applications
// For MESH: 10KB+
// For multi-connection: scale up proportionally

#define BLE_BUFF_MAX_LEN        27
// Maximum BLE buffer length in bytes
// ATT_MTU = BLE_BUFF_MAX_LEN - 4
// 27 → MTU 23 (default BLE)
// 67 → MTU 63
// 251 → MTU 247 (maximum BLE 5.0)

#define BLE_BUFF_NUM            10
// Number of BLE packet buffers
// Minimum: 5 for peripheral-only
// Recommended: 10 for peripheral + notifications
// For central: 15+ (needs buffers for multiple connections)

#define BLE_TX_NUM_EVENT        1
// Number of TX packets per connection event
// 1: conservative (reliable)
// 2-3: higher throughput (may cause buffer pressure)
```

## TX Power

```c
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
// TX power level options:
// LL_TX_POWEER_0_DBM        0 dBm (default, ~10m range)
// LL_TX_POWEER_MINUS_6_DBM  -6 dBm (lower power)
// LL_TX_POWEER_MINUS_20_DBM -20 dBm (minimum, very short range)
```

## Connection Limits

```c
#define PERIPHERAL_MAX_CONNECTION   1
// Maximum peripheral (slave) connections
// Range: 1-3 (bit field limitation in ConnectNumber)

#define CENTRAL_MAX_CONNECTION      3
// Maximum central (master) connections
// Range: 0-15
// Set to 0 for peripheral-only applications

// These are packed into bleConfig_t.ConnectNumber:
// ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2)
```

## Bonding / Security

```c
#define BLE_SMP                 TRUE
// TRUE: enable Security Manager Protocol (pairing/bonding)
// FALSE: disable SMP (no security)

#define BLE_SMP_IO              GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT
// I/O capabilities for pairing:
// GAPBOND_IO_CAP_DISPLAY_ONLY
// GAPBOND_IO_CAP_DISPLAY_YES_NO
// GAPBOND_IO_CAP_KEYBOARD_ONLY
// GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT (default, Just Works)

#define BLE_SMP_BONDING_FLAGS   GAPBOND_AUTHEN_NO_MITM_FLAG
// Bonding flags:
// GAPBOND_AUTHEN_NO_MITM_FLAG     Just Works
// GAPBOND_AUTHEN_MITM_FLAG        MITM protection
// GAPBOND_AUTHEN_SECURE_FLAG      Secure Connections
```

## Typical Configurations

### Simple Peripheral (Low Power)
```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             TRUE
#define HAL_SLEEP               TRUE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*4)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            5
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### Central + Peripheral (Multi-Role)
```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             TRUE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*8)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            15
#define BLE_TX_NUM_EVENT        2
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      3
```

### BLE Mesh
```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*12)
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### High Throughput (BLE 5.0 DLE)
```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             TRUE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*10)
#define BLE_BUFF_MAX_LEN        251
#define BLE_BUFF_NUM            8
#define BLE_TX_NUM_EVENT        3
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

---

## **[CH579]** Config Differences from CH572

The CH579 BLE config.h uses the same macro names but has these differences:

| Setting | CH572 Default | CH579 Default | Notes |
|---------|--------------|--------------|-------|
| `CHIP_ID` | `ID_CH572` | `ID_CH579` (0x79) | Must match chip |
| `BLE_MEMHEAP_SIZE` | `(1024*6)` | `(1024*8)` | CH579 has more RAM |
| `BLE_SNV_ADDR` | `0x77E00 - FLASH_ROM_MAX_SIZE` | `0x3EC00` | Different DataFlash region |
| `WAKE_UP_RTC_MAX_TIME` | `US_TO_RTC(1400)` | `US_TO_RTC(1500)` | Slightly longer wakeup time |
| Architecture | RISC-V (QingKe V4A) | ARM Cortex-M0 | Different toolchain |

The CH579 config.h also includes `CH57xBLE_LIB.H` (same library name as CH572) but the library is compiled for ARM Cortex-M0.
