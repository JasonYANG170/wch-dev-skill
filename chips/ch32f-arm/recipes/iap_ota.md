# Recipe: IAP (In-Application Programming)

## Overview

Implement bootloader and application firmware update mechanism on CH32F10x/CH32F20x.

## Flash Layout for IAP

### CH32F10x (64KB Flash example)
| Region | Address | Size | Description |
|--------|---------|------|-------------|
| Bootloader | 0x08000000 | 4KB | IAP bootloader |
| Application | 0x08001000 | 60KB | User application |

### CH32F20x (128KB Flash example)
| Region | Address | Size | Description |
|--------|---------|------|-------------|
| Bootloader | 0x08000000 | 8KB | IAP bootloader |
| Application | 0x08002000 | 120KB | User application |

## IAP Workflow

1. Bootloader starts at reset (0x08000000)
2. Bootloader checks for update request (flag in Flash/backup register)
3. If update requested: receive new firmware via UART/USB/CAN and write to Flash
4. If no update: jump to application (0x08001000 or 0x08002000)
5. Application can set update flag and reset to enter bootloader

## Key API Functions

```c
// Flash operations for IAP
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

// Fast Flash operations (CH32F10x)
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_BufReset(void);
void FLASH_BufLoad(uint32_t Address, uint32_t Data0, uint32_t Data1, uint32_t Data2, uint32_t Data3);
void FLASH_ErasePage_Fast(uint32_t Page_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address);

// ROM operations (CH32F20x)
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
```

## Example: Bootloader

```c
#include "debug.h"

#define APP_START_ADDR      0x08001000
#define UPDATE_FLAG_ADDR    0x0800FC00  // Last page of Flash
#define UPDATE_FLAG_MAGIC   0x55504441  // "UPDA"

typedef void (*pFunction)(void);

void JumpToApp(uint32_t appAddr)
{
    uint32_t jumpAddress;
    pFunction Jump_To_Application;

    // Check if application exists
    if(((*(__IO uint32_t*)appAddr) & 0x2FFE0000) == 0x20000000)
    {
        // Disable all interrupts
        __disable_irq();

        // Set main stack pointer
        __set_MSP(*(__IO uint32_t*)appAddr);

        // Get reset handler address
        jumpAddress = *(__IO uint32_t*)(appAddr + 4);
        Jump_To_Application = (pFunction)jumpAddress;

        // Jump to application
        Jump_To_Application();
    }
}

uint8_t CheckUpdateFlag(void)
{
    uint32_t* flag = (uint32_t*)UPDATE_FLAG_ADDR;
    return (*flag == UPDATE_FLAG_MAGIC) ? 1 : 0;
}

void ClearUpdateFlag(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(UPDATE_FLAG_ADDR);
    FLASH_Lock();
}

void SetUpdateFlag(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(UPDATE_FLAG_ADDR);
    FLASH_ProgramWord(UPDATE_FLAG_ADDR, UPDATE_FLAG_MAGIC);
    FLASH_Lock();
}

uint8_t Flash_WriteApp(uint32_t addr, uint8_t* data, uint32_t len)
{
    FLASH_Status status;

    FLASH_Unlock();

    // Erase pages
    for(uint32_t i = 0; i < len; i += 1024)
    {
        status = FLASH_ErasePage(addr + i);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return 1;
        }
    }

    // Write data (half-word by half-word)
    for(uint32_t i = 0; i < len; i += 2)
    {
        uint16_t halfword = data[i] | (data[i+1] << 8);
        status = FLASH_ProgramHalfWord(addr + i, halfword);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return 2;
        }
    }

    FLASH_Lock();
    return 0;
}

// UART receive and program firmware
void IAP_UART_Receive(void)
{
    uint32_t appAddr = APP_START_ADDR;
    uint8_t buffer[1024];
    uint32_t received = 0;
    uint32_t totalSize = 0;

    printf("IAP: Waiting for firmware...\r\n");

    // Receive firmware size (4 bytes)
    for(uint8_t i = 0; i < 4; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        ((uint8_t*)&totalSize)[i] = USART_ReceiveData(USART1);
    }

    printf("IAP: Firmware size: %d bytes\r\n", totalSize);

    // Receive and program firmware
    while(received < totalSize)
    {
        uint32_t chunkSize = (totalSize - received > 1024) ? 1024 : (totalSize - received);

        // Receive chunk
        for(uint32_t i = 0; i < chunkSize; i++)
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
            buffer[i] = USART_ReceiveData(USART1);
        }

        // Write to Flash
        if(Flash_WriteApp(appAddr + received, buffer, chunkSize) != 0)
        {
            printf("IAP: Flash write error!\r\n");
            return;
        }

        received += chunkSize;

        // Send ACK
        USART_SendData(USART1, 0x06);
    }

    printf("IAP: Update complete, jumping to app...\r\n");
    ClearUpdateFlag();
    JumpToApp(APP_START_ADDR);
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("Bootloader started\r\n");

    if(CheckUpdateFlag())
    {
        printf("Update flag detected\r\n");
        IAP_UART_Receive();
    }
    else
    {
        printf("Jumping to application...\r\n");
        JumpToApp(APP_START_ADDR);
    }

    // Should not reach here
    while(1) {}
}
```

## Example: Application (with update capability)

```c
#include "debug.h"

#define APP_START_ADDR      0x08001000
#define UPDATE_FLAG_ADDR    0x0800FC00
#define UPDATE_FLAG_MAGIC   0x55504441

void SetUpdateFlag(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(UPDATE_FLAG_ADDR);
    FLASH_ProgramWord(UPDATE_FLAG_ADDR, UPDATE_FLAG_MAGIC);
    FLASH_Lock();
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("Application started at 0x%08X\r\n", APP_START_ADDR);

    while(1)
    {
        // Check for update command (e.g., from UART)
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            uint8_t cmd = USART_ReceiveData(USART1);
            if(cmd == 'U')  // Update command
            {
                printf("Entering bootloader mode...\r\n");
                SetUpdateFlag();
                NVIC_SystemReset();  // Reset to enter bootloader
            }
        }

        // Normal application logic
        Delay_Ms(100);
    }
}
```

## IAP via USB or CAN

The same principle applies for USB or CAN-based IAP:
1. Bootloader initializes USB/CAN
2. Receives firmware data packets
3. Writes to Flash using the same Flash API
4. Jumps to application

Refer to the official WCH IAP examples in `EVT/EXAM/IAP/` for complete USB and CAN IAP implementations.
