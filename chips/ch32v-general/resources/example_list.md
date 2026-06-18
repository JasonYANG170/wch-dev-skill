# Example Project Index

Complete listing of all example projects available in each chip family's EVT package. Each example is a self-contained MounRiver Studio project under `EVT/EXAM/<Category>/`.

---

## CH32V103 Examples

| Category | Description |
|----------|-------------|
| ADC | Analog-to-digital converter |
| APPLICATION | Application-level examples |
| BKP | Backup register access |
| CAN | CAN bus communication |
| CRC | CRC calculation |
| DAC | Digital-to-analog converter |
| DMA | DMA transfer |
| DVP | Digital video port |
| ETH | Ethernet (external PHY required) |
| EXTI | External interrupts |
| FLASH | Flash read/write/erase |
| FPU | Floating-point unit |
| FSMC | Flexible static memory controller |
| FreeRTOS | FreeRTOS RTOS integration |
| GPIO | GPIO input/output |
| HarmonyOS | HarmonyOS Lite integration |
| I2C | I2C master/slave |
| I2S | I2S audio interface |
| IAP | In-application programming bootloader |
| INT | Interrupt examples |
| IWDG | Independent watchdog |
| OPA | Operational amplifier |
| PMP | Physical memory protection |
| PWR | Power management |
| RCC | Clock configuration |
| RNG | Random number generator |
| RT-Thread | RT-Thread RTOS integration |
| RTC | Real-time clock |
| SDIO | SD card interface |
| SDI_Printf | Serial debug interface printf |
| SPI | SPI master/slave |
| SRC | Shared peripheral driver source |
| SYSTICK | System tick timer |
| TIM | Timer/PWM |
| TOUCHKEY | Capacitive touch sensing (TKey) |
| TencentOS | TencentOS Tiny integration |
| USART | UART communication |
| USB | USB device/host |
| VoiceRcgExam | Voice recognition |
| WWDG | Window watchdog |

---

## CH32V20x Examples

| Category | Description |
|----------|-------------|
| ADC | Analog-to-digital converter |
| APPLICATION | Application-level examples |
| ARGB | Addressable RGB LED (WS2812) |
| BKP | Backup register access |
| CAN | CAN bus communication |
| CPU | CPU-level examples |
| CRC | CRC calculation |
| DAC | Digital-to-analog converter |
| DMA | DMA transfer |
| DVP | Digital video port |
| ETH | Ethernet (V208 variants) |
| EXTI | External interrupts |
| FLASH | Flash read/write/erase |
| FSMC | Flexible static memory controller |
| GPIO | GPIO input/output |
| I2C | I2C master/slave |
| I2S | I2S audio interface |
| I3C | I3C interface |
| IAP | In-application programming bootloader |
| INT | Interrupt examples |
| IWDG | Independent watchdog |
| LTDC | LCD-TFT display controller |
| OPA | Operational amplifier |
| PSRAM | Pseudo-SRAM interface |
| PWR | Power management |
| RCC | Clock configuration |
| RNG | Random number generator |
| RTC | Real-time clock |
| SDIO | SD card interface |
| SDI_Printf | Serial debug interface printf |
| SPI | SPI master/slave |
| SRC | Shared peripheral driver source |
| TIM | Timer/PWM |
| USART | UART communication |
| USBHS | USB high-speed device/host |
| WWDG | Window watchdog |

---

## CH32V307 Examples

| Category | Description |
|----------|-------------|
| ADC | Analog-to-digital converter |
| APPLICATION | Application-level examples |
| BKP | Backup register access |
| BLE | Bluetooth Low Energy |
| CAN | CAN bus communication |
| CRC | CRC calculation |
| DMA | DMA transfer |
| ETH | Ethernet (internal 10BASE-T PHY) |
| EXTI | External interrupts |
| FLASH | Flash read/write/erase |
| FreeRTOS | FreeRTOS RTOS integration |
| GPIO | GPIO input/output |
| HarmonyOS | HarmonyOS Lite integration |
| I2C | I2C master/slave |
| IAP | In-application programming bootloader |
| INT | Interrupt examples |
| IWDG | Independent watchdog |
| OPA | Operational amplifier |
| PMP | Physical memory protection |
| PWR | Power management |
| RCC | Clock configuration |
| RT-Thread | RT-Thread RTOS integration |
| RTC | Real-time clock |
| SDI_Printf | Serial debug interface printf |
| SPI | SPI master/slave |
| SRC | Shared peripheral driver source |
| SYSTICK | System tick timer |
| TIM | Timer/PWM |
| TOUCHKEY | Capacitive touch sensing (TKey) |
| TencentOS | TencentOS Tiny integration |
| USART | UART communication |
| USB | USB device/host |
| WWDG | Window watchdog |

---

## CH32V407 Examples

| Category | Description |
|----------|-------------|
| ADC | Analog-to-digital converter |
| APPLICATION | Application-level examples |
| BKP | Backup register access |
| CAN | CAN bus communication |
| CRC | CRC calculation |
| DAC | Digital-to-analog converter |
| DMA | DMA transfer |
| DVP | Digital video port |
| ETH | Ethernet (internal 10BASE-T PHY) |
| EXTI | External interrupts |
| FLASH | Flash read/write/erase |
| FPU | Floating-point unit |
| FSMC | Flexible static memory controller |
| FreeRTOS | FreeRTOS RTOS integration |
| GPIO | GPIO input/output |
| HarmonyOS | HarmonyOS Lite integration |
| I2C | I2C master/slave |
| I2S | I2S audio interface |
| IAP | In-application programming bootloader |
| INT | Interrupt examples |
| IWDG | Independent watchdog |
| OPA | Operational amplifier |
| PMP | Physical memory protection |
| PWR | Power management |
| RCC | Clock configuration |
| RNG | Random number generator |
| RT-Thread | RT-Thread RTOS integration |
| RTC | Real-time clock |
| SDIO | SD card interface |
| SDI_Printf | Serial debug interface printf |
| SPI | SPI master/slave |
| SRC | Shared peripheral driver source |
| SYSTICK | System tick timer |
| TIM | Timer/PWM |
| TOUCHKEY | Capacitive touch sensing (TKey) |
| TencentOS | TencentOS Tiny integration |
| USART | UART communication |
| USB | USB device/host |
| VoiceRcgExam | Voice recognition |
| WWDG | Window watchdog |

---

## Cross-Family Compatibility Matrix

| Peripheral | CH32V103 | CH32V20x | CH32V307 | CH32V407 |
|------------|----------|----------|----------|----------|
| GPIO | Yes | Yes | Yes | Yes |
| USART | 1/2/3 | 1/2/3/4/5 | 1/2/3/4/5 | 1/2/3/4/5/6/7/8 |
| SPI | 1/2 | 1/2 | 1/2/3 | 1/2/3 |
| I2C | 1/2 | 1/2 | 1/2 | 1/2 |
| ADC | 1 | 1 | 1/2 | 1/2 |
| TIM | 1-4 | 1-5 | 1-8 | 1-8 |
| CAN | 1 | 1 | 1/2 | 1/2 |
| USB | FS | FS | FS | FS + HS (USBHS) |
| ETH | No | V208 only | Yes (internal PHY) | Yes (internal PHY) |
| DAC | Yes | Yes | No (example only) | Yes |
| FSMC | Yes | Yes | No (example only) | Yes |
| SDIO | Yes | Yes | No (example only) | Yes |
| RNG | Yes | Yes | No | Yes |
| DVP | Yes | Yes | No | Yes |
| I2S | Yes | Yes | No | Yes |
| LTDC | No | Yes | No | No |
| ARGB | No | Yes | No | No |
| I3C | No | Yes | No | No |
| PSRAM | No | Yes | No | No |
| FPU | Yes | No | No | Yes |
| BLE | No | V208/V203 D8 | Yes | No |
| TOUCHKEY | Yes | No | Yes | Yes |
| OPA | Yes | Yes | Yes | Yes |
| PMP | Yes | No | Yes | Yes |
| RTOS | FreeRTOS, RT-Thread, HarmonyOS, TencentOS | None | FreeRTOS, RT-Thread, HarmonyOS, TencentOS | FreeRTOS, RT-Thread, HarmonyOS, TencentOS |

---

## RTOS Integration

| RTOS | CH32V103 | CH32V20x | CH32V307 | CH32V407 |
|------|----------|----------|----------|----------|
| FreeRTOS | Yes | No | Yes | Yes |
| RT-Thread | Yes | No | Yes | Yes |
| HarmonyOS Lite | Yes | No | Yes | Yes |
| TencentOS Tiny | Yes | No | Yes | Yes |

RTOS examples are under `EVT/EXAM/FreeRTOS/`, `EVT/EXAM/RT-Thread/`, `EVT/EXAM/HarmonyOS/`, and `EVT/EXAM/TencentOS/` respectively.

---

## How to Use Examples

1. Navigate to `CH*EVT/EVT/EXAM/<Category>/<SpecificExample>/`
2. Open the `.wvproj` file in MounRiver Studio
3. Build with Project -> Build Project (Ctrl+B)
4. Flash with Run -> Debug (requires WCH-LinkE)
5. Check UART output (typically USART1 @ 115200 baud)
