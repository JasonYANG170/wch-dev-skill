# RF Communication (2.4GHz Non-Standard)

> **Applicable summary**: Use the 2.4GHz RF transceiver for non-standard (non-BLE) wireless communication, including basic TX/RX, UART bridge, and direct test mode.

## Trigger Intent

- "RF communication"
- "2.4GHz wireless"
- "non-standard RF"
- "RF transceiver"
- "wireless UART"
- "RF basic"
- "DTM test"

## Prerequisites

| Condition | Requirement |
|---|---|
| Reference projects | `CH572EVT/EVT/EXAM/RF/RF_Basic/`, `RF_Uart/`, `RF_Extend/`, `RF_UartDongle/`, `Direct_Test_Mode/` |
| BLE library | Required -- RF driver is part of the BLE library (`LIB/`) |

## Available RF Examples

| Example | Directory | Description |
|---|---|---|
| RF_Basic | `RF/RF_Basic/` | Basic 2.4GHz TX/RX with configurable frequency, sync word, and power |
| RF_Uart | `RF/RF_Uart/` | Wireless UART bridge -- transparent serial-over-RF link |
| RF_Extend | `RF/RF_Extend/` | Extended RF features |
| RF_UartDongle | `RF/RF_UartDongle/` | USB dongle side of the wireless UART bridge |
| Direct_Test_Mode | `RF/Direct_Test_Mode/` | RF DTM test mode for certification testing |
| RF_PHY | `BLE/RF_PHY/` | Non-standard RF PHY example (BLE library based) |

## Call Chain

```
Step 1: Configure system clock (100MHz PLL)
Step 2: Initialize RF role with RFRole_BasicInit()
Step 3: Configure TX/RX parameters (frequency, sync word, power, CRC)
Step 4: Enable BLEB/BLEL interrupts
Step 5: Start TX or RX
Step 6: Handle RF_ProcessCallBack events
```

## Step-by-Step Instructions

### RF Basic -- TX Mode

```c
#include "rf_basic.h"

rfipTx_t gTxParam;
rfipRx_t gRxParam;

__attribute__((__aligned__(4))) uint8_t TxBuf[64];
__attribute__((__aligned__(4))) uint8_t RxBuf[264];

#define TEST_DATA_LEN    4
#define TEST_FREQUENCY   16
#define ACCESS_ADR       0x71764129  // Access address (sync word)
#define CRC_INIT         0x555555
#define CRC_POLY         0x0000065B

void RFRole_Init(void)
{
    sys_safe_access_enable();
    R32_MISC_CTRL = (R32_MISC_CTRL & (~(0x3f << 24))) | (0xe << 24);
    sys_safe_access_disable();
    PKT_DET_CFG4(0x3f);

    rfRoleConfig_t conf = {0};
    conf.rfProcessCB = RF_ProcessCallBack;
    conf.processMask = RF_STATE_RX | RF_STATE_RX_CRCERR | RF_STATE_TX_FINISH | RF_STATE_TIMEOUT;
    RFRole_BasicInit(&conf);

    TPROPERTIES_CFG Properties;
    Properties.cfgVal = PHY_MODE_PHY_2M;  // BLE 2M PHY mode

    // TX parameters
    gTxParam.accessAddress = ACCESS_ADR;
    gTxParam.crcInit = CRC_INIT;
    gTxParam.crcPoly = CRC_POLY;
    gTxParam.properties = Properties.cfgVal;
    gTxParam.waitTime = 80 * 2;           // Channel switch stabilization (~80us)
    gTxParam.txPowerVal = LL_TX_PWR_6_DBM;
    gTxParam.txLen = TEST_DATA_LEN;

    // RX parameters
    gRxParam.accessAddress = ACCESS_ADR;
    gRxParam.crcInit = CRC_INIT;
    gRxParam.crcPoly = CRC_POLY;
    gRxParam.properties = Properties.cfgVal;
    gRxParam.rxDMA = (uint32_t)RxBuf;
    gRxParam.rxMaxLen = TEST_DATA_LEN;

    PFIC_EnableIRQ(BLEB_IRQn);
    PFIC_EnableIRQ(BLEL_IRQn);
}
```

### RF Basic -- Start TX

```c
__HIGH_CODE
void rf_tx_start(uint8_t *pBuf)
{
    gTxParam.frequency = TEST_FREQUENCY;
    gTxParam.txDMA = (uint32_t)pBuf;
    gTxParam.waitTime = 40 * 2;
    RFIP_StartTx(&gTxParam);
}
```

### RF Basic -- Start RX

```c
__HIGH_CODE
void rf_rx_start(void)
{
    gRxParam.frequency = TEST_FREQUENCY;
    gRxParam.timeOut = 0;  // 0 = no timeout
    RFIP_SetRx(&gRxParam);
}
```

### RF Callback Handler

```c
__HIGH_CODE
void RF_ProcessCallBack(rfRole_States_t sta, uint8_t id)
{
    if (sta & RF_STATE_RX) {
        // Packet received
        uint8_t *pData = (uint8_t *)gRxParam.rxDMA;
        int8_t rssi = (int8_t)pData[TEST_DATA_LEN + RSSI_OFFSET];
        PRINT("RX data, RSSI=%d\n", rssi);
        rf_rx_start();  // Re-enable RX
    }
    if (sta & RF_STATE_RX_CRCERR) {
        rf_rx_start();  // CRC error, restart RX
    }
    if (sta & RF_STATE_TX_FINISH) {
        gTxCount++;
    }
    if (sta & RF_STATE_TIMEOUT) {
        PRINT("RX timeout\n");
        rf_rx_start();
    }
}
```

### Required Interrupt Handlers

```c
__INTERRUPT
__HIGH_CODE
void LLE_IRQHandler(void)
{
    LLE_LibIRQHandler();
}

__INTERRUPT
__HIGH_CODE
void BB_IRQHandler(void)
{
    BB_LibIRQHandler();
}
```

### TX Power Configuration

| Value | Constant | Power |
|---|---|---|
| -25 dBm | `LL_TX_PWR_NEG_25_DBM` | Minimum |
| 0 dBm | `LL_TX_PWR_0_DBM` | Default |
| +6 dBm | `LL_TX_PWR_6_DBM` | Maximum |

### PHY Mode Selection

| Mode | Constant | Description |
|---|---|---|
| BLE 1M | `PHY_MODE_PHY_1M` | Standard BLE 1Mbps |
| BLE 2M | `PHY_MODE_PHY_2M` | High-throughput 2Mbps |
| BLE Coded | `PHY_MODE_PHY_CODED` | Long-range coded PHY |

### RF State Flags

| Flag | Description |
|---|---|
| `RF_STATE_RX` | Packet received successfully |
| `RF_STATE_RX_CRCERR` | Received packet has CRC error |
| `RF_STATE_TX_FINISH` | Transmission completed |
| `RF_STATE_TIMEOUT` | RX timeout (address matched but no data) |

## RF UART Bridge

The `RF_Uart` example implements a wireless transparent serial link. It handles:

- Automatic pairing/binding via `PKT_CMD_BOUND_REQ` / `PKT_CMD_BOUND_RSP`
- Configurable baud rate, stop bits, parity via remote command (`OPCODE_BSP`)
- Data framing with sequence numbers and ACK
- Automatic retransmission on failure
- Flash-based binding info persistence

The `RF_UartDongle` example is the USB dongle counterpart.

## Common Errors

| Error | Cause | Solution |
|---|---|---|
| No TX/RX | Access address mismatch | Ensure TX and RX use same `accessAddress` |
| CRC errors | CRC init/poly mismatch | Verify `crcInit` and `crcPoly` match on both sides |
| TX power too low | Wrong power constant | Use `LL_TX_PWR_6_DBM` for max power |
| Channel switch unstable | `waitTime` too small | Set `waitTime >= 80*2` (80us minimum) |
| DMA buffer crash | Buffer not 4-byte aligned | Use `__attribute__((__aligned__(4)))` on TX/RX buffers |
| RF not initialized | Missing `RFRole_BasicInit()` | Call init before TX/RX operations |

## Reference Projects

- `CH572EVT/EVT/EXAM/RF/RF_Basic/` -- Basic 2.4GHz TX/RX
- `CH572EVT/EVT/EXAM/RF/RF_Uart/` -- Wireless UART bridge (TX side)
- `CH572EVT/EVT/EXAM/RF/RF_UartDongle/` -- Wireless UART bridge (dongle side)
- `CH572EVT/EVT/EXAM/RF/RF_Extend/` -- Extended RF features
- `CH572EVT/EVT/EXAM/RF/Direct_Test_Mode/` -- RF DTM certification test
- `CH572EVT/EVT/EXAM/BLE/RF_PHY/` -- Non-standard RF PHY (BLE library based)
- `resources/ble_api.md` -- BLE/RF API reference
