# Recipe: IAP (In-Application Programming) Bootloader

## When to Use
User wants to implement a bootloader for firmware update via UART, USB, or BLE OTA.

## Overview
IAP allows updating firmware without a physical programmer. The system has:
- **Bootloader**: Runs first, checks for update request, loads application
- **Application**: Normal firmware, can trigger bootloader for update

## Memory Layout

```
Flash Layout (example for CH32V20x with 256KB):
0x00000000 +-------------------+
           | Bootloader (16KB) |
0x00004000 +-------------------+
           | Application A     |
           | (112KB)           |
0x00020000 +-------------------+
           | Application B     |
           | (112KB)           |
0x0003C000 +-------------------+
           | Config/Flag (16KB)|
0x00040000 +-------------------+
```

## Bootloader Linker Script

```ld
/* Bootloader: starts at 0x00000000, limited size */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

## Application Linker Script

```ld
/* Application: starts after bootloader */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 112K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

## Example: Simple UART Bootloader

```c
#include "ch32v20x.h"
#include <string.h>

#define APP_START_ADDR   0x00004000
#define FLAG_ADDR        0x0003C000
#define UPDATE_MAGIC     0x55AA55AA

typedef void (*pFunction)(void);

void Jump_To_App(uint32_t app_addr)
{
    uint32_t jump_addr;
    pFunction Jump_To_Application;

    if((*(volatile uint32_t *)app_addr) & 0x2FFE0000 == 0x20000000)
    {
        // Disable all interrupts
        __disable_irq();

        // Set main stack pointer
        __set_MSP(*(volatile uint32_t *)app_addr);

        // Get reset handler address
        jump_addr = *(volatile uint32_t *)(app_addr + 4);
        Jump_To_Application = (pFunction)jump_addr;

        // Jump to application
        Jump_To_Application();
    }
}

void USART1_Init_IAP(uint32_t baudrate)
{
    // USART1 init for IAP communication
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);
}

uint8_t USART1_RecvByte(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (uint8_t)USART_ReceiveData(USART1);
}

void IAP_EraseApp(void)
{
    FLASH_Unlock();
    for(uint32_t addr = APP_START_ADDR; addr < FLAG_ADDR; addr += 4096)
    {
        FLASH_ErasePage(addr);
    }
    FLASH_Lock();
}

void IAP_ProgramData(uint32_t addr, uint8_t *data, uint32_t len)
{
    FLASH_Unlock();
    for(uint32_t i = 0; i < len; i += 4)
    {
        uint32_t word = *(uint32_t *)(data + i);
        FLASH_ProgramWord(addr + i, word);
    }
    FLASH_Lock();
}

int main(void)
{
    uint32_t flag = *(volatile uint32_t *)FLAG_ADDR;

    // Check if application exists and no update requested
    if(flag != UPDATE_MAGIC)
    {
        if((*(volatile uint32_t *)APP_START_ADDR) & 0x2FFE0000 == 0x20000000)
        {
            Jump_To_App(APP_START_ADDR);
        }
    }

    // Enter IAP mode
    USART1_Init_IAP(115200);

    while(1)
    {
        uint8_t cmd = USART1_RecvByte();

        switch(cmd)
        {
            case 0x01:  // Erase
                IAP_EraseApp();
                USART_SendData(USART1, 0x06);  // ACK
                break;

            case 0x02:  // Program
            {
                uint32_t addr = APP_START_ADDR;
                uint32_t len;
                uint8_t buf[256];

                // Receive address
                addr = USART1_RecvByte() << 24;
                addr |= USART1_RecvByte() << 16;
                addr |= USART1_RecvByte() << 8;
                addr |= USART1_RecvByte();

                // Receive length
                len = USART1_RecvByte();

                // Receive data
                for(uint32_t i = 0; i < len; i++)
                    buf[i] = USART1_RecvByte();

                IAP_ProgramData(addr, buf, len);
                USART_SendData(USART1, 0x06);  // ACK
                break;
            }

            case 0x03:  // Jump to app
            {
                // Clear update flag
                FLASH_Unlock();
                FLASH_ErasePage(FLAG_ADDR);
                FLASH_Lock();

                Jump_To_App(APP_START_ADDR);
                break;
            }
        }
    }
}
```

## Example: Application That Can Trigger Update

```c
// In application code
#define FLAG_ADDR       0x0003C000
#define UPDATE_MAGIC    0x55AA55AA

void Request_Bootloader_Update(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(FLAG_ADDR);
    FLASH_ProgramWord(FLAG_ADDR, UPDATE_MAGIC);
    FLASH_Lock();

    // Reset to enter bootloader
    NVIC_SystemReset();
}
```

## Pitfalls
- **Application linker script must offset** -- `FLASH ORIGIN = 0x00004000` (not 0x00000000)
- **Vector table relocation** -- application must set VTOR to its start address
- **Disable interrupts before jump** -- bootloader must disable all IRQs before jumping to app
- **Verify stack pointer** -- check that the first word at app_addr looks like a valid stack pointer
- **Flash erase before write** -- always erase the application area before programming new firmware
- **Atomic flag operations** -- use Flash operations for update flags; don't use RAM (lost on reset)
