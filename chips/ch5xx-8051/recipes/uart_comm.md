# UART Communication

> **Summary**: Configure and use UART0 and UART1 on CH5xx 8051 chips for serial communication -- init, baud rate calculation, send/receive, interrupt-driven operation.

## Trigger Intent

- "UART initialization"
- "Serial communication setup"
- "Configure baud rate"
- "UART interrupt receive"
- "Send data over UART"

## UART Resources

| Chip | UARTs | Notes |
|---|---|---|
| CH552/CH551 | UART0 + UART1 | |
| CH554 | UART0 + UART1 | |
| CH555 | UART0 + UART1 | |
| CH559 | UART0 + UART1 + UART2 + UART3 | UART2/3 are enhanced |
| CH543/CH549 | UART0 + UART1 | |

## UART0 Registers

| Register | Address | Purpose |
|---|---|---|
| SCON | 0x98 | Control (mode, enable RX, flags) |
| SBUF | 0x99 | Data buffer (read=RX, write=TX) |
| TMOD | 0x89 | Timer mode (for baud rate gen) |
| TH1/TL1 | 0x8D/0x8B | Timer1 reload (baud rate) |
| T2CON | 0xC8 | Timer2 control (alt baud rate gen) |
| RCAP2H/L | 0xCB/0xCA | Timer2 reload value |

## UART1 Registers

| Register | Address | Purpose |
|---|---|---|
| SCON1 | 0xC0 | Control (mode, enable RX, flags) |
| SBUF1 | 0xC1 | Data buffer |
| SBAUD1 | 0xC2 | Baud rate = 0 - FREQ_SYS/16/BAUD |

## Step-by-Step

### 1. UART0 Init (Polling Mode, 115200 baud)

```c
void UART0_Init(UINT32 baud)
{
    SM0 = 0; SM1 = 1;       // Mode 1: 8-bit UART, variable baud
    REN = 1;                 // Enable receiver

    // Timer1 as baud rate generator (mode 2, auto-reload)
    TMOD = TMOD & 0x0F | 0x20;  // Timer1 mode 2
    PCON |= SMOD;                // SMOD=1 for double speed
    TH1 = 0 - FREQ_SYS/16/baud; // Reload value
    TL1 = TH1;
    TR1 = 1;                     // Start Timer1
}
```

### 2. UART0 Init (Interrupt Mode)

```c
void UART0_InitInt(UINT32 baud)
{
    UART0_Init(baud);       // Same as above
    ES = 1;                 // Enable UART0 interrupt
    EA = 1;                 // Enable global interrupts
}

void UART0_ISR(void) interrupt INT_NO_UART0 using 1
{
    if (RI) {
        RI = 0;             // Clear receive flag
        UINT8 dat = SBUF;   // Read received byte
        // Process dat...
    }
    if (TI) {
        TI = 0;             // Clear transmit flag
        // Send next byte if any
    }
}
```

### 3. UART0 Send/Receive (Polling)

```c
void UART0_SendByte(UINT8 dat)
{
    SBUF = dat;
    while(TI == 0);         // Wait for transmit complete
    TI = 0;                 // Clear flag
}

UINT8 UART0_RecvByte(void)
{
    while(RI == 0);         // Wait for receive complete
    RI = 0;
    return SBUF;
}
```

### 4. UART1 Init (115200 baud)

```c
void UART1_Init(UINT32 baud)
{
    U1SM0 = 0;              // 8-bit data
    U1SMOD = 1;             // Fast mode (2x)
    U1REN = 1;              // Enable receiver
    SBAUD1 = 0 - FREQ_SYS/16/baud;  // Baud rate setting
    U1TI = 0;

    // Optional: remap pins to P3.4(RXD)/P3.2(TXD)
    // PIN_FUNC |= bUART1_PIN_X;
}
```

### 5. UART1 Send/Receive (Polling)

```c
void UART1_SendByte(UINT8 dat)
{
    SBUF1 = dat;
    while(U1TI == 0);
    U1TI = 0;
}

UINT8 UART1_RecvByte(void)
{
    while(U1RI == 0);
    U1RI = 0;
    return SBUF1;
}
```

### 6. UART1 Interrupt

```c
void UART1_ISR(void) interrupt INT_NO_UART1 using 1
{
    if (U1RI) {
        U1RI = 0;
        UINT8 dat = SBUF1;
        // Echo back
        UART1_SendByte(dat);
    }
}
```

### 7. Pin Remapping

```c
// UART0 default: RXD=P3.0, TXD=P3.1
// UART0 alternate: RXD_=P1.2, TXD_=P1.3
PIN_FUNC |= bUART0_PIN_X;   // Switch to alternate pins

// UART1 default: RXD1=P1.6, TXD1=P1.7
// UART1 alternate: RXD1_=P3.4, TXD1_=P3.2
PIN_FUNC |= bUART1_PIN_X;   // Switch to alternate pins
```

### 8. printf via UART0

```c
#include "stdio.h"

void main()
{
    CfgFsys();
    mInitSTDIO();           // Configures UART0 for printf
    printf("Hello %d\n", 42);
}
```

## Baud Rate Formulas

### UART0 (Mode 1, Timer1)
```
Baud = (2^SMOD / 32) * (Fsys / (12 * (256 - TH1)))
     = Fsys / (32 * (256 - TH1))     when SMOD=0
     = Fsys / (16 * (256 - TH1))     when SMOD=1

TH1 = 256 - Fsys / (16 * Baud)       when SMOD=1
```

### UART1
```
Baud = Fsys / (16 / (256 - SBAUD1))   when U1SMOD=1
     = Fsys / (32 / (256 - SBAUD1))   when U1SMOD=0

SBAUD1 = 256 - Fsys / (16 * Baud)     when U1SMOD=1
```

### Common Values (Fsys=12MHz, SMOD=1)

| Baud | TH1 (Mode 2) | SBAUD1 |
|---|---|---|
| 9600 | 0xE6 (230) | 0xE6 |
| 19200 | 0xF3 (243) | 0xF3 |
| 57600 | 0xFB (251) | 0xFB |
| 115200 | 0xFD (253) | 0xFD |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Garbage output | Wrong baud rate or FREQ_SYS mismatch | Verify FREQ_SYS matches actual CLOCK_CFG |
| No output | Pin not configured | Check P3.1 is output or PIN_FUNC for alternate |
| Missing bytes | Polling too slow | Use interrupt-driven receive |
| Double characters | TI flag not cleared | Always clear TI after checking |
