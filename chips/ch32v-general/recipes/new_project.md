# Recipe: Create New CH32V Project

## When to Use
User wants to create a new firmware project for CH32V103, CH32V20x, CH32V307, or CH32V407.

## Prerequisites
- MounRiver Studio installed
- WCH-LinkE debugger connected (for flashing)

## Step-by-Step

### 1. Identify Chip Family and Header

| Chip | Header | Peripheral Prefix |
|------|--------|-------------------|
| CH32V103 | `ch32v10x.h` | `ch32v10x_` |
| CH32V203/CH32V208 | `ch32v20x.h` | `ch32v20x_` |
| CH32V307 | `ch32v30x.h` | `ch32v30x_` |
| CH32V407 | `ch32v4x7.h` | `ch32v4x7_` |

### 2. Copy Closest Example from EVT

The best approach is to copy an existing example project:

```
CH32V20xEVT/EVT/EXAM/GPIO/GPIO_Toggle/     -- simplest GPIO blink
CH32V20xEVT/EVT/EXAM/USART/USART1_Printf/   -- debug UART
CH32V20xEVT/EVT/EXAM/SPI/SPI_DMA/           -- SPI with DMA
CH32V20xEVT/EVT/EXAM/I2C/I2C_EEPROM/        -- I2C EEPROM
CH32V20xEVT/EVT/EXAM/ADC/ADC_DMA/           -- ADC with DMA
CH32V20xEVT/EVT/EXAM/TIM/TIM_PWM/           -- Timer PWM
CH32V20xEVT/EVT/EXAM/CAN/Networking/         -- CAN bus
CH32V20xEVT/EVT/EXAM/ETH/                    -- Ethernet (V307/V407)
CH32V20xEVT/EVT/EXAM/FLASH/FLASH_Program/   -- Flash read/write
CH32V20xEVT/EVT/EXAM/FreeRTOS/FreeRTOS/      -- FreeRTOS integration
CH32V20xEVT/EVT/EXAM/IAP/UART_USB_IAP/      -- IAP bootloader
```

### 3. Project Structure After Copy

```
MyProject/
  User/
    main.c                    # Application code
    ch32v20x_conf.h           # Peripheral driver selection
    ch32v20x_it.c             # Interrupt handlers
    ch32v20x_it.h             # Interrupt declarations
    system_ch32v20x.h         # System config
  SRC/                        # Linked from shared EVT/SRC/
    Peripheral/inc/           # Driver headers
    Peripheral/src/           # Driver source
    Startup/                  # Assembly startup
    RVMSIS/                   # RISC-V core headers
  Ld/Link.ld                  # Linker script
  .project                    # MounRiver project file
  *.wvproj                    # MounRiver workspace project
```

### 4. Minimal main.c Template

```c
#include "ch32v20x.h"  // Change to appropriate chip header

void USART_Printf_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // USART1 TX - PA9
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // USART1 RX - PA10
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

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);

    printf("SystemClk: %d Hz\r\n", SystemCoreClock);
    printf("ChipID: %08x\r\n", DBGMCU_GetCHIPID());

    // Application code here

    while(1)
    {
        // Main loop
    }
}
```

### 5. Build and Flash

1. Open `.wvproj` in MounRiver Studio
2. Project -> Build Project (Ctrl+B)
3. Run -> Debug (WCH-LinkE)
4. Check USART1 output at 115200 baud

## Key Points
- Always copy from the closest existing example -- never start from scratch
- The `SRC/` directory is shared via Eclipse linked resources
- The linker script (`Ld/Link.ld`) defines flash/RAM sizes -- adjust for your chip variant
- `ch32v*_conf.h` controls which peripheral drivers are included
