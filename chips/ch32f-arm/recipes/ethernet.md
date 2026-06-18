# Recipe: Ethernet (WCHNET Library)

## Overview

Use the built-in Ethernet MAC on CH32F20x (CH32F207) with the WCHNET TCP/IP protocol stack library for TCP, UDP, DHCP, DNS, MQTT, and other network protocols. The library provides a socket-based API similar to BSD sockets.

**Availability**: CH32F20x only (CH32F207 variant with Ethernet MAC).

## Key API Functions

```c
// Library initialization
uint8_t WCHNET_Init(const uint8_t *ip, const uint8_t *gwip,
                    const uint8_t *mask, const uint8_t *macaddr);
uint8_t WCHNET_GetVer(void);
void WCHNET_GetMacAddr(uint8_t *macaddr);
uint8_t WCHNET_ConfigLIB(struct _WCH_CFG *cfg);

// Main loop functions (must be called cyclically)
void WCHNET_MainTask(void);
uint8_t WCHNET_QueryGlobalInt(void);
uint8_t WCHNET_GetGlobalInt(void);

// Socket operations
uint8_t WCHNET_SocketCreat(uint8_t *socketid, SOCK_INF *socinf);
uint8_t WCHNET_SocketSend(uint8_t socketid, uint8_t *buf, uint32_t *len);
uint8_t WCHNET_SocketRecv(uint8_t socketid, uint8_t *buf, uint32_t *len);
uint8_t WCHNET_SocketConnect(uint8_t socketid);
uint8_t WCHNET_SocketListen(uint8_t socketid);
uint8_t WCHNET_SocketClose(uint8_t socketid, uint8_t mode);
void WCHNET_ModifyRecvBuf(uint8_t socketid, uint32_t bufaddr, uint32_t bufsize);

// Socket queries
uint8_t WCHNET_GetSocketInt(uint8_t socketid);
uint32_t WCHNET_SocketRecvLen(uint8_t socketid, uint32_t *bufaddr);

// UDP specific
uint8_t WCHNET_SocketUdpSendTo(uint8_t socketid, uint8_t *buf,
                                uint32_t *slen, uint8_t *sip, uint16_t port);

// TCP keep-alive
uint8_t WCHNET_SocketSetKeepLive(uint8_t socketid, uint8_t enable);
void WCHNET_ConfigKeepLive(struct _KEEP_CFG *cfg);

// DHCP
uint8_t WCHNET_DHCPStart(dhcp_callback dhcp);
uint8_t WCHNET_DHCPStop(void);

// DNS
void WCHNET_InitDNS(uint8_t *dnsip, uint16_t port);
uint8_t WCHNET_HostNameGetIp(const char *hostname, uint8_t *addr,
                              dns_callback found, void *arg);

// PHY status
uint8_t WCHNET_GetPHYStatus(void);

// Address conversion
uint8_t WCHNET_Aton(const char *cp, uint8_t *addr);
uint8_t *WCHNET_Ntoa(uint8_t *ipaddr);
```

## Socket Information Structure

```c
typedef struct _SOCK_INF {
    uint32_t IntStatus;        // Interrupt status
    uint32_t SockIndex;        // Socket index
    uint32_t RecvStartPoint;   // Receive buffer start
    uint32_t RecvBufLen;       // Receive buffer length
    uint32_t RecvCurPoint;     // Current receive pointer
    uint32_t RecvReadPoint;    // Read pointer
    uint32_t RecvRemLen;       // Remaining data length
    uint32_t ProtoType;        // Protocol type (TCP/UDP/IP_RAW)
    uint32_t SockStatus;       // Socket state
    uint32_t DesPort;          // Destination port
    uint32_t SourPort;         // Source port
    uint8_t  IPAddr[4];        // Destination IP
    void *Resv1;               // Reserved (PCB pointer)
    void *Resv2;               // Reserved
    pSockRecv AppCallBack;     // Receive callback
} SOCK_INF;
```

## Protocol Types

| Constant | Value | Description |
|----------|-------|-------------|
| PROTO_TYPE_IP_RAW | 0 | IP layer raw data |
| PROTO_TYPE_UDP | 2 | UDP protocol |
| PROTO_TYPE_TCP | 3 | TCP protocol |

## Global Interrupt Flags

| Flag | Description |
|------|-------------|
| GINT_STAT_UNREACH | Destination unreachable |
| GINT_STAT_IP_CONFLI | IP address conflict |
| GINT_STAT_PHY_CHANGE | PHY link status change |
| GINT_STAT_SOCKET | Socket event |

## Socket Interrupt Flags

| Flag | Description |
|------|-------------|
| SINT_STAT_RECV | Data received |
| SINT_STAT_CONNECT | TCP connection established |
| SINT_STAT_DISCONNECT | TCP disconnected |
| SINT_STAT_TIM_OUT | TCP connection timeout |

## Example: TCP Client

```c
#include "string.h"
#include "eth_driver.h"

#define WCHNET_MAX_SOCKET_NUM  4
#define RECE_BUF_LEN           512

u8 MACAddr[6];
u8 IPAddr[4] = {192, 168, 1, 10};
u8 GWIPAddr[4] = {192, 168, 1, 1};
u8 IPMask[4] = {255, 255, 255, 0};
u8 DESIP[4] = {192, 168, 1, 100};
u16 desport = 1000;
u16 srcport = 1000;

u8 SocketId;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];

// Timer2 for WCHNET timing (10ms period)
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        WCHNET_MainTask();  // Call periodically
    }
}

void WCHNET_CreateTcpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset(&TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy(TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;

    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("SocketId %d\r\n", SocketId);

    i = WCHNET_SocketConnect(SocketId);
}

void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    if(intstat & SINT_STAT_RECV) {
        // Echo received data back
        u32 len = SocketInf[socketid].RecvRemLen;
        WCHNET_SocketSend(socketid, (u8*)SocketInf[socketid].RecvReadPoint, &len);
        WCHNET_SocketRecv(socketid, NULL, &len);
    }
    if(intstat & SINT_STAT_CONNECT) {
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connected\r\n");
    }
    if(intstat & SINT_STAT_DISCONNECT) {
        printf("TCP Disconnected\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT) {
        printf("TCP Timeout\r\n");
        WCHNET_CreateTcpSocket();  // Reconnect
    }
}

void WCHNET_HandleGlobalInt(void)
{
    u8 intstat = WCHNET_GetGlobalInt();

    if(intstat & GINT_STAT_PHY_CHANGE) {
        u16 phy = WCHNET_GetPHYStatus();
        if(phy & PHY_Linked_Status)
            printf("PHY Link Up\r\n");
    }
    if(intstat & GINT_STAT_SOCKET) {
        for(u16 i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            u8 sockint = WCHNET_GetSocketInt(i);
            if(sockint)
                WCHNET_HandleSockInt(i, sockint);
        }
    }
}

int main(void)
{
    u8 i;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("TCP Client Test\r\n");

    WCHNET_GetMacAddr(MACAddr);
    TIM2_Init();

    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);
    if(i == WCHNET_ERR_SUCCESS)
        printf("WCHNET Init Success\r\n");

    // Create TCP sockets
    for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateTcpSocket();

    while(1) {
        WCHNET_MainTask();
        if(WCHNET_QueryGlobalInt())
            WCHNET_HandleGlobalInt();
    }
}
```

## Example: DHCP Client

```c
uint8_t DHCP_Callback(uint8_t status, void *arg)
{
    if(status == 0) {
        printf("DHCP Success\r\n");
        printf("IP: %s\r\n", WCHNET_Ntoa(netif.ip_addr));
    }
    return 0;
}

// After WCHNET_Init():
WCHNET_DHCPStart(DHCP_Callback);
```

## Example: DNS Resolution

```c
void DNS_Callback(const char *name, uint8_t *ipaddr, void *arg)
{
    printf("DNS: %s -> %s\r\n", name, WCHNET_Ntoa(ipaddr));
}

// Initialize DNS with public DNS server
u8 dns_server[4] = {8, 8, 8, 8};
WCHNET_InitDNS(dns_server, 53);

// Resolve hostname
WCHNET_HostNameGetIp("example.com", resolved_ip, DNS_Callback, NULL);
```

## WCHNET Library Files

The Ethernet library consists of pre-compiled files:
- `wchnet.h` - API declarations and data structures
- `eth_driver.h` - Hardware driver and PHY configuration
- `libWCHNET.a` / `WCHNET.lib` - Pre-compiled protocol stack library
- `eth_driver.c` - ETH hardware initialization (source)

## Pitfalls

- **Timer requirement**: WCHNET requires a periodic timer call (typically TIM2 at 10ms) to drive the protocol stack timing. Call `WCHNET_MainTask()` from the timer ISR or main loop.
- **MAC address**: Get the unique chip MAC via `WCHNET_GetMacAddr()`. Do not use a random MAC.
- **Socket buffer**: After TCP connection, call `WCHNET_ModifyRecvBuf()` to assign a receive buffer to the socket. Without this, data reception will fail.
- **Main loop**: `WCHNET_MainTask()` must be called continuously in the main loop. Do not block for long periods.
- **Keep-alive**: For long-lived TCP connections, enable keep-alive with `WCHNET_SocketSetKeepLive()` and configure timing with `WCHNET_ConfigKeepLive()`.
- **Library version check**: Always verify `WCHNET_LIB_VER == WCHNET_GetVer()` at startup.
- **PHY link**: Wait for PHY link establishment before creating sockets. Check `WCHNET_GetPHYStatus()` for `PHY_Linked_Status`.
- **Chip variant**: Ethernet is only available on CH32F207 (CH32F20x_D8C variant). Other CH32F20x chips may not have Ethernet.

## Related Examples

- `CH32F20xEVT/EVT/EXAM/ETH/TcpClient` - TCP client with data loopback
- `CH32F20xEVT/EVT/EXAM/ETH/DHCP` - DHCP client
- `CH32F20xEVT/EVT/EXAM/ETH/DNS` - DNS resolution
- `CH32F20xEVT/EVT/EXAM/ETH/MQTT` - MQTT client
- `CH32F20xEVT/EVT/EXAM/ETH/IPRaw_PING` - ICMP ping
- `CH32F20xEVT/EVT/EXAM/ETH/MACRaw` - Raw Ethernet frames
- `CH32F20xEVT/EVT/EXAM/ETH/ETH_UART` - Ethernet to UART bridge
- `CH32F20xEVT/EVT/EXAM/ETH/ETH_IAP` - Ethernet-based IAP
- `CH32F20xEVT/EVT/EXAM/ETH/Mail` - Email client
