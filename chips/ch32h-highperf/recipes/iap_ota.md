# IAP/OTA Update

## Overview

CH32H417 supports In-Application Programming (IAP) for firmware updates. The bootloader occupies Flash 0x0000-0xFFFF, and the application starts at 0x10000. OTA (Over-The-Air) updates can be implemented via USB, UART, Ethernet, or other communication interfaces.

## Memory Layout for IAP

```
0x00000000 +------------------+
           | Bootloader (IAP) |  64KB (0x10000)
0x00010000 +------------------+
           | Application      |  416KB/896KB
           |                  |
           |                  |
0x00080000 +------------------+  (480KB flash)
  or
0x000F0000 +------------------+  (960KB flash)
```

## Boot Mode Switching

```c
#include "ch32h417.h"

// Switch to bootloader on next reset
void JumpToBootloader(void)
{
    // Set boot mode to BOOT
    FLASH_Unlock();
    SystemReset_StartMode(Start_Mode_BOOT);
    FLASH_Lock();

    // Trigger reset
    NVIC_SystemReset();
}

// Switch to application on next reset
void JumpToApplication(void)
{
    FLASH_Unlock();
    SystemReset_StartMode(Start_Mode_USER);
    FLASH_Lock();

    NVIC_SystemReset();
}
```

## Jump to Application (Direct)

```c
typedef void (*pFunction)(void);

void JumpToAppDirect(void)
{
    uint32_t app_addr = 0x00010000;
    uint32_t jump_addr;
    pFunction Jump_To_Application;

    // Check if application is present (check reset vector)
    if(((*(__IO uint32_t*)app_addr) & 0x2FFE0000) == 0x20000000)
    {
        // Disable all interrupts
        __disable_irq();

        // Set main stack pointer
        __set_MSP(*(__IO uint32_t*)app_addr);

        // Get reset handler address
        jump_addr = *(__IO uint32_t*)(app_addr + 4);
        Jump_To_Application = (pFunction)jump_addr;

        // Jump
        Jump_To_Application();
    }
}
```

## IAP Receive and Program

```c
// Receive firmware data via UART and write to flash
void IAP_UART_Receive(void)
{
    uint32_t flash_addr = 0x00010000;  // Application start
    uint8_t page_buf[256];  // 256 bytes at a time
    uint16_t page_offset = 0;

    FLASH_Unlock();

    while(1)
    {
        // Receive data from UART
        uint8_t byte = USART1_RecvByte();
        page_buf[page_offset++] = byte;

        if(page_offset >= 256)
        {
            // Erase page if needed (4KB boundary)
            if((flash_addr & 0xFFF) == 0)
            {
                FLASH_ErasePage(flash_addr);
            }

            // Program page
            for(int i = 0; i < 256; i += 4)
            {
                uint32_t word = *(uint32_t*)&page_buf[i];
                FLASH_ProgramWord(flash_addr + i, word);
            }

            flash_addr += 256;
            page_offset = 0;

            // Send ACK
            USART1_SendByte(0x06);
        }

        // Check for end of transfer
        // (implementation depends on protocol)
    }

    FLASH_Lock();
}
```

## OTA via Ethernet Example

```c
// Pseudo-code for OTA over TCP/IP
void OTA_Ethernet(void)
{
    // 1. Connect to update server
    // 2. Download firmware binary
    // 3. Verify checksum (CRC32)
    // 4. Erase application flash region
    // 5. Program new firmware
    // 6. Verify written data
    // 7. Switch boot mode and reset

    uint32_t crc = CalculateCRC32(firmware_data, firmware_len);
    if(crc == expected_crc)
    {
        FLASH_Unlock();

        // Erase application region
        for(uint32_t addr = 0x10000; addr < 0x80000; addr += 0x1000)
        {
            FLASH_ErasePage(addr);
        }

        // Program new firmware
        for(uint32_t i = 0; i < firmware_len; i += 4)
        {
            FLASH_ProgramWord(0x10000 + i, firmware_data[i/4]);
        }

        FLASH_Lock();

        // Switch to new application
        JumpToBootloader();  // Or directly jump to app
    }
}
```

## Option Byte Configuration

```c
// Configure bootloader download interfaces
void IAP_ConfigureDownload(void)
{
    FLASH_Unlock();
    FLASH_OB_Unlock();

    // Enable USB download and UART download in bootloader
    FLASH_UserOptionByteConfig(OB_IWDG_SW, OB_USBFSDL_EN, OB_USARTDL_EN);

    FLASH_OB_Lock();
    FLASH_Lock();
}
```

## Flash Protection

```c
// Enable write protection on application region
void IAP_ProtectFlash(void)
{
    FLASH_Unlock();

    // Protect sectors 4-31 (application region)
    FLASH_EnableWriteProtection(FLASH_WRProt_Sectors4 | FLASH_WRProt_Sectors5 | /* ... */);

    FLASH_Lock();
}
```

## CRC Verification

```c
#include "ch32h417_crc.h"

uint32_t CalculateAppCRC(uint32_t start_addr, uint32_t length)
{
    CRC_ResetDR();
    for(uint32_t i = 0; i < length; i += 4)
    {
        uint32_t word = *(__IO uint32_t*)(start_addr + i);
        CRC_CalcCRC(word);
    }
    return CRC_GetCRC();
}
```

## Best Practices

1. Always verify firmware integrity (CRC/checksum) before programming
2. Keep a backup of the working firmware if possible
3. Implement a watchdog timeout to recover from failed updates
4. Use the bootloader's built-in download support when available
5. Document the communication protocol for your OTA implementation
