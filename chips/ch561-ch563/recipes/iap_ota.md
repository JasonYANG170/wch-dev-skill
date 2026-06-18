# Recipe: IAP (In-Application Programming) / OTA

## Overview

Implement a bootloader for firmware updates over UART or Ethernet on CH561/CH563.
The IAP bootloader occupies the first 4KB of Flash, and the application starts at 0x1000.

## Flash Layout for IAP

```
0x00000000 +-----------------------+
           | IAP Bootloader        |  4KB (0x0000 - 0x0FFF)
0x00001000 +-----------------------+
           | Application Area      |  Running application
           |         ...           |
0x00030000 +-----------------------+
           | Download Buffer       |  For new firmware
           |         ...           |
0x00038000 +-----------------------+
           | Data Storage          |  Persistent config/data
0x0003F000 +-----------------------+
```

## Bootloader Flow

```
Power On
  |
  v
Bootloader (0x0000)
  |
  +-- Check update flag in Flash
  |     |
  |     +-- Flag set? --> Erase app area, copy new firmware, clear flag
  |     |
  |     +-- No flag  --> Jump to application at 0x1000
  |
  v
Application (0x1000)
```

## Application-Side: Triggering Update

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define UPDATE_FLAG_ADDR   0x00038000    // Sector for update flag
#define NEW_FW_ADDR        0x00030000    // Temporary area for new firmware

typedef void (*pFunction)(void);

void trigger_iap_update(void)
{
    UINT32 flag = 0x55AA55AA;

    // Write update flag
    FLASH_ROM_ERASE(UPDATE_FLAG_ADDR, 4096);
    FLASH_ROM_WRITE(UPDATE_FLAG_ADDR, &flag, sizeof(UINT32));

    // Software reset to enter bootloader
    // ARM software reset via AIRCR
    *((volatile UINT32 *)0xE000ED0C) = 0x05FA0004;
}

int main(void)
{
    mInitSTDIO();
    PRINT("Application running at 0x1000\n");

    // Check for update command (e.g., from UART)
    while(1)
    {
        if (R8_UART0_LSR & RB_LSR_DATA_RDY) {
            UINT8 cmd = R8_UART0_RBR;
            if (cmd == 'U') {
                PRINT("Triggering IAP update...\n");
                trigger_iap_update();
            }
        }
    }
}
```

## Bootloader-Side: Checking and Performing Update

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define UPDATE_FLAG_ADDR   0x00038000
#define APP_ADDR           0x00001000

typedef void (*pFunction)(void);

void jump_to_application(void)
{
    UINT32 jumpAddr = APP_ADDR;
    UINT32 stackAddr = *(volatile UINT32 *)jumpAddr;

    // Check if application is valid (stack pointer in SRAM range)
    if (stackAddr >= 0x00808000 && stackAddr < 0x00820000) {
        UINT32 resetVector = *(volatile UINT32 *)(jumpAddr + 4);

        // Set vector table offset
        *((volatile UINT32 *)0xE000ED08) = jumpAddr;

        // Jump to application
        pFunction appEntry = (pFunction)resetVector;
        appEntry();
    }
    // If invalid, stay in bootloader
}

void perform_update(void)
{
    // Read new firmware from download area
    // Erase application area (4KB blocks)
    // Copy new firmware to application area
    // Clear update flag
    UINT32 flag = 0xFFFFFFFF;
    FLASH_ROM_ERASE(UPDATE_FLAG_ADDR, 4096);
    FLASH_ROM_WRITE(UPDATE_FLAG_ADDR, &flag, sizeof(UINT32));
}

int main(void)
{
    // Check update flag
    UINT32 flag = *(volatile UINT32 *)UPDATE_FLAG_ADDR;
    if (flag == 0x55AA55AA) {
        perform_update();
    }

    // Jump to application
    jump_to_application();

    // Should never reach here
    while(1);
}
```

## Scatter File for IAP Application (Keil MDK)

The application must be linked at 0x1000 instead of 0x0000.
Create a modified scatter file:

```
LR_IAP 0x00001000 0x00037000 {
    ER_IAP 0x00001000 0x00037000 {
        *.o (RESET, +First)
        *(InRoot$$Sections)
        .ANY (+RO)
        .ANY (+XO)
    }
    RW_IRAM1 0x00808000 UNINIT 0x00004000 {
        .ANY (+RW +ZI)
    }
    RW_IRAM2 0x0080C000 UNINIT 0x00004000 {
        .ANY (+RW2)
    }
}
```

## Receiving Firmware via UART

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define FW_PACKET_SIZE  128

void UART0_SendByte(UINT8 dat)
{
    R8_UART0_THR = dat;
    while ((R8_UART0_LSR & RB_LSR_TX_ALL_EMP) == 0);
}

UINT8 UART0_RecvByte(void)
{
    while ((R8_UART0_LSR & RB_LSR_DATA_RDY) == 0);
    return R8_UART0_RBR;
}

void receive_firmware_uart(UINT32 dest_addr, UINT32 total_size)
{
    UINT8 buf[FW_PACKET_SIZE];
    UINT32 received = 0;
    UINT32 sector_addr = dest_addr;

    // Erase destination area (4KB blocks)
    UINT32 erase_size = (total_size + 4095) & ~4095;
    FLASH_ROM_ERASE(dest_addr, erase_size);

    while (received < total_size) {
        // Wait for packet header
        UINT8 header = UART0_RecvByte();

        if (header == 0x01) {  // SOH - start of packet
            // Read packet number
            UINT8 pktNum = UART0_RecvByte();
            UINT8 pktNumInv = UART0_RecvByte();

            // Read data
            for (int i = 0; i < FW_PACKET_SIZE; i++) {
                buf[i] = UART0_RecvByte();
            }

            // Read checksum
            UINT8 checksum = UART0_RecvByte();

            // Write to Flash
            FLASH_ROM_WRITE(sector_addr, buf, FW_PACKET_SIZE);
            sector_addr += FW_PACKET_SIZE;
            received += FW_PACKET_SIZE;

            // ACK
            UART0_SendByte(0x06);
        }
    }
}
```

## Receiving Firmware via Ethernet

For Ethernet OTA, use the CH561NET TCP/IP stack to receive firmware packets:

```c
// Connect to update server via TCP
// Receive firmware data in chunks
// Write to Flash download area
// Verify with CRC
// Set update flag and reset
```

## Notes

- Bootloader must be small (fit in 4KB)
- Application must not overlap bootloader region (0x0000-0x0FFF)
- Always validate firmware before jumping (check stack pointer and reset vector)
- Flash erase unit is 4KB -- erase whole blocks
- For Ethernet OTA, use CH561NET TCP client to download firmware
- Consider adding CRC verification of received firmware
- ARM software reset: write 0x05FA0004 to SCB AIRCR register

## Example Projects

- `CH561EVT/EXAM/IAP_DEMO/` - IAP bootloader demo
- `CH563EVT/EXAM/IAP_DEMO/` - IAP bootloader demo
