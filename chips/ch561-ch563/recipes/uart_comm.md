# Recipe: UART Communication (Register-Based)

## Overview

Configure and use UART0 and UART1 for serial communication on CH561/CH563.
All UART access is through direct register manipulation -- there are NO
UART0_DefInit() or UART0_SendByte() library functions.

## Available UARTs

| UART | RX Pin | TX Pin | FIFO Size | Notes |
|------|--------|--------|-----------|-------|
| UART0 | PB8 (RXD0) | PB9 (TXD0) | 16 bytes | Also used for printf |
| UART1 | PB10 (RXD1) | PB11 (TXD1) | 32 bytes | Supports IrDA |

## UART Register Map

| Register | Offset | R/W | Description |
|----------|--------|-----|-------------|
| `R8_UARTx_RBR` | 0 | RO | Receiver buffer (read) |
| `R8_UARTx_THR` | 0 | WO | Transmitter holding (write) |
| `R8_UARTx_IER` | 1 | RW | Interrupt enable |
| `R8_UARTx_IIR` | 2 | RO | Interrupt identification |
| `R8_UARTx_FCR` | 2 | WO | FIFO control |
| `R8_UARTx_LCR` | 3 | RW | Line control |
| `R8_UARTx_MCR` | 4 | RW | Modem control |
| `R8_UARTx_LSR` | 5 | RO | Line status |
| `R8_UARTx_MSR` | 6 | RO | Modem status |
| `R8_UARTx_DIV` | 7 | RW | Pre-divisor latch |
| `R8_UARTx_DLL` | 0 | RW | Divisor latch LSB |
| `R8_UARTx_DLM` | 1 | RW | Divisor latch MSB |

## Baud Rate Calculation

```
Baud Rate = FREQ_SYS / (8 * DIV * (DLM:DLL))
DIV = R8_UARTx_DIV (pre-divisor, 1-128)
DLM:DLL = 16-bit divisor latch
```

For 115200 baud at 100MHz:
```
divisor = 10 * FREQ_SYS * 2 / 16 / 115200
divisor = (divisor + 5) / 10  // Round
```

## UART0 Initialization (115200 baud, 8N1)

```c
void UART0_Init(UINT32 baudrate)
{
    UINT32 x;

    // Calculate baud rate divisor
    x = 10 * FREQ_SYS * 2 / 16 / baudrate;
    x = (x + 5) / 10;  // Round

    // Set baud rate
    R8_UART0_LCR = RB_LCR_DLAB;    // Enable divisor latch access
    R8_UART0_DIV = 1;               // Pre-divisor = 1
    R8_UART0_DLM = x >> 8;          // Divisor high byte
    R8_UART0_DLL = x & 0xff;        // Divisor low byte

    // Set line format: 8-bit, no parity, 1 stop bit
    R8_UART0_LCR = RB_LCR_WORD_SZ;  // 8-bit word size

    // Enable and clear FIFOs
    R8_UART0_FCR = RB_FCR_FIFO_TRIG |    // Trigger level: 14 bytes
                   RB_FCR_TX_FIFO_CLR |   // Clear TX FIFO
                   RB_FCR_RX_FIFO_CLR |   // Clear RX FIFO
                   RB_FCR_FIFO_EN;        // Enable FIFO

    // Enable TXD output
    R8_UART0_IER = RB_IER_TXD_EN;

    // Configure GPIO pins
    R32_PB_SMT |= RXD0 | TXD0;      // Schmitt input for RX, slow slew for TX
    R32_PB_PU  |= RXD0;              // Pull-up for RXD
    R32_PB_DIR |= TXD0;              // TXD as output
}
```

## Sending Data

```c
// Send single byte
void UART0_SendByte(UINT8 dat)
{
    R8_UART0_THR = dat;
    while ((R8_UART0_LSR & RB_LSR_TX_ALL_EMP) == 0);  // Wait for complete
}

// Send string
void UART0_SendStr(UINT8 *str)
{
    while (*str) {
        UART0_SendByte(*str++);
    }
}

// Send multiple bytes using FIFO
void UART0_SendBuf(UINT8 *data, UINT8 len)
{
    while (len > 0) {
        // Wait for TX FIFO empty
        while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0);
        // Fill FIFO (up to 16 bytes)
        UINT8 count = (len > UART0_FIFO_SIZE) ? UART0_FIFO_SIZE : len;
        for (UINT8 i = 0; i < count; i++) {
            R8_UART0_THR = *data++;
        }
        len -= count;
    }
}
```

## Receiving Data

```c
// Receive single byte (blocking)
UINT8 UART0_RecvByte(void)
{
    // Check for errors
    if (R8_UART0_LSR & (RB_LSR_OVER_ERR | RB_LSR_PAR_ERR |
                         RB_LSR_FRAME_ERR | RB_LSR_BREAK_ERR)) {
        R8_UART0_RBR;  // Read to clear error
        return 0;
    }
    // Wait for data ready
    while ((R8_UART0_LSR & RB_LSR_DATA_RDY) == 0);
    return R8_UART0_RBR;
}

// Receive multiple bytes (returns count)
UINT8 UART0_RecvBuf(UINT8 *buf)
{
    UINT8 count = 0;

    // Check for errors
    if (R8_UART0_LSR & (RB_LSR_OVER_ERR | RB_LSR_PAR_ERR |
                         RB_LSR_FRAME_ERR | RB_LSR_BREAK_ERR)) {
        R8_UART0_RBR;  // Read to clear
        return 0;
    }

    // Read all available bytes
    while (R8_UART0_LSR & RB_LSR_DATA_RDY) {
        *buf++ = R8_UART0_RBR;
        count++;
    }
    return count;
}
```

## Status Checking

```c
// Check line status
UINT8 status = R8_UART0_LSR;

if (status & RB_LSR_DATA_RDY)    { /* Data available in RX FIFO */ }
if (status & RB_LSR_TX_FIFO_EMP) { /* TX FIFO empty, can send more */ }
if (status & RB_LSR_TX_ALL_EMP)  { /* All TX data sent (FIFO + TSR) */ }
if (status & RB_LSR_OVER_ERR)    { /* RX overrun error */ }
if (status & RB_LSR_PAR_ERR)     { /* Parity error */ }
if (status & RB_LSR_FRAME_ERR)   { /* Frame error */ }
if (status & RB_LSR_BREAK_ERR)   { /* Break condition */ }
```

## FIFO Control

```c
// Clear FIFOs
R8_UART0_FCR = RB_FCR_TX_FIFO_CLR;  // Clear TX FIFO
R8_UART0_FCR = RB_FCR_RX_FIFO_CLR;  // Clear RX FIFO

// Set trigger level
// RB_FCR_FIFO_TRIG values:
// 0x00 = 1 byte trigger
// 0x40 = 4 bytes trigger (UART0) / 8 bytes (UART1)
// 0x80 = 8 bytes trigger (UART0) / 16 bytes (UART1)
// 0xC0 = 14 bytes trigger (UART0) / 28 bytes (UART1)
```

## Printf Support

```c
// Override fputc() to route printf to UART0
int fputc(int c, FILE *f)
{
    R8_UART0_THR = c;
    while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0);
    return c;
}

// Now printf() works:
printf("Hello CH561, value=%d\n", myValue);
```

## UART1 (Same Pattern)

```c
void UART1_Init(UINT32 baudrate)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / baudrate;
    x = (x + 5) / 10;

    R8_UART1_LCR = RB_LCR_DLAB;
    R8_UART1_DIV = 1;
    R8_UART1_DLM = x >> 8;
    R8_UART1_DLL = x & 0xff;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_IER = RB_IER_TXD_EN;

    // Configure pins (PB10=RXD1, PB11=TXD1)
    R32_PB_SMT |= RXD1 | TXD1;
    R32_PB_PU  |= RXD1;
    R32_PB_DIR |= TXD1;
}
```

## UART Interrupt

```c
// Enable UART0 receive interrupt
R8_UART0_IER |= RB_IER_RECV_RDY;   // Enable RX data ready interrupt

// In IRQ handler:
__irq void IRQ_Handler(void)
{
    if (R8_INT_FLAG_0 & RB_IF_UART0) {
        UINT8 iir = R8_UART0_IIR & RB_IIR_INT_MASK;
        if (iir == UART_II_RECV_RDY || iir == UART_II_RECV_TOUT) {
            // Data available
            UINT8 data = R8_UART0_RBR;
            // Process data
        }
    }
}
```

## Complete Example: UART Echo

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

void mInitSTDIO(void)
{
    UINT32 x;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;
    x = (x + 5) / 10;
    R8_UART0_LCR = 0x80;
    R8_UART0_DIV = 1;
    R8_UART0_DLM = x >> 8;
    R8_UART0_DLL = x & 0xff;
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART0_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= RXD0 | TXD0;
    R32_PB_PU  |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f)
{
    R8_UART0_THR = c;
    while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0);
    return c;
}

__irq void IRQ_Handler(void) { while(1); }
__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    mInitSTDIO();
    PRINT("UART0 echo mode:\n");

    while(1)
    {
        if (R8_UART0_LSR & RB_LSR_DATA_RDY) {
            UINT8 ch = R8_UART0_RBR;
            R8_UART0_THR = ch;
            while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0);
        }
    }
}
```
