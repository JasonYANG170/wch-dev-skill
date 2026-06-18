# CH32F Example Projects

## CH32F103EVT Examples

Located in `CH32F103EVT/EVT/EXAM/`:

### Basic Peripherals
| Directory | Sub-examples | Description |
|-----------|-------------|-------------|
| ADC | ADC_DMA, AnalogWatchdog, Auto_Injection, Discontinuous_mode, ExtLines_Trigger | ADC single/multi-channel conversion |
| BKP | BKP | Backup register access |
| CAN | Networking, TestMode, Time-triggered | CAN bus communication |
| CRC | CRC_Calculation | CRC calculation |
| DAC | DAC_DMA, DAC_Exit_9_Trig, DAC_Noise_Generation, DAC_Normal_OUT, DAC_Timer_Trig | DAC analog output |
| DMA | DMA_MEM2MEM, DMA_MEM2PERIP | DMA data transfer |
| EXTI | EXTI0 | External interrupt |
| FLASH | FLASH_Program | Flash read/write/erase |
| GPIO | GPIO_Toggle | GPIO output toggle |
| I2C | I2C_10bit_Mode, I2C_7bit_Mode, I2C_DMA, I2C_EEPROM, I2C_PEC | I2C master/slave communication |
| IWDG | IWDG | Independent watchdog |
| PWR | PVD_VoltageJudger, PVD_Wakeup, Sleep_Mode, Standby_Mode, Stop_Mode | Power management |
| RCC | HSE_CLK, HSI_Calibration, HSI_PLL_Source, LSI_Calibration, MCO | Clock configuration |
| RTC | RTC_Calendar, RTC_Calibrations | Real-time clock |
| SPI | 1Lines_half-duplex, 2Lines_FullDuplex, FullDuplex_HardNSS, SPI_CRC, SPI_DMA | SPI communication |
| SYSTICK | SYSTICK_Interrupt | System tick timer |
| TIM | Clock_Select, ComplementaryOutput_DeadTime, Encoder, ExtTrigger_Start_Two_Timer, Input_Capture | Timer/PWM |
| TOUCHKEY | TKey | Touch key detection |
| USART | USART_DMA, USART_HalfDuplex, USART_HardwareFlowControl, USART_Interrupt, USART_MultiProcessorCommunication | UART/USART communication |
| USB | USBD, USBFS | USB device/host examples |
| WWDG | WWDG | Window watchdog |

### RTOS Examples
| Directory | Description |
|-----------|-------------|
| FreeRTOS | FreeRTOS core integration |
| RT_Thread | RT-Thread RTOS integration |
| LiteOS_M | Huawei LiteOS_M integration |
| TencentOS_Tiny | TencentOS Tiny integration |

### Advanced Examples
| Directory | Description |
|-----------|-------------|
| IAP | In-Application Programming (UART/USB IAP) |

---

## CH32F20xEVT Examples

Located in `CH32F20xEVT/EVT/EXAM/`:

### Basic Peripherals
| Directory | Sub-examples | Description |
|-----------|-------------|-------------|
| ADC | ADC_DMA, AnalogWatchdog, Auto_Injection, Discontinuous_mode, DualADC_AlternateTrigger | ADC with more channels and features |
| BKP | BKP | Backup register access |
| CAN | Networking, TestMode, Time-triggered | CAN bus communication |
| CRC | CRC_Calculation | CRC calculation |
| DAC | DAC_DMA, DAC_Exit_9_Trig, DAC_Noise_Generation, DAC_Normal_OUT, DAC_Timer_Trig | DAC analog output |
| DMA | DMA_MEM2MEM, DMA_MEM2PERIP | DMA data transfer (more channels) |
| DVP | DVP_TFTLCD, DVP_UART | Digital Video Port (camera interface) |
| ETH | DHCP, DNS, ETH_CFG, ETH_IAP, ETH_UART, IPRaw_PING, MACRaw, MQTT, Mail, TcpClient, TcpServer, UdpClient, UdpServer, WebServer | Ethernet (CH32F207) |
| EXTI | EXTI0 | External interrupt |
| FLASH | FLASH_Program | Flash operations |
| FSMC | LCD, NANDFLASH, SRAM | Flexible Static Memory Controller |
| GPIO | GPIO_Toggle | GPIO control |
| I2C | I2C_10bit_Mode, I2C_7bit_Interrupt_Mode, I2C_7bit_Mode, I2C_DMA, I2C_EEPROM | I2C communication |
| I2S | HostRx_SlaveTx, I2S_DMA, I2S_Interupt | I2S audio interface |
| IWDG | IWDG | Independent watchdog |
| OPA | OPA_Voltage_Follower | Operational Amplifier |
| PWR | PVD_VoltageJudger, PVD_Wakeup, Sleep_Mode, Standby_Mode, Standby_RAM_LV_Mode | Power management |
| RCC | Get_CLK, HSE_CLK, HSI_Calibration, HSI_PLL_Source, MCO | Clock configuration |
| RNG | RNG | Random Number Generator |
| RTC | RTC_Calendar, RTC_Calibrations | Real-time clock |
| SDIO | SDIO_SD, SDIO_eMMC | SD card interface |
| SPI | 1Lines_half-duplex, 2Lines_FullDuplex, FullDuplex_HardNSS, SPI_CRC, SPI_DMA | SPI communication |
| SYSTICK | SYSTICK_Interrupt | System tick timer |
| TIM | Clock_Select, ComplementaryOutput_DeadTime, Encoder, ExtTrigger_Start_Two_Timer, Input_Capture | Timer/PWM |
| TOUCHKEY | TKey | Touch key detection |
| USART | USART_DMA, USART_HalfDuplex, USART_HardwareFlowControl, USART_Idle_Recv, USART_Interrupt | UART/USART communication |
| USB | USBD, USBFS, USBHS | USB device/host examples |
| WWDG | WWDG | Window watchdog |

### BLE Examples
| Directory | Description |
|-----------|-------------|
| BLE/Peripheral | BLE peripheral (slave) role |
| BLE/Central | BLE central (master) role |
| BLE/CentPeri | BLE central + peripheral combo |
| BLE/Broadcaster | BLE broadcaster (advertiser) |
| BLE/Observer | BLE observer (scanner) |
| BLE/HID_Keyboard | BLE HID keyboard |
| BLE/HID_Mouse | BLE HID mouse |
| BLE/HID_Consumer | BLE HID consumer control |
| BLE/HID_Touch | BLE HID touch device |
| BLE/HeartRate | BLE heart rate sensor |
| BLE/RunningSensor | BLE running sensor |
| BLE/CyclingSensor | BLE cycling sensor |
| BLE/MESH | BLE Mesh networking |
| BLE/BLE_UART | BLE UART bridge |
| BLE/BLE_USB | BLE USB bridge |
| BLE/RF_PHY | BLE RF PHY testing |
| BLE/RF_PHY_Auto | BLE RF PHY auto test |
| BLE/RF_PHY_Hop | BLE RF PHY hop test |
| BLE/Direct_Test_Mode | BLE direct test mode |
| BLE/SpeedTest_Central | BLE speed test (central) |
| BLE/SpeedTest_Peripheral | BLE speed test (peripheral) |
| BLE/MultiCentral | BLE multi-central |
| BLE/MultiCentPeri | BLE multi-central-peripheral |
| BLE/Peripheral_ETH | BLE peripheral with Ethernet |
| BLE/BackupUpgrade_IAP | BLE IAP upgrade |
| BLE/BackupUpgrade_OTA | BLE OTA upgrade |
| BLE/PAwR_ADV | BLE PAwR advertiser |
| BLE/PAwR_RSP | BLE PAwR responder |
| BLE/SYNC_ADV | BLE synchronized advertiser |
| BLE/SYNC_SCAN | BLE synchronized scanner |
| BLE/LWNS | BLE LWNS example |
| BLE/peripheral_ancs_client | BLE ANCS client |

### BLE HAL and Library
| Directory | Description |
|-----------|-------------|
| BLE/HAL | BLE hardware abstraction (KEY, LED, MCU, RTC, SLEEP) |
| BLE/LIB | BLE protocol stack library (wchble.lib) |

### RTOS Examples
| Directory | Description |
|-----------|-------------|
| FreeRTOS | FreeRTOS core integration |
| RT_Thread | RT-Thread RTOS integration |
| LiteOS_M | Huawei LiteOS_M integration |
| TencentOS_Tiny | TencentOS Tiny integration |

### Advanced Examples
| Directory | Description |
|-----------|-------------|
| APPLICATION | Application-level examples (WS2812_LED) |
| IAP | In-Application Programming (USB/UART IAP) |

---

## CH32M030EVT Examples

Located in `CH32M030EVT/EVT/EXAM/`:

### Basic Peripherals
| Directory | Sub-examples | Description |
|-----------|-------------|-------------|
| ADC | ADC_AVG, ADC_DMA, AnalogWatchdog, Auto_Injection, Discontinuous_mode | ADC conversion |
| DMA | DMA_MEM2MEM, DMA_MEM2PERIP | DMA data transfer |
| EXTI | EXTI | External interrupt |
| FLASH | FLASH_ECC, FLASH_Program | Flash operations with ECC |
| GPIO | GPIO_Toggle | GPIO output toggle |
| I2C | I2C_10bit_Mode, I2C_7bit_Interrupt_Mode, I2C_7bit_Mode, I2C_DMA, I2C_EEPROM | I2C communication |
| INT | Interrupt_VTF, VectorInRAM | VTF interrupt and vector-in-RAM |
| OPA | CMP, CMP_IRQ, OPA_PGA | Comparator and PGA |
| PWR | PWR_OVP, Sleep_Mode, Standby_Mode, Stop_Mode | Power management with OVP |
| RCC | Get_CLK, HSI_Calibration, MCO | Clock configuration |
| SPI | 1Lines_half-duplex, 2Lines_FullDuplex, FullDuplex_HardNSS, SPI_CRC, SPI_DMA | SPI communication |
| SYSTICK | SYSTICK_Interrupt | System tick timer |
| TIM | ADC_Capture, Bothedge_Capture, CenterAlignedMove, Clock_Select, ComplementaryOutput_DeadTime | Timer/PWM |
| USART | USART_DMA, USART_HalfDuplex, USART_HardwareFlowControl, USART_Idle_Recv, USART_Interrupt | UART/USART communication |
| USB | USBFS | USB full-speed device/host |
| USBPD | USBPD_SNK, USBPD_SRC | USB Power Delivery |
| WWDG | WWDG | Window watchdog |

### RTOS Examples
| Directory | Description |
|-----------|-------------|
| FreeRTOS | FreeRTOS integration |
| RT-Thread | RT-Thread RTOS integration |
| TencentOS | TencentOS integration |
| HarmonyOS | LiteOS_m (HarmonyOS) integration |

### Advanced Examples
| Directory | Sub-examples | Description |
|-----------|-------------|-------------|
| APPLICATION | Electric_Fan, PD, SoftUART | Application-level examples |
| IAP | UART_USB_IAP | In-Application Programming |
| SDI_Printf | SDI_Printf | SDI printf debug output |

---

## Example Usage

To use an example:

1. Navigate to the example directory
2. Open the `.wvproj` file in MounRiver Studio (or `.uvprojx` in Keil MDK)
3. Build the project (Ctrl+B or Build button)
4. Flash to the chip using WCH-Link or WCH ISP tool
5. Observe behavior (LED blink, UART output, etc.)

## Creating New Projects from Examples

1. Copy the closest example directory
2. Rename the project and directories
3. Modify `Main.c` with your application code
4. Add/remove peripheral headers in the appropriate `_conf.h` file
5. Add interrupt handlers in the appropriate `_it.c` file
6. Build and test
