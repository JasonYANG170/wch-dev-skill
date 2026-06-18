# Recipe: IAP (In-Application Programming) / OTA

## Overview

Implement a bootloader for firmware updates over UART, Ethernet, or USB on CH56x chips.
The IAP bootloader occupies the first 4KB of Flash, and the application starts at 0x1000.

## Flash Layout for IAP

```
0x00000000 +-----------------------+
           | IAP Bootloader        |  4KB (0x0000 - 0x0FFF)
0x00001000 +-----------------------+
           | Application Area 0    |  For new firmware download
           |         ...           |
0x00040000 +-----------------------+
           | Application Area 1    |  Running application
           |         ...           |
0x00070000 +-----------------------+
           | Data Storage          |  Persistent config/data
0x00080000 +-----------------------+
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
#include "CH56x_common.h"

#define UPDATE_FLAG_ADDR   0x0007F000    // Sector for update flag
#define NEW_FW_ADDR        0x00001000    // Temporary area for new firmware

void trigger_iap_update(void)
{
    UINT32 flag = 0x55AA55AA;

    // Write update flag
    FLASH_ROM_ERASE(UPDATE_FLAG_ADDR, 256);
    FLASH_ROM_WRITE(UPDATE_FLAG_ADDR, &flag, sizeof(UINT32));

    // Software reset to enter bootloader
    SYS_ResetExecute();
}

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    printf("Application running at 0x1000\n");

    // Check for update command (e.g., from UART)
    while(1)
    {
        if (UART0_GetLinSTA() & STA_RECV_DATA) {
            UINT8 cmd = UART0_RecvByte();
            if (cmd == 'U') {
                printf("Triggering IAP update...\n");
                trigger_iap_update();
            }
        }
    }
}
```

## Bootloader-Side: Checking and Performing Update

```c
#include "CH56x_common.h"

#define UPDATE_FLAG_ADDR   0x0007F000
#define APP_ADDR           0x00001000

typedef void (*pFunction)(void);

void jump_to_application(void)
{
    UINT32 jumpAddr = APP_ADDR;
    UINT32 stackAddr = *(volatile UINT32 *)jumpAddr;

    // Check if application is valid (stack pointer in RAM range)
    if (stackAddr >= 0x20000000 && stackAddr < 0x20028000) {
        pFunction appEntry;
        UINT32 resetVector = *(volatile UINT32 *)(jumpAddr + 4);

        // Set vector table offset
        PFIC_SetVTFAddr(resetVector);

        // Jump to application
        appEntry = (pFunction)resetVector;
        appEntry();
    }
    // If invalid, stay in bootloader
}

void perform_update(void)
{
    // Read new firmware from download area
    // Erase application area
    // Copy new firmware to application area
    // Clear update flag
    UINT32 flag = 0xFFFFFFFF;
    FLASH_ROM_ERASE(UPDATE_FLAG_ADDR, 256);
    FLASH_ROM_WRITE(UPDATE_FLAG_ADDR, &flag, sizeof(UINT32));
}

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);

    // Check update flag
    UINT32 flag = *(volatile UINT32 *)UPDATE_FLAG_ADDR;
    if (flag == 0x55AA55AA) {
        perform_update();
    }

    // Jump to application
    jump_to_application();

    // Should never reach here
    while(1) {}
}
```

## Linker Script for IAP Application

The application must be linked at 0x1000 instead of 0x0000.
Create a modified linker script:

```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 16K
    RAMX (xrw) : ORIGIN = 0x20020000, LENGTH = 32K
}
```

## Receiving Firmware via UART

```c
// Simple XMODEM-like protocol for firmware download
#define FW_PACKET_SIZE  128

void receive_firmware_uart(UINT32 dest_addr, UINT32 total_size)
{
    UINT8 buf[FW_PACKET_SIZE];
    UINT32 received = 0;
    UINT32 sector_addr = dest_addr;

    // Erase destination area
    FLASH_ROM_ERASE(dest_addr, total_size);

    while (received < total_size) {
        // Wait for packet header
        while (!(UART0_GetLinSTA() & STA_RECV_DATA));
        UINT8 header = UART0_RecvByte();

        if (header == 0x01) {  // SOH - start of packet
            // Read packet number
            UINT8 pktNum = UART0_RecvByte();
            UINT8 pktNumInv = UART0_RecvByte();

            // Read data
            for (int i = 0; i < FW_PACKET_SIZE; i++) {
                while (!(UART0_GetLinSTA() & STA_RECV_DATA));
                buf[i] = UART0_RecvByte();
            }

            // Read checksum
            while (!(UART0_GetLinSTA() & STA_RECV_DATA));
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

## Notes

- Bootloader must be small (fit in 4KB)
- Application must not overlap bootloader region (0x0000-0x0FFF)
- Always validate firmware before jumping (check stack pointer and reset vector)
- For Ethernet OTA, use TCP to receive firmware packets
- Consider adding CRC verification of received firmware
