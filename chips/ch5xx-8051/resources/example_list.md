# CH5xx 8051 EVT Example List

This document lists all available example projects in the WCH CH5xx 8051 EVT SDKs.

---

## CH543EVT Examples

Location: `CH543EVT/EVT/EXAM/`

| Example | Description | Files |
|---|---|---|
| ADC | ADC sampling | ADC.C, ADC.H, Main.C |
| Compare | Voltage comparator | Compare.C, Compare.H, Main.C |
| FlashRom | Flash/DataFlash read/write | FlashRom.C, FlashRom.H, Main.C |
| GPIO | GPIO configuration and interrupt | GPIO.C, GPIO.H, Main.C |
| I2C | I2C host and slave | IICHost/, IICSlave/ |
| PD_PROT | USB-PD protocol | PD_PROT.C, PD_PROT.H, Main.C |
| PWM | PWM output | PWM.C, PWM.H, Main.C |
| SPI | SPI master/slave | SPI.C, SPI.H, MainMaster.C, MainSlave.C |
| TIMER | Timer0/1/2 modes | TIMER.C, TIMER.H, Main.C |
| TouchKey | Capacitive touch detection | TouchKey.C, TouchKey.H, Main.C |
| UART | UART0/UART1 communication | UART.C, UART.H, Main.C |
| USB | USB device/host | Device/, Host/, USB_LIB/ |

---

## CH545EVT Examples

Location: `CH545EVT/EVT/EXAM/`

| Example | Description |
|---|---|
| ADC | ADC sampling |
| DATAFLASH | DataFlash read/write |
| GPIO | GPIO configuration |
| I2C | Hardware I2C host/slave |
| RGB | RGB LED control |
| SPI | SPI communication |
| TIMER | Timer modes |
| TOUCHKEY | Capacitive touch detection |
| UART | UART communication |
| USB | USB device |

---

## CH547EVT Examples

Location: `CH547EVT/EVT/`

CH547EVT contains only the chip header file:
- `CH547.H` -- Complete register definitions for CH547
- `CH547.INC` -- Assembly include file

Refer to CH549EVT or CH555EVT for working example code (register-compatible with minor differences).

---

## CH549EVT Examples

Location: `CH549EVT/EVT/EXAM/`

| Example | Description | Files |
|---|---|---|
| ADC | ADC sampling | ADC.C, ADC.H, Main.C |
| Compare | Voltage comparator | Compare.C, Compare.H, Main.C |
| FlashRom | Flash/DataFlash read/write | FlashRom.C, FlashRom.H, Main.C |
| GPIO | GPIO configuration and interrupt | GPIO.C, GPIO.H, Main.C |
| IAP | In-Application Programming | CH549IAP/, CH549User/ |
| PWM | PWM output | PWM.C, PWM.H, Main.C |
| SPI | SPI master/slave | SPI.C, SPI.H, MainMaster.C, MainSlave.C |
| Timer | Timer0/1/2 modes | Timer.C, Timer.H, Main.C |
| TouchKey | Capacitive touch detection | TouchKey.C, TouchKey.H, Main.C |
| Type_C | USB Type-C detection | Type_C.C, Type_C.H, Main.C |
| UART | UART0/UART1 communication | UART.C, UART.H, Main.C |
| USB | USB device/host | Device/, Host/, USB_LIB/ |

---

## CH552EVT Examples

Location: `CH552EVT/EVT/`

CH552EVT contains only the chip header file:
- `CH552.H` -- Complete register definitions for CH552/CH551
- `CH552.INC` -- Assembly include file

Refer to CH554EVT or CH549EVT for working example code (register-compatible with minor differences).

---

## CH554EVT Examples

Location: `CH554EVT/EVT/EXAM/`

| Example | Description | Files |
|---|---|---|
| ADC | ADC sampling, comparator | ADC.C, ADC.H, Main.C |
| Compare | Voltage comparator | Compare.C, Compare.H, Main.C |
| DataFlash | DataFlash read/write | DataFlash.C, DataFlash.H, Main.C |
| GPIO | GPIO config, interrupt | GPIO.C, GPIO.H, Main.C |
| IAP | In-Application Programming | CH554IAPDemo.C |
| PWM | PWM1/PWM2 output | PWM.C, PWM.H, Main.C |
| SPI | SPI0 master/slave | SPI.C, SPI.H, MainMaster.C, MainSlave.C |
| Timer | Timer0/1/2 modes | Timer.C, Timer.H, Main.C |
| TouchKey | 6-channel capacitive touch | TouchKey.C, TouchKey.H, Main.C |
| Type-C | USB Type-C detection | Type_C.C, Type_C.H, Main.C |
| UART1 | UART1 communication | UART1.C, UART1.H, Main.C |
| USB | USB device + host | Device/, Host/, USB_LIB/ |

Shared files in `Public/`:
- CH554.H -- Register definitions
- CH554.INC -- Assembly include
- Debug.C -- System utilities (CfgFsys, delay, UART)
- Debug.H -- Configuration defines

---

## CH555EVT Examples

Location: `CH555EVT/EVT/EXAM/`

| Example | Description |
|---|---|
| ADC | 12-bit ADC sampling |
| DATAFLASH | DataFlash read/write |
| GPIO | GPIO configuration |
| I2C | Hardware I2C host/slave |
| RGB | RGB LED control |
| SPI | SPI communication |
| TIMER | Timer modes |
| UART | UART communication |
| USB | USB device/host |

---

## CH557EVT Examples

Location: `CH557EVT/EVT/EXAM/`

| Example | Description |
|---|---|
| ADC | 12-bit ADC sampling |
| DATAFLASH | DataFlash read/write |
| GPIO | GPIO configuration |
| I2C | Hardware I2C host/slave |
| RGB | RGB LED control |
| SPI | SPI communication |
| TIMER | Timer modes |
| TOUCHKEY | Capacitive touch detection |
| UART | UART communication |
| USB | USB device/host |

---

## CH558EVT Examples

Location: `CH558EVT/EVT/`

CH558EVT contains only the chip header file:
- `CH558.H` -- Complete register definitions for CH558
- `CH558.INC` -- Assembly include file

Refer to CH559EVT for working example code (register-compatible with minor differences).

---

## CH559EVT Examples

Location: `CH559EVT/EVT/EXAM/`

| Example | Description |
|---|---|
| ADC | 12-bit ADC (manual + DMA) |
| CHIP_ID | Read chip unique ID |
| DataFlash | DataFlash read/write |
| Encryption | Chip encryption |
| GPIO | GPIO config, P4, interrupt, drive cap |
| IAP | UART/USB IAP bootloader |
| LED_CTL | LED + U-disk + Flash DMA |
| PWM | PWM1-PWM4 output |
| SPI0 | SPI0 master/slave |
| SPI1 | SPI1 master/slave |
| Timer0 | Timer0 modes |
| Timer1 | Timer1 modes |
| Timer2 | Timer2 + capture |
| Timer3 | Timer3 modes |
| UART0 | UART0 communication |
| UART1 | UART1 + RS485 |
| USB | USB device/host |
| USB_LIB | USB library |
| WDOG | Watchdog timer |
| XBUS | External bus |

Shared files:
- CH559.H -- Register definitions (extended: P0-P5, UART0-3, SPI0/1, Timer0-3)
- CH559.INC -- Assembly include
- DEBUG.C -- System utilities
- DEBUG.H -- Configuration defines

---

## Using Examples

1. Navigate to the chip directory (e.g., CH554EVT/EVT/EXAM/)
2. Open the `.uvproj` file in Keil C51
3. Build with F7
4. Flash via WCH ISP tool or WCH-LinkE

Each example is self-contained with its own Main.C and peripheral driver files. The `Public/` directory contains shared utility code.
