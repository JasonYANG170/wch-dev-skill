# Recipe: Ethernet Communication

## When to Use
User wants to implement Ethernet networking on CH32V307 or CH32V407 (which have built-in MAC and 10BASE-T PHY).

## API Reference (from ch32v30x_eth.h)

### Key Functions
```c
void ETH_DeInit(void);
void ETH_StructInit(ETH_InitTypeDef* ETH_InitStruct);
void ETH_SoftwareReset(void);
FlagStatus ETH_GetSoftwareResetStatus(void);
FlagStatus ETH_GetlinkStaus(void);
void ETH_Start(void);
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength);
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);
uint32_t ETH_GetRxPktSize(void);
void ETH_DropRxPkt(void);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
void ETH_MACTransmissionCmd(FunctionalState NewState);
void ETH_MACReceptionCmd(FunctionalState NewState);
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_DMATransmissionCmd(FunctionalState NewState);
void ETH_DMAReceptionCmd(FunctionalState NewState);
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState);
```

### ETH Init Structure (abbreviated)
```c
typedef struct {
    uint32_t ETH_AutoNegotiation;   // ETH_AutoNegotiation_Enable/Disable
    uint32_t ETH_Speed;             // ETH_Speed_10M, _100M
    uint32_t ETH_Mode;              // ETH_Mode_FullDuplex, _HalfDuplex
    uint32_t ETH_ReceiveAll;        // ETH_ReceiveAll_Enable/Disable
    uint32_t ETH_ChecksumOffload;   // Checksum hardware offload
    // ... many more configuration fields
} ETH_InitTypeDef;
```

### DMA Descriptor Structure
```c
typedef struct {
    uint32_t volatile Status;         // Status bits (OWN, etc.)
    uint32_t ControlBufferSize;       // Buffer sizes
    uint32_t Buffer1Addr;             // Buffer address
    uint32_t Buffer2NextDescAddr;     // Next descriptor or buffer2
} ETH_DMADESCTypeDef;
```

## Example: Basic Ethernet Send/Receive

```c
#include "ch32v30x.h"

#define ETH_TX_BUF_NUM  4
#define ETH_RX_BUF_NUM  4
#define ETH_BUF_SIZE    1524

ETH_DMADESCTypeDef DMARxDscrTab[ETH_RX_BUF_NUM];
ETH_DMADESCTypeDef DMATxDscrTab[ETH_TX_BUF_NUM];
uint8_t Rx_Buff[ETH_RX_BUF_NUM][ETH_BUF_SIZE];
uint8_t Tx_Buff[ETH_TX_BUF_NUM][ETH_BUF_SIZE];

void ETH_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                           RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

    // MII pins (CH32V307 has internal 10BASE-T PHY)
    // PA1 = ETH_RMII_REF_CLK (or MII_RX_CLK)
    // PA2 = ETH_RMII_MDIO
    // PA7 = ETH_RMII_CRS_DV (or MII_RX_DV)
    // PB11 = ETH_RMII_TX_EN (or MII_TX_EN)
    // PB12 = ETH_RMII_TXD0 (or MII_TXD0)
    // PB13 = ETH_RMII_TXD1 (or MII_TXD1)
    // PC4 = ETH_RMII_RXD0 (or MII_RXD0)
    // PC5 = ETH_RMII_RXD1 (or MII_RXD1)

    // Configure MII/RMII pins as AF push-pull
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;

    // PA2 (MDC), PA7 (RX_DV)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PB11 (TX_EN), PB12 (TXD0), PB13 (TXD1)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PC4 (RXD0), PC5 (RXD1)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void ETH_MAC_DMA_Init(uint8_t *mac_addr)
{
    ETH_InitTypeDef ETH_InitStruct;

    ETH_DeInit();
    ETH_SoftwareReset();
    while(ETH_GetSoftwareResetStatus());

    ETH_StructInit(&ETH_InitStruct);
    ETH_InitStruct.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStruct.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStruct.ETH_Speed = ETH_Speed_10M;
    ETH_InitStruct.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStruct.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    ETH_Init(&ETH_InitStruct);

    // Set MAC address
    ETH_MACAddressConfig(ETH_MAC_Address0, mac_addr);

    // Initialize DMA descriptors
    ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TX_BUF_NUM);
    ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RX_BUF_NUM);
}

uint32_t ETH_SendFrame(uint8_t *data, uint16_t len)
{
    return ETH_HandleTxPkt(data, len);
}

uint16_t ETH_ReceiveFrame(uint8_t *buf, uint16_t max_len)
{
    uint32_t len = ETH_GetRxPktSize();
    if(len == 0) return 0;
    if(len > max_len) len = max_len;
    ETH_HandleRxPkt(buf);
    return (uint16_t)len;
}

int main(void)
{
    uint8_t mac_addr[6] = {0x00, 0x0E, 0xC6, 0x12, 0x34, 0x56};
    uint8_t rx_buf[1524];

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();

    ETH_GPIO_Init();
    ETH_MAC_DMA_Init(mac_addr);
    ETH_Start();

    while(1)
    {
        if(ETH_GetlinkStaus() == PHY_Linked_Status)
        {
            uint16_t len = ETH_ReceiveFrame(rx_buf, sizeof(rx_buf));
            if(len > 0)
            {
                // Process received Ethernet frame
                // rx_buf[0..5] = dest MAC, rx_buf[6..11] = src MAC
                // rx_buf[12..13] = EtherType, rx_buf[14..] = payload
            }
        }
    }
}
```

## Pitfalls
- **CH32V307/V407 only** -- CH32V103 and CH32V20x do not have Ethernet
- **Internal 10BASE-T PHY** -- CH32V307 has built-in PHY for 10Mbps; 100Mbps needs external PHY
- **GPIO remap** -- check if MII or RMII mode needs GPIO remap (`GPIO_Remap_ETH`, `GPIO_Remap_MII_RMII_SEL`)
- **DMA descriptors must be aligned** -- typically 4-byte aligned
- **Link status check** -- always verify link is up before sending/receiving
- **MAC address must be unique** -- use WCH-provided MAC or locally-administered address
