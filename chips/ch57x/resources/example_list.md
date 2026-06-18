# CH57x EXAM Directory Structure

## CH572EVT (RISC-V, BLE 5.0)

```
CH572: Low-power Bluetooth - 32-bit RISC-V core microcontroller
└── EXAM:
    ├── SRC                          Shared source (linked into every project)
    │   ├── Ld                       Linker scripts
    │   ├── RVMSIS                   Core system headers
    │   ├── Startup                  CH57x startup file
    │   └── StdPeriphDriver          Peripheral driver source and headers
    │
    ├── ADC                          ADC sampling examples
    │                                (temperature, single channel, differential, TouchKey)
    │
    ├── CMP                          Comparator example
    │                                (internal reference voltage, interrupt, timer capture)
    │
    ├── FLASH                        On-chip Flash examples
    │                                (Code area, DataFlash erase/read/write)
    │
    ├── I2C                          I2C interface examples
    │
    ├── IAP                          In-Application Programming
    │   ├── APP                      IAP companion application examples
    │   ├── USB_IAP                  USB-based firmware update
    │   ├── UART_IAP                 UART-based firmware update
    │   └── WCHMcuIAP_WinAPP        IAP host tool and source
    │
    ├── KEYSCAN                      Hardware key scan matrix example
    │                                (interrupt-driven, sleep/wake support)
    │
    ├── PM                           System sleep mode and wake examples
    │                                (GPIOA_5 wake source, 4 power levels)
    │
    ├── PWMX                         PWM4-11 output examples
    │
    ├── RF                           2.4GHz RF communication examples
    │   ├── RF_Basic                  Basic 2.4GHz TX/RX (non-standard RF)
    │   ├── RF_Extend                 Extended RF features
    │   ├── RF_Uart                   Wireless UART bridge (TX side)
    │   ├── RF_UartDongle             Wireless UART bridge (USB dongle side)
    │   ├── Direct_Test_Mode          RF DTM certification test
    │   └── LIB                       RF library files and headers
    │
    ├── SPI                          SPI examples (Master/Slave mode)
    │
    ├── TMR                          Timer examples
    │
    ├── UART                         UART serial examples
    │
    ├── USB
    │   ├── Device
    │   │   ├── COM                  USB CDC serial device example
    │   │   ├── VendorDefinedDev     Custom USB device (CH32-like, 8 channels)
    │   │   ├── CompoundDev          USB compound HID device (keyboard + mouse)
    │   │   └── HID_CompliantDev     USB HID compliant device example
    │   │
    │   └── Host
    │       ├── HostEnum             USB host enumeration (HID keyboard/mouse, hub)
    │       ├── HostAOA              USB host AOA (Android Open Accessory)
    │       └── U_DISK               USB mass storage / U-disk filesystem
    │
    └── BLE                          Bluetooth Low Energy examples
        ├── Peripheral               BLE peripheral (slave) role example
        ├── HID_Consumer             BLE HID consumer control (e.g. camera shutter, volume)
        ├── HID_Mouse                BLE HID mouse example
        ├── RF_PHY                   Non-standard RF PHY example (BLE library based)
        ├── IoCHub_NET               BLE + IoCHub simple light control example
        ├── SpeedTest_Peripheral     BLE speed test peripheral example
        │
        ├── BackupUpgrade_IAP        Backup OTA IAP example
        ├── BackupUpgrade_JumpIAP    Backup OTA jump-to-IAP example
        ├── BackupUpgrade_OTA        Backup OTA user application (peripheral + OTA)
        │
        ├── HAL                      Shared hardware abstraction files
        └── LIB                      BLE protocol stack library and headers
```

## CH579EVT (ARM Cortex-M0, BLE + Ethernet + LCD)

The CH579 is a BLE-capable chip with an ARM Cortex-M0 core (not RISC-V). It adds LCD segment display, SPI-based Ethernet (NET), and dual SPI peripherals not found on CH572. Projects use Keil uVision (`.uvprojx`) instead of MounRiver Studio.

```
CH579: BLE + Ethernet + LCD - 32-bit ARM Cortex-M0 core microcontroller
└── EXAM:
    ├── SRC                          Shared source (linked into every project)
    │   ├── sct                      Keil scatter files (CH57x.sct)
    │   ├── CMSIS                    ARM CMSIS core headers
    │   ├── Startup                  ARM Cortex-M0 startup file (startup_ARMCM0.s)
    │   └── StdPeriphDriver          Peripheral driver source and headers
    │
    ├── ADC                          ADC sampling examples
    │
    ├── BLE                          Bluetooth Low Energy examples
    │   ├── Peripheral               BLE peripheral (slave) role example
    │   ├── Central                  BLE central (master) role example
    │   ├── CentPeri                 BLE combined central + peripheral role
    │   ├── Broadcaster              BLE broadcaster role example
    │   ├── Observer                 BLE observer role example
    │   ├── HID_Consumer             BLE HID consumer control
    │   ├── HID_Keyboard             BLE HID keyboard example
    │   ├── HID_Mouse                BLE HID mouse example
    │   ├── HID_Touch                BLE HID touch example
    │   ├── HeartRate                BLE Heart Rate Profile example
    │   ├── CyclingSensor            BLE Cycling Speed/Cadence Sensor
    │   ├── RunningSensor            BLE Running Speed/Cadence Sensor
    │   ├── MESH                     BLE Mesh examples
    │   ├── MultiCentral             BLE multi-central connection example
    │   ├── RF_PHY                   Non-standard RF PHY example
    │   ├── RF_PHY_Hop               RF PHY frequency hopping example
    │   ├── Direct_Test_Mode         RF DTM certification test
    │   ├── BackupUpgrade_IAP        Backup OTA IAP example
    │   ├── BackupUpgrade_OTA        Backup OTA user application
    │   ├── OnlyUpdateApp_IAP        OTA IAP (app-only update)
    │   ├── OnlyUpdateApp_Peripheral OTA peripheral (app-only update)
    │   ├── HAL                      Shared hardware abstraction files
    │   └── LIB                      BLE protocol stack library and headers
    │
    ├── FLASH                        On-chip Flash examples
    │
    ├── LCD                          LCD segment display example
    │                                (drives 8 digits via SEG0-SEG7, COM0-COM3)
    │
    ├── NET                          SPI-based Ethernet (WCHNET library)
    │   ├── NetLib                   WCHNET protocol stack library
    │   │   ├── CH57xNET.lib         Pre-compiled Ethernet library
    │   │   ├── wchnet.h             WCHNET API header
    │   │   ├── eth_driver.c/h       Ethernet PHY driver
    │   │   └── WCHNET*.PDF          Documentation
    │   ├── DHCP                     DHCP client example
    │   ├── DNS                      DNS resolver example
    │   ├── MQTT                     MQTT client example
    │   ├── TcpClient                TCP client example
    │   ├── TcpServer                TCP server example
    │   ├── UdpClient                UDP client example
    │   ├── UdpServer                UDP server example
    │   ├── FTP_Client               FTP client example
    │   ├── FTP_Server               FTP server example
    │   └── IPRaw_PING               IP raw ping example
    │
    ├── PM                           System sleep mode and wake examples
    │
    ├── PWMX                         PWM output examples
    │
    ├── SPI0                         SPI0 examples (Master/Slave mode)
    │
    ├── SPI1                         SPI1 examples (Master mode)
    │
    ├── TMR                          Timer examples
    │
    ├── UART1                        UART1 serial examples
    │
    └── USB
        ├── Device                   USB device examples
        └── Host                     USB host examples
```

## Notes

- CH572 examples are MounRiver Studio projects (`.wvproj`); CH579 examples are Keil uVision projects (`.uvprojx`)
- CH572 is RISC-V (QingKe V4A); CH579 is ARM Cortex-M0
- BLE examples require the pre-compiled BLE library in `BLE/LIB/`
- CH579 NET examples require the WCHNET library in `NET/NetLib/`
- CH579 LCD segment pins (SEG0-SEG23) share GPIO Port B (PB0-PB23) -- using LCD disables those pins for GPIO
- CH579 has dual SPI (SPI0 + SPI1); SPI1 is Master-only with no DMA or Slave mode
- RF examples (`RF/`) use the BLE library's RF driver layer
- IAP applications must be linked at Flash offset 0x1000
- The `SRC/` directory is shared across all projects via linked resources
