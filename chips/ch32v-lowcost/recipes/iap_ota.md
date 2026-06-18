# Recipe: IAP (In-Application Programming) Bootloader

## Scenario
Implement a bootloader for firmware updates via UART, with application running from offset Flash address.

## Memory Layout

### Standard (no IAP)
```
0x00000000 +------------------+
           | User Application |
           | (Full Flash)     |
0x00003FFF +------------------+
```

### With IAP Bootloader (CH32V003, 16K Flash)
```
0x00000000 +------------------+
           | Bootloader       |
           | (4K = 0x1000)    |
0x00000FFF +------------------+
           | User Application |
           | (12K)            |
0x00003FFF +------------------+
```

### With IAP Bootloader (CH32V006, 62K Flash)
```
0x00000000 +------------------+
           | Bootloader       |
           | (4K = 0x1000)    |
0x00000FFF +------------------+
           | User Application |
           | (58K)            |
0x0000F7FF +------------------+
```

## Key Functions

```c
// Flash operations
FLASH_Status FLASH_Unlock(void);
FLASH_Status FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);

// System reset
void SystemReset_StartMode(uint32_t Mode);  // Start_Mode_BOOT or Start_Mode_USER

// Option byte
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STDBY,
                                         uint16_t OB_RST, uint16_t OB_PowerON_Start_Mode);
```

## Complete Call Chains

### Application Linker Script (offset by 4K)

Modify `Link.ld` for the application:
```
ENTRY( _start )
__stack_size = 256;
PROVIDE( _stack_size = __stack_size );

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 12K  /* Offset by 4K */
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}

/* ... rest of SECTIONS same as default ... */
```

### Bootloader (UART-based)

```c
#include "debug.h"

#define APP_START_ADDR  0x00001000
#define APP_MAX_SIZE    (12 * 1024)

typedef void (*pFunction)(void);

void Jump_To_Application(void)
{
    uint32_t JumpAddress;
    pFunction Jump_To_Application;

    // Check if application exists (first word should be stack pointer)
    if(((*(__IO uint32_t *)APP_START_ADDR) & 0x2FFE0000) == 0x20000000)
    {
        printf("Jumping to application...\r\n");

        // Set vector table offset
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_START_ADDR);

        // Jump
        JumpAddress = *(__IO uint32_t *)(APP_START_ADDR + 4);
        Jump_To_Application = (pFunction)JumpAddress;

        // Set stack pointer
        __set_MSP(*(__IO uint32_t *)APP_START_ADDR);

        Jump_To_Application();
    }
    else
    {
        printf("No valid application found\r\n");
    }
}

void Bootloader_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

uint8_t Bootloader_ReceiveByte(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}

void Bootloader_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

FLASH_Status Bootloader_Program_Firmware(void)
{
    uint32_t addr = APP_START_ADDR;
    uint32_t size;
    uint32_t i;
    FLASH_Status status;
    uint8_t buf[4];

    // Receive firmware size (4 bytes, little-endian)
    for(i = 0; i < 4; i++)
        buf[i] = Bootloader_ReceiveByte();
    size = *(uint32_t *)buf;

    if(size > APP_MAX_SIZE)
    {
        printf("Firmware too large: %d bytes\r\n", size);
        return FLASH_ERROR_WRP;
    }

    printf("Receiving %d bytes...\r\n", size);

    // Unlock and erase
    FLASH_Unlock();
    for(i = 0; i < size; i += 64)  // 64-byte pages for CH32V003
    {
        status = FLASH_ErasePage(APP_START_ADDR + i);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return status;
        }
    }

    // Receive and program
    for(i = 0; i < size; i += 2)
    {
        buf[0] = Bootloader_ReceiveByte();
        buf[1] = Bootloader_ReceiveByte();
        status = FLASH_ProgramHalfWord(addr + i, *(uint16_t *)buf);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return status;
        }
        Bootloader_SendByte(0x06);  // ACK
    }

    FLASH_Lock();
    printf("Programming complete\r\n");
    return FLASH_COMPLETE;
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    Bootloader_UART_Init();

    printf("CH32V003 Bootloader v1.0\r\n");

    // Check for update command (e.g., button press or UART command)
    uint8_t cmd = 0;
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        cmd = USART_ReceiveData(USART1);
    }

    if(cmd == 'U')  // 'U' for update
    {
        printf("Entering update mode...\r\n");
        Bootloader_SendByte('K');  // ACK
        Bootloader_Program_Firmware();
    }

    // Jump to application
    Jump_To_Application();

    while(1);  // Should never reach here
}
```

### Application Code (for IAP-enabled project)

```c
#include "debug.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("Application running at 0x1000\r\n");

    while(1)
    {
        // Normal application logic
        Delay_Ms(1000);
    }
}
```

### Option Byte for Boot Mode

```c
// Configure to boot from bootloader after reset
void Set_Boot_From_Bootloader(void)
{
    FLASH_Unlock();
    FLASH_EraseOptionBytes();
    FLASH_UserOptionByteConfig(
        OB_IWDG_SW,
        OB_STDBY_NoRST,
        OB_RST_EN_DT12ms,
        OB_PowerON_Start_Mode_BOOT  // Boot from bootloader
    );
    FLASH_Lock();
}

// Configure to boot from user code
void Set_Boot_From_User(void)
{
    FLASH_Unlock();
    FLASH_EraseOptionBytes();
    FLASH_UserOptionByteConfig(
        OB_IWDG_SW,
        OB_STDBY_NoRST,
        OB_RST_EN_DT12ms,
        OB_PowerON_Start_Mode_USER  // Boot from user code
    );
    FLASH_Lock();
}
```

## Common Errors

1. **Application linker script not offset** -- Must start at 0x1000, not 0x0000
2. **Vector table not remapped** -- `NVIC_SetVectorTable()` needed in application
3. **Bootloader too large** -- Keep bootloader under 4K (0x1000)
4. **Flash page size wrong** -- CH32V003 uses 64-byte pages
5. **No ACK mechanism** -- Host must wait for ACK before sending next chunk
6. **Jump fails** -- Check first word of app is valid stack pointer (0x2000xxxx)
