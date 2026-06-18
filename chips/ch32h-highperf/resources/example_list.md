# CH32H417 Example Project List

All examples are located under `CH32H417EVT/EVT/EXAM/`.

## Peripheral Examples

| Directory | Sub-examples | Description |
|-----------|-------------|-------------|
| `ADC/` | | ADC single/dual mode, injection, watchdog |
| `CAN/` | | CAN bus communication (CAN1/2/3) |
| `CRC/` | `CRC_Calculation/` | CRC-32 block calculation |
| `DAC/` | `DAC_Normal_OUT/`, `DAC_DMA/`, `DAC_Exit_9_Trig/`, `DAC_Noise_Generation/`, `DualDAC_SineWave/`, `DualDAC_Triangle/` | DAC output modes: normal, DMA, noise, triangle, dual |
| `DFSDM/` | `DFSDM_InternalStandardMode/`, `DFSDM_InternalDualMode/`, `DFSDM_InternalInterleavedMode/`, `DFSDM_InternalADC/`, `DFSDM_I2S_Audio/`, `DFSDM_SAI_Audio/`, `DFSDM_SerialSPI/`, `DFSDM_DualFliter/`, `DFSDM_EXTITrigger/`, `DFSDM_Synchronous_Regular/`, `DFSDM_TIMTrigger/` | Sigma-Delta filter modes |
| `DMA/` | | DMA transfers and MUX configuration |
| `DVP/` | `DVP_UART/`, `DVP_TFTLCD/`, `DVP_LTDC/` | Camera: UART output, TFT display, LTDC display |
| `ECDC/` | `RAM_Block/`, `Single_Time/` | AES encryption: RAM block and single-time modes |
| `ETH/` | | Ethernet with internal PHY |
| `EXTI/` | `EXTI0/` | External interrupt on PB0 |
| `FLASH/` | | Flash read/write operations |
| `FMC/` | `SRAM/`, `NORFLASH/`, `SDRAM_16bit/`, `SDRAM_16bit_DMA/`, `SDRAM_32bit/`, `SDRAM_32bit _DMA/`, `NAND/`, `LCD/`, `LCD_8bit/` | External memory: SRAM, NOR, SDRAM, NAND, LCD |
| `GPIO/` | | GPIO control and toggle |
| `GPHA/` | `GPHA_R2M/`, `GPHA_M2M_WithLCD/` | Graphics: register-to-memory fill, M2M with LCD |
| `HSADC/` | | High-Speed ADC |
| `I2C/` | | I2C master/slave communication |
| `I2S/` | `I2S_DMA/`, `HostRx_SlaveTx/` | I2S DMA transfer, host RX / slave TX |
| `I3C/` | `I3C_DMA_Controller/`, `I3C_DMA_Target/`, `I3C_Controller_Check_ZeroIBI/` | I3C controller/target DMA, IBI check |
| `IAP/` | | In-Application Programming |
| `IWDG/` | `IWDG/` | Independent Watchdog |
| `LPTIM/` | `LPTIM_LP_WakeUp/`, `PWM_OnePulse_SingleMode/` | Low-power timer wakeup, PWM one-pulse |
| `LTDC/` | | LCD-TFT Display Controller |
| `OPA/` | `OPA/`, `CMP/` | Operational amplifier, comparator |
| `PIOC/` | `PIOC_UART/`, `PIOC_IIC/`, `PIOC_NEC/`, `PIOC_Single_Wire/`, `PIOC_1_Wire/`, `Tool_Manual/` | Programmable I/O: UART, I2C, NEC, 1-Wire |
| `PWR/` | | Power management |
| `QSPI/` | `QSPI_FLASH/`, `QSPI_FLASH_DMA/`, `QSPI_MemoryMap/` | Quad SPI: flash, DMA, memory-mapped |
| `RCC/` | `Get_CLK/`, `HSI_Calibration/`, `MCO/` | Clock: get freq, HSI cal, MCO output |
| `RNG/` | `RNG/` | Random Number Generator |
| `RTC/` | `RTC_Calendar/`, `RTC_Calibrations/` | RTC calendar and calibration |
| `SAI/` | `SAI_Play/` | Serial Audio Interface playback |
| `SDIO/` | `SDIO_SD/`, `SDIO_eMMC/` | SD card and eMMC via SDIO |
| `SDMMC/` | `SDMMC_SD/`, `SDMMC_eMMC/`, `SDMMC_eMMC_DDR/` | SDMMC: SD, eMMC, eMMC DDR |
| `SerDes/` | | Serializer/Deserializer |
| `SPI/` | | SPI communication |
| `SWPMI/` | `SWPMI_SwpCard/`, `SWPMI_MultiBUffering/` | Single Wire Protocol: card, multi-buffering |
| `TIM/` | | Timer/PWM |
| `TKey/` | `TKey/`, `TKey_8keys/`, `TKEYLIB/` | Touch key: single, 8-key, library |
| `UHSIF/` | `UHSIF_SLAVE/` | Ultra High Speed Interface slave |
| `USART/` | | USART/UART communication |
| `USBFS/` | `DEVICE/`, `HOST/` | USB Full-Speed device and host |
| `USBHS/` | `DEVICE/`, `HOST/` | USB High-Speed device and host |
| `USBPD/` | | USB Power Delivery |
| `USBSS/` | | USB 3.0 Superspeed |
| `WWDG/` | `WWDG/` | Window Watchdog |

## Application Examples

| Directory | Description |
|-----------|-------------|
| `APPLICATION/` | Combined application examples |
| `CPU/` | CPU-specific examples (dual-core) |

## Example Selection Guide

### Communication Interfaces
- **UART**: `USART/` - Serial communication at various baud rates
- **SPI**: `SPI/` - SPI master/slave with DMA support
- **I2C**: `I2C/` - I2C master/slave, EEPROM, sensor reading
- **CAN**: `CAN/` - CAN bus with filtering and interrupts
- **Ethernet**: `ETH/` - TCP/IP stack with internal PHY
- **USB**: `USBFS/`, `USBHS/` - USB device and host
- **USB 3.0**: `USBSS/` - Superspeed device
- **USB-PD**: `USBPD/` - Power delivery source/sink

### Display and Graphics
- **LTDC**: `LTDC/` - RGB display controller
- **GPHA**: `GPHA/` - Hardware graphics acceleration (DMA2D)
- **DVP**: `DVP/` - Camera interface

### Audio
- **I2S**: `I2S/` - Digital audio interface
- **SAI**: `SAI/` - Serial audio interface

### Analog
- **ADC**: `ADC/` - Standard ADC (12-bit)
- **HSADC**: `HSADC/` - High-speed ADC
- **DAC**: `DAC/` - Digital-to-analog converter
- **OPA**: `OPA/` - Operational amplifier

### Timers
- **TIM**: `TIM/` - PWM, input capture, encoder
- **LPTIM**: `LPTIM/` - Low-power timer

### Storage
- **FLASH**: `FLASH/` - Internal flash operations
- **FMC**: `FMC/` - External SRAM/NOR/NAND
- **QSPI**: `QSPI/` - Quad SPI flash
- **SDIO/SDMMC**: `SDIO/`, `SDMMC/` - SD card

### System
- **RCC**: `RCC/` - Clock configuration
- **PWR**: `PWR/` - Power modes, PVD
- **IWDG/WWDG**: `IWDG/`, `WWDG/` - Watchdog timers
- **RTC**: `RTC/` - Real-time clock
- **EXTI**: `EXTI/` - External interrupts
- **CRC**: `CRC/` - CRC calculation
- **RNG**: `RNG/` - Random number generation

### Special Interfaces
- **SerDes**: `SerDes/` - High-speed serial/deserializer
- **I3C**: `I3C/` - MIPI I3C bus
- **DFSDM**: `DFSDM/` - Sigma-Delta digital filter
- **SWPMI**: `SWPMI/` - Single wire protocol
- **TKey**: `TKey/` - Touch key sensing
- **UHSIF**: `UHSIF/` - Ultra high speed interface
- **ECDC**: `ECDC/` - External clock detection

## Building Examples

1. Open the `.wvproj` file in MounRiver Studio
2. Select the correct toolchain (RISC-V GCC)
3. Build: Project -> Build Project (Ctrl+B)
4. Flash: Run -> Debug (via WCH-LinkE)
5. Serial debug: UART1 @ 115200 baud, 8N1
