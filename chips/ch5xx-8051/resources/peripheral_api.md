# CH5xx 8051 Peripheral Register Quick Reference

This document provides a quick reference for all peripheral SFR registers on CH5xx 8051 chips. All registers are accessed directly via `sfr`/`sbit` keywords defined in `CH5xx.H`.

---

## System Registers

| Register | Address | Purpose |
|---|---|---|
| PSW | 0xD0 | Program status word (CY, AC, OV, P flags) |
| ACC | 0xE0 | Accumulator |
| B | 0xF0 | B register |
| SP | 0x81 | Stack pointer |
| DPL | 0x82 | Data pointer low |
| DPH | 0x83 | Data pointer high |
| SAFE_MOD | 0xA1 | Safe mode entry (write 0x55 then 0xAA) |
| GLOBAL_CFG | 0xB1 | Global config (write protect, LDO, watchdog) |
| CHIP_ID | 0xA1 | (RO) Chip ID (read instead of SAFE_MOD) |
| PCON | 0x87 | Power control (SMOD, reset flags, power-down) |
| CLOCK_CFG | 0xB9 | System clock config (oscillator, PLL divider) |
| WAKE_CTRL | 0xA9 | Wake-up source control |
| RESET_KEEP | 0xFE | Value keeper during reset |
| WDOG_COUNT | 0xFF | Watchdog timer count |
| XBUS_AUX | 0xA2 | xBUS auxiliary (DPTR auto-inc, dual DPTR) |
| PIN_FUNC | 0xC6 | Pin function multiplexing |

### CLOCK_CFG Values (MASK_SYS_CK_SEL = lower 3 bits)

| Value | Fsys | Notes |
|---|---|---|
| 0b111 (7) | 32 MHz | Fpll/3 |
| 0b110 (6) | 24 MHz | Fpll/4 |
| 0b101 (5) | 16 MHz | Fpll/6 |
| 0b100 (4) | 12 MHz | Fpll/8 |
| 0b011 (3) | 6 MHz | Fpll/16 |
| 0b010 (2) | 3 MHz | Fpll/32 |
| 0b001 (1) | 750 KHz | Fpll/128 |
| 0b000 (0) | 187.5 KHz | Fpll/512 |

### GLOBAL_CFG Bits

| Bit | Name | Purpose |
|---|---|---|
| 5 | bBOOT_LOAD | (RO) 1=bootloader, 0=application |
| 4 | bSW_RESET | Software reset (auto-clear) |
| 3 | bCODE_WE | Enable code Flash write |
| 2 | bDATA_WE | Enable DataFlash write |
| 1 | bLDO3V3_OFF | Disable 5V->3.3V LDO |
| 0 | bWDOG_EN | Enable watchdog reset |

---

## Interrupt Registers

| Register | Address | Purpose |
|---|---|---|
| IE | 0xA8 | Interrupt enable (EA, ET2, ES, ET1, EX1, ET0, EX0) |
| IP | 0xB8 | Interrupt priority (standard 8051) |
| IE_EX | 0xE8 | Extended interrupt enable |
| IP_EX | 0xE9 | Extended interrupt priority |
| GPIO_IE | 0xC7 | GPIO interrupt enable |

### IE Bits (sbit)

| Bit | Name | Purpose |
|---|---|---|
| 7 | EA | Global interrupt enable |
| 6 | E_DIS | Global interrupt disable (for Flash ops) |
| 5 | ET2 | Timer2 interrupt enable |
| 4 | ES | UART0 interrupt enable |
| 3 | ET1 | Timer1 interrupt enable |
| 2 | EX1 | External interrupt 1 enable |
| 1 | ET0 | Timer0 interrupt enable |
| 0 | EX0 | External interrupt 0 enable |

### IE_EX Bits (sbit)

| Bit | Name | Purpose |
|---|---|---|
| 7 | IE_WDOG | Watchdog interrupt enable |
| 6 | IE_GPIO | GPIO interrupt enable |
| 5 | IE_PWMX | PWM1/2 interrupt enable |
| 4 | IE_UART1 | UART1 interrupt enable |
| 3 | IE_ADC | ADC interrupt enable |
| 2 | IE_USB | USB interrupt enable |
| 1 | IE_TKEY | TouchKey interrupt enable |
| 0 | IE_SPI0 | SPI0 interrupt enable |

### GPIO_IE Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bIE_IO_EDGE | 0=level trigger, 1=edge trigger |
| 6 | bIE_RXD1_LO | RXD1 low/falling |
| 5 | bIE_P1_5_LO | P1.5 low/falling |
| 4 | bIE_P1_4_LO | P1.4 low/falling |
| 3 | bIE_P1_3_LO | P1.3 low/falling |
| 2 | bIE_RST_HI | RST high/rising |
| 1 | bIE_P3_1_LO | P3.1 low/falling |
| 0 | bIE_RXD0_LO | RXD0 low/falling |

### Interrupt Vector Addresses

| Address | Number | Source |
|---|---|---|
| 0x0003 | 0 | INT0 |
| 0x000B | 1 | Timer0 |
| 0x0013 | 2 | INT1 |
| 0x001B | 3 | Timer1 |
| 0x0023 | 4 | UART0 |
| 0x002B | 5 | Timer2 |
| 0x0033 | 6 | SPI0 |
| 0x003B | 7 | TouchKey |
| 0x0043 | 8 | USB |
| 0x004B | 9 | ADC |
| 0x0053 | 10 | UART1 |
| 0x005B | 11 | PWM1/2 |
| 0x0063 | 12 | GPIO |
| 0x006B | 13 | Watchdog |

---

## Port Registers

### Port 1 (CH552/CH554/CH559)

| Register | Address | Purpose |
|---|---|---|
| P1 | 0x90 | Port 1 data (read=input, write=output) |
| P1_MOD_OC | 0x92 | Output mode: 0=push-pull, 1=open-drain |
| P1_DIR_PU | 0x93 | Direction: 0=input, 1=output (or pullup) |

### Port 3 (CH552/CH554/CH559)

| Register | Address | Purpose |
|---|---|---|
| P3 | 0xB0 | Port 3 data |
| P3_MOD_OC | 0x96 | Output mode |
| P3_DIR_PU | 0x97 | Direction |

### GPIO Configuration Table

| Pn_MOD_OC | Pn_DIR_PU | Mode |
|---|---|---|
| 0 | 0 | Float input (no pullup) |
| 0 | 1 | Push-pull output |
| 1 | 0 | Open-drain, no pullup |
| 1 | 1 | Quasi-bidirectional (8051 standard, with pullup) |

### Pin Alternate Functions (P1)

| Pin | Default | SPI | UART | ADC | PWM | TouchKey |
|---|---|---|---|---|---|---|
| P1.0 | GPIO | -- | -- | -- | -- | TIN0 |
| P1.1 | GPIO | -- | -- | AIN0 | -- | TIN1 |
| P1.2 | GPIO | -- | RXD0_ | -- | -- | -- |
| P1.3 | GPIO | -- | TXD0_ | -- | -- | -- |
| P1.4 | GPIO | SCS | -- | AIN1 | -- | TIN2 |
| P1.5 | GPIO | MOSI | -- | AIN2 | PWM1 | TIN3 |
| P1.6 | GPIO | MISO | RXD1 | -- | -- | TIN4 |
| P1.7 | GPIO | SCK | TXD1 | -- | -- | TIN5 |

### Pin Alternate Functions (P3)

| Pin | Default | UART | Timer | ADC | PWM |
|---|---|---|---|---|---|
| P3.0 | GPIO | RXD | -- | -- | PWM1_ |
| P3.1 | GPIO | TXD | -- | -- | PWM2_ |
| P3.2 | GPIO | -- | INT0 | AIN3 | -- |
| P3.3 | GPIO | -- | INT1 | -- | -- |
| P3.4 | GPIO | RXD1_ | T0 | -- | PWM2 |
| P3.5 | GPIO | -- | T1 | -- | -- |
| P3.6 | GPIO | -- | -- | -- | UDP (USB) |
| P3.7 | GPIO | -- | -- | -- | UDM (USB) |

---

## Timer0/1 Registers

| Register | Address | Purpose |
|---|---|---|
| TCON | 0x88 | Control (TF1, TR1, TF0, TR0, IE1, IT1, IE0, IT0) |
| TMOD | 0x89 | Mode (gate, C/T, mode bits for T0 and T1) |
| TL0 | 0x8A | Timer0 count low |
| TH0 | 0x8C | Timer0 count high |
| TL1 | 0x8B | Timer1 count low |
| TH1 | 0x8D | Timer1 count high |

### Timer Modes

| Mode | Bits | Description |
|---|---|---|
| 0 | 00 | 13-bit (TH + lower 5 of TL) |
| 1 | 01 | 16-bit (TH:TL) |
| 2 | 10 | 8-bit auto-reload (TH=reload, TL=counter) |
| 3 | 11 | T0: split; T1: stopped |

---

## Timer2 Registers

| Register | Address | Purpose |
|---|---|---|
| T2CON | 0xC8 | Control (TF2, EXF2, RCLK, TCLK, EXEN2, TR2, C_T2, CP_RL2) |
| T2MOD | 0xC9 | Mode (clock speed, capture mode, T2OE) |
| TL2 | 0xCC | Timer2 count low |
| TH2 | 0xCD | Timer2 count high |
| RCAP2L | 0xCA | Reload/capture value low |
| RCAP2H | 0xCB | Reload/capture value high |
| T2CAP1L | 0xCE | (RO) Capture1 value low |
| T2CAP1H | 0xCF | (RO) Capture1 value high |

### T2MOD Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bTMR_CLK | 1=use original Fsys (no divider) |
| 6 | bT2_CLK | Timer2 clock: 0=standard(Fsys/12), 1=fast(Fsys/4 or Fsys) |
| 5 | bT1_CLK | Timer1 clock speed |
| 4 | bT0_CLK | Timer0 clock speed |
| 3:2 | bT2_CAP_M[1:0] | Capture mode: x0=falling, 01=any, 11=rising |
| 1 | T2OE | Timer2 clock output on T2 pin |
| 0 | bT2_CAP1_EN | Enable T2 capture1 |

---

## UART0 Registers

| Register | Address | Purpose |
|---|---|---|
| SCON | 0x98 | Control (SM0, SM1, SM2, REN, TB8, RB8, TI, RI) |
| SBUF | 0x99 | Data buffer |

### UART0 Modes

| SM0 | SM1 | Mode | Baud Rate |
|---|---|---|---|
| 0 | 0 | Shift register | Fsys/12 |
| 0 | 1 | 8-bit UART | Variable (Timer1/2) |
| 1 | 0 | 9-bit UART | Fsys/128 or Fsys/32 (SMOD) |
| 1 | 1 | 9-bit UART | Variable (Timer1/2) |

---

## UART1 Registers

| Register | Address | Purpose |
|---|---|---|
| SCON1 | 0xC0 | Control (U1SM0, U1SMOD, U1REN, U1TB8, U1RB8, U1TI, U1RI) |
| SBUF1 | 0xC1 | Data buffer |
| SBAUD1 | 0xC2 | Baud rate = 0 - FREQ_SYS/16/BAUD |

---

## SPI0 Registers

| Register | Address | Purpose |
|---|---|---|
| SPI0_STAT | 0xF8 | Status (FIFO, interrupt flags, free) |
| SPI0_DATA | 0xF9 | FIFO data port |
| SPI0_CTRL | 0xFA | Control (output enables, clock mode, FIFO clear) |
| SPI0_CK_SE | 0xFB | Clock divisor / slave preset |
| SPI0_SETUP | 0xFC | Setup (master/slave, interrupts, bit order) |

### SPI Clock

```
SPI_CLK = Fsys / (2 * (SPI0_CK_SE + 1))
```

---

## ADC Registers

| Register | Address | Purpose |
|---|---|---|
| ADC_CTRL | 0x80 | Control (CMPO, CMP_IF, ADC_IF, ADC_START, CMP_CHAN, ADC_CHAN) |
| ADC_CFG | 0x9A | Config (bADC_EN, bCMP_EN, bADC_CLK) |
| ADC_DATA | 0x9F | (RO) ADC result |

### ADC Channels

| ADC_CHAN1 | ADC_CHAN0 | Channel | Pin |
|---|---|---|---|
| 0 | 0 | AIN0 | P1.1 |
| 0 | 1 | AIN1 | P1.4 |
| 1 | 0 | AIN2 | P1.5 |
| 1 | 1 | AIN3 | P3.2 |

---

## PWM Registers

| Register | Address | Purpose |
|---|---|---|
| PWM_DATA1 | 0x9C | PWM1 duty cycle data |
| PWM_DATA2 | 0x9B | PWM2 duty cycle data |
| PWM_CTRL | 0x9D | Control (polarity, output enable, interrupt) |
| PWM_CK_SE | 0x9E | Clock divisor |

### PWM_CTRL Bits

| Bit | Name | Purpose |
|---|---|---|
| 7 | bPWM_IE_END | Cycle-end interrupt enable |
| 6 | bPWM2_POLAR | PWM2 output polarity |
| 5 | bPWM1_POLAR | PWM1 output polarity |
| 4 | bPWM_IF_END | Cycle-end interrupt flag |
| 3 | bPWM2_OUT_EN | PWM2 output enable |
| 2 | bPWM1_OUT_EN | PWM1 output enable |
| 1 | bPWM_CLR_ALL | Clear FIFO and count |

---

## TouchKey Registers

| Register | Address | Purpose |
|---|---|---|
| TKEY_CTRL | 0xC3 | Control (channel select, cycle) |
| TKEY_DATL | 0xC4 | (RO) Touch-key data low |
| TKEY_DATH | 0xC5 | (RO) Touch-key data high |

### TouchKey Channels

| Channel | Pin |
|---|---|
| TIN0 | P1.0 |
| TIN1 | P1.1 |
| TIN2 | P1.4 |
| TIN3 | P1.5 |
| TIN4 | P1.6 |
| TIN5 | P1.7 |

---

## Flash/DataFlash Registers

| Register | Address | Purpose |
|---|---|---|
| ROM_ADDR_L | 0x84 | Address low byte |
| ROM_ADDR_H | 0x85 | Address high byte |
| ROM_DATA_L | 0x8E | Data low byte |
| ROM_DATA_H | 0x8F | Data high byte |
| ROM_CTRL | 0x86 | Command (0x9A=write, 0x8E=read) / Status |

### Key Constants

| Name | Value | Purpose |
|---|---|---|
| ROM_CMD_WRITE | 0x9A | Write command |
| ROM_CMD_READ | 0x8E | Read command (DataFlash) |
| bROM_ADDR_OK | 0x40 | Address valid flag |
| bROM_CMD_ERR | 0x02 | Command error flag |
| DATA_FLASH_ADDR | 0xC000 | DataFlash start address |
