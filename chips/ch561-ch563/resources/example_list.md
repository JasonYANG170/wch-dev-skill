# CH561/CH563 Official Example Projects

## CH561 Examples (CH561EVT/EXAM/)

| Directory | Description |
|-----------|-------------|
| `ADC/` | ADC analog-to-digital conversion |
| `FLASH/` | Flash read/write/erase and EEPROM operations |
| `GPIO/` | GPIO input/output, LED control, button reading |
| `IAP_DEMO/` | In-Application Programming bootloader |
| `INT/` | Interrupt configuration and handling |
| `ISP_DEMO/` | In-System Programming demo |
| `NET/` | Ethernet networking (CH561NET TCP/IP stack) |
| `SPI/` | SPI0/1 master/slave communication |
| `SRC/` | Shared source code (startup, headers) |
| `TIM/` | Timer0-3 timing, PWM, input capture |
| `UART/` | UART0/1 serial communication |
| `WDOG/` | Watchdog timer |

### CH561 NET Examples (CH561EVT/EXAM/NET/)

| Directory | Description |
|-----------|-------------|
| `TCP_CLIENT/` | TCP client - connect to server |
| `TCP_SERVER/` | TCP server - listen for connections |
| `UDP_CLIENT/` | UDP client - send datagrams |
| `UDP_SERVER/` | UDP server - receive datagrams |
| `DHCP_CLIENT/` | DHCP client - auto IP assignment |
| `DNS/` | DNS resolver |
| `IP_RAW/` | Raw IP packets |
| `PING/` | ICMP ping |
| `NET_CFG/` | Network configuration |
| `NET_MAC/` | MAC address configuration |

## CH563 Examples (CH563EVT/EXAM/)

| Directory | Description |
|-----------|-------------|
| `ADC/` | ADC analog-to-digital conversion |
| `EEPROM/` | Data-Flash (EEPROM) operations |
| `FLASH/` | Flash read/write/erase operations |
| `GPIO/` | GPIO input/output, LED control, button reading |
| `IAP_DEMO/` | In-Application Programming bootloader |
| `INT/` | Interrupt configuration and handling |
| `ISP_DEMO/` | In-System Programming demo |
| `NET/` | Ethernet networking (CH561NET TCP/IP stack) |
| `PARA/` | Parallel/external bus interface |
| `POWER/` | Power management and low power modes |
| `SPI/` | SPI0/1 master/slave communication |
| `SRC/` | Shared source code (startup, headers) |
| `TIM/` | Timer0-3 timing, PWM, input capture |
| `UART/` | UART0/1 serial communication |
| `UCOS/` | uC/OS-II RTOS port |
| `USB/` | USB device/host examples (CH563 only) |
| `WDOG/` | Watchdog timer |

### CH563 NET Examples (CH563EVT/EXAM/NET/)

| Directory | Description |
|-----------|-------------|
| `TCP_CLIENT/` | TCP client |
| `TCP_CLIENT_X/` | TCP client (extended) |
| `TCP_SERVER/` | TCP server |
| `UDP_CLIENT/` | UDP client |
| `UDP_SERVER/` | UDP server |
| `DHCP_CLIENT/` | DHCP client |
| `DNS/` | DNS resolver |
| `DNS_CLIENT/` | DNS client |
| `IP_RAW/` | Raw IP packets |
| `PING/` | ICMP ping |
| `NET_CFG/` | Network configuration |
| `NET_MAC/` | MAC address configuration |
| `FTP_CLIENT/` | FTP client |
| `FTP_SERVER/` | FTP server |
| `WEB_SERVER/` | Web server |
| `TCP_MQTT/` | MQTT client |
| `MAIL/` | Email client |
| `MAIL_POP3/` | POP3 email |
| `MAIL_SMTP/` | SMTP email |
| `TFTP_CLIENT/` | TFTP client |
| `MULTICAST/` | Multicast |
| `UPNP/` | UPnP |
| `PUB/` | Shared network library |

## Shared Source (SRC/)

All projects share common source code:

| File | Description |
|------|-------------|
| `CH561SFR.H` | CH561 register definitions (SFR addresses, bit masks) |
| `CH561BAS.H` | CH561 base types (UINT8, UINT16, UINT32, etc.) |
| `CH563SFR.H` | CH563 register definitions (adds USB, PD port) |
| `CH563BAS.H` | CH563 base types |
| `STARTUP.S` | ARM assembly startup code (vector table, stack init) |
| `SYSFREQ.C` | PLL initialization (called by STARTUP.S) |
| `SYSFREQ.H` | Clock configuration (FREQ_SYS, MEM_DATA) |
| `ISPXT56X.H` | Flash/EEPROM programming library API |
| `ISPXT56X.O` | Flash/EEPROM programming library (pre-compiled) |

## CH561 vs CH563 Differences

| Feature | CH561 | CH563 |
|---------|-------|-------|
| Core | ARM7TDMI | ARM7TDMI |
| Flash | 224KB | 224KB |
| SRAM | 32-96KB | 32-96KB |
| Ethernet | Yes | Yes |
| USB | No | Yes |
| GPIO Ports | PA, PB | PA, PB, PD |
| UART | 2 (UART0/1) | 2 (UART0/1) |
| SPI | 2 (SPI0/1) | 2 (SPI0/1) |
| Timer | 4 (TMR0-3) | 4 (TMR0-3) |
| ADC | Yes | Yes |
| NET Examples | 12 | 25+ |
| RTOS | No | uC/OS-II |

## Keil MDK Project Files

Each example project contains:
- `*.uvproj` - Keil MDK project file
- `*.uvopt` - Keil MDK options file
- Source files (*.c, *.h)
- Scatter file (*.sct) for memory layout

To build: Open .uvproj in Keil MDK, then Project -> Build Target (F7).
