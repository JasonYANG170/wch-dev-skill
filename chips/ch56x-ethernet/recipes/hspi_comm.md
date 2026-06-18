# Recipe: HSPI High-Speed SPI Communication

## Overview

Use the HSPI (High-Speed SPI) peripheral for fast inter-chip communication on CH56x.
HSPI supports 8/16/32-bit data widths, DMA transfers, burst mode, and optional ECDC
encryption. It operates in host (master) or slave mode with hardware CRC protection.

## HSPI vs Standard SPI

| Feature       | SPI0/SPI1         | HSPI                |
|---------------|-------------------|---------------------|
| Data width    | 8-bit only        | 8/16/32-bit         |
| Max clock     | 30MHz (sysclk/4)  | 60MHz (sysclk/2)    |
| DMA           | Yes               | Yes, double-buffered |
| Burst mode    | No                | Yes                 |
| CRC check     | No                | Yes (hardware)      |
| ECDC encrypt  | No                | Yes (link to ECDC)  |
| Custom header | No                | Yes (UDF0/UDF1)     |

## GPIO Pin Assignment

HSPI uses fixed GPIO pins (no remap):

| Signal | Pin  | Direction (Host) | Direction (Slave) |
|--------|------|------------------|-------------------|
| TX     | PA9  | Output           | Input             |
| CLK    | PA11 | Output           | Input             |
| RX     | PA10 | Input            | Output            |
| CS     | PA21 | Output           | Input             |

```c
void HSPI_GPIO_Init(void)
{
    // TX, CLK, CS: push-pull output
    R32_PA_DIR |= (1 << 9) | (1 << 11) | (1 << 21);
    R32_PA_DRV |= (1 << 11);  // CLK 16mA drive

    // RX: input
    R32_PA_DIR |= (1 << 10);
}
```

## Data Size Modes

| Constant       | Register Value      | Description |
|----------------|---------------------|-------------|
| `DataSize_8bit`  | `RB_HSPI_DAT8_MOD`  | 8-bit       |
| `DataSize_16bit` | `RB_HSPI_DAT16_MOD` | 16-bit      |
| `DataSize_32bit` | `RB_HSPI_DAT32_MOD` | 32-bit      |

## DMA Double-Buffering

HSPI uses two DMA address/length pairs for ping-pong transfers:
- `R32_HSPI_TX_ADDR0` / `R32_HSPI_TX_ADDR1` - TX buffer addresses
- `R32_HSPI_RX_ADDR0` / `R32_HSPI_RX_ADDR1` - RX buffer addresses
- `R16_HSPI_DMA_LEN0` / `R16_HSPI_DMA_LEN1` - Buffer lengths

All DMA buffers must be in RAMX (0x20020000+).

## Complete Example: Normal Mode Host/Slave

### Host (Transmitter)

```c
#include "CH56x_common.h"

#define FREQ_SYS  120000000
#define DMA_LEN   512

volatile UINT8 Tx_Done = 0;

void HSPI_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void HSPI_Init_Host(void)
{
    // GPIO
    R32_PA_DIR |= (1 << 9) | (1 << 11) | (1 << 21);
    R32_PA_DRV |= (1 << 11);
    R32_PA_DIR |= (1 << 10);

    // Host mode, 32-bit data
    R8_HSPI_CFG = RB_HSPI_MODE | RB_HSPI_DAT32_MOD;

    // Disable hardware ACK (normal mode)
    R8_HSPI_CFG &= ~RB_HSPI_HW_ACK;
    R8_HSPI_CFG &= ~RB_HSPI_TX_TOG_EN;

    // Fast DMA, falling edge sampling
    R8_HSPI_AUX = RB_HSPI_REQ_FT | RB_HSPI_TCK_MOD;
    R8_HSPI_AUX &= ~RB_HSPI_ACK_TX_MOD;
    R8_HSPI_AUX &= ~RB_HSPI_ACK_CNT_SEL;

    // Reset
    R8_HSPI_CTRL &= ~(RB_HSPI_ALL_CLR | RB_HSPI_TRX_RST);

    // Enable TX done interrupt
    R8_HSPI_INT_EN = RB_HSPI_IE_T_DONE | RB_HSPI_IE_FIFO_OV;

    // Custom header
    R32_HSPI_UDF0 = 0x3ABCDEF;
    R32_HSPI_UDF1 = 0x3ABCDEF;

    // DMA addresses (RAMX)
    R32_HSPI_TX_ADDR0 = 0x20020000;
    R16_HSPI_DMA_LEN0 = DMA_LEN - 1;

    // Enable HSPI + DMA
    R8_HSPI_CTRL = RB_HSPI_ENABLE | RB_HSPI_DMA_EN;

    PFIC_EnableIRQ(HSPI_IRQn);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);

    HSPI_Init_Host();

    // Fill RAMX with test data
    for (UINT32 i = 0; i < 0x2000; i++) {
        *(UINT32 *)(0x20020000 + i * 4) = i;
    }

    // Trigger send
    R8_HSPI_INT_FLAG = 0x0F;
    R8_HSPI_CTRL |= RB_HSPI_SW_ACT;

    while (!Tx_Done);

    PRINT("HSPI TX complete\r\n");
    while(1);
}

void HSPI_IRQHandler(void)
{
    static UINT32 cnt = 0;

    if (R8_HSPI_INT_FLAG & RB_HSPI_IF_T_DONE) {
        R8_HSPI_INT_FLAG = RB_HSPI_IF_T_DONE;
        cnt++;

        if (cnt < 64) {  // Send 32KB in 512-byte chunks
            R32_HSPI_TX_ADDR0 += DMA_LEN;
            mDelaymS(10);
            R8_HSPI_CTRL |= RB_HSPI_SW_ACT;
        } else {
            Tx_Done = 1;
        }
    }

    if (R8_HSPI_INT_FLAG & RB_HSPI_IF_FIFO_OV) {
        R8_HSPI_INT_FLAG = RB_HSPI_IF_FIFO_OV;
        PRINT("FIFO OV\r\n");
    }
}
```

### Slave (Receiver)

```c
#include "CH56x_common.h"

#define FREQ_SYS  120000000
#define DMA_LEN   512

volatile UINT8 Rx_Done = 0;

void HSPI_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void HSPI_Init_Slave(void)
{
    // GPIO
    R32_PA_DIR |= (1 << 9) | (1 << 11) | (1 << 21);
    R32_PA_DRV |= (1 << 11);
    R32_PA_DIR |= (1 << 10);

    // Slave mode, 32-bit data
    R8_HSPI_CFG = RB_HSPI_DAT32_MOD;  // No RB_HSPI_MODE = slave

    R8_HSPI_CFG &= ~RB_HSPI_HW_ACK;
    R8_HSPI_CFG &= ~RB_HSPI_RX_TOG_EN;

    R8_HSPI_AUX = RB_HSPI_REQ_FT | RB_HSPI_TCK_MOD;
    R8_HSPI_AUX &= ~RB_HSPI_ACK_TX_MOD;
    R8_HSPI_AUX &= ~RB_HSPI_ACK_CNT_SEL;

    R8_HSPI_CTRL &= ~(RB_HSPI_ALL_CLR | RB_HSPI_TRX_RST);

    // Enable RX done interrupt
    R8_HSPI_INT_EN = RB_HSPI_IE_R_DONE | RB_HSPI_IE_FIFO_OV;

    // DMA addresses (RAMX)
    R32_HSPI_RX_ADDR0 = 0x20020000;
    R16_HSPI_DMA_LEN0 = DMA_LEN - 1;

    R8_HSPI_CTRL = RB_HSPI_ENABLE | RB_HSPI_DMA_EN;

    PFIC_EnableIRQ(HSPI_IRQn);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);

    mDelaymS(100);  // Wait for host to start first
    HSPI_Init_Slave();

    while (!Rx_Done);

    // Verify received data
    UINT8 err = 0;
    for (UINT32 i = 0; i < 0x2000; i++) {
        if (*(UINT32 *)(0x20020000 + i * 4) != i) {
            err = 1;
            break;
        }
    }
    PRINT(err ? "Verify FAILED\r\n" : "Verify OK\r\n");

    while(1);
}

void HSPI_IRQHandler(void)
{
    static UINT32 cnt = 0;

    if (R8_HSPI_INT_FLAG & RB_HSPI_IF_R_DONE) {
        R8_HSPI_INT_FLAG = RB_HSPI_IF_R_DONE;

        // Check CRC
        if (R8_HSPI_RTX_STATUS & RB_HSPI_CRC_ERR) {
            R8_HSPI_CTRL &= ~RB_HSPI_ENABLE;
            PRINT("CRC error\r\n");
            return;
        }

        // Check sequence number
        if (R8_HSPI_RTX_STATUS & RB_HSPI_NUM_MIS) {
            PRINT("Sequence mismatch\r\n");
            return;
        }

        cnt++;
        if (cnt < 64) {
            R32_HSPI_RX_ADDR0 += DMA_LEN;
        } else {
            Rx_Done = 1;
        }
    }

    if (R8_HSPI_INT_FLAG & RB_HSPI_IF_FIFO_OV) {
        R8_HSPI_INT_FLAG = RB_HSPI_IF_FIFO_OV;
    }
}
```

## Burst Mode

Burst mode sends multiple packets in a single hardware-managed sequence with
automatic ACK handshaking. Enable with `RB_HSPI_HW_ACK` and `RB_HSPI_BURST_EN`:

```c
// In HSPI_Init, enable hardware ACK for burst mode
R8_HSPI_CFG |= RB_HSPI_HW_ACK;
R8_HSPI_CFG |= RB_HSPI_TX_TOG_EN;   // Host
// or
R8_HSPI_CFG |= RB_HSPI_RX_TOG_EN;   // Slave

// Enable burst done interrupt
R8_HSPI_INT_EN |= RB_HSPI_IE_B_DONE;

// Configure burst: send 64 packets
R16_HSPI_BURST_CFG = (64 << 8) | RB_HSPI_BURST_EN;

// Trigger burst send
R8_HSPI_INT_FLAG = 0x0F;
R8_HSPI_CTRL |= RB_HSPI_SW_ACT;
```

In the ISR, handle `RB_HSPI_IF_B_DONE` for burst completion.

## ECDC-Encrypted HSPI

Link the ECDC engine to HSPI for transparent encryption:

```c
// Before enabling HSPI DMA, configure ECDC:
ECDC_Init(MODE_AES_ECB, ECDCCLK_240MHZ, KEYLENGTH_128BIT, key, NULL);

// Host: encrypt outgoing data
ECDC_Excute(RAM_TO_PERIPHERAL_ENCRY, 0);

// Slave: decrypt incoming data
ECDC_Excute(PERIPHERAL_TO_RAM_DECRY, 0);

// Then enable HSPI DMA as normal
R8_HSPI_CTRL = RB_HSPI_ENABLE | RB_HSPI_DMA_EN;
```

For CTR mode, reload the counter between packets in the ISR:
```c
ECDC_RloadCount(RAM_TO_PERIPHERAL_ENCRY, 0, CountValue);
```

## Status and Error Handling

| Status Bit          | Description                    |
|---------------------|--------------------------------|
| `RB_HSPI_CRC_ERR`   | CRC check failed               |
| `RB_HSPI_NUM_MIS`   | Packet sequence number mismatch |

On CRC or sequence error, reset the DMA addresses and counters:

```c
if (R8_HSPI_RTX_STATUS & RB_HSPI_CRC_ERR) {
    R32_HSPI_TX_ADDR0 = DMA_TX_Addr0;
    R32_HSPI_RX_ADDR0 = DMA_RX_Addr0;
    cnt = 0;
    R8_HSPI_RX_SC = 0;  // Clear RX sequence counter
}
```

## Notes

- All HSPI DMA buffers must be in RAMX (0x20020000+).
- System clock should be 120MHz for maximum HSPI throughput.
- In normal mode, the host must delay between packets if the slave is not ready.
- In burst mode, hardware manages the ACK handshake automatically.
- The `RB_HSPI_SW_ACT` bit triggers a software-initiated send. Set it after
  clearing interrupt flags and configuring DMA addresses.
- CRC is computed automatically on the TX side and verified on the RX side.

## Example Projects

- `CH569EVT/EVT/EXAM/HSPI/HSPI_NormalMode/` - Basic host/slave transfer
- `CH569EVT/EVT/EXAM/HSPI/HSPI_BurstMode/` - Burst mode transfer
- `CH569EVT/EVT/EXAM/HSPI/HSPI_DoubleDMAMode/` - Double-buffered DMA
- `CH569EVT/EVT/EXAM/HSPI/HSPI_ECDC/` - ECDC-encrypted HSPI transfer
