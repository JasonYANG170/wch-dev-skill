# IAP (In-Application Programming) Bootloader

> **Summary**: Implement IAP bootloader and firmware update mechanism on CH32X/CH6xx chips.

## Trigger Intent

- "IAP bootloader"
- "OTA update"
- "Firmware update"
- "Bootloader"
- "Jump to application"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` |
| Reference | `CH32X035EVT/EVT/EXAM/IAP/` |

## Step-by-Step

### Memory Layout for IAP

```
CH32X035 (62KB Flash):
  0x00000000 - 0x00000FFF : Bootloader (4KB)
  0x00001000 - 0x0000F7FF : Application (60KB)

CH32X315/CH643 (192KB Flash):
  0x00000000 - 0x00000FFF : Bootloader (4KB)
  0x00001000 - 0x0002FFFF : Application (~188KB)

CH641 (16KB Flash):
  0x00000000 - 0x00000FFF : Bootloader (4KB)
  0x00001000 - 0x00003BFF : Application (~11KB)
```

### Bootloader Linker Script

```
/* Bootloader: starts at 0x00000000, uses 4KB */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

### Application Linker Script

```
/* Application: starts at 0x00001000 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 60K  /* CH32X035 */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}
```

### Bootloader Implementation

```c
#include "ch32x035.h"
#include "debug.h"

#define APP_START_ADDR   0x00001000

typedef void (*pFunction)(void);

void JumpToApp(void) {
    uint32_t jump_addr;
    pFunction jump_to_app;

    // Check if application exists (first word should be stack pointer)
    if((*(uint32_t *)APP_START_ADDR) & 0x2FFE0000 == 0x20000000) {
        // Disable all interrupts
        __disable_irq();

        // Set stack pointer
        __set_MSP(*(uint32_t *)APP_START_ADDR);

        // Get reset handler address
        jump_addr = *(uint32_t *)(APP_START_ADDR + 4);
        jump_to_app = (pFunction)jump_addr;

        // Jump to application
        jump_to_app();
    }
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("IAP Bootloader\r\n");

    // Check if we should enter bootloader mode
    // (e.g., check a flag in Flash, or a button press)
    uint8_t enter_iap = Check_IAP_Flag();

    if(!enter_iap) {
        printf("Jumping to app...\r\n");
        JumpToApp();
    }

    // Bootloader mode - wait for firmware update via UART/USB
    printf("Waiting for firmware update...\r\n");

    while(1) {
        // Receive firmware data and program to Flash
        if(/* new firmware received */) {
            Program_Firmware();
            Clear_IAP_Flag();
            NVIC_SystemReset();
        }
    }
}
```

### UART IAP Firmware Receive

```c
#define FLASH_PAGE_SIZE   256

uint8_t rx_packet[FLASH_PAGE_SIZE + 8];  // Data + header

void IAP_ProgramFirmware(void) {
    uint32_t addr = APP_START_ADDR;
    uint32_t total_size = 0;
    uint32_t received = 0;

    // Protocol: [size(4)] [data(N)] [checksum(4)]

    // Receive size
    for(int i = 0; i < 4; i++) {
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        rx_packet[i] = USART_ReceiveData(USART1);
    }
    total_size = *(uint32_t *)rx_packet;

    FLASH_Unlock();

    while(received < total_size) {
        // Receive one page
        uint32_t chunk = total_size - received;
        if(chunk > FLASH_PAGE_SIZE) chunk = FLASH_PAGE_SIZE;

        for(uint32_t i = 0; i < chunk; i++) {
            while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
            rx_packet[i] = USART_ReceiveData(USART1);
        }

        // Erase page
        FLASH_ErasePage(addr);

        // Program words
        for(uint32_t i = 0; i < chunk; i += 4) {
            uint32_t word = *(uint32_t *)(rx_packet + i);
            FLASH_ProgramWord(addr + i, word);
        }

        addr += FLASH_PAGE_SIZE;
        received += chunk;

        // Send ACK
        USART_SendData(USART1, 0x06);  // ACK
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }

    FLASH_Lock();
    printf("Flash complete: %d bytes\r\n", total_size);
}
```

### Using SystemReset_StartMode

```c
// CH32X035 provides a built-in mechanism to switch between
// bootloader and application mode

void EnterBootloader(void) {
    SystemReset_StartMode(Start_Mode_BOOT);
    NVIC_SystemReset();
}

void EnterApplication(void) {
    SystemReset_StartMode(Start_Mode_USER);
    NVIC_SystemReset();
}
```

### Application Side: Jump Back to Bootloader

```c
// In application, to request firmware update:
void RequestUpdate(void) {
    // Set a flag in Flash to tell bootloader to stay in IAP mode
    FLASH_Unlock();
    FLASH_ErasePage(IAP_FLAG_ADDR);
    FLASH_ProgramWord(IAP_FLAG_ADDR, 0x55AA55AA);  // Magic flag
    FLASH_Lock();

    // Reset into bootloader
    NVIC_SystemReset();
}
```

## IAP Protocol Summary

| Step | Action | Description |
|------|--------|-------------|
| 1 | Bootloader starts | Check flag or button |
| 2 | If no update needed | Jump to application |
| 3 | If update needed | Wait for data on UART/USB |
| 4 | Receive firmware | Program page by page |
| 5 | Verify | Check CRC or magic bytes |
| 6 | Reset | Jump to new application |

## Common Errors

- Application linker script starts at 0x00000000 -- overwrites bootloader
- Not disabling interrupts before jumping to application -- hard fault
- Stack pointer check wrong -- jumps to invalid address
- Flash not unlocked before erase/program -- silent failure
- Page alignment wrong -- data corruption
- Not setting `SystemReset_StartMode()` before reset -- stays in bootloader forever
