# SPI Communication

> **Summary**: Configure and use SPI0 on CH5xx 8051 chips in master and slave mode -- clock setup, data exchange, interrupt handling.

## Trigger Intent

- "SPI master mode"
- "SPI slave configuration"
- "SPI clock divider"
- "SPI data exchange"
- "SPI interrupt"

## SPI0 Registers

| Register | Address | Purpose |
|---|---|---|
| SPI0_STAT | 0xF8 | Status flags (FIFO, interrupt, free) |
| SPI0_DATA | 0xF9 | FIFO data port (read=RX, write=TX) |
| SPI0_CTRL | 0xFA | Control (MISO/MOSI/SCK output enable, clock mode) |
| SPI0_CK_SE | 0xFB | Clock divisor / slave preset |
| SPI0_SETUP | 0xFC | Setup (master/slave, interrupt, bit order) |

### SPI0_CTRL Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bS0_MISO_OE | MISO output enable |
| 6 | bS0_MOSI_OE | MOSI output enable |
| 5 | bS0_SCK_OE | SCK output enable |
| 4 | bS0_DATA_DIR | Data direction: 0=write, 1=read |
| 3 | bS0_MST_CLK | Clock mode: 0=mode 0 (CPOL=0), 1=mode 3 (CPOL=1) |
| 2 | bS0_2_WIRE | 0=3-wire (SCK+MOSI+MISO), 1=2-wire (SCK+MISO) |
| 1 | bS0_CLR_ALL | Force clear FIFO and counters |
| 0 | bS0_AUTO_IF | Auto-clear S0_IF_BYTE on FIFO access |

### SPI0_SETUP Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bS0_MODE_SLV | 0=master, 1=slave |
| 6 | bS0_IE_FIFO_OV | Enable FIFO overflow interrupt |
| 5 | bS0_IE_FIRST | Enable first-byte-received interrupt |
| 4 | bS0_IE_BYTE | Enable byte-exchanged interrupt |
| 3 | bS0_BIT_ORDER | 0=MSB first, 1=LSB first |
| 1 | bS0_SLV_SELT | (RO) Slave chip-selected status |
| 0 | bS0_SLV_PRELOAD | (RO) Slave pre-loading status |

### SPI0_STAT Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | S0_FST_ACT | (RO) First byte received status |
| 6 | S0_IF_OV | FIFO overflow interrupt flag |
| 5 | S0_IF_FIRST | First byte received interrupt flag |
| 4 | S0_IF_BYTE | Byte exchanged interrupt flag |
| 3 | S0_FREE | (RO) SPI bus free |
| 2 | S0_T_FIFO | (RO) TX FIFO count |
| 0 | S0_R_FIFO | (RO) RX FIFO count |

## Pin Assignment

| Pin | Function |
|---|---|
| P1.7 (SCK) | Serial clock |
| P1.6 (MISO) | Master in, slave out |
| P1.5 (MOSI) | Master out, slave in |
| P1.4 (SCS) | Slave chip select (active low) |

## Step-by-Step

### 1. SPI Master Init (Mode 0)

```c
void SPI_MasterInit(void)
{
    SPI0_SETUP = 0;              // Master mode, MSB first
    SPI0_CTRL = 0x60;            // Mode 0: SCK idle low
                                 // bS0_SCK_OE=1, bS0_MOSI_OE=1
    SPI0_CK_SE = 0x02;           // Clock divider (Fsys/4)

    // Set SCK, MOSI, MISO pins
    P1_MOD_OC |= 0xFF;           // All P1 open-drain
    P1_DIR_PU |= 0xFF;           // All P1 output (for SCK, MOSI, SCS)
}
```

### 2. SPI Master Init (Mode 3)

```c
void SPI_MasterInit_Mode3(void)
{
    SPI0_SETUP = 0;              // Master mode
    SPI0_CTRL = 0x68;            // Mode 3: SCK idle high
                                 // bS0_SCK_OE=1, bS0_MOSI_OE=1, bS0_MST_CLK=1
    SPI0_CK_SE = 0x02;
    P1_MOD_OC |= 0xFF;
    P1_DIR_PU |= 0xFF;
}
```

### 3. SPI Master Write

```c
void SPI_Write(UINT8 dat)
{
    SPI0_DATA = dat;             // Write data to TX FIFO
    while(S0_FREE == 0);         // Wait for transfer complete
}
```

### 4. SPI Master Read

```c
UINT8 SPI_Read(void)
{
    SPI0_DATA = 0xFF;            // Send dummy byte to generate clock
    while(S0_FREE == 0);
    return SPI0_DATA;            // Read received data
}
```

### 5. SPI Master Write-Read (Full Duplex)

```c
UINT8 SPI_Transfer(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE == 0);
    return SPI0_DATA;
}
```

### 6. SPI Slave Init

```c
void SPI_SlaveInit(void)
{
    SPI0_SETUP = 0x80;           // Slave mode, MSB first
    SPI0_CTRL = 0x89;            // bS0_MISO_OE=1, bS0_AUTO_IF=1
    SPI0_CK_SE = 0xFF;           // Pre-load value for first byte

    // Set pins as input (slave)
    P1_MOD_OC &= 0x0F;
    P1_DIR_PU &= 0x0F;
}
```

### 7. SPI Interrupt

```c
void SPI_InitInterrupt(void)
{
    SPI0_SETUP |= bS0_IE_FIFO_OV | bS0_IE_BYTE;
    SPI0_CTRL |= bS0_AUTO_IF;
    SPI0_STAT |= 0xFF;          // Clear all flags
    IE_SPI0 = 1;                 // Enable SPI0 interrupt
    EA = 1;
}

void SPI_ISR(void) interrupt INT_NO_SPI0 using 1
{
    if (S0_IF_BYTE) {
        S0_IF_BYTE = 0;          // Clear flag
        UINT8 dat = SPI0_DATA;   // Read received data
    }
    if (S0_IF_OV) {
        S0_IF_OV = 0;            // Clear overflow flag
    }
}
```

### 8. Full Example: SPI Flash Read

```c
void SPI_FlashReadID(void)
{
    UINT8 id[3];

    // CS low
    SCS = 0;

    SPI_Write(0x9F);            // Read JEDEC ID command
    id[0] = SPI_Read();         // Manufacturer ID
    id[1] = SPI_Read();         // Memory Type
    id[2] = SPI_Read();         // Capacity

    // CS high
    SCS = 1;

    printf("Flash ID: %02X %02X %02X\n", (UINT16)id[0], (UINT16)id[1], (UINT16)id[2]);
}
```

## Clock Divider Reference

SPI clock = Fsys / (2 * (SPI0_CK_SE + 1))

| SPI0_CK_SE | Fsys=12MHz | Fsys=24MHz | Fsys=32MHz |
|---|---|---|---|
| 0 | 6 MHz | 12 MHz | 16 MHz |
| 1 | 3 MHz | 6 MHz | 8 MHz |
| 2 | 2 MHz | 4 MHz | 5.33 MHz |
| 5 | 1 MHz | 2 MHz | 2.67 MHz |
| 11 | 500 KHz | 1 MHz | 1.33 MHz |
| 59 | 100 KHz | 200 KHz | 267 KHz |

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Slave receives garbage | First byte not pre-loaded | Write to SPI0_S_PRE before chip select |
| No clock output | SCK output not enabled | Set bS0_SCK_OE in SPI0_CTRL |
| MISO stuck low | Slave MISO not enabled | Set bS0_MISO_OE in SPI0_CTRL |
| Data shifted wrong | Clock mode mismatch | Match CPOL/CPHA between master and slave |
