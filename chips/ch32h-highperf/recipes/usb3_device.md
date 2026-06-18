# USB 3.0 Superspeed Device

## Overview

CH32H417 is one of the very few microcontrollers with USB 3.0 Superspeed (5 Gbps) support. The USBSS peripheral works in conjunction with the SerDes PHY for high-speed serial communication.

## Architecture

```
Application Layer
       |
   USBSS Driver
       |
  USBSS Peripheral (register interface)
       |
  SerDes PHY (serializer/deserializer)
       |
  USB 3.0 Connector (SS TX+/TX-, SS RX+/RX-)
```

## SerDes Configuration

The SerDes PHY must be configured before USBSS can operate:

```c
#include "ch32h417.h"
#include "ch32h417_serdes.h"

void SerDes_Init(void)
{
    // Enable SerDes clock
    RCC->AHBPCENR |= RCC_AHBPeriph_SDS;

    SDS_CFG_TypeDef cfg = {0};
    cfg.RxPowerUp = 1;
    cfg.TxPowerUp = 1;
    cfg.RxEn = 1;
    cfg.TxEn = 1;

    SDS_Config(SDS1, &cfg);

    // Wait for PHY ready
    while(!(SDS_ReadCOMMAFlagBit(SDS1) & SDSIT_ST_PhyReady));
}
```

## SerDes API Functions

```c
void SDS_Config(SDS_TypeDef *SDSx, SDS_CFG_TypeDef *sds_cfg);
void SDS_DMA_Tx_CFG(SDS_TypeDef *SDSx, uint32_t DMAaddr, uint32_t DataLen, uint32_t CustomWord);
ErrorStatus SDS_DMA_Rx_CFG(SDS_TypeDef *SDSx, uint32_t DMAaddr0, uint32_t DMAaddr1);
uint32_t SDS_GetFirstWord(SDS_TypeDef *SDSx, uint8_t buffnum);
void SDS_RTX_Ctrl(SDS_TypeDef *SDSx, SDSRTXCtrl_TypeDef *ctrl);
void SDS_ClearIT(SDS_TypeDef *SDSx, SDSIT_FLAG_TypeDef sdsit_fl);
SDSIT_FLAG_TypeDef SDS_ReadIT(SDS_TypeDef *SDSx);
void SDS_ConfigIT(SDS_TypeDef *SDSx, SDSIT_EN_TypeDef sdsit_en, FunctionalState en);
SDSIT_ST_TypeDef SDS_ReadCOMMAFlagBit(SDS_TypeDef *SDSx);
void SDS_GetCurrentDMA(SDS_TypeDef *SDSx, uint32_t *Addr0, uint32_t *Addr1);
void SDS_Rx_Pwrp(SDS_TypeDef *SDSx);
```

## SerDes Status Flags

| Flag | Description |
|------|-------------|
| `SDSIT_ST_PhyReady` | PHY is ready |
| `SDSIT_ST_RxSEQMatch` | RX sequence match |
| `SDSIT_ST_RxCRCOk` | RX CRC OK |
| `SDSIT_ST_PLLLock` | PLL locked |
| `SDSIT_ST_LinkFree` | Link is free |
| `SDSIT_ST_RxFIFOReady` | RX FIFO ready |

## SerDes Interrupts

| Interrupt | Description |
|-----------|-------------|
| `SDSIT_EN_PhyReady` | PHY ready interrupt |
| `SDSIT_EN_TxInt` | TX interrupt |
| `SDSIT_EN_RxInt` | RX interrupt |
| `SDSIT_EN_RxErr` | RX error interrupt |
| `SDSIT_EN_FIFOOverFlow` | FIFO overflow interrupt |
| `SDSIT_EN_Cominit` | COMINIT interrupt |

## USBSS Device Example

```c
#include "ch32h417.h"

// USBSS endpoint buffer
uint8_t usbss_ep0_buf[64] __attribute__((aligned(4)));
uint8_t usbss_ep1_buf[1024] __attribute__((aligned(4)));

void USBSS_Device_Init(void)
{
    // 1. Initialize SerDes PHY
    SerDes_Init();

    // 2. Configure USBSS clocks
    RCC->AHBPCENR |= RCC_AHBPeriph_USBSS;

    // 3. Configure USBSS endpoint 0 (control)
    USBSS->EP0_CFG = /* EP0 configuration */;
    USBSS->EP0_DMA_ADDR = (uint32_t)usbss_ep0_buf;

    // 4. Configure USBSS endpoint 1 (bulk IN)
    USBSS->EP1_CFG = /* EP1 configuration for bulk IN */;
    USBSS->EP1_DMA_ADDR = (uint32_t)usbss_ep1_buf;

    // 5. Enable USBSS
    USBSS->CTRL |= USBSS_EN;
}

// Send data via USBSS bulk endpoint
void USBSS_BulkSend(uint8_t *data, uint32_t len)
{
    // Copy data to EP buffer
    memcpy(usbss_ep1_buf, data, len);

    // Set transfer length
    USBSS->EP1_LEN = len;

    // Start transfer
    USBSS->EP1_CTRL |= USBSS_EP_TX_START;

    // Wait for completion
    while(!(USBSS->EP1_STATUS & USBSS_EP_TX_DONE));
    USBSS->EP1_STATUS = USBSS_EP_TX_DONE;  // Clear flag
}
```

## USB 3.0 vs USB 2.0 Comparison

| Feature | USB 2.0 (USBFS/USBHS) | USB 3.0 (USBSS) |
|---------|------------------------|-----------------|
| Speed | 12/480 Mbps | 5 Gbps |
| Encoding | NRZI | 8b/10b |
| PHY | Integrated | SerDes |
| Power | 500mA max | 900mA max |
| Duplex | Half | Full |

## Important Notes

- USBSS requires proper PCB layout for differential pairs (90 ohm impedance)
- SerDes PLL lock time may vary; always check `SDSIT_ST_PLLLock`
- Use DMA for high-throughput transfers to avoid CPU bottleneck
- USBSS descriptors must include BOS descriptor with SuperSpeed capability
- See `CH32H417EVT/EVT/EXAM/USBSS/` for complete working examples
