---
name: wch-dev-skill
description: >-
  Unified AI Skill for all WCH (Nanjing Qinheng Microelectronics) microcontroller firmware development.
  Covers 35+ chip families across RISC-V, ARM Cortex-M3, ARM7TDMI, and 8051 architectures.
  Includes BLE (CH57x/CH58x/CH59x), Ethernet (CH56x/CH561/CH563), USB-PD (CH32X/CH6xx),
  USB 3.0 (CH569/CH32H417), high-performance dual-core (CH32H417), general-purpose (CH32V/CH32F),
  low-cost (CH32V003/CH32L103), and legacy 8051 (CH5xx).
  All recipes, API references, pitfalls, and example projects are merged under chips/ directory.
  Trigger words: "WCH", "沁恒", "CH32", "CH57", "CH58", "CH59", "CH56", "CH54", "CH55", "CH64",
  "RISC-V", "MounRiver", "BLE", "蓝牙", "USB-PD", "Ethernet", "8051"
tags:
  - embedded
  - WCH
  - RISC-V
  - ARM
  - 8051
  - BLE
  - bluetooth
  - USB-PD
  - Ethernet
  - USB3
  - microcontroller
  - firmware
license: MIT
compatibility: Build requires MounRiver Studio (RISC-V/ARM) or Keil C51/MDK; target is WCH MCU family
metadata:
  author: Community
  version: "2.0.0"
---

# wch-dev-skill

Unified AI Skill for all WCH microcontroller firmware development. All recipes, API references, pitfalls, and example projects are merged under `chips/` directory, organized by chip family.

## Directory Structure

```
wch-dev-skill/
  SKILL.md                    # This file - unified routing and guidance
  chips/
    ch57x/                    # CH572/CH579 BLE 5.0
      recipes/                # 20 scenario guides
      resources/              # API ref, pitfalls, config guide, EXAM examples
    ch58x-ch59x/              # CH583/CH585/CH592/CH595 BLE + USB + LCD
      recipes/                # 29 scenario guides
      resources/              # API ref, pitfalls, config guide, EXAM examples
    ch32v-general/            # CH32V103/CH32V20x/CH32V307/CH32V407
      recipes/                # 21 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch32v-lowcost/            # CH32V003/CH32V006/CH32L103
      recipes/                # 14 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch32f-arm/                # CH32F103/CH32F20x/CH32M030 ARM Cortex-M3
      recipes/                # 21 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch32x-usbpd/              # CH32X035/CH32X315/CH643/CH641/CH634 USB-PD
      recipes/                # 13 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch56x-ethernet/           # CH569 RISC-V ETH + USB 3.0
      recipes/                # 17 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch561-ch563/              # CH561/CH563 ARM7TDMI ETH
      recipes/                # 12 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch32h-highperf/           # CH32H417 dual-core USB3 + LTDC
      recipes/                # 38 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
    ch5xx-8051/               # CH543-CH559 8051
      recipes/                # 12 scenario guides
      resources/              # API ref, pitfalls, EXAM examples
```

## Chip Family Routing

**Step 1: Identify the target chip. Step 2: Navigate to the correct `chips/<family>/` directory. Step 3: Read the recipe, then copy the example.**

| Chip Family | Directory | Architecture | Header | Build Tool | Key Features |
|---|---|---|---|---|---|
| CH57x (CH572/CH579) | `chips/ch57x/` | RISC-V | `CH57x_common.h` | MounRiver | BLE 5.0, USB, RF, LCD (CH579), NET (CH579) |
| CH58x/CH59x (CH583/CH585/CH592/CH595) | `chips/ch58x-ch59x/` | RISC-V | `CH58x_common.h` / `CH59x_common.h` | MounRiver | BLE + USB, LCD (CH585/CH592), NET (CH595), ENCODER/KEYSCAN (CH595), NFCA (CH585), Mesh |
| CH32V General (CH32V103/CH32V20x/CH32V307/CH32V407) | `chips/ch32v-general/` | RISC-V | `ch32v10x.h` / `ch32v20x.h` / `ch32v30x.h` / `ch32v4x7.h` | MounRiver | CAN, ETH, USB, BLE (V20x), DAC, FSMC |
| CH32V Low-Cost (CH32V003/CH32V006/CH32L103) | `chips/ch32v-lowcost/` | RISC-V | `ch32v00x.h` / `ch32v00X.h` / `ch32l103.h` | MounRiver | Ultra-low-cost, OPA, USB-PD (CH32L103) |
| CH32F ARM (CH32F103/CH32F20x/CH32M030) | `chips/ch32f-arm/` | ARM Cortex-M3 | `ch32f10x.h` / `ch32f20x.h` | Keil MDK / MounRiver | STM32-compatible, BLE (CH32F20x) |
| CH32X USB-PD (CH32X035/CH32X315/CH643/CH641/CH634) | `chips/ch32x-usbpd/` | RISC-V | `ch32x035.h` / `ch32x3x5.h` / `ch643.h` / `ch641.h` | MounRiver | USB-PD, PIOC |
| CH569 Ethernet | `chips/ch56x-ethernet/` | RISC-V | `CH56x_common.h` | MounRiver | ETH, USB 3.0, eMMC, ECDC, HSPI |
| CH561/CH563 ARM | `chips/ch561-ch563/` | ARM7TDMI | `CH561SFR.H` / `CH563SFR.H` | Keil MDK | ETH, USB 2.0 HS (CH563), register-based |
| CH32H417 High-Perf | `chips/ch32h-highperf/` | RISC-V | `ch32h417.h` | MounRiver | USB 3.0, USB-PD, LTDC, SerDes, dual-core |
| CH5xx 8051 (CH543-CH559) | `chips/ch5xx-8051/` | 8051 | `CH5xx.H` | Keil C51 / SDCC | USB, TouchKey, register-based |

---

## Universal Core Principles

These apply to **all** WCH chip families:

1. **Never guess APIs** -- Check `chips/<family>/resources/` first; no matching doc = does not exist
2. **Enable peripheral clock before use** -- Every WCH chip requires clock enable before peripheral access
3. **Configure GPIO before peripheral use** -- Set pin mode (alternate function, input, output) before initializing the peripheral
4. **Flash erases in pages/sectors** -- Cannot overwrite in place; always erase then write (page size varies: 64B/256B/1KB/4KB depending on chip)
5. **Copy closest example, then modify** -- Never write from scratch; copy the recommended template from `chips/<family>/resources/EXAM/` and adapt it
6. **Check chip-specific headers** -- Each family has its own header file; do not mix headers between families
7. **IAP applications need offset linker scripts** -- Bootloader at 0x0000, application at offset (typically 0x1000 or 0x4000)
8. **Interrupt handler attributes vary by architecture** -- RISC-V: `__attribute__((interrupt("WCH-Interrupt-fast")))`; ARM: `__irq` or standard vector; 8051: `interrupt` keyword

---

## BLE-Specific Principles (CH57x/CH58x/CH59x/CH32V20x/CH32F20x)

1. **BLE initialization order is strict**: `SetSysClock` -> `BLEInit` -> `HAL_Init` -> `GAPRole_Init` -> `App_Init` -> `Main_Circulation`
2. **`Main_Circulation()` never returns** -- It runs the TMOS event loop; main() must not exit
3. **`config.h` controls BLE stack** -- Memory heap, buffer sizes, TX power, connection limits, sleep behavior
4. **`ATT_MTU = BLE_BUFF_MAX_LEN - 4`** -- Buffer must be 4 bytes larger than desired MTU
5. **GATT service registration order**: GAP -> GATT -> DevInfo -> Custom services
6. **BLE interrupt handlers must use `.highcode` section** -- Place in RAM for fast execution
7. **`BLE_MEMHEAP_SIZE` minimum 6KB** -- Too small causes stack crashes

---

## Scenario Quick Reference

### Project Setup

Every chip family has a `recipes/new_project.md`:

| Chip Family | Recipe | Template Example |
|---|---|---|
| CH57x | `chips/ch57x/recipes/new_project.md` | `chips/ch57x/resources/EXAM/BLE/Peripheral/` |
| CH58x/CH59x | `chips/ch58x-ch59x/recipes/new_project.md` | `chips/ch58x-ch59x/resources/EXAM/CH583/BLE/Peripheral/` |
| CH32V General | `chips/ch32v-general/recipes/new_project.md` | `chips/ch32v-general/resources/EXAM/CH32V103/GPIO/` |
| CH32V Low-Cost | `chips/ch32v-lowcost/recipes/new_project.md` | `chips/ch32v-lowcost/resources/EXAM/CH32V003/GPIO/` |
| CH32F ARM | `chips/ch32f-arm/recipes/new_project.md` | `chips/ch32f-arm/resources/EXAM/CH32F103/GPIO/` |
| CH32X USB-PD | `chips/ch32x-usbpd/recipes/new_project.md` | `chips/ch32x-usbpd/resources/EXAM/CH32X035/GPIO/` |
| CH569 | `chips/ch56x-ethernet/recipes/new_project.md` | `chips/ch56x-ethernet/resources/EXAM/CH569/GPIO/` |
| CH561/CH563 | `chips/ch561-ch563/recipes/new_project.md` | `chips/ch561-ch563/resources/EXAM/CH561/GPIO/` |
| CH32H417 | `chips/ch32h-highperf/recipes/new_project.md` | `chips/ch32h-highperf/resources/EXAM/GPIO/` |
| CH5xx 8051 | `chips/ch5xx-8051/recipes/new_project.md` | `chips/ch5xx-8051/resources/EXAM/CH554EVT/GPIO/` |

### BLE Applications

| Scenario | CH57x | CH58x/CH59x |
|---|---|---|
| BLE peripheral | `chips/ch57x/recipes/ble_peripheral.md` | `chips/ch58x-ch59x/recipes/ble_peripheral.md` |
| BLE central | -- | `chips/ch58x-ch59x/recipes/ble_central.md` |
| BLE HID | `chips/ch57x/recipes/ble_hid.md` | `chips/ch58x-ch59x/recipes/ble_hid.md` |
| BLE Mesh | -- | `chips/ch58x-ch59x/recipes/ble_mesh.md` |
| BLE OTA | `chips/ch57x/recipes/iap_ota.md` | `chips/ch58x-ch59x/recipes/ble_ota.md` |
| BLE ANCS | -- | `chips/ch58x-ch59x/recipes/ble_ancs.md` |
| BLE broadcaster | -- | `chips/ch58x-ch59x/recipes/ble_broadcaster.md` |
| BLE observer | -- | `chips/ch58x-ch59x/recipes/ble_observer.md` |
| BLE multi-central | -- | `chips/ch58x-ch59x/recipes/ble_multi_central.md` |
| BLE speed test | `chips/ch57x/recipes/ble_speed_test.md` | `chips/ch58x-ch59x/recipes/ble_speed_test.md` |
| BLE UART bridge | -- | `chips/ch58x-ch59x/recipes/ble_uart_bridge.md` |
| BLE USB combo | -- | `chips/ch58x-ch59x/recipes/ble_usb_combo.md` |
| BLE heartrate | -- | `chips/ch58x-ch59x/recipes/ble_heartrate.md` |
| BLE RF PHY | -- | `chips/ch58x-ch59x/recipes/ble_rf_phy.md` |
| BLE DTM test | `chips/ch57x/recipes/ble_rf_dtm.md` | `chips/ch58x-ch59x/recipes/ble_dtm.md` |
| BLE cent+peri | -- | `chips/ch58x-ch59x/recipes/ble_centperi.md` |
| 2.4GHz RF | `chips/ch57x/recipes/rf_comm.md` | -- |

### Common Peripheral Drivers

| Scenario | Recipe Path (pick the one for your chip family) |
|---|---|
| **GPIO** | `chips/<family>/recipes/gpio_control.md` -- available for all families |
| **UART** | `chips/<family>/recipes/uart_comm.md` -- available for all families |
| **SPI** | `chips/<family>/recipes/spi_comm.md` -- available for all families |
| **I2C** | `chips/ch32v-general/recipes/i2c_comm.md`, `chips/ch32f-arm/recipes/i2c_comm.md`, `chips/ch32v-lowcost/recipes/i2c_comm.md`, `chips/ch32h-highperf/recipes/i2c_comm.md`, `chips/ch58x-ch59x/recipes/i2c_comm.md` |
| **ADC** | `chips/<family>/recipes/adc_reading.md` -- available for most families |
| **Timer/PWM** | `chips/<family>/recipes/timer_pwm.md` -- available for all families |
| **Flash** | `chips/<family>/recipes/flash_storage.md` -- available for all families |
| **DMA** | `chips/ch32v-general/recipes/dma_transfer.md`, `chips/ch32v-lowcost/recipes/dma_transfer.md`, `chips/ch32h-highperf/recipes/dma_transfer.md` |
| **Power** | `chips/<family>/recipes/power_management.md` -- available for most families |
| **IAP/OTA** | `chips/<family>/recipes/iap_ota.md` -- available for all families |
| **Watchdog** | `chips/ch32v-general/recipes/watchdog.md`, `chips/ch32v-lowcost/recipes/watchdog.md`, `chips/ch32h-highperf/recipes/watchdog.md` |
| **EXTI** | `chips/ch32v-lowcost/recipes/exti_interrupt.md`, `chips/ch32h-highperf/recipes/exti_interrupt.md` |
| **RTC** | `chips/ch32v-general/recipes/rtc_config.md`, `chips/ch32h-highperf/recipes/rtc_clock.md` |
| **TouchKey** | `chips/ch32v-general/recipes/touchkey.md`, `chips/ch32h-highperf/recipes/touchkey.md`, `chips/ch32x-usbpd/recipes/touchkey_sensing.md`, `chips/ch5xx-8051/recipes/touch_key.md` |
| **OPA/Comparator** | `chips/ch57x/recipes/cmp.md`, `chips/ch32v-lowcost/recipes/opa_config.md`, `chips/ch32f-arm/recipes/opa_config.md`, `chips/ch32h-highperf/recipes/opa_config.md` |

### Specialized Peripherals

| Scenario | Recipe Path |
|---|---|
| **CAN bus** | `chips/ch32v-general/recipes/can_comm.md`, `chips/ch32f-arm/recipes/can_comm.md`, `chips/ch32h-highperf/recipes/can_comm.md`, `chips/ch32v-lowcost/resources/EXAM/CH32L103/CAN/` (CH32L103, see can_comm recipe) |
| **Ethernet** | `chips/ch32v-general/recipes/eth_comm.md`, `chips/ch32f-arm/recipes/ethernet.md`, `chips/ch56x-ethernet/recipes/eth_comm.md`, `chips/ch561-ch563/recipes/eth_comm.md`, `chips/ch32h-highperf/recipes/eth_comm.md` |
| **USB device** | `chips/ch57x/recipes/usb_device.md`, `chips/ch32v-general/recipes/usb_device.md`, `chips/ch32f-arm/recipes/usb_device.md`, `chips/ch32x-usbpd/recipes/usb_device.md`, `chips/ch58x-ch59x/recipes/usb_device.md`, `chips/ch5xx-8051/recipes/usb_device.md` |
| **USB host** | `chips/ch57x/recipes/usb_host.md`, `chips/ch58x-ch59x/recipes/usb_host.md` |
| **USB 3.0** | `chips/ch56x-ethernet/recipes/usb3_device.md`, `chips/ch32h-highperf/recipes/usb3_device.md` |
| **USB HS** | `chips/ch32h-highperf/recipes/usbhs_device.md` |
| **USB FS** | `chips/ch32h-highperf/recipes/usbfs_device.md` |
| **USB-PD** | `chips/ch32x-usbpd/recipes/usbpd_config.md`, `chips/ch32f-arm/recipes/usbpd.md`, `chips/ch32h-highperf/recipes/usbpd_config.md`, `chips/ch32v-lowcost/resources/EXAM/CH32L103/USBPD/` (CH32L103, see usbpd_config recipe) |
| **DAC** | `chips/ch32v-general/recipes/dac_output.md`, `chips/ch32f-arm/recipes/dac_output.md`, `chips/ch32h-highperf/recipes/dac_output.md` |
| **FSMC/LCD** | `chips/ch32v-general/recipes/fsmc_lcd.md`, `chips/ch32f-arm/recipes/fsmc_lcd.md`, `chips/ch32h-highperf/recipes/lcd_display.md` |
| **LCD segment** | `chips/ch58x-ch59x/recipes/lcd_display.md` |
| **LCD (CH579)** | `chips/ch57x/resources/EXAM/CH579/LCD/` (see CH579 peripheral_api.md) |
| **Ethernet (CH579 SPI)** | `chips/ch57x/resources/EXAM/CH579/NET/` (SPI-to-Ethernet, see CH579 peripheral_api.md) |
| **Ethernet (CH595)** | `chips/ch58x-ch59x/resources/EXAM/CH595/NET/` (see CH595 examples) |
| **NFCA (CH585)** | `chips/ch58x-ch59x/resources/EXAM/CH585/NFCA/` |
| **ENCODER (CH595)** | `chips/ch58x-ch59x/resources/EXAM/CH595/ENCODER/` |
| **LED (CH585/CH595)** | `chips/ch58x-ch59x/resources/EXAM/CH585/LED/` or `chips/ch58x-ch59x/resources/EXAM/CH595/LED/` |
| **I2S audio** | `chips/ch32v-general/recipes/i2s_audio.md`, `chips/ch32f-arm/recipes/i2s_audio.md`, `chips/ch32h-highperf/recipes/i2s_audio.md` |
| **SAI audio** | `chips/ch32h-highperf/recipes/sai_audio.md` |
| **SDIO/SD** | `chips/ch32v-general/recipes/sdio_sdcard.md`, `chips/ch32f-arm/recipes/sdio_sdcard.md`, `chips/ch32h-highperf/recipes/sdio_sdcard.md`, `chips/ch56x-ethernet/recipes/sd_card.md` |
| **eMMC** | `chips/ch56x-ethernet/recipes/emmc_storage.md`, `chips/ch561-ch563/recipes/emmc_storage.md` |
| **ECDC crypto** | `chips/ch56x-ethernet/recipes/ecdc_crypto.md`, `chips/ch561-ch563/recipes/ecdc_crypto.md`, `chips/ch32h-highperf/recipes/ecdc_crypto.md` |
| **HSPI** | `chips/ch56x-ethernet/recipes/hspi_comm.md`, `chips/ch561-ch563/recipes/hspi_comm.md` |
| **DVP camera** | `chips/ch32f-arm/recipes/dvp_camera.md`, `chips/ch56x-ethernet/recipes/dvp_camera.md`, `chips/ch32h-highperf/recipes/dvp_camera.md` |
| **PIOC** | `chips/ch32x-usbpd/recipes/pio_config.md` |
| **KeyScan** | `chips/ch57x/recipes/keyscan.md` |
| **CRC** | `chips/ch32h-highperf/recipes/crc_calculation.md` |
| **DFSDM** | `chips/ch32h-highperf/recipes/dfsdm_filter.md` |
| **GPHA (2D)** | `chips/ch32h-highperf/recipes/gpha_graphics.md` |
| **I3C** | `chips/ch32h-highperf/recipes/i3c_comm.md` |
| **LPTIM** | `chips/ch32h-highperf/recipes/lptim_timer.md`, `chips/ch32v-lowcost/resources/EXAM/CH32L103/LPTIM/` (CH32L103, see lptim_timer recipe) |
| **QSPI** | `chips/ch32h-highperf/recipes/qspi_flash.md` |
| **SWPMI** | `chips/ch32h-highperf/recipes/swpmi_comm.md` |
| **FMC** | `chips/ch32h-highperf/recipes/fmc_storage.md` |
| **8-bit bus** | `chips/ch56x-ethernet/recipes/bus8_interface.md` |
| **PWMX** | `chips/ch56x-ethernet/recipes/pwmx_output.md` |
| **RNG** | `chips/ch32f-arm/recipes/rng_random.md`, `chips/ch32h-highperf/recipes/rng_random.md` |
| **Low power** | `chips/ch56x-ethernet/recipes/lowpower.md`, `chips/ch561-ch563/recipes/lowpower.md` |
| **ARGB LED** | `chips/ch32v-general/resources/EXAM/CH32V407/ARGB/`, `chips/ch32x-usbpd/resources/EXAM/CH32X315/ARGB/` |
| **PSRAM** | `chips/ch32v-general/resources/EXAM/CH32V407/PSRAM/` |
| **LTDC (CH32V407)** | `chips/ch32v-general/resources/EXAM/CH32V407/LTDC/` (see also ch32h-highperf lcd_display recipe) |
| **8051 compare** | `chips/ch5xx-8051/recipes/compare.md` |
| **8051 Type-C** | `chips/ch5xx-8051/recipes/type_c.md` |

---

## Example Projects (EXAM)

Each chip family has example projects under `chips/<family>/resources/EXAM/`. **Copy the closest example to your project directory, then modify.**

### CH57x (`chips/ch57x/resources/EXAM/`)
| Use Case | Path |
|---|---|
| BLE peripheral | `BLE/Peripheral/` |
| BLE HID consumer | `BLE/HID_Consumer/` |
| BLE OTA | `BLE/BackupUpgrade_OTA/` |
| BLE speed test | `BLE/SpeedTest_Peripheral/` |
| BLE IoCHub | `BLE/IoCHub_NET/` |
| RF basic | `RF/RF_Basic/` |
| RF UART | `RF/RF_Uart/` |
| RF DTM | `RF/Direct_Test_Mode/` |
| UART | `UART/` |
| SPI | `SPI/` |
| I2C | `I2C/` |
| ADC | `ADC/` |
| CMP | `CMP/` |
| KEYSCAN | `KEYSCAN/` |
| Timer | `TMR/` |
| PWM | `PWMX/` |
| Flash | `FLASH/` |
| USB device | `USB/Device/COM/` |
| USB host | `USB/Host/HostEnum/` |
| Power management | `PM/` |
| IAP | `IAP/` |
| **CH579-only peripherals:** | |
| LCD segment | `CH579/LCD/` |
| Ethernet (SPI) | `CH579/NET/` |
| SPI0 | `CH579/SPI0/` |
| SPI1 | `CH579/SPI1/` |

### CH58x/CH59x (`chips/ch58x-ch59x/resources/EXAM/`)
| Use Case | Path |
|---|---|
| BLE peripheral | `CH583/BLE/Peripheral/` |
| BLE central | `CH583/BLE/Central/` |
| BLE HID keyboard | `CH583/BLE/HID_Keyboard/` |
| BLE HID mouse | `CH583/BLE/HID_Mouse/` |
| BLE Mesh | `CH583/BLE/MESH/` |
| BLE OTA | `CH583/BLE/BackupUpgrade_OTA/` |
| BLE speed test | `CH583/BLE/SpeedTest_Peripheral/` |
| UART | `CH583/UART1/` |
| SPI | `CH583/SPI/` |
| I2C | `CH583/I2C/` |
| ADC | `CH583/ADC/` |
| Timer | `CH583/TMR/` |
| PWM | `CH583/PWMX/` |
| Flash | `CH583/FLASH/` |
| USB device | `CH583/USB/Device/` |
| USB host | `CH583/USB/Host/` |
| LCD (CH592/CH585) | `CH592/LCD/` or `CH585/LCD/` |
| NFCA (CH585) | `CH585/NFCA/` |
| ENCODER (CH595) | `CH595/ENCODER/` |
| KEYSCAN (CH595) | `CH595/KEYSCAN/` |
| LED (CH585/CH595) | `CH585/LED/` or `CH595/LED/` |
| Ethernet (CH595) | `CH595/NET/` |
| Power management | `CH583/PM/` |
| IAP | `CH583/IAP/` |

### CH32V General (`chips/ch32v-general/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH32V103/GPIO/` |
| UART | `CH32V103/USART/` |
| SPI | `CH32V103/SPI/` |
| I2C | `CH32V103/I2C/` |
| ADC | `CH32V103/ADC/` |
| Timer | `CH32V103/TIM/` |
| CAN | `CH32V307/CAN/` |
| Ethernet | `CH32V307/ETH/` |
| USB | `CH32V103/USB/` |
| DAC | `CH32V307/DAC/` |
| FSMC LCD | `CH32V307/FSMC/` |
| I2S | `CH32V307/I2S/` |
| SDIO | `CH32V307/SDIO/` |
| Flash | `CH32V103/FLASH/` |
| Power | `CH32V103/PWR/` |
| IAP | `CH32V103/IAP/` |
| TouchKey | `CH32V103/TOUCHKEY/` |
| RTC | `CH32V103/RTC/` |
| IWDG | `CH32V103/IWDG/` |
| WWDG | `CH32V103/WWDG/` |
| **CH32V407-only peripherals:** | |
| LTDC display | `CH32V407/LTDC/` |
| ARGB LED | `CH32V407/ARGB/` |
| PSRAM | `CH32V407/PSRAM/` |
| I3C | `CH32V407/I3C/` |
| USB HS | `CH32V407/USBHS/` |
| CPU | `CH32V407/CPU/` |
| **CH32V20x BLE:** | |
| BLE | `CH32V20x/BLE/` |

### CH32V Low-Cost (`chips/ch32v-lowcost/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH32V003/GPIO/` |
| UART | `CH32V003/USART/` |
| SPI | `CH32V003/SPI/` |
| I2C | `CH32V003/I2C/` |
| ADC | `CH32V003/ADC/` |
| Timer | `CH32V003/TIM/` |
| Flash | `CH32V003/FLASH/` |
| DMA | `CH32V003/DMA/` |
| OPA | `CH32V003/OPA/` |
| EXTI | `CH32V003/EXTI/` |
| IWDG | `CH32V003/IWDG/` |
| WWDG | `CH32V003/WWDG/` |
| Power | `CH32V003/PWR/` |
| IAP | `CH32V003/IAP/` |
| USART IAP | `CH32V003/USART_IAP/` |
| **CH32V006 additions:** | |
| TouchKey | `CH32V006/TOUCHKEY/` |
| **CH32L103 additions:** | |
| CAN | `CH32L103/CAN/` |
| USB-PD | `CH32L103/USBPD/` |
| LPTIM | `CH32L103/LPTIM/` |
| RTC | `CH32L103/RTC/` |
| BKP | `CH32L103/BKP/` |
| CRC | `CH32L103/CRC/` |
| PMP | `CH32L103/PMP/` |
| TouchKey | `CH32L103/TOUCHKEY/` |
| USB | `CH32L103/USB/` |
| FreeRTOS | `CH32L103/FreeRTOS/` |
| RT-Thread | `CH32L103/RT-Thread/` |
| HarmonyOS | `CH32L103/HarmonyOS/` |

### CH32F ARM (`chips/ch32f-arm/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH32F103/GPIO/` |
| UART | `CH32F103/USART/` |
| SPI | `CH32F103/SPI/` |
| I2C | `CH32F103/I2C/` |
| ADC | `CH32F103/ADC/` |
| Timer | `CH32F103/TIM/` |
| CAN | `CH32F103/CAN/Networking/` |
| USB device | `CH32F103/USB/USBD/` |
| DAC | `CH32F103/DAC/DAC_Normal_OUT/` |
| Flash | `CH32F103/FLASH/` |
| Power | `CH32F103/PWR/` |
| IAP | `CH32F103/IAP/` |
| Ethernet | `CH32F20x/ETH/TcpClient/` |
| DVP camera | `CH32F20x/DVP/DVP_TFTLCD/` |
| FSMC LCD | `CH32F20x/FSMC/LCD/` |
| I2S | `CH32F20x/I2S/I2S_DMA/` |
| OPA | `CH32F20x/OPA/` |
| SDIO | `CH32F20x/SDIO/SDIO_SD/` |
| RNG | `CH32F20x/RNG/` |
| USB-PD | `CH32M030/USBPD/` |

### CH32X USB-PD (`chips/ch32x-usbpd/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH32X035/GPIO/` |
| UART | `CH32X035/USART/` |
| SPI | `CH32X035/SPI/` |
| ADC | `CH32X035/ADC/` |
| Timer | `CH32X035/TIM/` |
| Flash | `CH32X035/FLASH/` |
| USB device | `CH32X035/USB/` |
| USB-PD | `CH32X035/USBPD/` |
| PIOC | `CH32X035/PIOC/` |
| IAP | `CH32X035/IAP/` |
| Power | `CH32X035/PWR/` |
| EXTI | `CH32X035/EXTI/` |
| DMA | `CH32X035/DMA/` |
| I2C | `CH32X035/I2C/` |
| **CH641 equivalents:** | |
| GPIO (CH641) | `CH641/GPIO/` |
| UART (CH641) | `CH641/USART/` |
| ADC (CH641) | `CH641/ADC/` |
| Timer (CH641) | `CH641/TIM/` |
| Flash (CH641) | `CH641/FLASH/` |
| USB-PD (CH641) | `CH641/USBPD/` |
| IAP (CH641) | `CH641/IAP/` |
| Power (CH641) | `CH641/PWR/` |
| I2C (CH641) | `CH641/I2C/` |
| DMA (CH641) | `CH641/DMA/` |
| EXTI (CH641) | `CH641/EXTI/` |
| USART IAP (CH641) | `CH641/USART_IAP/` |

> **Note:** CH634EVT contains only hardware reference designs (schematics/PCB), no firmware examples. Use CH32X035 or CH643 examples as reference for CH634 development.

**CH32X315 equivalents:** `CH32X315/` has GPIO, USART, SPI, ADC, TIM, FLASH, I2C, DMA, EXTI, RCC, PWR, SDI_Printf, USBHS, USBSS, ARGB, CPU, CRC, RTC, IAP, IWDG, WWDG

**CH643 equivalents:** `CH643/` has the same peripheral set as CH32X035 (GPIO, USART, SPI, ADC, TIM, FLASH, USB, USBPD, PIOC, IAP, PWR, EXTI, DMA, I2C, OPA, TouchKey, FreeRTOS, RT-Thread, HarmonyOS)

### CH569 Ethernet (`chips/ch56x-ethernet/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH569/GPIO/` |
| UART | `CH569/UART/` |
| SPI | `CH569/SPI/` |
| Timer | `CH569/TMR/` |
| Flash | `CH569/FLASH/` |
| Ethernet | `CH569/ETH/` |
| USB 3.0 | `CH569/USBSS/` |
| eMMC | `CH569/EMMC/` |
| ECDC crypto | `CH569/ECDC/` |
| HSPI | `CH569/HSPI/` |
| DVP camera | `CH569/DVP/` |
| BUS8 | `CH569/BUS8/` |
| PWMX | `CH569/PWMX/` |
| SD card | `CH569/SD/` |
| Low power | `CH569/LOWPOWER/` |
| IAP | `CH569/IAP/` |

### CH561/CH563 ARM (`chips/ch561-ch563/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `CH561/GPIO/GPIOA_IO/` |
| UART | `CH563/UART/UART0/` |
| SPI | `CH561/SPI/` |
| Timer | `CH563/TIM/` |
| Flash | `CH561/FLASH/` |
| Ethernet | `CH563/NET/` |
| Low power | `CH563/POWER/` |
| IAP | `CH561/IAP_DEMO/` |
| ADC | `CH561/ADC/` |
| WDOG | `CH561/WDOG/` or `CH563/WDOG/` |
| EEPROM | `CH563/EEPROM/` |
| Parallel bus | `CH563/PARA/` |
| UCOS RTOS | `CH563/UCOS/` |

### CH32H417 High-Perf (`chips/ch32h-highperf/resources/EXAM/`)
| Use Case | Path |
|---|---|
| GPIO | `GPIO/` |
| UART | `USART/` |
| SPI | `SPI/` |
| I2C | `I2C/` |
| ADC | `ADC/` |
| HSADC | `HSADC/` |
| Timer | `TIM/` |
| Flash | `FLASH/` |
| USB 3.0 | `USBSS/` |
| USB-PD | `USBPD/` |
| USB HS | `USBHS/` |
| USB FS | `USBFS/` |
| Ethernet | `ETH/` |
| LTDC display | `LTDC/` |
| CAN | `CAN/` |
| DAC | `DAC/` |
| DMA | `DMA/` |
| ECDC | `ECDC/` |
| FMC | `FMC/` |
| GPHA | `GPHA/` |
| I2S | `I2S/` |
| I3C | `I3C/` |
| LPTIM | `LPTIM/` |
| QSPI | `QSPI/` |
| RNG | `RNG/` |
| RTC | `RTC/` |
| SAI | `SAI/` |
| SDIO | `SDIO/` |
| SWPMI | `SWPMI/` |
| TKey | `TKey/` |
| DFSDM | `DFSDM/` |
| CRC | `CRC/` |
| OPA | `OPA/` |
| IWDG | `IWDG/` |
| WWDG | `WWDG/` |
| PWR | `PWR/` |
| IAP | `IAP/` |
| DVP | `DVP/` |
| SerDes | `SerDes/` |
| UHSIF | `UHSIF/` |
| SDMMC | `SDMMC/` |
| SWPMI | `SWPMI/` |
| Application | `APPLICATION/` |

### CH5xx 8051 (`chips/ch5xx-8051/resources/EXAM/`)

Common peripherals (available on most 8051 variants):
| Use Case | Path |
|---|---|
| GPIO | `CH554EVT/GPIO/` |
| UART | `CH554EVT/UART1/` |
| SPI | `CH554EVT/SPI/` |
| ADC | `CH554EVT/ADC/` |
| Timer | `CH554EVT/Timer/` |
| PWM | `CH554EVT/PWM/` |
| Flash | `CH554EVT/DataFlash/` |
| USB | `CH554EVT/USB/` |
| TouchKey | `CH554EVT/TouchKey/` |
| IAP | `CH549EVT/IAP/` |
| Type-C | `CH549EVT/Type_C/` or `CH554EVT/Type-C/` |
| Compare | `CH543EVT/Compare/` or `CH554EVT/Compare/` |
| I2C | `CH543EVT/I2C/` or `CH545EVT/I2C/` |
| RGB LED | `CH545EVT/RGB/` or `CH555EVT/RGB/` |

CH559-specific peripherals:
| Use Case | Path |
|---|---|
| SPI0 | `CH559EVT/SPI0/` |
| SPI1 | `CH559EVT/SPI1/` |
| UART0 | `CH559EVT/UART0/` |
| UART1 | `CH559EVT/UART1/` |
| Timer0-3 | `CH559EVT/Timer0/` .. `CH559EVT/Timer3/` |
| LED control | `CH559EVT/LED_CTL/` |
| Encryption | `CH559EVT/Encryption/` |
| XBUS | `CH559EVT/XBUS/` |
| Chip ID | `CH559EVT/CHIP_ID/` |
| USB lib | `CH559EVT/USB_LIB/` |
| WDOG | `CH559EVT/WDOG/` |

---

## Critical Pitfalls by Architecture

### RISC-V Chips (CH57x/CH58x/CH59x/CH32V/CH32X/CH569/CH32H)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | Peripheral clock not enabled | Always call `RCC_*PeriphClockCmd()` or `PWR_PeriphClkCfg()` before peripheral use |
| 2 | GPIO mode not set for alternate function | Use `GPIO_Mode_AF_PP` (StdPeriphDriver) or `GPIOA_ModeCfg()` (CH57x/CH58x) before peripheral init |
| 3 | Flash write without erase | Always erase page/sector first; page size varies by chip |
| 4 | Interrupt handler missing `.highcode` (BLE chips) | Use `__attribute__((interrupt("WCH-Interrupt-fast")))` + `__attribute__((section(".highcode")))` |
| 5 | IAP app at wrong flash offset | Bootloader at 0x0000, app at 0x1000 (BLE) or 0x4000/0x8000 (CH32V) |
| 6 | BLE init order wrong (BLE chips) | `SetSysClock` -> `BLEInit` -> `HAL_Init` -> `GAPRole_Init` -> `App_Init` -> `Main_Circulation` |
| 7 | `Main_Circulation()` returns (BLE chips) | Never return; it runs the TMOS event loop |
| 8 | `BLE_MEMHEAP_SIZE` too small | Minimum 6KB for most BLE applications |
| 9 | GATT service registration order wrong | GAP -> GATT -> DevInfo -> Custom services |
| 10 | ADC calibration skipped | Always `ADC_ResetCalibration()` -> `ADC_StartCalibration()` -> wait -> then convert |

### ARM Cortex-M3 Chips (CH32F103/CH32F20x/CH32M030)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | Wrong header include | CH32F10x: `ch32f10x.h`; CH32F20x: `ch32f20x.h` -- do not mix |
| 2 | NVIC priority group not set | Call `NVIC_PriorityGroupConfig()` before configuring any interrupts |
| 3 | Flash erase granularity wrong | CH32F10x: 1KB pages; CH32F20x: 256-byte sectors |
| 4 | USB register access | Uses custom register-based API, not STM32 USB OTG library |

### ARM7TDMI Chips (CH561/CH563)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | No StdPeriphDriver | All code is register-level with R8_/R16_/R32_ macros |
| 2 | Clock config is compile-time | Define `FREQ_SYS` before including SYSFREQ.H |
| 3 | Flash erase unit is 4KB | Not 256 bytes like CH569 |
| 4 | Interrupt handlers use `__irq` | Not GCC attributes |
| 5 | No ECDC/HSPI/eMMC | These are CH569 (RISC-V) only |

### 8051 Chips (CH543-CH559)

| # | Pitfall | Fix |
|---|---------|-----|
| 1 | Safe mode sequence for protected registers | Always: `SAFE_MOD=0x55; SAFE_MOD=0xAA;` then write, then `SAFE_MOD=0x00;` |
| 2 | Not disabling interrupts during Flash write | Set `EA=0` before Flash operations, restore after |
| 3 | Wrong memory qualifier | Use `data` for small vars, `xdata` for large buffers |
| 4 | ADC channel pin not float input | Clear the pin's output mode bit before ADC sampling |
| 5 | USB buffer address misalignment | Buffer addresses must be even; check UEPn_DMA values |

---

## API References and Guides

Each chip family has documentation under `chips/<family>/resources/`:

| Document | Description | Available For |
|---|---|---|
| `peripheral_api.md` | Complete peripheral API function signatures | All families |
| `pitfalls.md` | Detailed error scenarios and fixes | All families |
| `memory_layout.md` | Flash/RAM layout, linker script reference | All families |
| `example_list.md` | Index of all example projects | All families |
| `ble_api.md` | BLE stack API reference | CH57x |
| `config_reference.md` | BLE stack config.h options | CH57x, CH58x/CH59x |
| `hal_reference.md` | HAL layer reference | CH57x |

---

## Execution Workflow

| Step | Name | Description |
|------|------|-------------|
| 1 | **Identify chip** | Determine the exact chip variant. Navigate to `chips/<family>/`. |
| 2 | **Read recipe** | Find the matching scenario in `chips/<family>/recipes/`. Read it first. |
| 3 | **Check API** | For APIs not in recipes, check `chips/<family>/resources/peripheral_api.md`. |
| 4 | **Check pitfalls** | Review `chips/<family>/resources/pitfalls.md` before coding. |
| 5 | **Validate** | Verify all API signatures, header includes, pin assignments, clock configs. |
| 6 | **Confirm** | Present plan: includes, pin config, init sequence, main loop. |
| 7 | **Execute** | **New project:** copy closest example from `chips/<family>/resources/EXAM/`, then modify. **Existing:** edit in place. |
| 8 | **Build** | RISC-V/ARM: MounRiver Studio (.wvproj). ARM7: Keil MDK (.uvproj). 8051: Keil C51 (.uvproj). |
| 9 | **Debug** | UART debug output (typically 115200 baud). BLE: nRF Connect. USB-PD: USB-C analyzer. |

---

## Failure Strategies

| Situation | Action |
|---|---|
| API does not exist in resources | Stop immediately, inform user |
| Chip family unclear | Ask user for exact chip part number |
| Pin conflict detected | Check datasheet for alternate functions, suggest reassignment or remap |
| BLE stack config unsure | Default: `BLE_MEMHEAP_SIZE=(1024*6)`, `BLE_BUFF_MAX_LEN=27` |
| Flash write fails | Verify page size, check write protection, ensure unlock before write |
| Peripheral not responding | Verify clock enable for that peripheral |
| Ethernet link down | Check PHY init, MII/RMII mode, GPIO remap for ETH pins |
| USB-PD not negotiating | Check CC pin config, comparator thresholds, BMC timer values |
| IAP memory overlap | Verify bootloader at 0x0000, app at correct offset, no overlap |
