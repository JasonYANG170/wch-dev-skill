# CH32X/CH6xx Example Project Index

Example projects are located in each chip family's `EVT/EXAM/` directory.

## CH32X035 Examples (CH32X035EVT/EVT/EXAM/)

| Directory | Description | Key Features |
|-----------|-------------|--------------|
| `ADC/` | ADC single/scan conversion | Regular channels, injected channels, analog watchdog |
| `APPLICATION/` | Application examples | Real-world use cases |
| `DMA/` | DMA transfer | Memory-to-peripheral, peripheral-to-memory |
| `EXTI/` | External interrupt | GPIO edge/level interrupt |
| `FLASH/` | Flash read/write/erase | Page erase, fast program, option bytes |
| `FreeRTOS/` | FreeRTOS RTOS | Task creation, queues, semaphores |
| `GPIO/` | GPIO control | Input, output, toggle, LED blink |
| `HarmonyOS/` | HarmonyOS integration | LiteOS-M kernel |
| `I2C/` | I2C communication | Master/slave, EEPROM access |
| `IAP/` | In-Application Programming | UART IAP bootloader |
| `INT/` | Interrupt examples | NVIC configuration, priority |
| `IWDG/` | Independent watchdog | Window watchdog, reset |
| `OPA/` | Operational Amplifier | Internal OPA configuration |
| `PIOC/` | Port I/O Controller | Programmable I/O, flexible pin mapping |
| `PMP/` | Parallel Master Port | External memory interface |
| `PWR/` | Power management | STOP mode, STANDBY, PVD |
| `RCC/` | Clock configuration | HSE, PLL, system clock switch |
| `RT-Thread/` | RT-Thread RTOS | Thread, timer, IPC |
| `RunInRam/` | Run code from RAM | Execute functions from SRAM |
| `SDI_Printf/` | SDI debug printf | Single-wire debug interface |
| `SPI/` | SPI communication | Master/slave, DMA |
| `SYSTICK/` | System tick timer | Delay, periodic interrupt |
| `TIM/` | Timer/PWM | PWM output, input capture, encoder |
| `TOUCHKEY/` | Touch key detection | Capacitive touch sensing |
| `TencentOS/` | TencentOS Tiny | RTOS integration |
| `USART/` | USART communication | Polling, interrupt, DMA |
| `USB/` | USB device/host | CDC, HID, vendor class |
| `USBPD/` | USB Power Delivery | Source, sink, DRP, PD state machine |
| `WWDG/` | Window watchdog | Windowed watchdog timer |

## CH32X315 Examples (CH32X315EVT/EVT/EXAM/)

| Directory | Description | Key Features |
|-----------|-------------|--------------|
| `ADC/` | ADC conversion | Same as CH32X035 with extended channels |
| `APPLICATION/` | Application examples | Real-world use cases |
| `ARGB/` | Addressable RGB LED | WS2812-style LED control |
| `CPU/` | CPU features | Core configuration, debug |
| `CRC/` | CRC calculation | Hardware CRC engine |
| `DMA/` | DMA transfer | Enhanced DMA controller |
| `EXTI/` | External interrupt | GPIO edge/level interrupt |
| `FLASH/` | Flash operations | 192KB Flash, fast program |
| `GPIO/` | GPIO control | Input, output, interrupt |
| `I2C/` | I2C communication | Master/slave |
| `IAP/` | IAP bootloader | UART/USB IAP |
| `IWDG/` | Watchdog | Independent watchdog |
| `PWR/` | Power | Low-power modes |
| `RCC/` | Clock | PLL, HSE config |
| `RTC/` | Real-Time Clock | Calendar, alarm, wakeup |
| `SDI_Printf/` | SDI printf | Debug output |
| `SPI/` | SPI | Master/slave |
| `TIM/` | Timer/PWM | PWM, capture, encoder |
| `USART/` | USART | Serial communication |
| `USBHS/` | USB High-Speed | USB 2.0 HS device/host |
| `USBSS/` | USB Super-Speed | USB 3.x device/host |
| `WWDG/` | Window WDG | Watchdog |

## CH643 Examples (CH643EVT/EVT/EXAM/)

| Directory | Description | Key Features |
|-----------|-------------|--------------|
| `ADC/` | ADC | Analog-to-digital conversion |
| `APPLICATION/` | Applications | Real-world use cases |
| `ARGB/` | Addressable RGB LED | WS2812-style LED control |
| `CPU/` | CPU features | Core configuration |
| `CRC/` | CRC calculation | Hardware CRC |
| `DMA/` | DMA | Transfer engine |
| `EXTI/` | External interrupt | GPIO interrupt |
| `FLASH/` | Flash | Read/write/erase |
| `FreeRTOS/` | FreeRTOS RTOS | Task creation, queues |
| `GPIO/` | GPIO | Pin control |
| `HarmonyOS/` | HarmonyOS | LiteOS-M integration |
| `I2C/` | I2C | Bus communication |
| `IAP/` | IAP | Bootloader |
| `INT/` | Interrupt examples | NVIC configuration |
| `IWDG/` | Watchdog | Independent WDG |
| `OPA/` | Operational Amplifier | OPA, comparator, PGA |
| `PIOC/` | Port I/O Controller | Programmable I/O |
| `PMP/` | Parallel Master Port | External memory |
| `PWR/` | Power | Low-power modes |
| `RCC/` | Clock | System clock |
| `RT-Thread/` | RT-Thread RTOS | Thread, timer, IPC |
| `RunInRam/` | RAM execution | Run from SRAM |
| `RTC/` | Real-Time Clock | Calendar, alarm |
| `SDI_Printf/` | SDI printf | Debug |
| `SPI/` | SPI | Bus communication |
| `SYSTICK/` | SysTick | System timer |
| `TIM/` | Timer/PWM | Timing, PWM |
| `TOUCHKEY/` | Touch key | Capacitive sensing |
| `TencentOS/` | TencentOS Tiny | RTOS integration |
| `USART/` | USART | Serial |
| `USB/` | USB device/host | CDC, HID, vendor |
| `USBPD/` | USB Power Delivery | Source, sink, DRP |
| `WWDG/` | Watchdog | Window WDG |

## CH641 Examples (CH641EVT/EVT/EXAM/)

| Directory | Description | Key Features |
|-----------|-------------|--------------|
| `ADC/` | ADC | Basic ADC |
| `APPLICATION/` | Applications | Use cases |
| `DMA/` | DMA | Transfer |
| `EXTI/` | EXTI | GPIO interrupt |
| `FLASH/` | Flash | 16KB Flash ops |
| `GPIO/` | GPIO | Pin control |
| `I2C/` | I2C | Bus communication |
| `IAP/` | IAP | Bootloader |
| `PWR/` | Power | Low-power |
| `RCC/` | Clock | Clock config |
| `SDI_Printf/` | SDI printf | Debug |
| `SPI/` | SPI | Bus communication |
| `SYSTICK/` | SysTick | Timer |
| `TIM/` | Timer/PWM | Timing |
| `USART/` | USART | Serial |
| `USART_IAP/` | UART IAP | UART bootloader |
| `USBPD/` | USB-PD | Power delivery |
| `WWDG/` | Watchdog | Window WDG |

## CH634 Notes

CH634EVT does not contain example code (EXAM) directory. It only contains schematics and PCB design reference documents in the EVT directory. CH634 shares its codebase with CH32X035/CH643 -- use those examples as reference.

## How to Use Examples

1. Navigate to the chip family directory (e.g., `CH32X035EVT/`)
2. Open `EVT/EXAM/<Peripheral>/` for the desired example
3. Each example is a self-contained MounRiver Studio project with `.project` and `.wvproj` files
4. Open `.wvproj` in MounRiver Studio to build and flash
5. The project's `.project` file defines linked resources to shared `SRC/` code
