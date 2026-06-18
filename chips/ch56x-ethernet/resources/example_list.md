# CH569 Official Example Projects

Examples are located at `CH569EVT/EVT/EXAM/`.

> Note: CH561/CH563 (ARM) examples are in ch561-ch563-arm-dev-skill.

## Ethernet Examples (ETH/)

| Directory | Description |
|-----------|-------------|
| `ETH/TCPClient/` | TCP client - connect to server, send/receive data |
| `ETH/TCPServer/` | TCP server - listen for connections, handle clients |
| `ETH/UDPClient/` | UDP client - send datagrams to server |
| `ETH/UDPServer/` | UDP server - receive and respond to datagrams |
| `ETH/DHCP/` | DHCP client - automatic IP address assignment |
| `ETH/DNS/` | DNS resolver - hostname to IP address lookup |
| `ETH/IPRaw_PING/` | Raw IP ping (ICMP) - network connectivity test |
| `ETH/MAC_RAW/` | Raw MAC frame - low-level Ethernet frame send/receive |
| `ETH/ETH_SRC/` | Ethernet source - basic Ethernet initialization and operation |

All Ethernet examples use the WCHNET TCP/IP stack library.

## USB 3.0 Examples (USBSS/)

| Directory | Description |
|-----------|-------------|
| `USBSS/USBD/` | USB 3.0 Superspeed device examples |
| `USBSS/USBH/` | USB 3.0 Superspeed host examples |

## HSPI Examples (HSPI/)

| Directory | Description |
|-----------|-------------|
| `HSPI/HSPI_NormalMode/` | HSPI normal mode host/slave transfer |
| `HSPI/HSPI_BurstMode/` | HSPI burst mode with hardware auto-ACK |
| `HSPI/HSPI_BurstMode_UpDown_Switch/` | Dynamic host/slave role switching |
| `HSPI/HSPI_DoubleDMAMode/` | Double DMA mode for continuous transfers |
| `HSPI/HSPI_ECDC/` | HSPI with ECDC hardware encryption |

## ECDC Example (ECDC/)

| Directory | Description |
|-----------|-------------|
| `ECDC/` | AES/SM4 hardware encryption/decryption |

## DVP Example (DVP/)

| Directory | Description |
|-----------|-------------|
| `DVP/` | Digital Video Port - OV2640 camera capture |

## SD Card Example (SD/)

| Directory | Description |
|-----------|-------------|
| `SD/` | SD card read/write via SDIO controller |

## Low Power Example (LOWPOWER/)

| Directory | Description |
|-----------|-------------|
| `LOWPOWER/` | Low power modes (Idle, Halt, Sleep) with GPIO wakeup |

## Peripheral Examples

| Directory | Description |
|-----------|-------------|
| `GPIO/` | GPIO input/output, LED control, button reading |
| `UART/` | UART0-3 serial communication, printf, echo |
| `SPI/` | SPI0/1 master/slave communication, DMA transfers |
| `TMR/` | Timer0-2 timing, PWM output, input capture |
| `PWMX/` | PWM0-3 independent channel output |
| `FLASH/` | Flash read/write/erase operations |
| `IAP/` | In-Application Programming bootloader |
| `EMMC/` | eMMC card read/write via SDIO with AES encryption |
| `BUS8/` | 8-bit external bus interface (CH372) |
| `UserOptionBytes/` | User option byte configuration |

## Shared Source (SRC/)

All projects share common source code in `EXAM/SRC/`:

| Directory | Description |
|-----------|-------------|
| `SRC/Peripheral/` | CH56x peripheral driver library (CH56x_*.c/h) |
| `SRC/Startup/` | Assembly startup file |
| `SRC/Ld/` | Linker script (Link.ld) |
| `SRC/RVMSIS/` | Core RISC-V system headers |
| `SRC/Flash_Lib/` | Flash programming library |

## WCHNET Protocol Stack

The Ethernet examples use WCHNET, a pre-compiled TCP/IP stack library.
Documentation: `ETH/WCHNET Protocol Stack Library Application Note.pdf`
