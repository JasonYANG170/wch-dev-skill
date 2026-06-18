# Recipe: SPI Communication (Register-Based)

## Overview

Configure SPI0 and SPI1 for master/slave communication on CH561/CH563 using
direct register manipulation. Supports polling, interrupt, and DMA transfer modes.

## Available SPI Instances

| SPI | CS Pin | SCK Pin | MOSI Pin | MISO Pin |
|-----|--------|---------|----------|----------|
| SPI0 | PB12 (SCS) | PB13 (SCK0) | PB14 (MOSI) | PB15 (MISO) |
| SPI1 | PB16 (ADCS) | PB17 (SCK1) | PB18 (SDO) | PB19 (SDI) |

## SPI Register Map

| Register | R/W | Description |
|----------|-----|-------------|
| `R8_SPIx_CTRL_MOD` | RW | Mode control |
| `R8_SPIx_CTRL_DMA` | RW | DMA control |
| `R8_SPIx_INTER_EN` | RW | Interrupt enable |
| `R8_SPIx_CLOCK_DIV` | RW | Master clock divisor |
| `R8_SPIx_BUFFER` | RW | Data buffer (single byte) |
| `R8_SPIx_RUN_FLAG` | RO | Work flag |
| `R8_SPIx_INT_FLAG` | RW1 | Interrupt flag (write 1 to clear) |
| `R8_SPIx_FIFO_COUNT` | RO | FIFO count |
| `R32_SPIx_FIFO` | RW | FIFO register (32-bit) |
| `R16_SPIx_DMA_NOW` | RW | DMA current address |
| `R16_SPIx_DMA_BEG` | RW | DMA begin address |
| `R16_SPIx_DMA_END` | RW | DMA end address |

## SPI0 Master Mode

### Initialization

```c
void SPI0_MasterInit(void)
{
    // Enable SPI0 clock
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_SPI0;

    // Configure: master mode, 3-wire, mode 0, SCK/MOSI/MISO output enable
    R8_SPI0_CTRL_MOD = RB_SPI_SCK_OE | RB_SPI_MOSI_OE | RB_SPI_MISO_OE;

    // Set clock divisor (FREQ_SYS / (2 * divisor))
    R8_SPI0_CLOCK_DIV = 10;  // e.g., 100MHz / 20 = 5MHz SPI clock

    // Clear FIFO
    R8_SPI0_CTRL_MOD |= RB_SPI_ALL_CLEAR;
    R8_SPI0_CTRL_MOD &= ~RB_SPI_ALL_CLEAR;

    // Configure GPIO pins
    R32_PB_DIR |= (1 << 12) | (1 << 13) | (1 << 14);  // CS, SCK, MOSI output
    R32_PB_DIR &= ~(1 << 15);                           // MISO input
    R32_PB_PU |= (1 << 15);                             // MISO pull-up
}
```

### SPI Data Modes

```c
// Mode 0 (CPOL=0, CPHA=0) - default
R8_SPI0_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;

// Mode 3 (CPOL=1, CPHA=1)
R8_SPI0_CTRL_MOD |= RB_SPI_MST_SCK_MOD;

// MSB first - default (no bit to set)
// LSB first - set RB_SPI_2WIRE_MOD for 2-wire mode
```

### Single Byte Transfer

```c
// Send and receive single byte
UINT8 SPI0_TransferByte(UINT8 txdat)
{
    R8_SPI0_BUFFER = txdat;                    // Write data to buffer
    while (!(R8_SPI0_RUN_FLAG & RB_SPI_FREE)); // Wait for transfer complete
    return R8_SPI0_BUFFER;                      // Read received data
}
```

### Multi-Byte Transfer (Polling)

```c
void SPI0_TransferBuf(UINT8 *txbuf, UINT8 *rxbuf, UINT16 len)
{
    for (UINT16 i = 0; i < len; i++) {
        R8_SPI0_BUFFER = txbuf ? txbuf[i] : 0xFF;
        while (!(R8_SPI0_RUN_FLAG & RB_SPI_FREE));
        if (rxbuf) rxbuf[i] = R8_SPI0_BUFFER;
    }
}
```

### DMA Transfer

```c
void SPI0_DMATransfer(UINT8 *buf, UINT16 len, UINT8 is_tx)
{
    // Enable DMA
    R8_SPI0_CTRL_DMA = RB_SPI_DMA_ENABLE;

    // Set DMA addresses
    R16_SPI0_DMA_BEG = (UINT16)(UINT32)buf;
    R16_SPI0_DMA_END = (UINT16)(UINT32)buf + len;
    R16_SPI0_DMA_NOW = (UINT16)(UINT32)buf;

    // Set total byte count
    R16_SPI0_TOTAL_CNT = len;

    // Set FIFO direction
    if (is_tx) {
        R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;  // 0=out (TX)
    } else {
        R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;   // 1=in (RX)
    }

    // Enable DMA interrupt
    R8_SPI0_INTER_EN = RB_SPI_IE_DMA_END;

    // Wait for DMA complete
    while (!(R8_SPI0_INT_FLAG & RB_SPI_IF_DMA_END));
    R8_SPI0_INT_FLAG = RB_SPI_IF_DMA_END;  // Clear flag
}
```

## SPI0 Slave Mode

```c
void SPI0_SlaveInit(void)
{
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_SPI0;

    // Slave mode, 3-wire
    R8_SPI0_CTRL_MOD = RB_SPI_MODE_SLAVE | RB_SPI_MOSI_OE | RB_SPI_MISO_OE;

    // Set first data to send when master reads
    R8_SPI0_SLAVE_PRE = 0xAB;

    // Configure pins
    R32_PB_DIR &= ~((1 << 12) | (1 << 13) | (1 << 14));  // CS, SCK, MOSI input
    R32_PB_DIR |= (1 << 15);                               // MISO output

    // Enable first-byte interrupt
    R8_SPI0_INTER_EN = RB_SPI_IE_FST_BYTE;
}
```

## SPI Interrupt Flags

```c
// Enable interrupt
R8_SPI0_INTER_EN |= RB_SPI_IE_DMA_END;   // DMA complete
R8_SPI0_INTER_EN |= RB_SPI_IE_BYTE_END;  // Byte exchanged
R8_SPI0_INTER_EN |= RB_SPI_IE_CNT_END;   // Counter end
R8_SPI0_INTER_EN |= RB_SPI_IE_FIFO_HF;   // FIFO half full
R8_SPI0_INTER_EN |= RB_SPI_IE_FIFO_OV;   // FIFO overflow

// Check and clear flags
if (R8_SPI0_INT_FLAG & RB_SPI_IF_DMA_END) {
    R8_SPI0_INT_FLAG = RB_SPI_IF_DMA_END;  // Write 1 to clear
    // Handle DMA complete
}
```

## Complete Example: SPI Flash Read ID

```c
#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"

// SPI CS on PB12
#define SPI_CS_LOW()   { R32_PB_CLR |= (1 << 12); }
#define SPI_CS_HIGH()  { R32_PB_OUT |= (1 << 12); }

void SPI0_Init(void)
{
    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_SPI0;
    R8_SPI0_CTRL_MOD = RB_SPI_SCK_OE | RB_SPI_MOSI_OE | RB_SPI_MISO_OE;
    R8_SPI0_CLOCK_DIV = 10;
    R8_SPI0_CTRL_MOD |= RB_SPI_ALL_CLEAR;
    R8_SPI0_CTRL_MOD &= ~RB_SPI_ALL_CLEAR;

    R32_PB_DIR |= (1 << 12) | (1 << 13) | (1 << 14);
    R32_PB_DIR &= ~(1 << 15);
    R32_PB_PU |= (1 << 15);
}

UINT8 SPI0_RW(UINT8 dat)
{
    R8_SPI0_BUFFER = dat;
    while (!(R8_SPI0_RUN_FLAG & RB_SPI_FREE));
    return R8_SPI0_BUFFER;
}

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
    R32_PB_PU |= RXD0;
    R32_PB_DIR |= TXD0;
}

int fputc(int c, FILE *f) { R8_UART0_THR = c; while ((R8_UART0_LSR & RB_LSR_TX_FIFO_EMP) == 0); return c; }
__irq void IRQ_Handler(void) { while(1); }
__irq void FIQ_Handler(void) { while(1); }

int main(void)
{
    mInitSTDIO();
    SPI0_Init();

    // Read JEDEC ID (command 0x9F)
    SPI_CS_LOW();
    UINT8 mfg  = SPI0_RW(0x9F);
    UINT8 type = SPI0_RW(0xFF);
    UINT8 cap  = SPI0_RW(0xFF);
    SPI_CS_HIGH();

    PRINT("Flash ID: %02X %02X %02X\n", mfg, type, cap);

    while(1);
}
```

## Notes

- SPI chip select (CS) is managed manually via GPIO
- For DMA transfers, buffer must be in accessible SRAM region
- SPI clock = FREQ_SYS / (2 * R8_SPIx_CLOCK_DIV)
- SPI0 FIFO: 32 bytes, SPI1 FIFO: 16 bytes
