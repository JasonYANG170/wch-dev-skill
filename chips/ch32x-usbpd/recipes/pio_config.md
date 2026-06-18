# PIOC (Port I/O Controller) Configuration

> **Summary**: Configure and use the PIOC programmable I/O controller on CH32X035/CH643 chips for custom serial protocols, UART emulation, I2C bit-banging, NEC IR, and single-wire communication.

## Trigger Intent

- "PIOC configuration"
- "Programmable I/O controller"
- "PIOC UART"
- "PIOC I2C"
- "Custom serial protocol"
- "PIO controller setup"

## Prerequisites

| Condition | Requirement |
|---|---|
| Header | `ch32x035.h` + `PIOC_SFR.h` |
| Clock | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE)` |
| Pins | PC18 (RX/IO0), PC19 (TX/IO1) for UART mode |
| Reference | `CH32X035EVT/EVT/EXAM/PIOC/` |

## PIOC Overview

PIOC is a programmable microcontroller core embedded in CH32X035/CH643. It runs custom microcode from SRAM and can implement flexible I/O protocols. Key features:

- Independent RISC core running from PIOC SRAM
- Programmable I/O pins (IO0, IO1) mapped to GPIO pins
- Double-buffered data registers for streaming TX/RX
- Interrupt-driven data transfer to/from main CPU

## Step-by-Step

### PIOC Initialization

```c
#include "ch32x035.h"
#include "debug.h"
#include "PIOC_SFR.h"
#include "string.h"

// PIOC microcode (pre-compiled for UART protocol)
__attribute__((aligned(16))) const unsigned char PIOC_CODE[] = {
    // ... microcode bytes from PIOC tool ...
};

void PIOC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Enable GPIOC and AFIO clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    // PC19 = PIOC TX (alternate function push-pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_19;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // PC18 = PIOC RX (input pull-up)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_18;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Enable PIOC interrupt
    NVIC_EnableIRQ(PIOC_IRQn);
    NVIC_SetPriority(PIOC_IRQn, 0xf0);

    // Load microcode into PIOC SRAM
    memcpy((uint8_t *)(PIOC_SRAM_BASE), PIOC_CODE, sizeof(PIOC_CODE));

    // Reset PIOC, enable IO0 & IO1, enable clock
    R8_SYS_CFG |= RB_MST_RESET;
    R8_SYS_CFG = RB_MST_IO_EN1 | RB_MST_IO_EN0;
    R8_SYS_CFG |= RB_MST_CLK_GATE;
}
```

### PIOC UART Configuration

```c
// Baud rate register values at 48MHz system clock
// Supports: 921600, 460800, 230400, 115200, 57600, 38400, 19200, 9600, etc.
void PIOC_UART_Init(uint32_t baudrate, uint8_t parity, uint8_t stopbits, uint8_t datalen) {
    // Reset and enable PIOC
    R8_SYS_CFG |= RB_MST_RESET;
    R8_SYS_CFG = RB_MST_IO_EN1 | RB_MST_IO_EN0;
    R8_SYS_CFG |= RB_MST_CLK_GATE;

    // Set baud rate (pre-computed register values)
    switch(baudrate) {
        case 921600: R32_DATA_REG0_3 = 0x5008F525; R32_DATA_REG4_7 = 0x0080FF25; break;
        case 460800: R32_DATA_REG0_3 = 0xB00FE825; R32_DATA_REG4_7 = 0x0010FC25; break;
        case 230400: R32_DATA_REG0_3 = 0xE01CCE25; R32_DATA_REG4_7 = 0x0070F625; break;
        case 115200: R32_DATA_REG0_3 = 0xE0369A25; R32_DATA_REG4_7 = 0x0070E925; break;
        case  57600: R32_DATA_REG0_3 = 0xA06A3225; R32_DATA_REG4_7 = 0x0070CF25; break;
        case  38400: R32_DATA_REG0_3 = 0xF04F6524; R32_DATA_REG4_7 = 0x0070B525; break;
        case  19200: R32_DATA_REG0_3 = 0x704F6423; R32_DATA_REG4_7 = 0x00306725; break;
        case   9600: R32_DATA_REG0_3 = 0x0027B222; R32_DATA_REG4_7 = 0x00006524; break;
        default: break;
    }

    // Set parity: 0=None, 1=Odd, 2=Even, 3=Mark, 4=Space
    switch(parity) {
        case 1: R8_DATA_REG3 |= 0x0C; R8_DATA_REG6 |= 0x03; break;
        case 2: R8_DATA_REG3 |= 0x04; R8_DATA_REG6 |= 0x02; break;
        case 3: R8_DATA_REG3 |= 0x0C; R8_DATA_REG6 |= 0x03; R8_DATA_REG7 |= 0x10; break;
        case 4: R8_DATA_REG3 |= 0x04; R8_DATA_REG6 |= 0x02; R8_DATA_REG7 |= 0x10; break;
        default: break;
    }

    // Set stop bits: 0=1, 1=1.5, 2=2
    switch(stopbits) {
        case 1: R8_DATA_REG3 |= 0x01; break;
        case 2: R8_DATA_REG3 |= 0x03; break;
        default: break;
    }

    // Set data length: 5, 6, 7, 8
    switch(datalen) {
        case 5: R8_DATA_REG7 |= 0x20; break;
        case 6: R8_DATA_REG7 |= 0x40; break;
        case 7: R8_DATA_REG7 |= 0x80; break;
        default: break;  // 8 bits (default)
    }
}
```

### PIOC UART Send

```c
#define UART_SFR_ADDR1  ((uint8_t *)&(PIOC->D8_DATA_REG8))
#define UART_SFR_ADDR2  ((uint8_t *)&(PIOC->D8_DATA_REG16))

volatile uint8_t  PIOC_TX_FLAG = 0;
volatile uint16_t PIOC_TX_RemainLEN = 0;
volatile uint8_t *PIOC_TX_ADDR;

void PIOC_UART_Send(uint8_t *data, uint16_t len) {
    PIOC_UART_Init(115200, 0, 0, 8);  // Re-init for TX

    if (len <= 16) {
        memcpy(UART_SFR_ADDR1, data, len);
        PIOC_TX_FLAG = 0;
        PIOC_TX_RemainLEN = 0;
    } else {
        memcpy(UART_SFR_ADDR1, data, 16);
        PIOC_TX_FLAG = 1;
        PIOC_TX_RemainLEN = len - 16;
        PIOC_TX_ADDR = data + 16;
    }

    R16_DATA_REG24_25 = len;   // Total byte count
    R8_DATA_REG7 |= 0x01;     // Bit 0 = 1: TX mode
    R8_CTRL_WR = 0x33;        // Start transfer
}
```

### PIOC Interrupt Handler

```c
volatile uint8_t rx_buf[100];
volatile uint8_t rx_count = 0;

void PIOC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PIOC_IRQHandler(void) {
    if ((R8_SYS_CFG & RB_INT_REQ) != RESET) {
        if ((R8_DATA_REG7 & 0x01) == 0) {
            // RX mode: read received byte
            rx_buf[rx_count++] = R8_DATA_REG31;
        } else {
            // TX mode: double-buffered, fill next buffer
            if (PIOC_TX_FLAG == 1) {
                if (PIOC_TX_RemainLEN <= 8) {
                    memcpy(UART_SFR_ADDR1, PIOC_TX_ADDR, PIOC_TX_RemainLEN);
                    PIOC_TX_FLAG = 0;
                } else {
                    memcpy(UART_SFR_ADDR1, PIOC_TX_ADDR, 8);
                    PIOC_TX_ADDR += 8;
                    PIOC_TX_RemainLEN -= 8;
                    PIOC_TX_FLAG = 2;
                }
            } else if (PIOC_TX_FLAG == 2) {
                if (PIOC_TX_RemainLEN <= 8) {
                    memcpy(UART_SFR_ADDR2, PIOC_TX_ADDR, PIOC_TX_RemainLEN);
                    PIOC_TX_FLAG = 0;
                } else {
                    memcpy(UART_SFR_ADDR2, PIOC_TX_ADDR, 8);
                    PIOC_TX_ADDR += 8;
                    PIOC_TX_RemainLEN -= 8;
                    PIOC_TX_FLAG = 1;
                }
            }
        }
        R8_CTRL_RD = 11;  // Clear interrupt flag (write any value)
    }
}
```

### PIOC Pin Remap

```c
// Enable PIOC function on GPIO pins
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_PIOC, ENABLE);

// Disable SWJ to free PC18/PC19
GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
```

### Available PIOC Examples

| Example | Directory | Description |
|---|---|---|
| PIOC UART | `PIOC/PIOC_UART/` | Half-duplex UART via PIOC |
| PIOC I2C | `PIOC/PIOC_IIC/` | I2C bit-bang via PIOC |
| PIOC NEC | `PIOC/PIOC_NEC/` | NEC IR remote protocol |
| PIOC Single Wire | `PIOC/PIOC_Single_Wire/` | Single-wire bidirectional |
| 1-Wire | `PIOC/1_Wire/` | Dallas 1-Wire protocol |

## PIOC SFR Map

| Register | Purpose |
|---|---|
| `R8_SYS_CFG` | System config: reset, IO enable, clock gate, interrupt flag |
| `R8_DATA_REG0` - `R8_DATA_REG31` | General-purpose data registers |
| `R32_DATA_REG0_3` | 32-bit access to REG0-3 (baud rate config) |
| `R32_DATA_REG4_7` | 32-bit access to REG4-7 (baud rate config) |
| `R8_DATA_REG7` | Mode select (bit 0: 0=RX, 1=TX) and data length |
| `R8_CTRL_WR` | Write to start PIOC operation |
| `R8_CTRL_RD` | Write to clear interrupt flag |
| `PIOC_SRAM_BASE` | Base address for PIOC microcode |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| PIOC not running | Clock gate not enabled | Set `RB_MST_CLK_GATE` in `R8_SYS_CFG` |
| No TX output | PC19 not configured as AF_PP | Use `GPIO_Mode_AF_PP` for TX pin |
| No RX data | SWJ not disabled | Call `GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE)` |
| Wrong baud rate | System clock mismatch | Baud rate register values assume 48MHz; adjust for other clocks |
| Microcode not loaded | PIOC not reset before loading | Reset PIOC (`RB_MST_RESET`) before `memcpy` to SRAM |
