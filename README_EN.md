**[中文](README.md)** | English

# wch-dev-skill

Unified AI Skill for WCH (Nanjing Qinheng Microelectronics) full-series microcontroller firmware development. Enables AI Agents to automatically generate embedded code conforming to WCH SDK conventions, covering 35+ chip families across 10 architecture groups.

Supported chips: CH572/CH579 (BLE), CH583/CH585/CH592/CH595 (BLE+USB), CH32V003/CH32V006/CH32L103 (low-cost), CH32V103/CH32V20x/CH32V307/CH32V407 (general-purpose), CH32F103/CH32F20x/CH32M030 (ARM), CH32X035/CH32X315/CH641/CH643 (USB-PD), CH32H417 (high-performance), CH569 (Ethernet+USB3.0), CH561/CH563 (ARM7TDMI), CH543-CH559 (8051).

## Features

- Unified routing: automatically identifies chip model and navigates to the correct `chips/<family>/` directory
- 10 chip families, each with complete recipes (scenario guides), API references, pitfalls docs, and EXAM examples
- Covers all BLE roles, Ethernet, USB 3.0, USB-PD, LCD, NFC-A, and more
- Each scenario provides complete call chains, init sequences, and copy-ready code templates
- Documents common errors and pitfalls for each architecture (RISC-V/ARM/ARM7TDMI/8051)

## Chip Family Coverage

| Family | Directory | Chips | Architecture | Key Features |
|--------|-----------|-------|--------------|--------------|
| CH57x | `chips/ch57x/` | CH572, CH579 | RISC-V | BLE 5.0, LCD (CH579), NET (CH579) |
| CH58x/CH59x | `chips/ch58x-ch59x/` | CH583, CH585, CH592, CH595 | RISC-V | BLE+USB, LCD, NFCA, ENCODER, KEYSCAN |
| CH32V General | `chips/ch32v-general/` | CH32V103, CH32V20x, CH32V307, CH32V407 | RISC-V | CAN, ETH, USB, BLE, LTDC, ARGB |
| CH32V Low-Cost | `chips/ch32v-lowcost/` | CH32V003, CH32V006, CH32L103 | RISC-V | OPA, USB-PD (CH32L103), LPTIM |
| CH32F ARM | `chips/ch32f-arm/` | CH32F103, CH32F20x, CH32M030 | ARM Cortex-M3 | STM32-compatible, BLE, USB-PD |
| CH32X USB-PD | `chips/ch32x-usbpd/` | CH32X035, CH32X315, CH641, CH643 | RISC-V | USB-PD, PIOC |
| CH32H417 | `chips/ch32h-highperf/` | CH32H417 | RISC-V | USB 3.0, LTDC, SerDes, dual-core |
| CH569 | `chips/ch56x-ethernet/` | CH569 | RISC-V | ETH, USB 3.0, eMMC, ECDC |
| CH561/CH563 | `chips/ch561-ch563/` | CH561, CH563 | ARM7TDMI | ETH, USB 2.0 HS |
| CH5xx 8051 | `chips/ch5xx-8051/` | CH543-CH559 | 8051 | USB, TouchKey |

## Installation

### 1. Clone to your skill directory

Find or create the skill directory based on your AI Agent documentation:

```bash
git clone <repo-url> wch-dev-skill
```

Examples:

> **Claude Code**
> **Project scope**: `.claude/skills` in project root
> **User scope**: `~/.claude/skills` (applies to all projects)
> Navigate to the skills folder and run `git clone <repo-url> wch-dev-skill`

> **QwenCode**
> **Project scope**: `.qwen/skills` in project root
> **User scope**: `~/.qwen/skills` (applies to all projects)

> **OpenCode**
> **Project scope**: `.opencode/skills` in project root
> **User scope**: `~/.config/opencode/skills` (applies to all projects)

### 2. Use the skill

Confirm the skill is loaded in your AI Agent, then invoke it via command.

> **Claude Code**
> Run `claude` in terminal, then type `/wch-dev-skill` and describe your needs

> **QwenCode**
> Run `qwen` in terminal, type `/skills`, select wch-dev-skill

> **OpenCode**
> Run `opencode` in terminal, type `/skills`, select wch-dev-skill

## How It Works

The skill defines a workflow that the AI Agent follows when generating firmware code:

| Step | Name | Description |
|------|------|-------------|
| 1 | Identify chip | Determine exact chip variant, navigate to `chips/<family>/` |
| 2 | Read recipe | Find matching scenario in `recipes/`, get complete call chain |
| 3 | Check API | Look up `resources/peripheral_api.md` for function signatures |
| 4 | Check pitfalls | Review `resources/pitfalls.md` before coding |
| 5 | Validate | Verify API signatures, header includes, pin assignments, clock configs |
| 6 | Confirm | Present plan: includes, pin config, init sequence, main loop |
| 7 | Execute | Copy closest example from `resources/EXAM/`, then modify |
| 8 | Build | RISC-V/ARM: MounRiver Studio (.wvproj). ARM7: Keil MDK. 8051: Keil C51 |
| 9 | Debug | UART debug output (115200 baud). BLE: nRF Connect. USB-PD: USB-C analyzer |

## Covered Scenarios

### BLE Applications (CH57x / CH58x/CH59x / CH32V20x / CH32F20x)
- BLE Peripheral — advertising, GATT services, notifications
- BLE Central — scanning, connecting, service discovery, read/write
- BLE HID — keyboard, mouse, consumer control
- BLE Mesh — provisioning, Alibaba light model, vendor model
- BLE OTA — dual-image backup upgrade, IAP bootloader
- BLE Speed Test — throughput optimization, LE 2M PHY
- BLE IoCHub — custom data point protocol, multi-device networking
- BLE ANCS / HeartRate / UART Bridge / USB Combo

### Wired Communication
- Ethernet — TCP/UDP Socket (CH32V307/CH32F20x/CH569/CH561/CH563/CH32H417/CH579/CH595)
- USB Device — CDC, HID, vendor-defined (all families)
- USB Host — device enumeration, AOA, U-disk
- USB 3.0 — SuperSpeed device (CH569/CH32H417/CH32X315)
- USB-PD — PD negotiation, voltage/current config (CH32X035/CH641/CH643/CH32L103/CH32M030/CH32H417)
- CAN — CAN 2.0B send/receive (CH32V307/CH32F103/CH32L103/CH32H417)

### Display & HMI
- Segment LCD — CH579/CH585/CH592/CH595
- FSMC/TFT LCD — CH32V307/CH32F20x
- LTDC Display — CH32V407/CH32H417
- ARGB LED — CH32V407/CH32X315
- TouchKey — all families
- KEYSCAN Matrix — CH572/CH595
- ENCODER — CH595
- NFCA NFC-A — CH585

### Peripheral Drivers
- GPIO / UART / SPI / I2C / ADC / Timer / PWM — all families
- DAC / OPA / CMP — most families
- Flash read/write/erase, DMA transfer, EXTI interrupt
- RTC / IWDG / WWDG / Low-power modes

### RTOS Support
- FreeRTOS / RT-Thread / HarmonyOS / TencentOS — CH32V/CH32F/CH32X/CH58x/CH59x
- UCOS — CH563

## Directory Structure

```
wch-dev-skill/
  SKILL.md                        # Unified routing and guidance
  README.md                       # Chinese documentation
  README_EN.md                    # This file (English)
  CHANGELOG.md                    # Version history
  chips/
    ch57x/                        # CH572/CH579 BLE 5.0
      recipes/                    # 20 scenario guides
      resources/                  # API ref, pitfalls, config, EXAM examples
    ch58x-ch59x/                  # CH583/CH585/CH592/CH595 BLE+USB
      recipes/                    # 30 scenario guides
      resources/                  # API ref, pitfalls, config, EXAM examples
    ch32v-general/                # CH32V103/V20x/V307/V407
      recipes/                    # 21 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch32v-lowcost/                # CH32V003/V006/L103
      recipes/                    # 15 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch32f-arm/                    # CH32F103/F20x/M030 ARM Cortex-M3
      recipes/                    # 22 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch32x-usbpd/                  # CH32X035/X315/CH641/CH643 USB-PD
      recipes/                    # 14 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch32h-highperf/               # CH32H417 dual-core USB3+LTDC
      recipes/                    # 38 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch56x-ethernet/               # CH569 ETH+USB3.0
      recipes/                    # 18 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch561-ch563/                  # CH561/CH563 ARM7TDMI ETH
      recipes/                    # 12 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
    ch5xx-8051/                   # CH543-CH559 8051
      recipes/                    # 12 scenario guides
      resources/                  # API ref, pitfalls, EXAM examples
```

## References

- WCH Official Website → http://wch.cn
- MounRiver Studio → http://www.mounriver.com
- EVT SDK Source → `EVT/EXAM/` under each chip directory
- BLE Reference Manual → `chips/ch57x/resources/EXAM/BLE/`
- Per-family API Reference → `chips/<family>/resources/peripheral_api.md`
- Common Pitfalls → `chips/<family>/resources/pitfalls.md`
