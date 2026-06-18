# CH561/CH563 Peripheral Register Reference

Complete register reference for CH561/CH563 ARM7TDMI microcontrollers.
All peripherals are accessed via direct register manipulation -- there is NO
peripheral driver library.

## Register Naming Convention

| Prefix | Width | Example |
|--------|-------|---------|
| `R32_` | 32-bit | `R32_PA_DIR` |
| `R16_` | 16-bit | `R16_SPI0_DMA_NOW` |
| `R8_` | 8-bit | `R8_UART0_THR` |
| `RB_` | Bit mask | `RB_LSR_DATA_RDY` |
| `BA_` | Base address | `BA_UART0` |

## System Registers

```c
// Safe access (unlock protected registers)
R8_SAFE_ACCESS_SIG = 0x57;  // First unlock byte
R8_SAFE_ACCESS_SIG = 0xA8;  // Second unlock byte
// Now can write to protected registers
R8_SAFE_ACCESS_SIG = 0x00;  // Re-lock

// Sleep/clock control
R8_SLP_CLK_OFF0    // Peripheral clock gate (0=on, 1=off)
R8_SLP_CLK_OFF1    // Additional clock gate
R8_SLP_POWER_CTRL  // Power control

// Reset
R8_RST_BOOT_CTRL   // Boot control
R8_RST_BOOT_STAT   // Boot status

// Watchdog
R8_WDOG_COUNT      // Watchdog counter
R8_WDOG_CTRL       // Watchdog control
```

## GPIO Registers

### Port A (PA0-PA21)

```c
R32_PA_DIR    // Direction: 0=input, 1=output
R32_PA_PIN    // Input pin values (read-only)
R32_PA_OUT    // Output values
R32_PA_CLR    // Clear output (write 1 to clear)
R32_PA_PU     // Pull-up enable
R32_PA_PD     // Pull-down / open-drain enable
R32_PA_DRV    // Drive strength: 0=4mA, 1=16mA
R32_PA_SMT    // Schmitt trigger / slew rate
```

### Port B (PB0-PB23)

```c
R32_PB_DIR    // Direction: 0=input, 1=output
R32_PB_PIN    // Input pin values (read-only)
R32_PB_OUT    // Output values
R32_PB_CLR    // Clear output (write 1 to clear)
R32_PB_PU     // Pull-up enable
R32_PB_PD     // Pull-down / open-drain enable
R32_PB_DRV    // Drive strength: 0=4mA, 1=16mA
R32_PB_SMT    // Schmitt trigger / slew rate
```

### Port D (PD0-PD31, CH563 only)

```c
R32_PD_DIR    // Direction: 0=input, 1=output
R32_PD_PIN    // Input pin values (read-only)
R32_PD_OUT    // Output values
R32_PD_CLR    // Clear output (write 1 to clear)
R32_PD_PU     // Pull-up enable
R32_PD_PD     // Pull-down / open-drain enable
R32_PD_DRV    // Drive strength: 0=4mA, 1=16mA
R32_PD_SMT    // Schmitt trigger / slew rate
```

### GPIO Interrupt Registers

```c
R32_INT_STATUS_PA   // PA interrupt flag (write 1 to clear)
R32_INT_ENABLE_PA   // PA interrupt enable
R32_INT_MODE_PA     // PA interrupt mode (0=level, 1=edge)
R32_INT_POLAR_PA    // PA interrupt polarity

R32_INT_STATUS_PB   // PB interrupt flag (write 1 to clear)
R32_INT_ENABLE_PB   // PB interrupt enable
R32_INT_MODE_PB     // PB interrupt mode (0=level, 1=edge)
R32_INT_POLAR_PB    // PB interrupt polarity
```

## UART Registers

### UART0 (PB8=RXD0, PB9=TXD0, 16-byte FIFO)

```c
R8_UART0_RBR   // Receiver buffer (read)
R8_UART0_THR   // Transmitter holding (write)
R8_UART0_IER   // Interrupt enable
R8_UART0_IIR   // Interrupt identification (read)
R8_UART0_FCR   // FIFO control (write)
R8_UART0_LCR   // Line control
R8_UART0_MCR   // Modem control
R8_UART0_LSR   // Line status (read)
R8_UART0_MSR   // Modem status (read)
R8_UART0_DIV   // Pre-divisor latch
R8_UART0_DLL   // Divisor latch LSB
R8_UART0_DLM   // Divisor latch MSB

// LSR bits
RB_LSR_DATA_RDY     // Data ready
RB_LSR_OVER_ERR     // Overrun error
RB_LSR_PAR_ERR      // Parity error
RB_LSR_FRAME_ERR    // Frame error
RB_LSR_BREAK_ERR    // Break error
RB_LSR_TX_FIFO_EMP  // TX FIFO empty
RB_LSR_TX_ALL_EMP   // TX all empty
RB_LSR_RX_FIFO_EMP  // RX FIFO empty

// FCR bits
RB_FCR_FIFO_EN      // FIFO enable
RB_FCR_RX_FIFO_CLR  // Clear RX FIFO
RB_FCR_TX_FIFO_CLR  // Clear TX FIFO
RB_FCR_FIFO_TRIG    // Trigger level mask

// IER bits
RB_IER_RECV_RDY     // Receive data ready interrupt
RB_IER_THR_EMPTY    // THR empty interrupt
RB_IER_LINE_STAT    // Line status interrupt
RB_IER_TXD_EN       // TXD output enable
```

### UART1 (PB10=RXD1, PB11=TXD1, 32-byte FIFO)

```c
R8_UART1_RBR   // Receiver buffer (read)
R8_UART1_THR   // Transmitter holding (write)
R8_UART1_IER   // Interrupt enable
R8_UART1_IIR   // Interrupt identification (read)
R8_UART1_FCR   // FIFO control (write)
R8_UART1_LCR   // Line control
R8_UART1_MCR   // Modem control
R8_UART1_LSR   // Line status (read)
R8_UART1_MSR   // Modem status (read)
R8_UART1_DIV   // Pre-divisor latch
R8_UART1_DLL   // Divisor latch LSB
R8_UART1_DLM   // Divisor latch MSB
```

## SPI Registers

### SPI0 (PB12=CS, PB13=SCK, PB14=MOSI, PB15=MISO)

```c
R8_SPI0_CTRL_MOD    // Mode control
R8_SPI0_CTRL_DMA    // DMA control
R8_SPI0_INTER_EN    // Interrupt enable
R8_SPI0_CLOCK_DIV   // Master clock divisor
R8_SPI0_BUFFER      // Data buffer (single byte)
R8_SPI0_RUN_FLAG    // Work flag (read-only)
R8_SPI0_INT_FLAG    // Interrupt flag (write 1 to clear)
R8_SPI0_FIFO_COUNT  // FIFO count (read-only)
R32_SPI0_FIFO       // FIFO register (32-bit)
R16_SPI0_DMA_NOW    // DMA current address
R16_SPI0_DMA_BEG    // DMA begin address
R16_SPI0_DMA_END    // DMA end address
R16_SPI0_TOTAL_CNT  // DMA total count

// Control mode bits
RB_SPI_SCK_OE       // SCK output enable
RB_SPI_MOSI_OE      // MOSI output enable
RB_SPI_MISO_OE      // MISO output enable
RB_SPI_ALL_CLEAR    // Clear all FIFOs
RB_SPI_MST_SCK_MOD  // Master SCK mode (mode 3)
RB_SPI_MODE_SLAVE   // Slave mode
RB_SPI_FIFO_DIR     // FIFO direction (0=out, 1=in)

// Run flag bits
RB_SPI_FREE         // SPI free (transfer complete)

// Interrupt flags
RB_SPI_IE_FST_BYTE  // First byte
RB_SPI_IE_BYTE_END  // Byte end
RB_SPI_IE_DMA_END   // DMA end
RB_SPI_IE_FIFO_HF   // FIFO half full
RB_SPI_IE_FIFO_OV   // FIFO overflow
RB_SPI_IE_CNT_END   // Counter end
```

### SPI1 (PB16=CS, PB17=SCK, PB18=SDO, PB19=SDI)

Same register layout as SPI0 with `R8_SPI1_xxx` prefix.

## Timer Registers

### TMR0-TMR3 (each has identical register layout)

```c
R8_TMR0_CTRL_MOD    // Mode control
R8_TMR0_CTRL_DMA    // DMA control
R8_TMR0_INTER_EN    // Interrupt enable
R8_TMR0_INT_FLAG    // Interrupt flag (write 1 to clear)
R8_TMR0_FIFO_COUNT  // FIFO count (read-only)
R32_TMR0_COUNT      // Current count (28-bit, read-only)
R32_TMR0_CNT_END    // End count value (period)
R32_TMR0_FIFO       // FIFO register (duty cycle for PWM)
R16_TMR0_DMA_NOW    // DMA current address
R16_TMR0_DMA_BEG    // DMA begin address
R16_TMR0_DMA_END    // DMA end address

// Control mode bits
RB_TMR_ALL_CLEAR    // Clear timer
RB_TMR_COUNT_EN     // Count enable
RB_TMR_OUT_EN       // Output enable
RB_TMR_PWM_REPEAT   // PWM repeat mode
RB_TMR_MODE_IN      // Input/capture mode
RB_TMR_CAT_WIDTH    // Capture min pulse width

// Interrupt flags
RB_TMR_IE_CYC_END   // Cycle end interrupt
RB_TMR_IE_DATA_ACT  // Data active interrupt (capture)
RB_TMR_IF_CYC_END   // Cycle end flag
RB_TMR_IF_DATA_ACT  // Data active flag
```

### Timer PWM Output Pins

| Timer | PWM Pin |
|-------|---------|
| TMR0 | PB0 (PWM0) |
| TMR1 | PB2 (PWM1) |
| TMR2 | PB4 (PWM2) |
| TMR3 | PB6 (PWM3) |

## ADC Registers

```c
R8_ADC_CTRL         // ADC control
R8_ADC_CONVERT      // ADC convert start
R32_ADC_DATA        // ADC data (read-only)
R8_ADC_INT_FLAG     // ADC interrupt flag

// Channel selection
// ADC channels are on PA pins (check datasheet)
```

## Interrupt Controller Registers

```c
// IRQ enable
R8_INT_EN_IRQ_0     // IRQ group 0 enable
R8_INT_EN_IRQ_1     // IRQ group 1 enable
R8_INT_EN_IRQ_GLOB  // Global IRQ enable

// FIQ enable
R8_INT_EN_FIQ_0     // FIQ group 0 enable
R8_INT_EN_FIQ_1     // FIQ group 1 enable
R8_INT_EN_FIQ_GLOB  // Global FIQ enable

// Interrupt flags
R8_INT_FLAG_0       // Interrupt flag group 0
R8_INT_FLAG_1       // Interrupt flag group 1
R32_INT_FLAG        // Combined interrupt flag

// Interrupt source bits (for R8_INT_EN_IRQ_0)
RB_IE_IRQ_TMR0      // Timer0 interrupt
RB_IE_IRQ_TMR1      // Timer1 interrupt
RB_IE_IRQ_TMR2      // Timer2 interrupt
RB_IE_IRQ_TMR3      // Timer3 interrupt
RB_IE_IRQ_SPI0      // SPI0 interrupt
RB_IE_IRQ_SPI1      // SPI1 interrupt
RB_IE_IRQ_UART0     // UART0 interrupt
RB_IE_IRQ_UART1     // UART1 interrupt
RB_IE_IRQ_ETH       // Ethernet interrupt
RB_IE_IRQ_ADC       // ADC interrupt

// Interrupt source bits (for R8_INT_EN_IRQ_1)
RB_IE_IRQ_PB        // PB GPIO interrupt
RB_IE_IRQ_USB       // USB interrupt (CH563 only)
```

## Ethernet Registers

```c
// MAC configuration
R32_ETH_MAC_CFG       // MAC configuration
R32_ETH_MAC_FRAME_FLT // MAC frame filter
R32_ETH_MAC_ADDR0_H   // MAC address 0 high
R32_ETH_MAC_ADDR0_L   // MAC address 0 low

// DMA
R32_ETH_DMA_BUS_MODE  // DMA bus mode
R32_ETH_DMA_TX_DESC   // TX descriptor list address
R32_ETH_DMA_RX_DESC   // RX descriptor list address
R32_ETH_DMA_OP_MODE   // DMA operation mode
R32_ETH_DMA_INT_EN    // DMA interrupt enable
R32_ETH_DMA_INT_FLAG  // DMA interrupt flag
R32_ETH_DMA_TX_POLL   // TX poll demand
R32_ETH_DMA_RX_POLL   // RX poll demand

// MII (PHY access)
R32_ETH_MII_ADDR      // MII address
R32_ETH_MII_DATA      // MII data
```

## USB Registers (CH563 only)

```c
// USB SFR registers at 0x00404000 - 0x00405FFF
// See CH563SFR.H for complete USB register definitions
```

## Peripheral Clock Bits

```c
// R8_SLP_CLK_OFF0 bits (0=clock on, 1=clock off)
RB_SLP_CLK_TMR0    0x01  // Timer0
RB_SLP_CLK_TMR1    0x02  // Timer1
RB_SLP_CLK_TMR2    0x04  // Timer2
RB_SLP_CLK_TMR3    0x08  // Timer3
RB_SLP_CLK_SPI0    0x10  // SPI0
RB_SLP_CLK_SPI1    0x20  // SPI1
RB_SLP_CLK_UART0   0x40  // UART0
RB_SLP_CLK_UART1   0x80  // UART1

// R8_SLP_CLK_OFF1 bits
RB_SLP_CLK_ADC     0x10  // ADC
RB_SLP_CLK_GPIO    0x20  // GPIO
RB_SLP_CLK_ETH     0x80  // Ethernet
RB_SLP_CLK_USB     0x40  // USB (CH563 only)
```

## Flash/EEPROM Programming (ISPXT56X Library)

```c
#include "ISPXT56X.H"

// Flash ROM operations (4KB erase, 4-byte write minimum)
UINT8 FLASH_ROM_ERASE(UINT32 addr, UINT32 len);
UINT8 FLASH_ROM_WRITE(UINT32 addr, PUINT32 pbuf, UINT32 len);
UINT32 FLASH_ROM_VERIFY(UINT32 addr, PUINT32 pbuf, UINT32 len);
void FLASH_ROM_LOCK(UINT8 mode);

// EEPROM (Data-Flash) operations (byte-writable, 28KB)
void EEPROM_READ(UINT16 addr, PUINT8 pbuf, UINT16 len);
UINT8 EEPROM_ERASE(UINT16 addr, UINT16 len);
UINT8 EEPROM_WRITE(UINT16 addr, PUINT8 pbuf, UINT16 len);

// Unique ID
void GET_UNIQUE_ID(PUINT32 pbuf);
```

## Delay Functions

```c
// SysFreq() is called by STARTUP.S before main()
// Delay functions use Timer0

void Delay_us(UINT32 n);   // Microsecond delay
void Delay_ms(UINT32 n);   // Millisecond delay
```

## Baud Rate Calculation

```
Baud Rate = FREQ_SYS / (8 * DIV * (DLM:DLL))
DIV = R8_UARTx_DIV (pre-divisor, 1-128)
DLM:DLL = 16-bit divisor latch

For 115200 baud:
divisor = 10 * FREQ_SYS * 2 / 16 / 115200
divisor = (divisor + 5) / 10  // Round
```

## SPI Clock Calculation

```
SPI Clock = FREQ_SYS / (2 * R8_SPIx_CLOCK_DIV)
```
