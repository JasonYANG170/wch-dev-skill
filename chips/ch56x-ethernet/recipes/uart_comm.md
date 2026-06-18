# Recipe: UART Communication

## Overview

Configure and use UART0-3 for serial communication on CH56x chips.
All four UARTs support configurable baud rate, FIFO, byte-trigger interrupts,
and multi-byte send/receive.

## Available UARTs

| UART | Default Pins | Notes |
|------|-------------|-------|
| UART0 | TX=PA9, RX=PA10 | Also used for printf debug output |
| UART1 | TX=PB4, RX=PB7 | |
| UART2 | TX=PB0, RX=PB1 | |
| UART3 | TX=PB2, RX=PB3 | |

(Pin assignments may vary by chip variant and pin remap configuration.)

## Quick Start: UART0 Default Init

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    // Enable UART0 peripheral clock
    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);

    // Default init: 115200 baud, 8N1
    UART0_DefInit();

    printf("UART0 ready\n");

    while(1) {}
}
```

## Baud Rate Configuration

```c
// Configure custom baud rate
UART0_BaudRateCfg(9600);
UART0_BaudRateCfg(115200);
UART0_BaudRateCfg(460800);
UART0_BaudRateCfg(1000000);  // 1Mbps

// Must call DefInit() first, then BaudRateCfg() to change
```

## Sending Data

```c
// Single byte (macro - direct register write)
UART0_SendByte('A');

// Multi-byte string
UINT8 buf[] = "Hello CH569\r\n";
UART0_SendString(buf, sizeof(buf) - 1);

// Printf (routed to debug UART, default UART0)
printf("Value: %d\n", myValue);
```

## Receiving Data

```c
// Single byte (macro - direct register read)
UINT8 byte = UART0_RecvByte();

// Multi-byte (returns number of bytes received)
UINT8 rxBuf[64];
UINT16 len = UART0_RecvString(rxBuf);
```

## FIFO and Status

```c
// Clear FIFOs
UART0_CLR_RXFIFO();
UART0_CLR_TXFIFO();

// Check line status
UINT8 status = UART0_GetLinSTA();
if (status & STA_RECV_DATA) {
    // Data available in RX FIFO
}
if (status & STA_TXFIFO_EMP) {
    // TX FIFO empty, can send more
}
if (status & STA_TXALL_EMP) {
    // All TX data has been sent
}

// Check for errors
if (status & STA_ERR_FRAME) { /* Frame error */ }
if (status & STA_ERR_PAR)   { /* Parity error */ }
if (status & STA_ERR_FIFOOV){ /* RX overflow */ }
if (status & STA_ERR_BREAK) { /* Break condition */ }
```

## Byte Trigger Interrupts

```c
// Configure interrupt trigger level
UART0_ByteTrigCfg(UART_1BYTE_TRIG);  // Interrupt on 1 byte
UART0_ByteTrigCfg(UART_2BYTE_TRIG);  // Interrupt on 2 bytes
UART0_ByteTrigCfg(UART_4BYTE_TRIG);  // Interrupt on 4 bytes
UART0_ByteTrigCfg(UART_7BYTE_TRIG);  // Interrupt on 7 bytes

// Enable interrupt
// s: ENABLE/DISABLE
// i: interrupt type (refer to UART interrupt defines)
UART0_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
```

## Using Other UARTs

The API is identical for UART1, UART2, UART3:

```c
// UART1
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART1);
UART1_DefInit();
UART1_SendByte('X');
UINT8 b = UART1_RecvByte();

// UART2
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART2);
UART2_DefInit();

// UART3
PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART3);
UART3_DefInit();
```

## Software Reset

```c
UART0_Reset();  // Software reset UART0
```

## Complete Example: UART Echo

```c
#include "CH56x_common.h"

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_120MHz);
    Delay_Init(GetSysClock());

    PWR_PeriphClkCfg(ENABLE, BIT_SLP_CLK_UART0);
    UART0_DefInit();

    printf("UART0 echo mode - type something:\n");

    while(1)
    {
        // Check if data is available
        if (UART0_GetLinSTA() & STA_RECV_DATA) {
            UINT8 ch = UART0_RecvByte();
            UART0_SendByte(ch);  // Echo back
        }
    }
}
```

## Printf Redirect

To redirect `printf()` to a different UART, modify `CH56x_common.h`:

```c
#define Debug_UART1   1   // Route printf to UART1
```

Or implement `_write()` for custom output routing.
