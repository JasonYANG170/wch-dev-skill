# Example Project Index

All examples are located in the respective chip EVT packages under `EVT/EXAM/`.

## CH32V003 Examples

Base path: `CH32V003EVT/EVT/EXAM/`

| Category | Example | Description |
|----------|---------|-------------|
| **ADC** | `ADC/ADC_DMA` | Multi-channel ADC with DMA continuous sampling |
| | `ADC/AnalogWatchdog` | ADC analog watchdog threshold detection |
| | `ADC/Auto_Injection` | ADC injected channel auto-conversion |
| | `ADC/Discontinuous_mode` | ADC discontinuous conversion mode |
| | `ADC/ExtLines_Trigger` | ADC triggered by external pin (EXTI) |
| | `ADC/TIM_Trigger` | ADC triggered by timer event |
| **APPLICATION** | `APPLICATION/SoftUART` | Software (bit-bang) UART implementation |
| **DMA** | `DMA/DMA_MEM2MEM` | Memory-to-memory DMA transfer |
| | `DMA/DMA_MEM2PERIP` | DMA memory-to-peripheral (see peripheral examples) |
| **EXTI** | `EXTI/EXTI0` | External interrupt on PD0, falling edge |
| **FLASH** | `FLASH/FLASH_Program` | Standard and fast Flash erase/program |
| | `FLASH/BootAsUser` | Boot from user area configuration |
| **GPIO** | `GPIO/GPIO_Toggle` | PD0 push-pull output, LED toggle |
| **I2C** | `I2C/I2C_7bit_Mode` | I2C master/slave 7-bit polling transceiver |
| | `I2C/I2C_7bit_Interrupt_Mode` | I2C master/slave 7-bit interrupt-driven |
| | `I2C/I2C_10bit_Mode` | I2C 10-bit addressing mode |
| | `I2C/I2C_DMA` | I2C master TX / slave RX via DMA |
| | `I2C/I2C_EEPROM` | AT24Cxx EEPROM read/write via I2C |
| | `I2C/I2C_PEC` | I2C with packet error checking (PEC) |
| **IAP** | `IAP/V00x_APP` | IAP application (runs at 0x1000) |
| **INT** | `INT/Interrupt_VTF` | Vector table free interrupt routing |
| | `INT/VectorInRAM` | Interrupt vector in RAM |
| **IWDG** | `IWDG/IWDG` | Independent watchdog, button feed |
| **OPA** | `OPA/OPA` | OPA1 voltage follower, ADC reads output |
| **PWR** | `PWR/Sleep_Mode` | WFI sleep, EXTI wakeup |
| | `PWR/Standby_Mode` | Standby mode, auto-wakeup |
| | `PWR/PVD_VoltageJudger` | PVD voltage monitoring |
| | `PWR/PVD_Wakeup` | PVD interrupt wakeup |
| **RCC** | `RCC/Get_CLK` | Read system and peripheral clock frequencies |
| | `RCC/MCO` | Microcontroller clock output on pin |
| **SDI_Printf** | `SDI_Printf/SDI_Printf` | Single-wire debug print (PD7) |
| **SPI** | `SPI/2Lines_FullDuplex` | SPI master/slave full-duplex |
| | `SPI/1Lines_half-duplex` | SPI single-wire half-duplex |
| | `SPI/FullDuplex_HardNSS` | SPI with hardware NSS |
| | `SPI/SPI_CRC` | SPI with CRC error checking |
| | `SPI/SPI_DMA` | SPI master/slave with DMA |
| **SYSTICK** | `SYSTICK/SYSTICK_Interrupt` | SysTick timer periodic interrupt |
| **TIM** | `TIM/PWM_Output` | PWM generation |
| | `TIM/Input_Capture` | Timer input capture |
| | `TIM/Encoder` | Encoder interface |
| | `TIM/Clock_Select` | Timer clock source selection |
| | `TIM/ComplementaryOutput_DeadTime` | TIM1 complementary PWM with dead-time |
| | `TIM/ExtTrigger_Start_Two_Timer` | Timer synchronization |
| | `TIM/One_Pulse` | One-pulse mode |
| | `TIM/Output_Compare_Mode` | Output compare toggle |
| | `TIM/PWM_6_Step` | 6-step BLDC motor PWM |
| | `TIM/PWM_Phase-shifted` | Phase-shifted PWM |
| | `TIM/Synchro_ExtTrigger` | Synchronized timer with external trigger |
| | `TIM/Synchro_Timer` | Timer synchronization |
| | `TIM/TIM_Continuous` | Continuous counting mode |
| | `TIM/TIM_DMA` | Timer DMA burst transfer |
| | `TIM/TIM_INT` | Timer update interrupt |
| **USART** | `USART/USART_Printf` | UART debug output (printf) |
| | `USART/USART_Polling` | UART polling TX/RX |
| | `USART/USART_Interrupt` | UART interrupt-driven TX/RX |
| | `USART/USART_DMA` | UART DMA transmit/receive |
| | `USART/USART_HalfDuplex` | UART half-duplex mode |
| | `USART/USART_HardwareFlowControl` | UART RTS/CTS flow control |
| | `USART/USART_MultiProcessorCommunication` | UART multi-processor addressing |
| | `USART/USART_SynchronousMode` | UART synchronous clock mode |
| **USART_IAP** | `USART_IAP/CH32V003_IAP` | UART-based IAP bootloader |
| | `USART_IAP/CH32V003_APP` | IAP application for USART bootloader |
| **WWDG** | `WWDG/WWDG` | Window watchdog with window feed |

---

## CH32V006 Examples

Base path: `CH32V006EVT/EVT/EXAM/`

Same as CH32V003 except:

- Uses `ch32v00X.h` header (capital X)
- Additional OPA sub-examples: `CMP`, `CMP_BKIN`, `CMP_Poll`, `OPA_PGA`, `OPA_Poll`
- Additional ADC example: `ADC/SLTIM_Trigger` (SLTIM-triggered ADC)
- Additional TIM examples: `TIM/SLTIM_DMA` (SLTIM DMA)
- Additional APPLICATION: `APPLICATION/Motor` (motor control)
- Additional TOUCHKEY: `TOUCHKEY/TKEY`, `TOUCHKEY/TKYLIB`, `TOUCHKEY/Touch_EX001`, `TOUCHKEY/Touch_EX002`
- Additional USART: `USART/USART_Idle_Recv` (idle line detection)
- Missing: `TIM/PWM_6_Step`, `TIM/PWM_Phase-shifted`, `TIM/TIM_Continuous`

---

## CH32L103 Examples

Base path: `CH32L103EVT/EVT/EXAM/`

Includes all CH32V003 examples (with `ch32l103.h` header), plus:

| Category | Example | Description |
|----------|---------|-------------|
| **ADC** | `ADC/ADC_FastConvent` | ADC fast conversion mode |
| | `ADC/ADC_PGA` | ADC programmable gain amplifier |
| | `ADC/Internal_Temperature` | Internal temperature sensor reading |
| **APPLICATION** | `APPLICATION/PD_Electric_Fan` | USB-PD electric fan control |
| | `APPLICATION/WS2812_LED` | WS2812 addressable LED driver |
| **BKP** | `BKP/` | Backup register access |
| **CAN** | `CAN/CANFD` | CAN FD (Flexible Data-rate) |
| | `CAN/CAN_Wakeup` | CAN bus wakeup from sleep |
| | `CAN/Networking` | CAN network communication |
| | `CAN/TestMode` | CAN loopback/silent test modes |
| | `CAN/Time-triggered` | CAN time-triggered communication |
| **CRC** | `CRC/CRC_Calculation` | Hardware CRC calculation |
| **FreeRTOS** | `FreeRTOS/FreeRTOS` | FreeRTOS RTOS integration |
| **HarmonyOS** | `HarmonyOS/LiteOS_m` | HarmonyOS LiteOS_m integration |
| **IAP** | `IAP/USB_UART` | IAP via USB or UART |
| **LPTIM** | `LPTIM/LPTIM_LP_WakeUp` | Low-power timer wakeup from stop |
| | `LPTIM/PWM_OnePulse_SingleMode` | LPTIM PWM and one-pulse mode |
| **OPA** | `OPA/CMP` | Comparator mode |
| | `OPA/CMP_TIM2` | Comparator output to TIM2 |
| | `OPA/CMP_WakeUp` | Comparator wakeup from sleep |
| | `OPA/OPA_BKIN` | OPA as TIM break input |
| | `OPA/OPA_IRQ` | OPA interrupt on output change |
| | `OPA/OPA_PGA` | OPA programmable gain amplifier |
| | `OPA/OPA_Poll` | OPA polling read |
| | `OPA/OPA_Reset` | OPA-triggered system reset |
| **PMP** | `PMP/PMP` | Physical memory protection |
| **PWR** | `PWR/Stop_Mode` | Stop mode (deeper than sleep) |
| | `PWR/Standby_RAM_Application` | Standby with RAM retention |
| | `PWR/Standby_RAM_LV_Mode` | Standby RAM low-voltage mode |
| | `PWR/Standby_RAM_Mode` | Standby RAM mode |
| **RCC** | `RCC/HSI_Calibration` | HSI oscillator calibration |
| | `RCC/LSI_Calibration` | LSI oscillator calibration |
| **RT-Thread** | `RT-Thread/rt-thread` | RT-Thread RTOS integration |
| **RTC** | `RTC/` | Real-time clock with alarm |
| **RunInRam_LP** | `RunInRam_LP/RunInRAM` | Execute code from RAM |
| | `RunInRam_LP/RunInRAM_HSI_LP` | RAM execution with HSI low-power |
| | `RunInRam_LP/RunInRAM_Select` | Selective RAM execution |
| | `RunInRam_LP/RunInRAM_Select_Sleep` | RAM execution with sleep |
| **SPI** | `SPI/SPI_FLASH` | SPI Flash (W25Q) read/write |
| **TencentOS** | `TencentOS/TencentOS` | TencentOS Tiny RTOS integration |
| **TIM** | (same as CH32V003) | |
| **TOUCHKEY** | `TOUCHKEY/TKey` | Capacitive touch key detection |
| | `TOUCHKEY/TKey_MulShield` | Multi-shield touch key |
| **USART** | `USART/USART_SmartCard` | Smart card interface |
| | `USART/USART_Idle_Recv` | Idle line detection for variable-length RX |
| **USB** | `USB/USBFS` | Full-speed USB device/host |
| **USBPD** | `USBPD/USBPD_SNK` | USB-PD sink (consumer) |
| | `USBPD/USBPD_SRC` | USB-PD source (provider) |

---

## Example Directory Structure

Each example follows this structure:

```
ExampleName/
  User/
    main.c                  # Main application code
    ch32v00x_it.c           # Interrupt handlers (ch32v00X_it.c for CH32V006)
    system_ch32v00x.c       # System clock configuration
  SRC/                      # Linked from shared SRC/ (via .project)
    Core/
    Debug/
    Ld/
    Peripheral/
    Startup/
  .project                  # MounRiver Studio Eclipse project
  *.wvproj                  # MounRiver Studio workspace project
```

---

## Recommended Starting Points

| Use Case | Recommended Example |
|----------|-------------------|
| First project / LED blink | `GPIO/GPIO_Toggle` |
| Serial debug output | `USART/USART_Printf` or `SDI_Printf/` |
| Read analog sensor | `ADC/ADC_DMA` |
| Drive servo/LED strip | `TIM/PWM_Output` |
| Communicate with I2C sensor | `I2C/I2C_EEPROM` |
| Communicate with SPI device | `SPI/2Lines_FullDuplex` |
| Store configuration | `FLASH/FLASH_Program` |
| Low-power application | `PWR/Sleep_Mode` |
| Firmware update | `IAP/V00x_APP` + `USART_IAP/` |
| External button interrupt | `EXTI/EXTI0` |
| System reset protection | `IWDG/IWDG` or `WWDG/WWDG` |
| Analog amplification | `OPA/OPA` |
| Memory copy | `DMA/DMA_MEM2MEM` |
| Touch interface | `TOUCHKEY/TKEY` (CH32V006/CH32L103) |
| USB device | `USB/USBFS` (CH32L103 only) |
| CAN bus | `CAN/Networking` (CH32L103 only) |
| USB-PD | `USBPD/USBPD_SNK` or `USBPD_SRC` (CH32L103 only) |
| RTOS integration | `FreeRTOS/FreeRTOS` or `RT-Thread/rt-thread` (CH32L103 only) |
